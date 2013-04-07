#ifndef _IASK_NLP_DOCPROP_H
#define _IASK_NLP_DOCPROP_H

#include <stdio.h>

//ÎÄ±¾ÏòÁ¿µÄÊôĞÔ
#define MAX_FEATWORDS_NUM 64
#define FEAT_WORD_LEN 17

//Ó¦ÓÃ²ã·ÖÎöµÃµ½µÄ¹ØÓÚ¸ÃÎÄµµµÄÊôĞÔ
#define DOC_MAX_DOMAIN_NUM  3  //Ò»¸öÎÄµµ×î¶à¿ÉÄÜÊôÓÚ¶àÉÙ¸öÁìÓò
#define DOC_DEFAULT_DOMAIN_NUM  2  //È±Ê¡Çé¿öÏÂÈ¡32öÁìÓò

typedef struct {		
	  unsigned short domainid;
	  char domain[128];
	  float weight;  //ÊôÓÚ¸ÃdomainµÄÈ¨ÖØ
	  float trust;
} DomainInfo;
 
typedef struct
{	
	unsigned char domainnum; //¸ÃÎÄµµÊôÓÚ¼¸¸öÁìÓò(µÚ¶ş²ã)
	DomainInfo domains[DOC_MAX_DOMAIN_NUM]; //¸ÃÎÄµµµÄÁìÓò(µÚ¶ş²ã)	
	DomainInfo domain_level1;//µÚÒ»²ãÊÇÊôÓÚÄÄ¸öÁìÓò
} DocInfo;

typedef struct
{
	char featword[FEAT_WORD_LEN];
	float weight;
	unsigned char postagid;
} FeatItem;

struct DocFeatVec
{
	unsigned short feat_num;
	FeatItem featitem[MAX_FEATWORDS_NUM];
};

#endif
