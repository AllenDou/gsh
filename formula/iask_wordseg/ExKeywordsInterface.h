/* Copyright (c) 2005-2008
* All rights reserved.
* 
* 文件名称：
* 摘    要：关键词提取
* 作    者：孙健 2006/11/13
* 版    本：1.0
* 
*/

#ifndef _IASK_NLP_KeyInterface_H
#define _IASK_NLP_KeyInterface_H

#include "BaseDef.h"
#include "WordSegment.h"
#include "POSTagger_new.h"
#include "phraseterm.h"
#include "docprop.h"

#define DEFAULT_KEYWORDS_NUM 5
#define AOP_EXTRACTKEYWORDS_DEFAULT 0x0010
#define AOP_EXTRACTKEYWORDS_SORT_BY_IDF 0x0020
#define AOP_EXTRACTKEYWORDS_ADV 0x0030
#define MAX_KEYWORDS_NUM 20

//关于文本中一个关键词的描述
typedef struct
{				
	unsigned short key[WORD_MAX_LEN_SHORT+1];

	unsigned int wordPos; //在文本第一次出现的位置 我们不假定每一个关键词都是文本中的词，因此可能为0xFFFFFFFF
	unsigned short len;
	char *pTerm;				//保存Term信息

	float fWeight;
	unsigned short tf;
	unsigned short tf_title;//在标题中出现频度
	unsigned short tf_context;//在正文中出现频度
	float idf;

	unsigned short wordCat; // 从分词系统带过来的类别属性，Attribute of word item, such as personal name, corporation name, and so on
	unsigned char postagid; // 该词的词性id，请见POSTagger_new.h
	char postag[6];
} KeywordItem;

typedef struct{
	unsigned short key_num;
	unsigned short name_num;
	unsigned short locate_num;
	unsigned short event_num;
	unsigned short n_z_num;
	KeywordItem KeyWords[MAX_KEYWORDS_NUM];
	KeywordItem NameList[MAX_KEYWORDS_NUM];
	KeywordItem LocateList[MAX_KEYWORDS_NUM];
	KeywordItem EventList[MAX_KEYWORDS_NUM];
	KeywordItem N_ZList[MAX_KEYWORDS_NUM];
	struct DocFeatVec DocFeat;
}KeyWordStore;

#ifdef __cplusplus
class IKeywords {
	public:

		/**
		 * 函数介绍：初始化
		 * 输入参数：
		 * 输出参数：
		 * 返回值  ：	0		成功
		 *				      <0	失败
		 */
		virtual int InitKeywords(IWordSegment *pWordSegment, IPOSTagger *pPOSTagger, IPhraseTerm *pPhraseTerm) = 0;

		/**
		 * 函数介绍：提取webpage的关键词
		 * 输入参数： KeyWordStore *keystore ,需要用户提供存储空间 可以为空
		 * 输出参数：
		 * 返回值  ：	0		成功
		 *				      <0	失败
		 */		
		virtual int ExtractKeywords(unsigned short* szSrc, unsigned int nLen, KeyWordStore *keystore, unsigned short nOption) = 0;

		virtual KeywordItem * GetNodeListHead() = 0;

		virtual KeywordItem * GetNextNode(KeywordItem *pCurKeyword) = 0;

		virtual  KeywordItem * GetPNNodeListHead()=0;//人物
		virtual  KeywordItem * GetPNNextNode(KeywordItem *pCurKeyword)=0;

		virtual  KeywordItem * GetLNNodeListHead()=0;//地点
		virtual  KeywordItem * GetLNNextNode(KeywordItem *pCurKeyword)=0;

		virtual  KeywordItem * GetTimeNodeListHead()=0;//时间
		virtual  KeywordItem * GetTimeNextNode(KeywordItem *pCurKeyword)=0;

		virtual  KeywordItem * GetEventNodeListHead()=0;//事件
		virtual  KeywordItem * GetEventNextNode(KeywordItem *pCurKeyword)=0;

};

extern "C"  IKeywords* CreateIAskExKeywordsInstance(void);
typedef IKeywords* (* CREATEIASKEXKEYWORDSINSTANCE)(void);

extern "C"  int ReleaseIAskExKeywords(IKeywords *);
typedef int (*RELEASEIASKEXKEYWORDS)(IKeywords*);

#endif //end __cplusplus

#endif
