#include "blog_tag.h"
#include <math.h>
#include "lib/cJSON.h"

#include "lib/hiredis.h"
#include "lib/sds.h"
#include "lib/dict.h"
#include "lib/zmalloc.h"


#define BUFLEN          (64*BUFSIZ)
#define NOTUSED(x)      (void)x

#define CLS_IP          "218.30.115.72"
#define CLS_PORT        36382

#define NTOP			5

#define JS_GOItem(x,y)  cJSON_GetObjectItem(x,y)
#define JS_GAS(x)       cJSON_GetArraySize(x)
#define JS_GAI(x,y)     cJSON_GetArrayItem(x,y)
#define JS_INT(x)       ((x)->type==cJSON_String?atoi((x)->valuestring):((x)->type==cJSON_Number?(x)->valueint:0))
#define JS_STR(x)       ((x)->type==cJSON_String?(x)->valuestring:0)
#define JS_DEL(x)   do {\
		cJSON_Delete(x);\
}while(0)



char *def_tag = "杂谈";

/*redis*/
redisContext *redis_cls;    /*36382*/
redisReply *reply;


typedef struct _it_ {
		char *p;
		int count;
		struct _it_ *next;
} item;

item *head;

struct _top5_ {
		char *p;
		int count;
} ntop[NTOP];

void init_vars (void) {


		/*init redis_cls-server*/
		struct timeval timeout = { 1, 500 }; // 1.5 seconds
		redis_cls = redisConnectWithTimeout((char*)CLS_IP, CLS_PORT, timeout);
		if (redis_cls->err) {
				printf("Connection error: %s Port:%d\n", redis_cls->errstr, CLS_PORT);
				exit(1);
		}
		head = NULL;
}

/*
   return 0: failed.
   return 1: success.
   */
int gsh_formula_blog_tag_init (void *arg, void *ret) {

		init_vars();
		return 1;
}

int topn (void) {


		if (!head) return 0;

		item *iter = head;
		while (iter) {

				int k,j;
				for (k = 0; k < NTOP; k++) {

						if (iter->count >= ntop[k].count) {

								for (j = 1; j >= k; j--) {
										ntop[j+1].count = ntop[j].count;
										ntop[j+1].p = ntop[j].p;
								}

								ntop[k].count = iter->count;
								ntop[k].p = iter->p;
								break;
						}
				}
				item *tmp = iter;
				iter = iter->next;
				free(tmp);
		}
		head = NULL;

		return 1;
}

void insert (char *str, int count) {

		/*empty list.*/
		if (!head) {
				item * it = (item*)malloc(sizeof(item));
				it->p = str;
				it->count = count;
				it->next = NULL;
				head = it;
				return ;
		}

		item *iter = head;
		item *prev;
		do {
				/*founded.*/
				if (!strcmp(iter->p, str)) {
						iter->count += count;
						return ;
				}
				prev = iter;
				iter = iter->next;
		} while(iter);

		/*not founded.*/

		item * it = (item*)malloc(sizeof(item));
		it->p = str;
		it->count = count;
		it->next = NULL;


		prev->next = it;

		return ;
}

int predict (int cls, cJSON* blogid, cJSON *keyword, void *ret) {

		memset(ntop, 0, sizeof(ntop[0])*NTOP);
		
		int i,kw_sz,len;
		cJSON *k,*word,*count;	
		kw_sz = JS_GAS(keyword);

		if (kw_sz < 1 || cls < 1 || cls > 22) goto err;

		reply = redisCommand(redis_cls, "SELECT %d", cls);
		freeReplyObject(reply);

		redisReply **rp = (redisReply**)malloc(sizeof(redisReply*) * kw_sz);

		for (i = 0; i < kw_sz; i++) {

				k = JS_GAI(keyword, i);
				word = JS_GOItem(k, "word");
				count = JS_GOItem(k, "count");

				rp[i] = redisCommand(redis_cls, "ZREVRANGE %s 0 5 withscores", word->valuestring);
				if (rp[i]->type != REDIS_REPLY_ARRAY)
						goto err;
				else {
						int j;
						for (j=0; j < rp[i]->elements/2; j++) {
								char*p = rp[i]->element[2*j]->str;
								int sc = atoi(rp[i]->element[2*j+1]->str);
								insert(p, sc/**JS_INT(count)*/);	
						}
				}
		}


		if (!topn()) goto err;
		len = sprintf(ret,"%s:%d %s:%d %s:%d %s:%d %s:%d", ntop[0].p, ntop[0].count, ntop[1].p, ntop[1].count, ntop[2].p, ntop[2].count, ntop[3].p, ntop[3].count, ntop[4].p, ntop[4].count);
		for (i = 0; i < kw_sz; i++) 
			freeReplyObject(rp[i]);
		if(rp) free(rp);
		return len;

err:
		if(rp) free(rp);
		len = sprintf(ret,"%s", def_tag);
		return len;
}

int gsh_formula_blog_tag_run (void *arg, void *ret) {

		cJSON *root,*blogid,*keyword,*cls;

		root = (cJSON*)arg; 

		blogid = JS_GOItem(root, "blogid");
		keyword = JS_GOItem(root, "keyWords");
		cls = JS_GOItem(root, "classid");

		int len = predict(JS_INT(cls), blogid, keyword, ret);

		if(len >= FORMULA_BUFLEN)
				return 0;
		return 1;
}

