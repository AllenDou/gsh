#include "common/fmacros.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include <limits.h>
#include <sys/time.h>
#include <ctype.h>

#include "dict.h"
#include "common/zmalloc.h"

/* Using dictEnableResize() / dictDisableResize() we make possible to
 * enable/disable resizing of the hash table as needed. This is very important
 * for Redis, as we use copy-on-write and don't want to move too much memory
 * around when there is a child performing saving operations.
 *
 * Note that even when dict_can_resize is set to 0, not all resizes are
 * prevented: an hash table is still allowed to grow if the ratio between
 * the number of elements and the buckets > dict_force_resize_ratio. */
static int dict_can_resize = 1;
static unsigned int dict_force_resize_ratio = 5;

/* -------------------------- private prototypes ---------------------------- */

static int _dictExpandIfNeeded(dict *ht);
static unsigned long _dictNextPower(unsigned long size);
static int _dictKeyIndex(dict *ht, const void *key);
static int _dictInit(dict *ht, dictType *type, void *privDataPtr);

/* -------------------------- hash functions -------------------------------- */

/* Thomas Wang's 32 bit Mix Function */
unsigned int dictIntHashFunction(unsigned int key)
{
		key += ~(key << 15);
		key ^=  (key >> 10);
		key +=  (key << 3);
		key ^=  (key >> 6);
		key += ~(key << 11);
		key ^=  (key >> 16);
		return key;
}

/* Identity hash function for integer keys */
unsigned int dictIdentityHashFunction(unsigned int key)
{
		return key;
}

/* Generic hash function (a popular one from Bernstein).
 * I tested a few and this was the best. */
unsigned int dictGenHashFunction(const unsigned char *buf, int len) {
		unsigned int hash = 5381;

		while (len--)
				hash = ((hash << 5) + hash) + (*buf++); /* hash * 33 + c */
		return hash;
}

/* And a case insensitive version */
unsigned int dictGenCaseHashFunction(const unsigned char *buf, int len) {
		unsigned int hash = 5381;

		while (len--)
				hash = ((hash << 5) + hash) + (tolower(*buf++)); /* hash * 33 + c */
		return hash;
}

/* ----------------------------- API implementation ------------------------- */

/* Reset an hashtable already initialized with ht_init().
 * NOTE: This function should only called by ht_destroy(). */
static void _dictReset(dictht *ht)
{
		ht->table = NULL;
		ht->size = 0;
		ht->sizemask = 0;
		ht->used = 0;
}

/* Create a new hash table */
dict *dictCreate(dictType *type,
				void *privDataPtr)
{
		dict *d = zmalloc(sizeof(*d));

		_dictInit(d,type,privDataPtr);
		return d;
}

/* Initialize the hash table */
int _dictInit(dict *d, dictType *type,
				void *privDataPtr)
{
		_dictReset(&d->ht[0]);
		_dictReset(&d->ht[1]);
		d->type = type;
		d->privdata = privDataPtr;
		d->rehashidx = -1;
		d->iterators = 0;
		return DICT_OK;
}


/* Expand or create the hashtable */
int dictExpand(dict *d, unsigned long size)
{
		dictht n; /* the new hashtable */
		unsigned long realsize = _dictNextPower(size);

		/* the size is invalid if it is smaller than the number of
		 * elements already inside the hashtable */
		if (dictIsRehashing(d) || d->ht[0].used > size)
				return DICT_ERR;

		/* Allocate the new hashtable and initialize all pointers to NULL */
		n.size = realsize;
		n.sizemask = realsize-1;
		n.table = zcalloc(realsize*sizeof(dictEntry*));
		n.used = 0;

		/* Is this the first initialization? If so it's not really a rehashing
		 * we just set the first hash table so that it can accept keys. */
		if (d->ht[0].table == NULL) {
				d->ht[0] = n;
				return DICT_OK;
		}

		/* Prepare a second hash table for incremental rehashing */
		d->ht[1] = n;
		d->rehashidx = 0;
		return DICT_OK;
}

/* Performs N steps of incremental rehashing. Returns 1 if there are still
 * keys to move from the old to the new hash table, otherwise 0 is returned.
 * Note that a rehashing step consists in moving a bucket (that may have more
 * thank one key as we use chaining) from the old to the new hash table. */
int dictRehash(dict *d, int n) {
		if (!dictIsRehashing(d)) return 0;

		while(n--) {
				dictEntry *de, *nextde;

				/* Check if we already rehashed the whole table... */
				if (d->ht[0].used == 0) {
						zfree(d->ht[0].table);
						d->ht[0] = d->ht[1];
						_dictReset(&d->ht[1]);
						d->rehashidx = -1;
						return 0;
				}

				/* Note that rehashidx can't overflow as we are sure there are more
				 * elements because ht[0].used != 0 */
				while(d->ht[0].table[d->rehashidx] == NULL) d->rehashidx++;
				de = d->ht[0].table[d->rehashidx];
				/* Move all the keys in this bucket from the old to the new hash HT */
				while(de) {
						unsigned int h;

						nextde = de->next;
						/* Get the index in the new hash table */
						h = dictHashKey(d, de->key) & d->ht[1].sizemask;
						de->next = d->ht[1].table[h];
						d->ht[1].table[h] = de;
						d->ht[0].used--;
						d->ht[1].used++;
						de = nextde;
				}
				d->ht[0].table[d->rehashidx] = NULL;
				d->rehashidx++;
		}
		return 1;
}


/* This function performs just a step of rehashing, and only if there are
 * no safe iterators bound to our hash table. When we have iterators in the
 * middle of a rehashing we can't mess with the two hash tables otherwise
 * some element can be missed or duplicated.
 *
 * This function is called by common lookup or update operations in the
 * dictionary so that the hash table automatically migrates from H1 to H2
 * while it is actively used. */
static void _dictRehashStep(dict *d) {
		if (d->iterators == 0) dictRehash(d,1);
}

/* Add an element to the target hash table */
int dictAdd(dict *d, void *key, void *val)
{
		int index;
		dictEntry *entry;
		dictht *ht;

		if (dictIsRehashing(d)) _dictRehashStep(d);

		/* Get the index of the new element, or -1 if
		 * the element already exists. */
		if ((index = _dictKeyIndex(d, key)) == -1)
				return DICT_ERR;

		/* Allocates the memory and stores key */
		ht = dictIsRehashing(d) ? &d->ht[1] : &d->ht[0];
		entry = zmalloc(sizeof(*entry));
		entry->next = ht->table[index];
		ht->table[index] = entry;
		ht->used++;

		/* Set the hash entry fields. */
		dictSetHashKey(d, entry, key);
		dictSetHashVal(d, entry, val);
		return DICT_OK;
}

dictEntry *dictFind(dict *d, const void *key)
{
		dictEntry *he;
		unsigned int h, idx, table;

		if (d->ht[0].size == 0) return NULL; /* We don't have a table at all */
		if (dictIsRehashing(d)) _dictRehashStep(d);
		h = dictHashKey(d, key);
		for (table = 0; table <= 1; table++) {
				idx = h & d->ht[table].sizemask;
				he = d->ht[table].table[idx];
				while(he) {
						if (dictCompareHashKeys(d, key, he->key))
								return he;
						he = he->next;
				}
				if (!dictIsRehashing(d)) return NULL;
		}
		return NULL;
}

void *dictFetchValue(dict *d, const void *key) {
		dictEntry *he;

		he = dictFind(d,key);
		return he ? dictGetEntryVal(he) : NULL;
}

dictIterator *dictGetIterator(dict *d)
{
    dictIterator *iter = zmalloc(sizeof(*iter));

    iter->d = d;
    iter->table = 0;
    iter->index = -1;
    iter->safe = 0;
    iter->entry = NULL;
    iter->nextEntry = NULL;
    return iter;
}

dictIterator *dictGetSafeIterator(dict *d) {
    dictIterator *i = dictGetIterator(d);

    i->safe = 1;
    return i;
}

dictEntry *dictNext(dictIterator *iter)
{
    while (1) {
        if (iter->entry == NULL) {
            dictht *ht = &iter->d->ht[iter->table];
            if (iter->safe && iter->index == -1 && iter->table == 0)
                iter->d->iterators++;
            iter->index++;
            if (iter->index >= (signed) ht->size) {
                if (dictIsRehashing(iter->d) && iter->table == 0) {
                    iter->table++;
                    iter->index = 0;
                    ht = &iter->d->ht[1];
                } else {
                    break;
                }
            }
            iter->entry = ht->table[iter->index];
        } else {
            iter->entry = iter->nextEntry;
        }
        if (iter->entry) {
            /* We need to save the 'next' here, the iterator user
             * may delete the entry we are returning. */
            iter->nextEntry = iter->entry->next;
            return iter->entry;
        }
    }
    return NULL;
}

void dictReleaseIterator(dictIterator *iter)
{
    if (iter->safe && !(iter->index == -1 && iter->table == 0))
        iter->d->iterators--;
    zfree(iter);
}


/* ------------------------- private functions ------------------------------ */

/* Expand the hash table if needed */
static int _dictExpandIfNeeded(dict *d)
{
		/* Incremental rehashing already in progress. Return. */
		if (dictIsRehashing(d)) return DICT_OK;

		/* If the hash table is empty expand it to the intial size. */
		if (d->ht[0].size == 0) return dictExpand(d, DICT_HT_INITIAL_SIZE);

		/* If we reached the 1:1 ratio, and we are allowed to resize the hash
		 * table (global setting) or we should avoid it but the ratio between
		 * elements/buckets is over the "safe" threshold, we resize doubling
		 * the number of buckets. */
		if (d->ht[0].used >= d->ht[0].size &&
						(dict_can_resize ||
						 d->ht[0].used/d->ht[0].size > dict_force_resize_ratio))
		{
				return dictExpand(d, ((d->ht[0].size > d->ht[0].used) ?
										d->ht[0].size : d->ht[0].used)*2);
		}
		return DICT_OK;
}

/* Our hash table capability is a power of two */
static unsigned long _dictNextPower(unsigned long size)
{
		unsigned long i = DICT_HT_INITIAL_SIZE;

		if (size >= LONG_MAX) return LONG_MAX;
		while(1) {
				if (i >= size)
						return i;
				i *= 2;
		}
}

/* Returns the index of a free slot that can be populated with
 * an hash entry for the given 'key'.
 * If the key already exists, -1 is returned.
 *
 * Note that if we are in the process of rehashing the hash table, the
 * index is always returned in the context of the second (new) hash table. */
static int _dictKeyIndex(dict *d, const void *key)
{
		unsigned int h, idx, table;
		dictEntry *he;

		/* Expand the hashtable if needed */
		if (_dictExpandIfNeeded(d) == DICT_ERR)
				return -1;
		/* Compute the key hash value */
		h = dictHashKey(d, key);
		for (table = 0; table <= 1; table++) {
				idx = h & d->ht[table].sizemask;
				/* Search if this slot does not already contain the given key */
				he = d->ht[table].table[idx];
				while(he) {
						if (dictCompareHashKeys(d, key, he->key))
								return -1;
						he = he->next;
				}
				if (!dictIsRehashing(d)) break;
		}
		return idx;
}

