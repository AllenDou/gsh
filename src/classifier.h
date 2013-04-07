#ifndef _IASK_NLP_CLASSIFIER_H
#define _IASK_NLP_CLASSIFIER_H

#include <stdio.h>
#include "docprop.h"

#define CLASSIFY_USING_DOMAIN 0x1000     //使用得分类器 知识库德投票机
#define CLASSIFY_USING_KNN 0x2000        //knn分类器
#define CLASSIFY_USING_MAXENT 0X4000	 //maxent classifier

#define CLASSIFY_LOAD_DOMAIN 0X01
#define CLASSIFY_LOAD_KNN 0X02
#define CLASSIFY_LOAD_MAXENT 0X04

#ifdef __cplusplus

class Classifier
{
public:

	/*
	 *	说明:分类管理器初始化，传入底层的指针
	 */
	virtual int InitClsResource(const char *path) = 0;
	
	virtual int GetDocPropInfo(const char *src, const char seg, DocInfo *docinfo, unsigned char pagetype) = 0;
	virtual int GetDocPropInfo(DocFeatVec *docfeat, DocInfo *docinfo, unsigned char pagetype) = 0;
};
#endif

#endif
