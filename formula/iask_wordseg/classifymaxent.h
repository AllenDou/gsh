#ifndef _IASK_CLS_MAXENT_H
#define _IASK_CLS_MAXENT_H

#include <stdio.h>
#include "classifier.h"

#ifdef __cplusplus

class Classify_Maxent:public Classifier
{
public:
	virtual int InitClsResource(const char *maxentpath = NULL) = 0;
	virtual int GetDocPropInfo(const char *srz, char seg, DocInfo *docinfo, unsigned char pagetype) = 0;
	virtual int GetDocPropInfo(DocFeatVec *docfeat, DocInfo *docinfo, unsigned char pagetype) = 0;

};
extern "C" Classify_Maxent *CreateClassifyMaxentInstance(void);
typedef Classify_Maxent* (*CREATECLASSIFYMAXENTINSTANCE)(void);

extern "C" int ReleaseClassifyMaxent(Classify_Maxent *);
typedef int(*RELEASECLASSIFYMAXENT)(Classify_Maxent *);

#endif //end __cplusplus
#endif
