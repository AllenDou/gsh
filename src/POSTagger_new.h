/*
* Copyright (c) 2005-2008
* All rights reserved.
* 
* 文件名称：POSTagger.h
* 摘    要：词性标注接口
* 作    者：孙健 2006/01/05
* 版    本：1.0
* 
*/

#ifndef _IASK_NLP_POSTagger_H
#define _IASK_NLP_POSTagger_H

/************************************************************/
//		词性定义部分										//
/************************************************************/
#define POSTAG_ID_UNKNOW 0	//不知道

#define POSTAG_ID_A  10	//	形容词

#define POSTAG_ID_B	20	//	区别词

#define POSTAG_ID_C	30	//	连词
#define POSTAG_ID_C_N	31	//	体词连接
#define POSTAG_ID_C_Z	32	//	分句连接

#define POSTAG_ID_D	 40   //副词
#define POSTAG_ID_D_B	41	//副词	"不"
#define POSTAG_ID_D_M	42	//副词	"没"

#define POSTAG_ID_E	  50	//叹词

#define POSTAG_ID_F	  60  //方位词
#define POSTAG_ID_F_S	61	//方位短语 处所词+方位词
#define POSTAG_ID_F_N	62	//方位短语 名词+方位词“地上”
#define POSTAG_ID_F_V	63	//方位短语 动词+方位词“取前”
#define POSTAG_ID_F_Z	64	//方位短语 动词+方位词“取前”

#define POSTAG_ID_H	70	   //前接成分
#define POSTAG_ID_H_M	71	//数词前缀 “数”---数十
#define POSTAG_ID_H_T	72	//时间词前缀 “公元”“明永乐”
#define POSTAG_ID_H_NR 73	//姓氏
#define POSTAG_ID_H_N 74	//姓氏

#define POSTAG_ID_K	80	//后接成分
#define POSTAG_ID_K_M	81	//数词后缀	“来”-- 十来个
#define POSTAG_ID_K_T	82	//时间词后缀	“初”“末”“时”
#define POSTAG_ID_K_N	83	//名词后缀	“们”
#define POSTAG_ID_K_S	84	//处所词后缀	“苑”“里”
#define POSTAG_ID_K_Z	85	//状态词后缀	“然”
#define POSTAG_ID_K_NT	86	//状态词后缀	“然”
#define POSTAG_ID_K_NS	87	//状态词后缀	“然”

#define POSTAG_ID_M	90	//数词

#define POSTAG_ID_N	95	//名词
#define POSTAG_ID_N_RZ 96	//人名	“毛泽东”
#define POSTAG_ID_N_T	 97 //机构团体	“团”的声母为t，名词代码n和t并在一起。“公司”
#define POSTAG_ID_N_TA	98
#define POSTAG_ID_N_TZ	99	//机构团体名	"北大"
#define POSTAG_ID_N_Z	100	//其他专名	“专”的声母的第1个字母为z，名词代码n和z并在一起。
#define POSTAG_ID_NS	101	//名处词
#define POSTAG_ID_NS_Z	102	//地名	名处词专指：“中国”
#define POSTAG_ID_N_M	103	//	n-m	数词开头的名词	三个学生
#define POSTAG_ID_N_RB 104	//n-rb	以区别词/代词开头的名词	该学校，该生

#define POSTAG_ID_O	107	  //拟声词
#define POSTAG_ID_P	108	  //介词

#define POSTAG_ID_Q	110	  //量词
#define POSTAG_ID_Q_V	111	//动量词	“趟”“遍”
#define POSTAG_ID_Q_T	112	//时间量词	“年”“月”“期”
#define POSTAG_ID_Q_H	113	//货币量词 “元”“美元”“英镑” 

#define POSTAG_ID_R	120	  //代词
#define POSTAG_ID_R_D	121	//副词性代词	“怎么”
#define POSTAG_ID_R_M	122	//数词性代词	“多少”
#define POSTAG_ID_R_N	123	//名词性代词	“什么”“谁”
#define POSTAG_ID_R_S	124	//处所词性代词	“哪儿”
#define POSTAG_ID_R_T	125	//时间词性代词	“何时”
#define POSTAG_ID_R_Z	126	//谓词性代词	“怎么样”
#define POSTAG_ID_R_B	127	//区别词性代词	“某”“每”

#define POSTAG_ID_S		130 //处所词	取英语space的第1个字母。“东部”
#define POSTAG_ID_S_Z		131 //处所词	取英语space的第1个字母。“东部”
#define POSTAG_ID_T   132 //时间词	取英语time的第1个字母
#define POSTAG_ID_T_Z	133 //时间专指	“唐代”“西周”

#define POSTAG_ID_U		140 //助词
#define POSTAG_ID_U_N	141 //定语助词	“的”
#define POSTAG_ID_U_D	142	//状语助词	“地”
#define POSTAG_ID_U_C	143	//补语助词	“得”
#define POSTAG_ID_U_Z	144	//谓词后助词	“了、着、过”
#define POSTAG_ID_U_S	145	//体词后助词	“等、等等”
#define POSTAG_ID_U_SO	146	//助词 “所” 

#define POSTAG_ID_W		150 //标点符号
#define POSTAG_ID_W_D	151 //顿号	“、”
#define POSTAG_ID_W_SP	152 //句号	“。”
#define POSTAG_ID_W_S	153 //分句尾标点 “，”“；” 
#define POSTAG_ID_W_L	154 //搭配型标点左部
#define POSTAG_ID_W_R	155 //搭配型标点右部 “》”“]”“）” 
#define POSTAG_ID_W_H	156 //中缀型符号

#define POSTAG_ID_Y		160 //语气词	取汉字“语”的声母。“吗”“吧”“啦”

#define POSTAG_ID_V	170 //及物动词	取英语动词verb的第一个字母。
#define POSTAG_ID_V_O 171	//不及物谓词	谓宾结构“剃头”
#define POSTAG_ID_V_E 172		//动补结构动词	“取出”“放到”
#define POSTAG_ID_V_SH	173	//动词“是”
#define POSTAG_ID_V_YO	174	//动词“有”
#define POSTAG_ID_V_Q	175	//趋向动词 “来”“去”“进来” 
#define POSTAG_ID_V_A	176	//助动词 “应该”“能够” 

#define POSTAG_ID_Z		180 //状态词 不及物动词	v-o、sp之外的不及物动词

#define POSTAG_ID_X   190 //语素字
#define POSTAG_ID_X_N 191  //名词语素	“琥”
#define POSTAG_ID_X_V 192  //动词语素	“酹”
#define POSTAG_ID_X_S 193  //处所词语素	“中”“日”“美”
#define POSTAG_ID_X_T 194  //时间词语素	“唐”“宋”“元”
#define POSTAG_ID_X_Z 195  //状态词语素	“伟”“芳”
#define POSTAG_ID_X_B 196  //状态词语素	“伟”“芳”

#define POSTAG_ID_SP  200  //不及物谓词	主谓结构“腰酸”“头疼”
#define POSTAG_ID_MQ  201  //数量短语	“叁个”
#define POSTAG_ID_RQ  202  //代量短语	“这个”

#define POSTAG_ID_AD  210  //副形词	直接作状语的形容词
#define POSTAG_ID_AN  211  //名形词	具有名词功能的形容词
#define POSTAG_ID_VD  212  //副动词	直接作状语的动词
#define POSTAG_ID_VN  213  //名动词	指具有名词功能的动词

#define POSTAG_ID_SPACE  230  //空格

#include "WordSegment.h"

//提供给用户层的节点信息
struct WordPOSNode
{
	_WordNodeU outnode;   //提供给用户层的词信息（由分词结果而来，在词性标注过程中不会修改其中的任何信息; 但是在短语合并过程中可能改变这里面的类别标记）

	unsigned char chType; //这个词的词性信息是否在词典中
	//1 该词只有一个词性
	//2 该词的词性来自于分词的节点属性
	//3 该词有多个词性
	//4 在词性词典中不存在该词，是猜测出来的
	
	//5 该词是应用层合并几个分词结果得到
	//6 该词只是分词结果节点
	//7 既是应用层节点，也是分词结果节点
	unsigned short synonymtype;
	
	unsigned char postagid;//提供给用户层的词性id
	char postag[6];      //词性最长5个字节, 提供给用户层的词性信息 
};

typedef struct WordPOSNode _WordPOSNode, *_PWordPOSNode;


#define POSTAGGER_OPT_ALL			  0x7fff
#define POSTAGGER_OPT_USESEGINFO 0x0001      //继承分词系统中的类别属性

#ifdef __cplusplus
// Interface for POSTagger:
class  IPOSTagger {
public:

	// 返回值  ：	0			成功
	//						<0		失败
	virtual int InitPOSTagger(IWordSegment *pWordSegment) = 0;
	
	//接口函数1
	//文本->WordSegment->POSTagger
	//与爱问分词系统接口
	// 返回值  ：	0			成功
	//						<0		失败
	virtual int POSTagger(unsigned short* szSrc, unsigned int nLen, _PWordNodeU pHeadNode, unsigned short nOption) = 0;
	
	//接口函数2
	//文本 -> WordSegmentPOSTagger，分词与词性标注一起来完成
	// 返回值  ：	0			成功
	//						<0		失败
	virtual int WordSegmentPOSTagger(unsigned short* szSrc, unsigned int nLen, unsigned short nOption) = 0;
	
	//接口函数3
	//用空格隔开的文本->POSTagger
	//输入串为char *pSentence：用空格隔开的句子 w1 w2 w3 ...,以'\0'为结束符
	//virtual int POSTag_SegmentedSent(unsigned short* szSrc, unsigned int nLen) = 0;

	//Traversing node list methods:
	virtual _PWordPOSNode GetNodeListHead() = 0;
	virtual _PWordPOSNode GetNextNode(_PWordPOSNode pCurNode) = 0;

	//由词性id -> 词性
	// 返回值  ：	0			成功
	//						<0		失败
	virtual int TagId2POSTag(unsigned char posid,  char *postag) = 0;

	//由词性 -> 词性id
	// 返回值  ：	0			成功
	//						<0		失败
	virtual int POSTag2Id(const char *postag, unsigned char *posid) = 0;
	
	//由单词查到该单词的最大概率的那个词性id
	//并且给出共有多少个词性
	//如果该词没有词性,那么posnum=0
	
	// 返回值  ：	0			成功
	//					<0		失败
	virtual int GetWordPOSId(unsigned short *uniword, unsigned char *posid, int *posnum) = 0;
	
};


// Create POSTagger instance, return pointer to the interface of it(Call Release to destroy it):
extern "C"  IPOSTagger* CreatePOSTaggerInstance(void);
typedef IPOSTagger* (* CREATEPOSTAGGERINSTANCE)(void);

//Call it to release POSTagger instance
extern "C"  int ReleasePOSTagger(IPOSTagger *);
typedef int (*RELEASEPOSTAGGER)(IPOSTagger*);

#endif //end ifdef __cplusplus

#endif //end _IASK_NLP_POSTagger_H
