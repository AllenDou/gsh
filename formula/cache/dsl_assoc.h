#ifndef __DSL_ASSOC__
#define __DSL_ASSOC__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "common.h"
#include "hash.h"
#include "rbtree.h"

#define KEY_TYPE_NUMBER		1
#define KEY_TYPE_STRING		2

#define MEM_BLOCK_INDEX		4
#define MEM_BLOCK_DATA		8


#pragma pack(4)
typedef unsigned int UINT;
typedef struct _index_{
		unsigned int key_type;	//KEY_TYPE_NUMMBER,KEY_TYPE_STRING
		unsigned int key_offset;
		unsigned int key_size;
		unsigned int val_offset;
		unsigned int val_size;
		unsigned int hv0;
		unsigned int hv1;

		unsigned int h_next;
}INDEX;

typedef struct _m_item_ {
		unsigned int size;
		unsigned int offset;
		unsigned int mem_type;
		struct rb_node node;
} m_item;
#pragma pack()

void assoc_init(unsigned int); 
int  assoc_find(void *key,unsigned int key_sz, void *val,unsigned int key_type);
void *assoc_query(void *key,unsigned int key_sz, void *val,	unsigned int val_sz, unsigned int key_type);
void *assoc_del(void *key, unsigned int key_sz, unsigned int key_type);
int load_data(char *fdata, char* findex, char* frecy);
int dump_data(char *fdata, char* findex, char* frecy);

#endif
