#include "suggest_predict_sub.h"
#include <math.h>
#include "lib/cJSON.h"

#include "lib/hiredis.h"
#include "lib/sds.h"
#include "lib/dict.h"
#include "lib/zmalloc.h"


#define BUFLEN			(64*BUFSIZ)
#define NOTUSED(x)		(void)x

#define CLS_IP			"10.69.3.72"
#define CLS_PORT 		36379

#define JS_GOItem(x,y) 	cJSON_GetObjectItem(x,y)
#define JS_GAS(x) 		cJSON_GetArraySize(x)
#define JS_GAI(x,y)		cJSON_GetArrayItem(x,y)
#define JS_INT(x) 		((x)->type==cJSON_String?atoi((x)->valuestring):((x)->type==cJSON_Number?(x)->valueint:0))
#define JS_STR(x) 		((x)->type==cJSON_String?(x)->valuestring:0)
#define JS_DEL(x) 	do {\
		cJSON_Delete(x);\
}while(0)

unsigned int word_all[23];
unsigned int blog_all[23];
typedef struct _cls_{
		unsigned int all;
		unsigned int n_blog;
}CLS;

CLS *g_cls[23];
//dict *cls_d[23];

/*
 * scope.
 */
int g_scope[23] = {0,/*unused.*/
		0,/*娱乐*/
		0,/*文化*/
		0,/*体育*/
		0,/*情感*/
		26,/*it*/
		0,/*财经*/
		0,/*股票*/
		0,/*校园*/
		0,/*教育*/
		0,/*时尚*/
		0,/*休闲*/
		0,/*美食*/
		0,/*旅游*/
		0,/*汽车*/
		0,/*军事*/
		0,/*房产*/
		0,/*家居*/
		0,/*育儿*/
		0,/*星座*/
		0,/*健康*/
		0,/*游戏*/
		0/*杂谈*/
};

/*redis*/
redisContext *redis_cls;	/*36379*/
redisReply *reply;

sds g_zero;

int dictSdsKeyCompare(void *privdata, const void *key1,
				const void *key2)
{
		int l1,l2;
		DICT_NOTUSED(privdata);

		l1 = sdslen((sds)key1);
		l2 = sdslen((sds)key2);
		if (l1 != l2) return 0;
		return memcmp(key1, key2, l1) == 0;
}
unsigned int dictSdsHash(const void *key) {
		return dictGenHashFunction((unsigned char*)key, sdslen((char*)key));
}

dictType keyptrDictType = {
		dictSdsHash,               /* hash function */
		NULL,                      /* key dup */
		NULL,                      /* val dup */
		dictSdsKeyCompare,         /* key compare */
		NULL,                      /* key destructor */
		NULL                       /* val destructor */
};

void init_vars(void){

		int i;
		for(i = 1;i<23;i++){
				g_cls[i] = (CLS*) malloc(sizeof(CLS)*g_scope[i]);
		}


		g_zero = sdsnew("0");
		/*
		   dict *d = dictCreate(&keyptrDictType,NULL);
		   sds key = sdsnew("hahaha");
		   sds key1 = sdsnew("222");
		   sds val = sdsnew("11111111");
		   int retval = dictAdd(d,key,val);
		   dictEntry *de = dictFind(d,key1);
		   */

		/*init redis_cls-server*/
		struct timeval timeout = { 1, 500 }; // 1.5 seconds
		redis_cls = redisConnectWithTimeout((char*)CLS_IP, CLS_PORT, timeout);
		if (redis_cls->err) {
				printf("Connection error: %s Port:%d\n", redis_cls->errstr,CLS_PORT);
				exit(1);
		}

		int j;
		for(i = 1;i<23;i++){

				word_all[i] = 0;
				blog_all[i] = 0;
				for(j = 1;j<= g_scope[i];j++){
						reply = redisCommand(redis_cls,"SELECT %d",i*10000+j);
						freeReplyObject(reply);

						reply = redisCommand(redis_cls,"GET all");
						if(reply->type == REDIS_REPLY_STRING ){
								g_cls[i][j].all = atoi(reply->str); 
								word_all[i] += g_cls[i][j].all;
						}
						freeReplyObject(reply);

						reply = redisCommand(redis_cls,"GET n_blog");
						if(reply->type == REDIS_REPLY_STRING ){
								g_cls[i][j].n_blog = atoi(reply->str);
								blog_all[i] += g_cls[i][j].n_blog;
						}
						freeReplyObject(reply);
						//cls_d[i] = dictCreate(&keyptrDictType,NULL);
				}

		}
		return ;
}

/*
   static int cache_hit(int cls, char* keyword ,double * score){

   sds key = sdsnew(keyword);
   dictEntry *de = dictFind(cls_d[cls],key);
   sdsfree(key);
   if(!de) return 0;
   char *ped = de->val+strlen(de->val);
 *score = strtod(de->val,&ped);
 return 1;
 }

 static void cache_store(int cls, char *keyword, char* score){

 sds key = sdsnew(keyword);
 if(score==NULL){
 dictAdd(cls_d[cls],key,g_zero);	
 return ;
 }
 sds val = sdsnew(score);
 dictAdd(cls_d[cls],key,val);
 return ;
 }
 */

double cal(int bcls, int scls, cJSON* p){

		//if(cls < 1 || cls > 230000) return 0;

		reply = redisCommand(redis_cls,"SELECT %d",bcls*10000+scls);
		freeReplyObject(reply);

		double PA = 0.000001;
		double PT = 1;
		double P = 0;
		int i,size = JS_GAS(p);
		cJSON *count,*word,*tfidf,*k;

		PA =(double)g_cls[bcls][scls].n_blog/blog_all[bcls];

		/*calculate all count in this blogid.*/
		int wc=0;
		for(i = 0 ; i < size ; i++ ){

				k = JS_GAI(p,i);
				count = JS_GOItem(k,"count");
				word = JS_GOItem(k,"word");
				wc +=JS_INT(count);

		}

		/*calculate every word's score.*/	
		for(i = 0 ; i < size ; i++ ){

				k = JS_GAI(p,i);
				count = JS_GOItem(k,"count");
				word = JS_GOItem(k,"word");
				tfidf = JS_GOItem(k,"tfidf");

				double PTA,score;

				if(0 /*cache_hit(cls, word->valuestring,&score)*/){
						PTA = score/g_cls[bcls][scls].all;
				}else{ 

						reply = redisCommand(redis_cls,"ZSCORE keyword %s",word->valuestring);

						if(reply->type == REDIS_REPLY_STRING){
								PTA = (double)atoi(reply->str)/g_cls[bcls][scls].all;
						}
						else{
								PTA = 0;//(double)1/g_cls[cls].all;
						}

						/*cache store*/
						//cache_store(cls, word->valuestring, reply->str);

						/*free*/
						freeReplyObject(reply);
				}


				PT = (double)JS_INT(count)/wc;
				P += PTA*PA*tfidf->valuedouble/PT;
		}	

		return P;
}

int predict(int bcls ,cJSON* blogid, cJSON* p,char *ret){

		int i;
		double score;


		struct _top3_ {
				int cls;
				double score;
		} top3[3];

		memset(top3,0,sizeof(struct _top3_)*3);

		for(i = 1 ;i <= g_scope[bcls]; i++){

				score = cal(bcls,i,p);

				/*if(i==18 || i==22)
				  score = (double)0.45*score;

				  if(i==6 || i==7)
				  score = (double)0.7*score;*/

				int k,j;
				for(k=0;k<3;k++){

						if(score>=top3[k].score){

								for(j=1;j>=k;j--){
										top3[j+1].score = top3[j].score;
										top3[j+1].cls = top3[j].cls;
								}

								top3[k].score = score;
								top3[k].cls = i;
								break;
						}
				}


		}

		int len = sprintf(ret,"%d:%.15f %d:%.15f %d:%.15f",top3[0].cls,top3[0].score,\
						top3[1].cls,top3[1].score,\
						top3[2].cls,top3[2].score);
		return len;
}

/*
   return 0: failed.
   return 1: success.
   */
int gsh_formula_suggest_predict_sub_init(void *arg,void *ret){

		init_vars();
		return 1;
}

int gsh_formula_suggest_predict_sub_run(void *arg,void *ret){


		cJSON *root,*bcls,*blogid,*keyword;
		root = (cJSON*)arg;

		bcls	= JS_GOItem(root,"classid");
		keyword	= JS_GOItem(root,"keyWords");
		blogid	= JS_GOItem(root,"blogid");

		if(!(bcls && blogid && keyword)) return 0;

		int len = predict(JS_INT(bcls),blogid,keyword,ret);

		if( len >= FORMULA_BUFLEN)
				return 0;
		return 1;
}

