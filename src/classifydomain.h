/*
* Copyright (c) 2005-2008
* All rights reserved.
* 
* ÎÄ¼şÃû³Æ£ºclassifydomain.h
* Õª    Òª£º·Ö´ÊÖ®ÉÏµÄÓ¦ÓÃ²ã
* ×÷    Õß£ºËï½¡ 2006/04/03
* °æ    ±¾£º1.0
* 
*/

#ifndef _IASK_NLP_Domain_H
#define _IASK_NLP_Domain_H

#include "classifier.h"
#include "ExKeywordsInterface.h"
#include <stdio.h>

#define IN
#define OUT

#define CONCEPT_MAX_LEN 20

#define MAX_VOTE_NUM 100
//#define SUN_DEBUG_VOTE 1

// ****** tagoptionËµÃ÷ ********** //
//unsigned shortµÄ9-16Î»ÓÃÓÚ·ÖÀà 
//×î¸ßËÄÎ»ÓÃÓÚ·ÖÀàÆ÷Ñ¡Ôñ£¬µÍËÄÎ»ÓÃÓÚ·ÖÀàÎÄ±¾µÄÑ¡Çø
 
//knn·ÖÀàÆ÷·ÖÀàÀàĞÍ
#define TAG_KNN_ADSENSE 0
#define TAG_KNN_NEWS 1
#define TAG_KNN_BLOG 2

#ifdef __cplusplus

class ClassifyDomain
{
public:
	virtual int InitClsResource(IWordSegment *pWordSegment, IPOSTagger *pPOSTagger, IPhraseTerm *pPhraseTerm, IKeywords *pKeywords) = 0;

	/*Ê¹ÓÃ·½·¨: ´«µİµÄ²ÎÊıÒoptionªÇó°üº¬ÖÁÉÙ3¸ö²¿·Ö
	 * ¶ÌÓïºÏ²¢²¿·Ö:AOP_PHRASETERM_XX Ã»ÓĞ¾Í²»×ö¶ÌÓïºÏ²¢
	 * 
	 * featvec ÎÄ±¾µÄÌØÕ÷ÏòÁ¿´®£¬ÓÃ seg ·Ö¸î£¬Èç¹ûÓÃGetDocFeatµÃµ½ÁË¹Ø¼ü´Ê´®
	 * ÔòfeatvecÖÃNULL, seg²»Æğ×÷ÓÃ
	 */
	virtual int GetDocPropInfo(DocInfo *docinfo, unsigned char pagetype) = 0;
	//virtual int GetDocFeat(unsigned short *srz, unsigned int nlen, unsigned char pagetype, unsigned short Option) = 0;
};

extern "C" ClassifyDomain *CreateClassifyDomainInstance(void);
typedef ClassifyDomain* (*CREATECLASSIFYDOMAININSTANCE)(void);

extern "C" int ReleaseClassifyDomain(ClassifyDomain *);
typedef int(*RELEASECLASSIFYDOMAIN)(ClassifyDomain *);

#endif //end __cplusplus
#endif
