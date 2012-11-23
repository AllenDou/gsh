#include "cache.h"
#include "../../src/common/cJSON.h"


#define JS_GOItem(x,y) cJSON_GetObjectItem(x,y)

cJSON *data,*cmd,*key,*val;
/*
   return 0: failed.
   return 1: success.
   */
int gsh_formula_cache_init(void *arg,void *ret)
{
		assoc_init(25);
		load_data("__cache__.dat__","__cache__.idx__","__cache__.recy__");
		return 1;
}

int gsh_formula_cache_run(void *arg,void *ret)
{
		/*
		   int len = sprintf(ret,"cache formula test....");
		   if( len >= FORMULA_BUFLEN)
		   return 0;
		   */
		data = (cJSON*)arg;
		if(!(cmd = JS_GOItem(data,"command")))
				goto err;
		if(!(key = JS_GOItem(data,"key")) || key->type != cJSON_String)
				goto err;
		if(!(val = JS_GOItem(data,"value")) || val->type != cJSON_String)
				goto err;
		char *pk = key->valuestring;
		char *pv = val->valuestring;
		
		/*processCommand.*/
		if(!strcasecmp(cmd->valuestring,"get"))
		{
				int len = assoc_find( pk,strlen(pk)+1,ret, KEY_TYPE_STRING);
				if( !len || len > FORMULA_BUFLEN ) goto err;
				return 1;
		}
		else if(!strcasecmp(cmd->valuestring,"set"))
		{
				if(strlen(pv) >= FORMULA_BUFLEN || !strlen(pv)) goto err;
				assoc_query( pk,strlen(pk)+1, pv, strlen(pv)+1,KEY_TYPE_STRING);
		}
		else if(!strcasecmp(cmd->valuestring,"del"))
				assoc_del( pk,strlen(pk)+1, KEY_TYPE_STRING);
		else if(!strcasecmp(cmd->valuestring,"dump"))
				dump_data("__cache__.dat__","__cache__.idx__","__cache__.recy__");
		else
				goto err;
		sprintf(ret,"OK");
		return 1;
err:
		return 0;
}

