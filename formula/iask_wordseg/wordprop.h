/*
* Copyright (c) 2006, 新浪网研发中心
* All rights reserved.
* 
* 文件名称：wordprop.h
* 版    本：
*/


#ifndef _WORD_PROP_H_
#define _WORD_PROP_H_

//约定　具有type属性的词都是禁用词。
#define WORDTYPE_INTERPUNCTION  0x01    //标点符号
#define WORDTYPE_ENDSENTENCE    0x02    //句子结束标志
#define WORDTYPE_LINKWORD_ENGLISH    	0x04    //英文数字类的连接词 2004:03:05 2004/03/05  
#define WORDTYPE_LINKWORD_CHINESE    	0x08    //汉语的连接词	哈利·波特 哈利-波特 哈利.波特 " ' ‘ ’ “ ”  

#define WORDTYPE_PHRASE_MARK	0X10	//类似 〈〉《》
#define WORDTYPE_WORD_DROP	0X20	//类似 ▉ ▊ ▋┲ ┳ ┴

#define WORDSTAT_NOPARSE        0x01            //不记录分词结果
#define WORDSTAT_NOSEARCH       0x02            //禁止搜索

#define WORDSTAT_COMMONWORD		0X04	// 得 的 啊 了
#define WORDSTAT_URLWORD		0X08	//www com net 

#define WORDSTAT_HASSTRU        0x40            //该词存在结构
#define WORDSTAT_STRU   		0x80                  //结构分词结果

#define IS_SENTENCEMARK(type) ((type)&0x01)
#define IS_ENDSENTENCE(type) ((type)&0x02)
#define IS_LINKWORD_ENGLISH(type) ((type)&WORDTYPE_LINKWORD_ENGLISH)
#define IS_LINKWORD_CHINESE(type) ((type)&WORDTYPE_LINKWORD_CHINESE)

#define IS_NOPARSE(stat)  ((stat) & WORDSTAT_NOPARSE)						//禁用词

#define IS_COMMONWORD(stat) ((stat)&WORDSTAT_COMMONWORD)
#define IS_URLWORD(stat) ((stat)&WORDSTAT_URLWORD)

#define IS_STUCTWORD(stat) ((stat)&WORDSTAT_STRU) //结构分词结果
#define IS_HAS_STUCTWORD(stat) ((stat)&WORDSTAT_HASSTRU)//该词存在结构

#define IS_SPACE(wordCat) ((wordCat) == 0x50)								//空
#define IS_ENG_WORD(wordCat) (((wordCat)==0x0003) || ((wordCat)==0x0011))	//英文词或数字	


#endif 
