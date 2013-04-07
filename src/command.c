#include "gsh.h"
#include "common/cJSON.h"
#include "common/formula.h"
#include <sys/stat.h>
#include "../formula/iask_wordseg/iask_wordseg.h"

#define JS_GOItem(x,y) cJSON_GetObjectItem(x,y) 

typedef int formuaProc(void*,void*);
typedef struct kwit {
		char *kw;
		int  type;
}KWIT;

KWIT kw_list[] = {
		{"time",		cJSON_Number},
		{"ip",			cJSON_String},
		{"script",		cJSON_String},
		{"programmer",	cJSON_String},
		{"formula",		cJSON_String},
		{"data",		cJSON_Object}
};

/*function: formula_init, formula_run*/
typedef struct fmitem {
		formuaProc *init;
		formuaProc *run;
}FMITEM;
FMITEM *fmlist;

/*formula buf*/
void *fm_buf;

cJSON *root,*data,*formula;

void* loadfm(char *fm_name)
{
		gsh_formula_iask_wordseg_init(0,0);
		char path[BUFSIZ];
		sprintf(path,"./lib/lib%s.so",fm_name);

		FMITEM *it = zmalloc(sizeof(FMITEM));

		int ret;
		void *handle = dlopen(path,/*RTLD_LAZY*/RTLD_NOW);
		if(!handle) 
		{
				fprintf(stderr, "%s\n", dlerror());
				goto err;
		}

		/*export formula function.*/
		it->init = zmalloc(sizeof(formuaProc*));
		it->run = zmalloc(sizeof(formuaProc*));

		/*export formula_run function*/
		sprintf(path,"gsh_formula_%s_run",fm_name);
		it->run = dlsym(handle,path);
		if(!it->run)
		{
				fprintf(stderr,"load <<%s>> function failed.\r\n",path);
				goto err;
		}

		/*export formula_init function*/
		sprintf(path,"gsh_formula_%s_init",fm_name);
		it->init = dlsym(handle,path);
		if(!it->init)
		{
				fprintf(stderr,"load gsh_formula_%s_init failed.\r\n",path);
				goto err;
		}

		/*formula init.*/
		ret = it->init(0,0);
		if(!ret)
		{
				fprintf(stderr,"<<gsh_formula_%s_init>> failed.\r\n",path);
				goto err;
		}

		return it;
err:
		return 0;
}

void gsh_init(void)
{
		fm_buf = zmalloc(FORMULA_BUFLEN);
		return ;
}

void grunCommand(redisClient *c) 
{
		char *cmd = c->argv[2]->ptr;
		root = cJSON_Parse(cmd);
		if(!root)
		{
				redisLog(REDIS_WARNING, "Fatal error, command format's not JSON: %s",cmd);
				goto err;
		}
		int i;
		cJSON *cj;
		for(i = 0 ; i < sizeof(kw_list)/sizeof(struct kwit); i++)
		{
				cj = JS_GOItem(root,kw_list[i].kw);
				if(!cj || cj->type != kw_list[i].type)
				{
						redisLog(REDIS_WARNING, "Fatal error, command format {%s} is wrong", kw_list[i].kw);
						goto err;
				}
				else if(!strcmp(cj->string,"data"))	
						data = cj;
				else if(!strcmp(cj->string,"formula"))	
						formula = cj;
		}

		/*find formula_func from server.fms(dict)*/
		sds s= sdsnew(formula->valuestring);

		if(!strcmp(s,"iask_wordseg")){
				gsh_formula_iask_wordseg_run(data,fm_buf);		
				addReplyBulkCString(c,fm_buf);
				cJSON_Delete(root);
				sdsfree(s);
				return ;
		}

		/*
		void* val = dictFetchValue(server.fms,s);
		sdsfree(s);



		FMITEM *it = (FMITEM*)val;
		if(!it || !it->run(data,fm_buf)) goto err;

		addReplyBulkCString(c,fm_buf);
		cJSON_Delete(root);
		return ;
		*/
err:
		addReply(c,shared.err);
		cJSON_Delete(root);
		return ;
}

void loadCommand(redisClient *c)
{
		char path[BUFSIZ];
		char *fm_name = c->argv[1]->ptr;
		char *fm_body = c->argv[2]->ptr;
		FILE* fp = fopen(path,"w+");
		char ch[BUFSIZ];
		char *stop;

		/*exist or not.*/
		dictEntry *de = dictFind(server.fms,fm_name);
		if(de)
		{
				redisLog(REDIS_WARNING, "formula [%s] was already loaded.",fm_name);
				goto err;
		}

		sprintf(path,"./lib/lib%s.so",fm_name);

		/*create new file.*/
		if(!fp)
		{
				fprintf(stderr,"open %s failed.\r\n",fm_name);
				goto err;
		}

		/*write file.*/
		while(1)
		{	
				ch[0]=strtol(fm_body,&stop,10);
				if(1!=fwrite(ch,1,1,fp))
				{
						redisLog(REDIS_WARNING, "write file %s failed.",fm_name);
						fclose(fp);
						goto err;
				}
				if(stop[1] == '\0') break;
				fm_body = stop + 1;
		}

		/*close*/
		fclose(fp);
		if(chmod(path,S_IXUSR|S_IRUSR|S_IWUSR))/*rwx*/
		{
				redisLog(REDIS_NOTICE,"chmod file wrong.");
				goto err;	
		}

		/*load new formula.*/
		void *val = loadfm(fm_name);
		if(!val) goto err;

		/*add new formula's func and key to dict.*/
		int retval = dictAdd(server.fms, sdsnew(fm_name), val);
		if(retval != DICT_OK) goto err;

		redisLog(REDIS_NOTICE,"formula [%s] was loaded successfully.",fm_name);

		addReply(c,shared.ok);
		return ;
err:
		addReply(c,shared.err);
		return ;
}

void setCommand(redisClient *c) 
{
		addReply(c,shared.ok);
		return ;
}

void getCommand(redisClient *c) 
{
		addReply(c,shared.ok);
}

/*
   void setnxCommand(redisClient *c){}
   void setexCommand(redisClient *c){}
   void delCommand(redisClient *c){}
   void existsCommand(redisClient *c){}
   void setbitCommand(redisClient *c){}
   void getbitCommand(redisClient *c){}
   void setrangeCommand(redisClient *c){}
   void getrangeCommand(redisClient *c){}
   void appendCommand(redisClient *c){}
   void strlenCommand(redisClient *c){}
   */
