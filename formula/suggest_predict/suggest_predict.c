#include "suggest_predict.h"
#include <math.h>
#include "lib/cJSON.h"

#include "lib/hiredis.h"



#define BUFLEN			(64*BUFSIZ)
#define NOTUSED(x)		(void)x

//#define CLS_IP			"10.69.3.72"
#define CLS_IP			"127.0.0.1"
#define CLS_PORT 		36379

#define JS_GOItem(x,y) 	cJSON_GetObjectItem(x,y)
#define JS_GAS(x) 		cJSON_GetArraySize(x)
#define JS_GAI(x,y)		cJSON_GetArrayItem(x,y)
#define JS_INT(x) 		((x)->type==cJSON_String?atoi((x)->valuestring):((x)->type==cJSON_Number?(x)->valueint:0))
#define JS_STR(x) 		((x)->type==cJSON_String?(x)->valuestring:0)
#define JS_DEL(x) 	do {\
		cJSON_Delete(x);\
}while(0)

int dsl = 0;
unsigned int word_all;
unsigned int blog_all;
typedef struct _cls_{
		unsigned int all;
		unsigned int n_blog;
}CLS;

CLS g_cls[23];

/*redis*/
redisContext *redis_cls;	/*36379*/
redisReply *reply;

void init_vars(void){

		/*init redis_cls-server*/
		struct timeval timeout = { 1, 500 }; // 1.5 seconds
		redis_cls = redisConnectWithTimeout((char*)CLS_IP, CLS_PORT, timeout);
		if (redis_cls->err) {
				printf("Connection error: %s Port:%d\n", redis_cls->errstr,CLS_PORT);
				exit(1);
		}

		int i;
		word_all = 0;
		for(i = 1;i<sizeof(g_cls)/sizeof(g_cls[0]);i++){

				reply = redisCommand(redis_cls,"SELECT %d",i);
				freeReplyObject(reply);

				reply = redisCommand(redis_cls,"GET all");
				g_cls[i].all = atoi(reply->str); 
				word_all += g_cls[i].all;
				freeReplyObject(reply);

				reply = redisCommand(redis_cls,"GET n_blog");
				g_cls[i].n_blog = atoi(reply->str);
				blog_all += g_cls[i].n_blog;
				freeReplyObject(reply);

		}
		return ;
}

double cal(int cls, cJSON* p){

		if(cls < 1 || cls > 128) return 0;

		reply = redisCommand(redis_cls,"SELECT %d",cls);
		freeReplyObject(reply);

		double PA = 0.000001;
		double PT = 1;
		double P = 0;
		int i,size = JS_GAS(p);
		cJSON *count,*word,*tfidf,*k;

		PA =(double)g_cls[cls].n_blog/blog_all;

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
				reply = redisCommand(redis_cls,"ZSCORE keyword %s",word->valuestring);
				
				double PTA;
				if(reply->type == REDIS_REPLY_STRING){
						PTA = (double)atoi(reply->str)/g_cls[cls].all;
						freeReplyObject(reply);
				}
				else{
						PTA = 0;//(double)1/g_cls[cls].all;
						freeReplyObject(reply);
				}
				PT = (double)JS_INT(count)/wc;
				P += PTA*PA*tfidf->valuedouble/PT;

		}	

		return P;
}

int predict(int cls ,cJSON* blogid, cJSON* p){

		int i,m_cls;
		double score = 0,max_s = 0;

		for(i = 1;i<sizeof(g_cls)/sizeof(g_cls[0]);i++){

				score = cal(i,p);
				if(score > max_s){
						max_s = score;
						m_cls = i;
				}

		}
		return m_cls;
}

/*
   return 0: failed.
   return 1: success.
   */
int gsh_formula_suggest_predict_init(void *arg,void *ret){

		init_vars();
		return 1;
}

int gsh_formula_suggest_predict_run(void *arg,void *ret){

		
		cJSON *root,*cls,*blogid,*keyword;
		//if(!(root = cJSON_Parse(p))) continue;
		root = (cJSON*)arg;

		cls		= JS_GOItem(root,"classid");
		keyword	= JS_GOItem(root,"keyWords");
		blogid	= JS_GOItem(root,"blogid");

		if(!(cls && blogid && keyword)) return 0;

		int pre = predict(JS_INT(cls),blogid,keyword);
	
		if(!pre) return 0;

		int len = sprintf(ret,"%d",pre);
		
		if( len >= FORMULA_BUFLEN)
				return 0;
		return 1;
}

