#include "iask_wordseg.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <ctype.h>
#include <iconv.h>
#include "lexicon.h"
#include "fromuni.h"
#include "touni.h"
#include "ustring.h"
#include "common/cJSON.h"


unsigned long long flags = 0;
unsigned short segopt;
unsigned short postagopt;
unsigned short appopt;
unsigned short lexiconopt;

LEXICON *lexicon = NULL;
WORD_SEGMENT *wordseg = NULL;

#define GBKBUF_LEN (4*1024*1024)
char *gbkbuf;

/*
   return 0: failed.
   return 1: success.
   */
int gsh_formula_iask_wordseg_init(void *arg,void *ret){

		if( !(gbkbuf = malloc(GBKBUF_LEN))) return 0;

		lexicon = OpenLexicon_Opt("./src/iask-wordseg/", 0x1f);
		setLexiconWS(lexicon, 0);
		wordseg = InitWordSeg(10000);

		segopt = SEG_OPT_ALL | SEG_OPT_ZUHEQIYI | SEG_OPT_PER | SEG_OPT_SUFFIX;;
		postagopt = 0x7FFF;
		appopt = AOP_TERM_ALL | AOP_EXTRACTKEYWORDS_DEFAULT | CLASSIFY_USING_KNN;
		lexiconopt = 0x0114;

		flags = lexiconopt;
		flags = flags<<16;

		flags = flags | appopt;
		flags = flags<<16;

		flags = flags | postagopt;
		flags = flags<<16;

		flags = flags | segopt;
		return 1;
}


int code_convert(char *from_charset,char *to_charset,char *inbuf,int inlen,char *outbuf,int outlen){

		iconv_t cd;
		int rc;
		char **pin = &inbuf;
		char **pout = &outbuf;

		cd = iconv_open(to_charset,from_charset);
		if (cd==0)
				return -1;
		memset(outbuf,0,outlen);
		if (iconv(cd,pin,&inlen,pout,&outlen) == -1)
				return -1;
		iconv_close(cd);
		return 0;

}

int u2g(char *inbuf,int inlen,char *outbuf,int outlen){

		return code_convert("utf-8","gbk",inbuf,inlen,outbuf,outlen);
}

int g2u(char *inbuf,size_t inlen,char *outbuf,size_t outlen){

		return code_convert("gbk","utf-8",inbuf,inlen,outbuf,outlen);
}


int gsh_formula_iask_wordseg_run(void *arg,void *ret){

		cJSON *text = cJSON_GetObjectItem(arg,"text");
		if(!text || !text->valuestring) return 0;

		char *str = text->valuestring;
		u2g(str,strlen(str),gbkbuf,GBKBUF_LEN);

		if((AnalyTextWithLex2(lexicon, gbkbuf, GBKBUF_LEN, "GBK", wordseg, flags, 0)) <0) return 0;

		int len,i;

		cJSON *root = cJSON_CreateObject();
		cJSON *array = cJSON_CreateArray();

		for(i = 0,len = 0; i < wordseg->listitem_num; i++){

				char word[512];
				int wlen;
				wlen = uniToBytes(wordseg->uni+(wordseg->list[i]).word->wordPos,(wordseg->list[i]).word->wordLen, word, sizeof(word)-1, "UTF-8");
				if(word[0] == ' ') continue;
				word[wlen] = 0;

				WORD_ITEM *wd = wordseg->list[i].word;

				{
						cJSON *obj,*cjwd,*attr,*pos,*idf;
						obj = cJSON_CreateObject();
						cjwd = cJSON_CreateString(word);
						attr = cJSON_CreateNumber(wd->postagid);
						pos = cJSON_CreateNumber(wd->wordPos);
						idf = cJSON_CreateNumber(wd->idf);

						cJSON_AddItemToObject(obj,"word",cjwd);
						cJSON_AddItemToObject(obj,"pos",pos);
						cJSON_AddItemToObject(obj,"attr",attr);
						cJSON_AddItemToObject(obj,"idf",idf);
						cJSON_AddItemToArray(array,obj);
				}

		}
		//if(wordseg) FreeWordSeg(wordseg);
		cJSON_AddItemToObject(root,"results",array);

		char *p = cJSON_PrintUnformatted(root);
		len = sprintf(ret,"%s",p);

		if(p) free(p);

		if(root) cJSON_Delete(root);

		if( len >= FORMULA_BUFLEN) return 0;

		return 1;
}

