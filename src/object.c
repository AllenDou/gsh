#include "gsh.h"
#include <pthread.h>
#include <math.h>

robj *createObject(int type, void *ptr) {
		robj *o = zmalloc(sizeof(*o));
		o->type = type;
		o->encoding = REDIS_ENCODING_RAW;
		o->ptr = ptr;
		o->refcount = 1;

		/* Set the LRU to the current lruclock (minutes resolution).
		 * We do this regardless of the fact VM is active as LRU is also
		 * used for the maxmemory directive when Redis is used as cache.
		 *
		 * Note that this code may run in the context of an I/O thread
		 * and accessing server.lruclock in theory is an error
		 * (no locks). But in practice this is safe, and even if we read
		 * garbage Redis will not fail. */
		o->lru = server.lruclock;
		/* The following is only needed if VM is active, but since the conditional
		 * is probably more costly than initializing the field it's better to
		 * have every field properly initialized anyway. */
		return o;
}

robj *createStringObject(char *ptr, size_t len) {
		return createObject(REDIS_STRING,sdsnewlen(ptr,len));
}


robj *dupStringObject(robj *o) {
		redisAssert(o->encoding == REDIS_ENCODING_RAW);
		return createStringObject(o->ptr,sdslen(o->ptr));
}


void freeStringObject(robj *o) {
		if (o->encoding == REDIS_ENCODING_RAW) {
				sdsfree(o->ptr);
		}
}

void incrRefCount(robj *o) {
		o->refcount++;
}

void decrRefCount(void *obj) {
		robj *o = obj;

		if (o->refcount <= 0) redisPanic("decrRefCount against refcount <= 0");
		/* Object is in memory, or in the process of being swapped out.
		 *
		 * If the object is being swapped out, abort the operation on
		 * decrRefCount even if the refcount does not drop to 0: the object
		 * is referenced at least two times, as value of the key AND as
		 * job->val in the iojob. So if we don't invalidate the iojob, when it is
		 * done but the relevant key was removed in the meantime, the
		 * complete jobs handler will not find the key about the job and the
		 * assert will fail. */
		if (o->refcount == 1) {
				switch(o->type) {
						case REDIS_STRING: freeStringObject(o); break;
						default: redisPanic("Unknown object type"); break;
				}
				zfree(o);
		} else {
				o->refcount--;
		}
}


/* Get a decoded version of an encoded object (returned as a new object).
 * If the object is already raw-encoded just increment the ref count. */
robj *getDecodedObject(robj *o) {
		robj *dec;

		if (o->encoding == REDIS_ENCODING_RAW) {
				incrRefCount(o);
				return o;
		}
		if (o->type == REDIS_STRING && o->encoding == REDIS_ENCODING_INT) {
				char buf[32];

				ll2string(buf,32,(long)o->ptr);
				dec = createStringObject(buf,strlen(buf));
				return dec;
		} else {
				redisPanic("Unknown encoding type");
		}
}

