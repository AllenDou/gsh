/*
* Copyright (c) 2005, 新浪网
* All rights reserved.
* 
* 文件名称：BaseDef.h
* 摘    要：Basic definition for IASK Word Segment API (Unicode)
* 作    者：Sun Jian 2005/10/16
* 作    者：Sun Jian 2006/6/25
* 版    本：1.0
* $Id: 
*/

#ifndef __BASEDEFU_H__
#define __BASEDEFU_H__

#define __STRU_INFO__ 1

typedef unsigned int				WORDID;			// ID of Word item
typedef unsigned int*				LPWORDID;       // Pointer to Word Id
#define INVALID_WORDID			 0xFFFFFFFF     // Invalid WORDID

#define MAX_INPUT_TEXT_LEN (1024*8)  //分词系统 输入文本的最大长度（8K）
#define WORD_MAX_LEN_SHORT 15 //unicode长度 不包括最后的0

/////////////////////////////////////////////////////////////////////////
// Error codes
#define SUCCESS				    0  //成功
#define ERR_SUCCESS				0  //成功
#define ERR_NO_MEMORY			-1 //内存不够
#define ERR_NO_FILE				-2 //文件没有找到
#define ERR_BAD_INPUT			-3 //输入参数不对
#define ERR_BAD_OUTPUT			-4 //输出有问题
#define ERR_LIB_NOT_READY		-5 //词典库 没有准备好
#define ERR_NO_FIND				-6   //没有找到
#define ERR_BAD_PARAM			-7   //参数不对
#define ERR_UNKNOWN				-99  //其它错误
// End of definition of error codes.
/////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
///////////Segmentation Option Tags
//#define SEG_OPT_    			      //0000 0000 0000 0001 空闲
#define SEG_OPT_AMB 0x0002        //0000 0000 0000 0010 歧义切分
#define SEG_OPT_SUFFIX 0x0004     //0000 0000 0000 0100 处理后缀词　"地铁化 地图业 地铁费"
#define SEG_OPT_PREFIX 0x0008     //0000 0000 0000 1000 处理前缀词　"总投资 反分裂 反美 小女孩 非公有制 副会长 小男孩 副所长 反倾销 总人数"

#define SEG_OPT_QUANTIFIER 0x0010 //0000 0000 0001 0000 处理数量词　数词＋量词
#define SEG_OPT_ZUHEQIYI 0x0020   //0000 0000 0010 0000 处理组合歧义　"才能　中将　打的"
#define SEG_OPT_OVERLAP 0x0040    //0000 0000 0100 0000 处理重叠词　ABAB(休息休息) AA(聊聊) AAB(聊聊天)
#define SEG_OPT_LIHE    0x0080    //0000 0000 1000 0000 处理离合词　"聊一聊　打不倒　打得倒"

#define SEG_OPT_PER 0x0100     //0000 0001 0000 0000 动态识别人名
#define SEG_OPT_LOC 0x0200     //0000 0010 0000 0000 动态识别地名
#define SEG_OPT_ORG 0x0400     //0000 0100 0000 0000 动态识别机构名
#define SEG_OPT_QUERY 0x0800   //0000 1000 0000 0000 对query进行切分

#define SEG_OPT_DICT  0x1000		//0001 0000 0000 0000 查词典
//#define SEG_OPT_    			//0010 0000 0000 0000 空闲
//#define SEG_OPT_    			//0100 0000 0000 0000 空闲
#define SEG_OPT_MM  0x8000  //1000 0000 0000 0000 最高位是１，那么正向最大切分

#define SEG_OPT_ALL	0x07ff  //0000 0111 1111 1111
///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
//Word categories dynamically merged
//用两个字节来表示
//高字节表示 形态属性
//低字节表示 语义属性
//nCat就是把这两个字节直接＂或＂在一起

#define MASK_XINGTAI 0xFF00
#define MASK_YUYI    0x00FF

//语义属性开始
#define CAT_UNKNOWN				    0x0000	// Can't specify the category

#define CAT_HAN_PERSON_NAME   0x00B1	//汉族人名及类似的其他民族人名
#define CAT_JPN_NAME			          0x00B2	//日本人名
#define CAT_OUMEI_PERSON_NAME 0x00B3	//外国人名，主要是欧美人名和非以上两类人名
#define CAT_SURNAME_TITLE  0x00B4	    //姓+头衔
#define CAT_PN_OTHER 0x00B5	          //其它 类型的人名 "汉武帝"

#define CAT_BOOK_NAME  0x00B6	         //书名号中或引号中

#define CAT_DIMING		    0x00C1			    //地名
#define CAT_DIMING_SUFFIX   0x00C2	//地名后缀 如: "省"

//#define CAT_CORP_NAME			0x00D1		//机构名
#define CAT_TRADE_MARK       0x00D2			//商标
#define CAT_CORP_SUFFIX       0x00D3			//机构名后缀 如:“公司”
#define CAT_CORP_NAME_ABCD			0x00D4		     //机构名 北京/新浪/互联网/公司
#define CAT_CORP_NAME_ABD			0x00D5		     //机构名 北京/新浪/公司
#define CAT_CORP_NAME_ACD			0x00D6		     //机构名 北京/邮电/大学
#define CAT_CORP_NAME_AD			    0x00D7		     //机构名  北京/大学
#define CAT_CORP_NAME_AACD			0x00D8		     //机构名
#define CAT_CORP_NAME_BCD			0x00D9		     //机构名  新浪/互联网/公司
#define CAT_CORP_NAME_BD			    0x00DA		     //机构名  新浪/公司
#define CAT_CORP_NAME_OTHER		0x00DB		     //机构名  其它形式的机构名

#define CAT_DATE          0x00E1    //日期
#define CAT_TIME		    0x00E2      //时间

#define CAT_TELEPHONE_NUM 0x00F1 //电话号码
#define CAT_EMAIL 0x00F2 //e-mail地址
#define CAT_URL 0x00F3 //url
#define CAT_IP 0x00F4 //ip地址
#define CAT_MAYBE_VERSION 0x00F5 //may be version
//语义属性结束

//形态属性开始
#define CAT_ENG_WORD	    0x0100		//英文串 全部是英文
#define CAT_DIGITAL		        0x0200      //数字串 全部是数字

#define CAT_SPACES				0x1000			//Spaces

#define CAT_SUF_PHRASE			0x2000	  //Suffix phrase
#define CAT_PREFIX_PHRASE		0x2100	  //Prefix phrase

#define __GPWS_QUANTIFIER__ 0
#ifdef __GPWS_QUANTIFIER__
#define CAT_QUANTIFIER 			0x3000	        //数量词
#endif

#define CAT_OVERLAP_AA  	  0x4000	          //重叠形式AA
#define CAT_OVERLAP_ABAB  	0x4100	          //重叠形式ABAB
#define CAT_OVERLAP_AABB  	0x4200	          //重叠形式AABB
#define CAT_OVERLAP_AAB  	  0x4300	      //重叠形式AAB
#define CAT_OVERLAP_ABB  	  0x4400	      //重叠形式ABB

#define __GPWS_VERB_ADJ_VARIATION__ 1
#ifdef __GPWS_VERB_ADJ_VARIATION__		//
//宾
#define CAT_VAR_BIN2			0x5000 			//V了N V着N V过N

//单
#define CAT_VAR_DAN3			0x5100			//V一V[N]

//补
#define CAT_VAR_BU1				0x5200			//V不C V得C
#endif // __GPWS_VERB_ADJ_VARIATION__
//形态属性结束

// End of categories dynamically merged.
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// Word node for segment list
typedef unsigned long long ATTRIBUTE;
#define TERM_MAX_DOMAIN_NUM 2 //每一个term最多有几个领域
#define TERM_MAX_SEMANTIC_NUM 2 //每一个term最多有几个语义属性
struct tagWordNodeU
{
   //Data
   int            nStart;		//Position in source string
   WORDID         Id;		        //WORDID of cut-out word item
		  			//如果不是未登录词，那么就是词典中的wordid
			          	//如果是未登录词，那么Id就为-1
   unsigned short nCat;		  //Attribute of word item, such as personal name, corporation name, and so on
   
   unsigned char nLen;	    //Length of this word
   unsigned char idf;      //0-255. 2007.12.10
   unsigned char domainnum; //该term有几个领域
   unsigned char semanticnum; //该term有几个语义属性
   
   unsigned short domainid[TERM_MAX_DOMAIN_NUM]; //领域属性
   unsigned short semanticid[TERM_MAX_SEMANTIC_NUM]; //语义属性
   
   //Pointer
   unsigned char right;		// Point to right node
};
typedef struct tagWordNodeU   _WordNodeU, *_PWordNodeU;

// Word item
struct tagWordItemU
{
	WORDID      Id;				// Id of word item
	unsigned short sWord[WORD_MAX_LEN_SHORT+1];	// Word item
};
typedef struct tagWordItemU         _WordItemU, *_PWordItemU;

/////////////////////////////////////////////////////////////////////

#endif //__BASEDEFU_H__
