#ifndef CLASSIFY_H
#define CLASSIFY_H

#include <stdio.h>
#include "BaseDef.h"
#include "phraseterm.h"

#define DEFAULT_IDF 13
#define ERR 		-1

//classid
#define OTHERS 		-1
#define ZHAOPIN 	0
#define QIYEJIESHAO	1
#define JIANLI 		2
//maxclass
#define MAX_CLASS 	15

//segtag
#define SEG_MAX	1
#define SEG_ALL	0


typedef struct classify_result
{
		int		classid;
		float 	credibility; //可信度，值域在[0-1]之间
} PAGE_CLASS_RESULT;


#ifdef __cplusplus
class Classify
{
	public:
		//********************************************************************
		//	功能描述:	init_classify
		//	参数说明:
		//	返 回 值:	0: success; <0: error
		//	备    忘:	只初始化一次
		//********************************************************************
		virtual int init_classify(const char *filename) = 0;
		
		//********************************************************************
		//	功能描述:	classify
		//	参数说明:
		//	返 回 值:	0: success; <0: error
		//	备    忘:   
		//********************************************************************
		virtual int classify(IPhraseTerm *phraseterm, unsigned short *pCurr, unsigned int nLen, int type, PAGE_CLASS_RESULT *result, unsigned short segtag) = 0;
		
		//********************************************************************
        //      功能描述:       print_feat_str
        //      参数说明:
		//      返 回 值
		//      备    忘:
		//********************************************************************
		virtual void print_feat_str(FILE *file, int type) = 0;			
};

extern "C" Classify *CreateClassifyInstance(void);
typedef Classify* (*CREATECLASSIFYINSTANCE)(void);

extern "C" int ReleaseClassify(Classify *);
typedef int(*RELEASECLASSIFY)(Classify *);

#endif //end #ifdef __cplusplus

#endif //end CLASSIFY_H
