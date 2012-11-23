#include "dsl_assoc.h"

#define DEF_SEG_UNUSED		100
#define INIT_ITEM_SIZE	    100000
#define INIT_BLOCK_SIZE 	128*1024*1024 //128M
#define hashsize(n) 		((UINT)1<<(n))
#define hashmask(n) 		(hashsize(n)-1)

static unsigned int 	malloc_size = 0;
#define m_plus(n) ((malloc_size)+=(n))
#define m_minus(n) ((malloc_size)-=(n))

//////////////////////////////////////
static unsigned int  *hashtable = 0;
static unsigned int  last_hv = 0;
static unsigned int  HASHPOWER = 0;
static unsigned int  HASHSIZE = 0;
//////////////////////////////////////
void *pdata;
static unsigned int curr_size;
static unsigned int total_size;
//////////////////////////////////////
INDEX *dindex;
static unsigned int curr_index;
static unsigned int total_index;
//////////////////////////////////////
static struct rb_root root;

const unsigned int confilt_n = 100;
int en_q;

///////////////////////////////////////////////////mem recycle start.
void rb_insert(struct rb_root *root, m_item *it)
{
		struct rb_node **p = &(root->rb_node);
		struct rb_node *parent = NULL;
		m_item *sec;

		while (*p) {
				parent = *p;
				sec = rb_entry(parent, m_item, node);

				if (it->size <= sec->size)
						p = &(*p)->rb_left;
				else if (it->size > sec->size) 
						p = &(*p)->rb_right;
		}

		rb_link_node( &it->node, parent, p );
		rb_insert_color( &it->node, root );

		return ;
}

m_item *rb_search(struct rb_root *root, unsigned int size,unsigned int memtype)
{
		struct rb_node *n = root->rb_node;
		while (n)
		{
				m_item *pm = rb_entry(n, m_item, node);
				if( pm->size > size )
						n = n->rb_left;
				else if( pm->size < size )
						n = n->rb_right;
				else
						return (pm->mem_type==memtype)?pm:0;
		}
		return 0;
}

void rb_lastn(struct rb_root *root, unsigned int lastn)
{
		int i;
		struct rb_node *n = rb_first(root);
		for( i = 0 ; i < lastn && n ; i++, n = rb_next(n) )
				;//print n
}
void rb_topn(struct rb_root *root, unsigned int topn)
{
		int i;
		struct rb_node *n = rb_last(root);
		for( i = 0 ; i < topn && n ; i++, n = rb_prev(n) )
				;//print n
}
///////////////////////////////////////////////////mem recycle end.

void enable_q(void)
{
		if( !en_q && (confilt_n < curr_index/HASHSIZE))
				en_q =1;
		return ;
}

//enlarge index size.
static void enlarge_index()
{
		unsigned int nsize = total_index + INIT_ITEM_SIZE;
		size_t new_size = nsize * sizeof(INDEX);
		INDEX *new_index = realloc(dindex,new_size);
		if(!new_index)
		{
				fprintf(stderr, "enlarge_index failed %u\r\n", new_size);
				exit(-1);
		}
		dindex = new_index;
		total_index = nsize;
		fprintf(stdout, "enlarge_index %u\r\n", new_size);
		m_plus(INIT_ITEM_SIZE*sizeof(INDEX));
}

//enlarge datablock size.
static void enlarge_datablock()
{
		unsigned int new_size = total_size + INIT_BLOCK_SIZE;
		char* new_p = realloc(pdata , new_size);
		if(!new_p)
		{
				fprintf(stderr, "enlarge_datablock failed %u\r\n", new_size);
				exit(-1);
		}
		pdata = new_p;
		total_size = new_size;
		fprintf(stdout, "enlarge_datablock  %u\r\n", new_size);
		m_plus(INIT_BLOCK_SIZE);
}

//assoc init.
void assoc_init(unsigned int hashpower) 
{
		if(malloc_size)
				return;
		HASHPOWER = hashpower;
		/////////////////////////////////////////////////////////////////////
		HASHSIZE = hashsize(HASHPOWER);
		unsigned int hash_size = HASHSIZE * sizeof(unsigned int);
		hashtable = (unsigned int *)malloc(hash_size);
		if ( !hashtable) {
				fprintf(stderr, "Failed to init hashtable.\n");
				exit(-1);
		}
		m_plus(hash_size);
		memset(hashtable, 0, hash_size);
		/////////////////////////////////////////////////////////////////////
		unsigned int index_size = sizeof(INDEX) * INIT_ITEM_SIZE;
		dindex = malloc(index_size);
		if(!dindex)
		{
				fprintf(stderr,"dindex malloc failed\r\n");
				exit(-1);
		}
		memset(dindex, 0, INIT_ITEM_SIZE * sizeof(INDEX));
		curr_index = 1;
		total_index = INIT_ITEM_SIZE;
		m_plus(index_size);
		/////////////////////////////////////////////////////////////////////
		unsigned int block_size = INIT_BLOCK_SIZE;
		pdata = (void*)malloc(INIT_BLOCK_SIZE);	
		if (!pdata) 
		{
				fprintf(stderr, "data malloc failed\r\n");
				exit(-1);
		}
		memset(pdata, 0, INIT_BLOCK_SIZE);
		curr_size = DEF_SEG_UNUSED;
		total_size = INIT_BLOCK_SIZE;
		m_plus(block_size);
		/////////////////////////////////////////////////////////////////////
		return ;
}

//assoc insert.
void assoc_insert( INDEX *a)
{
		unsigned int hv;
		unsigned int hv0,hv1;
		if(a->key_type == KEY_TYPE_NUMBER)
				hv = dictIntHashFunction( *(unsigned int*)(pdata + a->key_offset) ) & hashmask(HASHPOWER);
		else
		{
				hv = dictGenHashFunction((unsigned char *)(pdata + a->key_offset), strlen(pdata + a->key_offset)) & hashmask(HASHPOWER);
				if(en_q){
						hv0 = ELFhash((char *)(pdata + a->key_offset));
						hv1 = lh_strhash((void *)(pdata + a->key_offset));
						a->hv0 = hv0; 
						a->hv1 = hv1;
				}
		}
		a->h_next = hashtable[hv]; 
		hashtable[hv] = a - dindex;
}

//find key under number model.
static INDEX *assoc_number_find(void *key)
{
		unsigned int ikey = *((unsigned int *)key);
		unsigned int hv = dictIntHashFunction(ikey) & hashmask(HASHPOWER);
		unsigned int idx = hashtable[hv];
		unsigned int *k;
		last_hv = idx;
		while (idx) {
				k = (unsigned int*)(pdata + dindex[idx].key_offset);
				if(*k == ikey)
						return &dindex[idx];
				idx = dindex[idx].h_next;
		}
		return 0;
}

//find key under string model.
static INDEX *assoc_string_find(char *key)
{
		if(!key) return 0;
		unsigned int len;
		len = strlen(key);
		unsigned int hv = dictGenHashFunction((unsigned char *)key, len) & hashmask(HASHPOWER);
		unsigned int hv0,hv1;
		if(en_q){
				hv0 = ELFhash((char *)key);
				hv1 = lh_strhash((void *)key);
		}
		unsigned int idx = hashtable[hv];
		last_hv = idx;
		while(idx)
		{
				if(en_q && hv0 == dindex[idx].hv0 && hv1 == dindex[idx].hv1 && !memcmp(pdata + dindex[idx].key_offset,key,len))
						return &dindex[idx];
				if(!memcmp(pdata + dindex[idx].key_offset,key,len))
						return &dindex[idx];
				idx = dindex[idx].h_next;
		}
		return 0;
}

//load data by data filename & index filename.
int load_data(char *fdata, char* findex, char* frecy)
{
		FILE *fd = fopen(fdata, "r");
		if(!fd)
				return 0;
		//data file.
		off_t fsize = get_file_size(fdata);
		curr_size = total_size = fsize;
		pdata = malloc(fsize);
		if(!pdata)
		{
				fprintf(stderr, "data_block malloc failed\r\n");
				exit(-1);
		}
		if(fsize != fread(pdata ,1,fsize,fd))
		{
				fprintf(stderr, "data_block fread failed\r\n");
				exit(-1);
		}
		m_plus(fsize);
		fclose(fd);

		//index file.
		fd = fopen(findex, "r");
		fsize = get_file_size(findex);
		curr_index = total_index = fsize/sizeof(INDEX);
		dindex = malloc(curr_index * sizeof(INDEX));
		if(!dindex)
		{
				fprintf(stderr, "data index malloc failed\r\n");
				exit(-1);
		}
		if(curr_index != fread(dindex,sizeof(INDEX),curr_index,fd))
		{
				fprintf(stderr, "data index fread failed\r\n");
				exit(-1);
		}
		m_plus(fsize);

		int i;
		for(i = 1 ; i < curr_index ; i++ )
		{
				assoc_insert(&dindex[i]);
		}	
		fclose(fd);

		//recycle file.
		unsigned int m_item_n;
		fd = fopen(frecy, "r");
		fsize = get_file_size(frecy);
		m_item_n = fsize/sizeof(m_item);
		m_plus(fsize);
		for(i = 0 ; i < m_item_n ; i++ )
		{
				m_item *pit = (m_item*)malloc(sizeof(m_item));
				if(1!=fread(pit,sizeof(m_item),1,fd))
				{
						fprintf(stderr,"read recycle file failed!\r\n");
						exit(-1);
				}
				rb_insert(&root,pit);
		}

		return 1;
}

//dump data to datafile & indexfile.
int dump_data(char *fdata, char* findex, char* frecy)
{
		//data.
		if(!fdata) return 0;
		FILE *fd = fopen(fdata, "w");
		if (!fd) {
				fprintf(stderr, "open %s failed <<%s>>\r\n", fdata, strerror(errno));
		}
		if (curr_size != fwrite(pdata, 1, curr_size, fd)) {
				fprintf(stderr, "write %s failed when dump data <<%s>>\r\n", fdata, strerror(errno));
		}
		fclose(fd);

		//index.
		if(!findex)	return 0;
		fd = fopen(findex, "w");
		if (!fd) {
				fprintf(stderr, "open %s failed <<%s>>\r\n", findex, strerror(errno));
		}
		if (curr_index != fwrite(dindex, sizeof(INDEX), curr_index, fd)) {
				fprintf(stderr, "write %s failed when dump data <<%s>>\r\n", findex, strerror(errno));
		}
		fclose(fd);

		//recycle.
		if(!frecy)	return 0;
		struct rb_node *n;
		fd = fopen(frecy, "w");
		if (!fd) {
				fprintf(stderr, "open %s failed <<%s>>\r\n", frecy, strerror(errno));
		}
		for (n = rb_last(&root); n; n = rb_prev(n))
		{
				m_item *pm = rb_entry(n, m_item, node);
				if( 1!=(fwrite(pm, sizeof(m_item), 1, fd)))
				{
						fprintf(stderr,"write recycle file failed!\r\n");
						return 0;
				}
		}
		return 1;
}

//query key and delete key & val.
void *assoc_del(void *key, unsigned int key_sz, unsigned int key_type)
{
		if(!key || !key_sz) return 0;
		INDEX *a; 
		a = (key_type==KEY_TYPE_NUMBER)?assoc_number_find(key):assoc_string_find(key);

		if(a)//found
		{
				//recover index.
				m_item *it = (m_item*)malloc(sizeof(m_item));
				it->offset = (a-dindex)*sizeof(INDEX) ; 
				it->size = sizeof(INDEX);
				it->mem_type = MEM_BLOCK_INDEX;
				rb_insert(&root,it);
				m_plus(sizeof(m_item));
				//recover key & val.
				if(a->key_size)
				{
						it = (m_item*)malloc(sizeof(m_item));
						it->offset = a->key_offset ; 
						it->size = a->key_size+a->val_size;
						if( it->size % 4 != 0 )
								it->size = it->size + 4 - it->size%4 ;
						it->mem_type = MEM_BLOCK_DATA;
						rb_insert(&root,it);
						m_plus(sizeof(m_item));
				}
				//rebuild hash.
				int idx = last_hv;
				if(idx == a-dindex)
						hashtable[idx] = a->h_next;
				else
				{
						while(idx)
						{
								if(dindex[idx].h_next == a-dindex)	
										dindex[idx].h_next = a->h_next;
								idx = dindex[idx].h_next; 
						}
				}
				memset(a,0,sizeof(INDEX));
		}
		return 0;
}

int assoc_find(void *key,unsigned int key_sz, void *val,unsigned int key_type)
{
		if(!key || !key_sz) return 0;
		INDEX *a; 
		a = (key_type==KEY_TYPE_NUMBER)?assoc_number_find(key):assoc_string_find(key);

		if(!a) return 0;

		memcpy( val ,pdata + a->val_offset, a->val_size);
		return a->val_size;

}

//query key and insert key & val if necessary. 
void *assoc_query(void *key, unsigned int key_sz, void *val, unsigned int val_sz, unsigned int key_type)
{
		if(!key || !key_sz) return 0;
		INDEX *a; 
		a = (key_type==KEY_TYPE_NUMBER)?assoc_number_find(key):assoc_string_find(key);

		if(a /* && val*/)
		{
				//need to reconsider.	memcpy( pdata + a->val_offset, val, val_sz);
				if(a->val_size == val_sz)
						memcpy( pdata + a->val_offset, val, val_sz);
				else
				{
						assoc_del(key,key_sz,key_type);
						assoc_query(key,key_sz,val,val_sz,key_type);	
				}
				return 0;
		}
		else
		{
				//if(!forceinsert)//not insert & return.
				//		return 0;
				//else
				{
						unsigned int data_offset,index_offset;
						unsigned int idx;
						unsigned int size;
						m_item *mit;
						size = aline(key_sz+val_sz);
						//data.
						if((mit=rb_search(&root,size,MEM_BLOCK_DATA)))
						{
								data_offset = mit->offset;
								rb_erase(&mit->node,&root);
								free(mit);
								m_minus(sizeof(m_item));
						}
						else
						{
								if(curr_size + key_sz + val_sz + 1 >= total_size)
										enlarge_datablock();
								data_offset = curr_size;
								curr_size +=size;
								curr_size = aline(curr_size);
						}
						memcpy( pdata + data_offset, key, key_sz);
						memcpy( pdata + data_offset+key_sz, val, val_sz);
						//index.
						if((mit=rb_search(&root,sizeof(INDEX),MEM_BLOCK_INDEX)))
						{
								index_offset = mit->offset;
								idx = index_offset/sizeof(INDEX);
								rb_erase(&mit->node,&root);
								free(mit);
								m_minus(sizeof(m_item));
						}
						else
						{
								if(curr_index +1 >= total_index)
										enlarge_index();
								idx = curr_index++;	
						}
						a = &dindex[idx];
						a->key_type 	= key_type;
						a->key_offset 	= data_offset;
						a->key_size 	= key_sz;
						a->val_offset 	= data_offset+key_sz;
						a->val_size 	= val_sz;

						enable_q();

						assoc_insert(a);	
				}
		}
		return 0;
}


