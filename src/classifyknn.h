#ifndef _IASK_CLS_KNN_H
#define _IASK_CLS_KNN_H

#include <stdio.h>
#include "classifier.h"
#ifdef __cplusplus

typedef struct DocFeatVec DocFeatVec;

class Classify_KNN:public Classifier
{
public:
	/*参数说明:
	 */
	
	virtual int InitClsResource(const char *path = NULL) = 0;
	virtual int GetDocPropInfo(const char *srz, char seg, DocInfo *docinfo, unsigned char pagetype) = 0;
	virtual int GetDocPropInfo(DocFeatVec *docfeat, DocInfo *docinfo, unsigned char pagetype) = 0;

};
extern "C" Classify_KNN *CreateClassifyKnnInstance(void);
typedef Classify_KNN* (*CREATECLASSIFYKNNINSTANCE)(void);

extern "C" int ReleaseClassifyKnn(Classify_KNN *);
typedef int(*RELEASECLASSIFYKNN)(Classify_KNN *);

#endif //end __cplusplus
#endif
