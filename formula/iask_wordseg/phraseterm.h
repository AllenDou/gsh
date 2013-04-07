/*
* Copyright (c) 2005-2008
* All rights reserved.
* 
* 文件名称：phraseterm.h
* 摘    要：分词之上的应用层
* 作    者：孙健 2006/04/03
* 版    本：1.0
* 
*/

#ifndef _IASK_NLP_PhraseTerm_H
#define _IASK_NLP_PhraseTerm_H

#include "BaseDef.h"
#include "WordSegment.h"
#include "POSTagger_new.h"

#define IN
#define OUT

//提交给用户的term的信息
typedef struct
{
	//unsigned short concept[CONCEPT_MAX_LEN+1];//该term的词条
	unsigned int   termid;             //该term的id 该id是这个层面的id与分词结果的id没有任何关系

	unsigned char domainnum; //该term有几个领域
	unsigned char semanticnum; //该term有几个语义属性
	unsigned char postagid; //该应用层的词性id
	unsigned char idf;

	unsigned short domainid[TERM_MAX_DOMAIN_NUM]; //领域属性
	unsigned short semanticid[TERM_MAX_SEMANTIC_NUM]; //语义属性

}TermInfo_touser;

//App Option Tags
#define APP_MARK_PHRASETERM 0x000f

#define AOP_TERM_MM  0x0001 //查找概念的方式：正向最大切分 
#define AOP_TERM_ALL 0x0002 //查找概念的方式：全切分 (缺省值)
#define AOP_TERM_NOPHRASE 0x0000 //只把分词结果copy到相应的数据结构，不做短语的合并 

//最后选项就是把这两个字节进行“或”操作

#ifdef __cplusplus
class IPhraseTerm {
public:
	
	/**
	* 函数介绍：初始化
	* 输入参数：
	* 输出参数：
	* 返回值  ：	0		成功
	*				      <0	失败
	*/
	virtual int InitIPhraseTerm(IWordSegment *pWordSegment, IPOSTagger *pPOSTagger) = 0;
	
	/**
	* 函数介绍：识别短语
	* 输入参数：
	* 输出参数：
	* 返回值  ：	0		成功
	*				      <0	失败
	*/
	//接口函数1
	//文本->WordSegment->PhraseTerm
	//与爱问分词系统接口
	virtual int PhraseTerm(unsigned short* szSrc, unsigned int nLen, _PWordNodeU pHeadNode, unsigned short nOption) = 0;//分词+应用层
	virtual int PhraseTerm(unsigned short* szSrc, unsigned int nLen, _PWordPOSNode pHeadNode, unsigned short nOption) = 0;//分词+词性标注+应用层
	
	/**
	* 函数介绍：Traversing node list method
	* 输入参数：
	* 输出参数：
	* 返回值  ：	0		成功
	*				      <0	失败
	*
	*/	
	virtual int GetListHeadState(int *iNextTime, int *NodeNum) = 0;
	
	/**
	* 函数介绍：获取下一个时刻 获取这个时刻的短语结果个数
	* 输入参数：
	* 输出参数：
	* 返回值  ：	0		成功
	*				      <0	失败
	*
	*/
	virtual int GetNextState(int iCurTime, int *iNextTime, int *NodeNum) = 0;
	virtual int GetMaxLenNextState(int iCurTime, int *iNextTime, int *NodeNum) = 0;
	
	/**
	* 函数介绍：获取给定时刻的某一个概念
	* 输入参数：
	* 输出参数：
	* 返回值  ：	0		成功
	*				      <0	失败
	*
	*/	
	virtual _PWordPOSNode GetNode(int iCurTime, int nIndex) = 0;

	/**
	* 函数介绍：获取给定时刻的某一个概念
	* 输入参数：
	* 输出参数：
	* 返回值  ：    0               成功
	*                                     <0        失败
	*
	*/

	virtual _PWordPOSNode GetNextNode(int *NextIdx) = 0;
	virtual _PWordPOSNode GetHeadNode(int *NextIdx) = 0;
	
	/**
	* 函数介绍：获取这个概念的信息
	* 输入参数：
	* 输出参数：
	* 返回值  ：	0		成功
	*				      <0	没有
	*
	*/
	virtual int GetNodeInfo(int iCurTime, int nIndex, TermInfo_touser *info) = 0;
	
	/**
	* 2007/12/09
	* 函数介绍：	根据单词得到该term的IDF
	* pIDF 地址空间由调用者提供
	* 返回值  ：	0		成功
	*						  <0	失败
	*/
	virtual int GetTermIDF(unsigned short *word, int wordlen,  unsigned char *pIDF) = 0;
	
	/**
	* 2007/12/09
	* 函数介绍：	
	* pTerm 地址空间由调用者提供
	* 返回值  ：	0		成功
	*							<0	失败或不存在
	*/
	virtual int GetTerm(unsigned short *word, int wordlen,  TermInfo_touser *pTerm) = 0;
	
};

// Create IPhraseTerm instance, return pointer to the interface of it(Call Release to destroy it):
extern "C"  IPhraseTerm* CreatePhraseTermInstance(void);
typedef IPhraseTerm* (* CREATEPHRASETERMINSTANCE)(void);

//Call it to release IPhraseTerm instance
extern "C"  int ReleasePhraseTerm(IPhraseTerm *);
typedef int (*RELEASEPHRASETERM)(IPhraseTerm*);

#endif //end __cplusplus

#endif
