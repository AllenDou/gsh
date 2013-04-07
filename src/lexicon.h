/*
* Copyright (c) 2004, ĞÂÀËÍøÑĞ·¢ÖĞĞÄ
* All rights reserved.
* 
* ÎÄ¼şÃû³Æ£ºlexicon.h
* Õª    Òª£ºÓÃÓÚ´ÊµäµÄÊı¾İºÍ¹¦ÄÜ¶¨Òå
* ×÷    Õß£ºÅíÖĞ»ª 2004/07/20
*           Ëï½¡ 2006.04.12
* °æ    ±¾£º1.0
* $Id: lexicon.h,v 1.3 2005/04/04 10:26:46 zhonghua Exp $
*/


#ifndef LEXICON_H
#define LEXICON_H

#include "searchdef.h"

#include "BaseDef.h"
#include "WordSegment.h" //·Ö´Ê
#include "POSTagger_new.h" //´ÊĞÔ±ê×¢
#include "phraseterm.h"  //¶ÌÓïºÏ²¢
#include "ExKeywordsInterface.h" //¹Ø¼ü´ÊÌáÈ¡
//#include "classifydomain.h"  //ÎÄ±¾·ÖÀà1

#include "classify.h" //ÎÄ±¾·ÖÀà2
#include "classifyknn.h" //ÎÄ±¾·ÖÀà2

#include "dllcall.h"
#include "wordprop.h"

/* ´Êµä´Ê¿â´ÊÊôĞÔĞÅÏ¢Êı¾İ½á¹¹¶¨Òå */
typedef struct 
{
	BYTE type;					//´ÊÀà±ğ È±Ê¡Îª0
	BYTE stat;					//Ïà¹Ø×´Ì¬ È±Ê¡Îª0
	WORD IDF;					  //Ïà¹ØÊôĞÔ È±Ê¡Îª0
}WORD_PROPERTY;


/* ´ÊµäÃèÊöĞÅÏ¢Êı¾İ½á¹¹¶¨Òå */
typedef struct {
	char rdir[256];				//¸ùÄ¿Â¼£¬È±Ê¡Îª0
	char fn_config[100];		//´ÊµäÅäÖÃÎÄ¼ş

	int lexdata;				//¶ÔÓ¦µÄ´ÊµäÊı¾İ±àºÅ
	int ws_sel;					//´Êµäµ±Ç°Ñ¡ÔñµÄÓïÖÖ
}LEXICON;


/* ·Ö´Ê½á¹ûÊı¾İ½á¹¹¶¨Òå */
#define DOMAIN_SEMANTIC 1
typedef struct {
	unsigned int wordPos;		    // Position in source string, ÔÚunicode´®ÖĞµÄÎ»ÖÃ
	unsigned long long wordID;	// WORDID
	
	unsigned char wordLen;	    // Length of this word£¬      unicodeµÄ³¤¶È
	unsigned char postagid;     // ¸Ã´ÊµÄ´ÊĞÔid£¬Çë¼ûPOSTagger_new.h
	unsigned short wordCat;		  // ´Ó·Ö´ÊÏµÍ³´ø¹ıÀ´µÄÀà±ğÊôĞÔ£¬Attribute of word item, such as personal name, corporation name, and so on.
		
	WORD_PROPERTY wordprop;		  // word property
	
	unsigned short childnodes; //2007.7.13 sunjian
											 //¸Ã×Ö¶ÎÖ»ÓĞÔÚ»ú¹¹ÃûÊ±ÓĞĞ§£»ÆäËüÇé¿öÔİÊ±¿ÕÏĞ£»
											 //16Î»·ÖÎª4×é 1111/1111/1111/1111
											 //Èç¹û»ú¹¹ÃûÎªADĞÍ£¬ÄÇÃ´È¡×ó±ß8Î»£¬Ã¿4Î»·Ö±ğ¶ÔÓ¦aºÍd
											 //Èç¹û»ú¹¹ÃûÎªABDĞÍ£¬ÄÇÃ´È¡×ó±ß12Î»£¬Ã¿4Î»·Ö±ğ¶ÔÓ¦a b d
											 //Èç¹û»ú¹¹ÃûÎªACDĞÍ£¬ÄÇÃ´È¡×ó±ß12Î»£¬Ã¿4Î»·Ö±ğ¶ÔÓ¦a c d
											 //Èç¹û»ú¹¹ÃûÎªABCDĞÍ£¬ÄÇÃ´È¡16Î»£¬Ã¿4Î»·Ö±ğ¶ÔÓ¦a b c d
											 //a b c dµÄ×î´óÖµÎª16
											 //a b c dµÄÖµ·Ö±ğ±íÊ¾ÔÚwordlistÖĞÆä¶ÔÓ¦µÄWORD_ITEM½ÚµãÏà¶ÔÓÚÉÏÒ»¸öWORD_ITEMµÄÆ«ÒÆ
											 
											 //Èç£º±±¾©ÊĞ¹«½»¹«Ë¾Îª ACDÀàĞÍµÄ»ú¹¹Ãû
											 
											 // 0                                          1                        2           3             4               5
											 //±±¾©ÊĞ¹«½»¹«Ë¾(0,7) ±±¾©ÊĞ(0,3)  ±±¾©(0,2) ÊĞ(2,1) ¹«½»(3,2) ¹«Ë¾(5,2)
											 //64                                      192                 128            128         128          128
											 
											 //±±¾©ÊĞ¹«½»¹«Ë¾(0,7)µÄ3¸öº¢×Ó·Ö±ğA=±±¾©ÊĞ(0,3), C=¹«½»(3,2), D=¹«Ë¾(5,2)
											 //a =1-0=1, c=4-1=3, d=5-4=1
											 
											 //Òò´Ë£¬"±±¾©ÊĞ¹«½»¹«Ë¾"µÄchildnodes = a<<12 | c<<8 | d<<4

#ifdef DOMAIN_SEMANTIC //Êä³ö·Ö´Ê½á¹û½ÚµãµÄÁìÓòÊôĞÔºÍÓïÒåÊôĞÔ 20070310 
   unsigned char domainnum; //¸ÃtermÓĞ¼¸¸öÁìÓò
   unsigned short domainid[TERM_MAX_DOMAIN_NUM]; //ÁìÓòÊôĞÔ
	
   unsigned char semanticnum; //¸ÃtermÓĞ¼¸¸öÓïÒåÊôĞÔ
   unsigned short semanticid[TERM_MAX_SEMANTIC_NUM]; //ÓïÒåÊôĞÔ
#endif

   unsigned char wordvalue;
   //µ±¶Ôquery·ÖÎöÊ±ÓÃ, wordvalueµÄËµÃ÷:
   //¸ß4Î»±íÊ¾¸Ã´Êµ¥ÔªµÄÀàĞÍ µÍ4Î»±íÊ¾¸ÃÀàĞÍÖĞ´ÊµÄÈ¨ÖØ
   // (0000 00) ÒÉÎÊ´Ê  Ê®½øÖÆ·¶Î§(0)
   // (0001 00**) ¹Ø¼üÃû´Ê Ê®½øÖÆ·¶Î§(4-7) 
   // (0010 00**) ¹Ø¼ü¶¯´Ê Ê®½øÖÆ·¶Î§(8-11)
   // (0011 00**) ÆäËû¹Ø¼ü´Ê Ê®½øÖÆ·¶Î§12-15)
   // (0100 00**) ÆäËû´Ê Ê®½øÖÆ·¶Î§(16-19)
   // (0101 00**) Í£ÓÃ´Ê Ê®½øÖÆ·¶Î§(20-23)
   //µ±¶ÔÎÄ±¾·ÖÎöÊ±£¬wordvalue×Ö¶ÎµÄÖµÁíÍâ½âÎö 20070418
  
   unsigned char idf;          //idf * 10, È¡Öµ·¶Î§ÔÚ0-255£¬ÓÃµÄÊ±ºòĞèÒªÔÙ³ıÒÔ10 
   //unsigned long long reserved;

}WORD_ITEM;

typedef struct {
	WORD_ITEM word_item;
	unsigned int appwordPos;		  //ÆğÊ¼ÓëwordlistÊı×éµÄµÚ¼¸¸ö´Ê£¬¶ø²»ÊÇµÚ¼¸¸ö×Ö·û
	//unsigned short appwordnum;	 //Length:  °üº¬µÄ·Ö´Ê½á¹ûµÄ´Ê¸öÊı.¸Ã×Ö¶ÎÔİÊ±Ò»Ö±=0
	
} APP_WORD_ITEM;//Ó¦ÓÃ²ãµÄterm

#define LIST_MAX_CHILD_NUM 10

typedef struct{
	WORD_ITEM *word;
	unsigned int childnum;
	unsigned int children[LIST_MAX_CHILD_NUM];//×Ó½áµãµÄ´ÊĞò
}LIST_ITEM;


/* ·Ö´ÊÇëÇóÓë½á¹ûÊı¾İ½á¹¹¶¨Òå */
typedef struct {
	int buflen;					//¶ÔÏó·ÖÅä¿Õ¼ä´óĞ¡
	char textfmt[10];		//ÇëÇó·Ö´ÊµÄÎÄ×Ö¸ñÊ½£¬"GBK","UNICODE"
	
	int len_uni;				//Unicode¸ñÊ½µÄÎÄ×Ö³¤¶È
	unsigned short* uni;//Unicode¸ñÊ½µÄÎÄ×ÖÄÚÈİ
	
	int word_num;				//µÃµ½µÄ·Ö´Ê½á¹û¸öÊı
	WORD_ITEM *wordlist;		//·Ö´Ê½á¹ûÁĞ±í
	
	int appword_num;				//µÃµ½µÄÓ¦ÓÃ²ã term¸öÊı
	APP_WORD_ITEM *appwordlist;		//Ó¦ÓÃ²ã termÁĞ±í
	
	int listitem_num;
	LIST_ITEM *list;

	DocInfo  docprop;    //Ó¦ÓÃ²ã·ÖÎöºó¡¡µÃµ½µÄ¶ÔÕâÆªÎÄµµµÄÁìÓòÃèÊöĞÅÏ¢
	KeyWordStore keywords; //Ó¦ÓÃ²ã·ÖÎöºó¡¡µÃµ½µÄ¶ÔÕâÆªÎÄµµµÄ¹Ø¼ü´Ê£¬ÈËÃû µØÃû ÊÂ¼ş ×¨Ö¸
	
}WORD_SEGMENT;


#ifdef __cplusplus
extern "C" {
#endif
/*	¶Ô´Êµä²Ù×÷µÄÏà¹Øº¯Êı */

/**
* º¯Êı½éÉÜ£º	´ò¿ªÒ»¸ö´Êµä¿â
* ÊäÈë²ÎÊı£º	rdir		´Êµä¿â¸ùÄ¿Â¼
* Êä³ö²ÎÊı£º 
* ·µ»ØÖµ  £º	lexicon		´Êµä¿âÃèÊöĞÅÏ¢£¬NULL±íÊ¾´ò¿ªÊ§°Ü
*/
LEXICON *OpenLexicon(const char *rdir);

/**
* º¯Êı½éÉÜ£º	´ò¿ªÒ»¸ö´Êµä¿â
* ÊäÈë²ÎÊı£º	rdir		´Êµä¿â¸ùÄ¿Â¼£¬´øÑ¡ÔñµÄload×ÊÔ´
* Êä³ö²ÎÊı£º 
* ·µ»ØÖµ  £º	lexicon		´Êµä¿âÃèÊöĞÅÏ¢£¬NULL±íÊ¾´ò¿ªÊ§°Ü
* option      ¾ö¶¨loadÄÄĞ©×ÊÔ´
*                 16Î»
*                       ·ÖÀà   ¹Ø¼ü´ÊÌáÈ¡   ¶ÌÓïºÏ²¢   ´ÊĞÔ±ê×¢   ·Ö´Ê
* 00 000 000 00         1          1             1         1       1
* 
* ×¢ÒâÊÂÏî:
* Load×ÊÔ´µÄ²ÎÊıÑ¡ÏîÓëAnalyzeµÄ²ÎÊıÑ¡ÏîÊÇÏà¶Ô¶ÀÁ¢µÄ¡£µ«ÊÇÈç¹ûÖ»loadÁË·Ö´ÊµÄ×ÊÔ´£¬È»ºóÒªÇó¼È·Ö´ÊÓÖ´ÊĞÔ±ê×¢Ê±¾ÍÓĞÎÊÌâ
* 
*/
LEXICON *OpenLexicon_Opt(const char *rdir, unsigned short option);

/**
* º¯Êı½éÉÜ£º	±£´æ´Êµä¿âÊôĞÔÊı¾İ
* ÊäÈë²ÎÊı£º	lexicon		´Êµä¿âÃèÊöĞÅÏ¢£¬Ö¸Õë
* Êä³ö²ÎÊı£º 
* ·µ»ØÖµ  £º	0			³É¹¦
*				<0			Ê§°Ü
*/
int SaveLexicon(LEXICON *lexicon);

/**
* º¯Êı½éÉÜ£º	¹Ø±ÕÒ»¸ö´Êµä¿â
* ÊäÈë²ÎÊı£º	lexicon		´Êµä¿âÃèÊöĞÅÏ¢£¬Ö¸Õë
* Êä³ö²ÎÊı£º 
* ·µ»ØÖµ  £º	0			³É¹¦
*				<0			Ê§°Ü
*/
int CloseLexicon(LEXICON *lexicon);

/**
* º¯Êı½éÉÜ£º	´Ó´ÊµäÖĞĞŞ¸ÄÖ¸¶¨È±Ê¡Ê¹ÓÃµÄÓïÖÖ
* ÊäÈë²ÎÊı£º	lexicon		´Êµä¿âÃèÊöĞÅÏ¢£¬Ö¸Õë
*				ws_sel		0Îª¼òÌå£¬1Îª·±Ìå
* Êä³ö²ÎÊı£º 	
* ·µ»ØÖµ  £º	0			³É¹¦
*				<0			Ê§°Ü
*/
int setLexiconWS(LEXICON *lexicon, int ws_sel);

/**
* º¯Êı½éÉÜ£º	·ÖÅä²¢³õÊ¼»¯Ò»¸ö±£´æ·Ö´Ê½á¹ûµÄÊı¾İ¶ÔÏó
* ÊäÈë²ÎÊı£º	buflen		Ô¤·ÖÅäµÄ¿Õ¼ä´óĞ¡
* Êä³ö²ÎÊı£º 
* ·µ»ØÖµ  £º	word_segment			³É¹¦
*				NULL					Ê§°Ü
*/
WORD_SEGMENT*  InitWordSeg(int buflen);

/**
* º¯Êı½éÉÜ£º	ÊÍ·ÅÒ»¸ö·Ö´Ê½á¹û¶ÔÏó
* ÊäÈë²ÎÊı£º	word_segment		·Ö´Ê½á¹û¶ÔÏó
* Êä³ö²ÎÊı£º 
* ·µ»ØÖµ  £º	0			³É¹¦
*				<0			Ê§°Ü
*/
int FreeWordSeg(WORD_SEGMENT *word_segment);

/**
* º¯Êı½éÉÜ£º	ÀûÓÃ´Êµä·ÖÎöÎÄ±¾´®£¬¸ù¾İĞèÒªµÃµ½·Ö´Ê½á¹ûÊı¾İ½á¹¹£¨¿ÉÒÔ°üÀ¨·Ö´Ê½Ó¿Ú½á¹û¡¢·Ö´Ê½á¹ûÁĞ±í£©
* ÊäÈë²ÎÊı£º	lexicon		´Êµä¿âÃèÊöĞÅÏ¢£¬Ö¸Õë

				text		±»·ÖÎöµÄÎÄ±¾£¬'\0'½áÊøµÄ×Ö·û´®
				
				textfmt	ÎÄ±¾µÄ¸ñÊ½
				Ä¿Ç°ÊäÈëÎÄ±¾¿ÉÒÔÊÇ"GBK"»òÕß"UNICODE"±àÂë
				°®ÎÊ·Ö´ÊÏµÍ³²»Ö§³ÖBIG5 ±àÂë¡£Ò²²»Ö§³ÖÆäËü±àÂë¡£
				
				8×Ö½ÚµÄflags (unsigned long long)
				0xFFFF ¡¡¡¡¡¡¡¡¡¡ FFFF ¡¡¡¡¡¡¡¡¡¡¡¡FFFF ¡¡¡¡¡¡¡¡¡¡¡¡¡¡FFFF
				lexicon.hÑ¡Ïî     Ó¦ÓÃ²ãÑ¡Ïî       ´ÊĞÔ±ê×¢Ñ¡Ïî¡¡¡¡   ·Ö´ÊÑ¡Ïî
				
				·Ö´Ê²ÎÊıÑ¡ÏîÇë¼ûBaseDef.h               (#define SEG_OPT_ALL 0x7fff)
				´ÊĞÔ±ê×¢²ÎÊıÑ¡ÏîÇë¼ûPOSTagger_new.h     (#define POSTAGGER_OPT_ALL 0x7fff)
				Ó¦ÓÃ²ã²ÎÊıÑ¡ÏîÇë¼ûphraseterm.h          Èç¹ûÁìÓò·ÖÀà£¬Ó¦ÓÃ²ã²ÎÊı¿ÉÒÔÉèÖÃÎª¡¡0x0003
				
				lexicon.h²ÎÊıÑ¡Ïî
				0x0001 ·Ö´Ê
				0x0002 ·Ö´Ê + ´ÊĞÔ±ê×¢
				0x0004 ·Ö´Ê + ´ÊĞÔ±ê×¢ + Ó¦ÓÃ²ã
				0x0008 ·Ö´Ê + Ó¦ÓÃ²ã
				
				0x0100 ¶Ô´æÔÚ½á¹¹ĞÅÏ¢µÄ´ÊÉú³ÉÏ¸Á£¶È·Ö´Ê½á¹û
				0x0010 ¶ÔÉú³ÉµÄÏ¸Á£¶È·Ö´Ê½á¹û ¸ø³öwordcat posidµÈĞÅÏ¢(È±Ê¡Çé¿öÏÂ£¬Ã»ÓĞÕâĞ©ĞÅÏ¢)
				
				Ö»ÓĞÔÚlexicon.h²ÎÊıÑ¡ÏîÖĞ°üº¬ÁËÄ³Ò»Ä£¿éÊ±£¬¸ÃÄ£¿é¶ÔÓ¦µÄ²ÎÊı²ÅÆğ×÷ÓÃ
				
				ÎªÁË¼òµ¥Æğ¼û£¬µ±µÍ£¶×Ö½ÚÎª ¿Õ Ê±£¬¾ÍÊÇÄ¬ÈÏÖµ£®Ö»ĞèÒªÉè¶¨×î¸ßµÄ¶ş×Ö½Ú¼´¿É
								
* Êä³ö²ÎÊı£º 	word_segment	±£´æ·Ö´Ê½á¹ûµÄÊı¾İ¶ÔÏó£¬ĞèÒªÔ¤ÏÈ³õÊ¼»¯
*¡¡¡¡¡¡¡¡¡¡¡¡¡¡docprop¡¡¡¡¡¡Êä³ö¸ÃÎÄµµµÄÊôĞÔĞÅÏ¢£¨ÁìÓò¡¡Ê±Ğ§ĞÔ£©
* ·µ»ØÖµ  £º	0	³É¹¦
*				         <0	Ê§°Ü
*/
//µÚÒ»¸öº¯Êı½ÓÊÜµÄÊäÈëÎª×Ö·û´®£¬²»Ö§³Öunicode;¸Ãº¯ÊıÊÇÎªÁË¼æÈİÀÏ°æ±¾
int AnalyTextWithLex(LEXICON *lexicon, const char *text, char *textfmt, WORD_SEGMENT* word_segment, unsigned long long flags);

//µÚ¶ş¸öº¯Êı½ÓÊÜµÄÊäÈëÎª×Ö·û´®»òunicode´®
//Èç¹ûÊÇUNICODE´®£¬µ÷ÓÃÕß×ª»»Îªchar*£¬³¤¶ÈÎª×Ö½Ú³¤¶È
//ÒÔºó¾ÍÎ¬»¤¸Ãº¯Êı
//»ñÈ¡ÎÄ±¾·ÖÎöÒÔºóµÄÁìÓò·ÖÀà£¬¿´phraseterm.h

//textsrcid 
//0-Î´Öª 1-ĞÂÎÅ 2-²©¿Í 3-ÖªÊ¶ÈË 4-ÂÛÌ³ 5-¹ã¸æ
int AnalyTextWithLex2(LEXICON *lexicon, const char *text, int iByteLen, char *textfmt, WORD_SEGMENT* word_segment, unsigned long long flags, unsigned char textsrcid);

//´Ëº¯ÊıÓÃÓÚÄÃÌØÕ÷ÏòÁ¿Ö±½ÓÊ¹ÓÃK·ÖÀàÆ÷¥×ö·ÖÀà
//texttype 0-ĞÂÎÅÀàÎÄÌå 1-ËæÒâÎÄÌå
//classifier "KNN" "SVM" "ME"
//vec "feat:val\tfeat:val\t ... feat:val";
//int ClsVecWithModel(LEXICON *lexicon, const char *vec, int veclen, int texttype, const char *classifier, DocInfo *docprop);

//¶ÔÎÄ±¾½øĞĞ·Ö´Ê/¶ÌÓïºÏ²¢/²¢½øĞĞÎÄ±¾ÀàĞÍµÄÅĞ¶¨(ÊÇ·ñÊÇÕĞÆ¸ÀàĞÍÒ³Ãæ£¬ÊÇ·ñÊÇ¹«Ë¾¼ò½éÒ³Ãæ)
//ÀıÈç ÓÃÓÚÕĞÆ¸ËÑË÷ÖĞÒ³ÃæµÄÅĞ¶¨
//2007.11.18 sunjian@staff.sina.com.cn
//»ñÈ¡ÎÄ±¾·ÖÎöÒÔºóµÄÒ³ÃæÀàĞÍ£¬¿´classifiy.h
//unsigned long long flagsÔİÊ±Ã»ÓĞÓÃ£¬ÎªÒÔºóÔ¤Áô
//²»¼ÇÂ¼·Ö´Ê½á¹û
//* ·µ»ØÖµ  £º	0	     ³É¹¦
//*				     <0	Ê§°Ü

#define PAGE_CLASS_NUM 10
//result[0] - Ö°Î»ÕĞÆ¸ ÅĞ±ğ½á¹û 
//result[1] - ¹«Ë¾¼ò½é ÅĞ±ğ½á¹û
int AnalyTextAndClassify(LEXICON *lexicon, 
										const char *text,
										int iByteLen,
										char *textfmt,
										WORD_SEGMENT* word_segment,
										unsigned long long flags,
										PAGE_CLASS_RESULT result[]);

//¶Ôquery½øĞĞÇĞ·Ö //2006.12.31 sunjian@staff.sina.com.cn
int AnalyQueryWithLex2(LEXICON *lexicon, const char *text, int iByteLen, char *textfmt, WORD_SEGMENT* word_segment, unsigned long long flags);


/**
* º¯Êı½éÉÜ£º	´Ó´ÊµäÖĞĞŞ¸ÄÖ¸¶¨´ÊµÄstat
* ÊäÈë²ÎÊı£º	lexicon		´Êµä¿âÃèÊöĞÅÏ¢£¬Ö¸Õë
*				wordID		´ÊID
*				stat		´Ê×´Ì¬
* Êä³ö²ÎÊı£º 	
* ·µ»ØÖµ  £º	0			³É¹¦
*				1			´Ê²»´æÔÚ
*				<0			Ê§°Ü
*/
int setWordStat(LEXICON *lexicon, unsigned long long wordID, BYTE stat);

/**
* º¯Êı½éÉÜ£º	´Ó´ÊµäÖĞĞŞ¸ÄÖ¸¶¨´ÊµÄtype
* ÊäÈë²ÎÊı£º	lexicon		´Êµä¿âÃèÊöĞÅÏ¢£¬Ö¸Õë
*				wordID		´ÊID
*				type		´ÊÀàĞÍ
* Êä³ö²ÎÊı£º 	
* ·µ»ØÖµ  £º	0			³É¹¦
*				1			´Ê²»´æÔÚ
*				<0			Ê§°Ü
*/
int setWordType(LEXICON *lexicon, unsigned long long wordID, BYTE type);

/**
* º¯Êı½éÉÜ£º	´Ó´ÊµäÖĞĞŞ¸ÄÖ¸¶¨´ÊµÄIDF
* ÊäÈë²ÎÊı£º	lexicon		´Êµä¿âÃèÊöĞÅÏ¢£¬Ö¸Õë
*				wordID		´ÊID
*				IDF			´ÊµÄIDFÖµ
* Êä³ö²ÎÊı£º 	
* ·µ»ØÖµ  £º	0			³É¹¦
*				1			´Ê²»´æÔÚ
*				<0			Ê§°Ü
*/
int setWordIDF(LEXICON *lexicon, unsigned long long wordID, WORD IDF);

/**
* º¯Êı½éÉÜ£º	´Ó´ÊµäÖĞ²éÕÒÒ»¸ö´ÊÊÇ·ñ´æÔÚ£¬Èç¹û´æÔÚ£¬·µ»Ø´ÊID
* ÊäÈë²ÎÊı£º	lexicon		´Êµä¿âÃèÊöĞÅÏ¢£¬Ö¸Õë
*				      word		´Ê
* Êä³ö²ÎÊı£º 	wordID		´ÊID£¬µ±´Ê²»´æÔÚÊ±ÎªËæ»úÖµ
* ·µ»ØÖµ  £º	0			´Ê´æÔÚ
*				1			´Ê²»´æÔÚ
*				<0			Ê§°Ü
*/
int getWordID(LEXICON *lexicon, char *word, unsigned long long *wordID);

/**
* º¯Êı½éÉÜ£º	´Ó´ÊµäÖĞ²éÕÒÒ»¸ö´ÊÊÇ·ñ´æÔÚ£¬Èç¹û´æÔÚ£¬·µ»Ø´ÊID
* ÊäÈë²ÎÊı£º	lexicon		´Êµä¿âÃèÊöĞÅÏ¢£¬Ö¸Õë
*				word		´Ê
*				wordlen		´Ê³¤
* Êä³ö²ÎÊı£º 	wordID		´ÊID£¬µ±´Ê²»´æÔÚÊ±ÎªËæ»úÖµ
* ·µ»ØÖµ  £º	0			´Ê´æÔÚ
*				1			´Ê²»´æÔÚ
*				<0			Ê§°Ü
*/
int getWordID_U(LEXICON *lexicon, unsigned short *word, int wordlen, unsigned long long *wordID);


/**
* º¯Êı½éÉÜ£º	´Ó´ÊµäÖĞ²éÕÒÒ»¸öwordIDµÄºº×Ö´ÊÊÇ·ñ´æÔÚ£¬Èç¹û´æÔÚ£¬·µ»Ø´Ê´®ÄÚÈİ
* ÊäÈë²ÎÊı£º	lexicon		´Êµä¿âÃèÊöĞÅÏ¢£¬Ö¸Õë
*				wordID		´ÊID
* Êä³ö²ÎÊı£º 	word		´Ê´®ÄÚÈİ£¬ÓÉµ÷ÓÃÕßÌá¹©×ã¹»´óµÄ¿Õ¼ä£¬·µ»Ø×Ö·û´®ÒÔ'\0'½áÊø
* ·µ»ØÖµ  £º	0			¸ÃID´Ê²»´æÔÚ
*				>0			¸ÃID´ÊGBK³¤¶È
*				<0			Ê§°Ü
*/
int getWordStr(LEXICON *lexicon, unsigned long long wordID, char *word);

/**
* º¯Êı½éÉÜ£º	´Ó´ÊµäÖĞÈ¡Ö¸¶¨IDµÄ´ÊµÄIDFÖµ
* ÊäÈë²ÎÊı£º	lexicon		´Êµä¿âÃèÊöĞÅÏ¢£¬Ö¸Õë
*				wordID		´ÊID
* Êä³ö²ÎÊı£º 	
* ·µ»ØÖµ  £º	>=0			IDFÖµ
*				<0			Ê§°Ü
*/
int getWordIDF(LEXICON *lexicon, unsigned long long wordID);

/**
* º¯Êı½éÉÜ£º	´Ó´ÊµäÖĞÈ¡Ö¸¶¨IDµÄ´ÊµÄstatÖµ
* ÊäÈë²ÎÊı£º	lexicon		´Êµä¿âÃèÊöĞÅÏ¢£¬Ö¸Õë
*				wordID		´ÊID
* Êä³ö²ÎÊı£º 	
* ·µ»ØÖµ  £º	>=0			statÖµ
*				<0			Ê§°Ü
*/
int getWordStat(LEXICON *lexicon, unsigned long long wordID);

/**
* º¯Êı½éÉÜ£º	´Ó´ÊµäÖĞÈ¡Ö¸¶¨IDµÄ´ÊµÄtypeÖµ
* ÊäÈë²ÎÊı£º	lexicon		´Êµä¿âÃèÊöĞÅÏ¢£¬Ö¸Õë
*				wordID		´ÊID
* Êä³ö²ÎÊı£º 	
* ·µ»ØÖµ  £º	>=0			typeÖµ
*				<0			Ê§°Ü
*/
int getWordType(LEXICON *lexicon, unsigned long long wordID);


/**2006/06/03
* º¯Êı½éÉÜ£º	¸ù¾İ´ÊĞÔid£¬µÃµ½´ÊĞÔ¶ÔÓ¦µÄ±ê¼Ç
* ·µ»ØÖµ  £º	0		³É¹¦
*						<0		Ê§°Ü
*/
int getPOSTag(LEXICON *lexicon, unsigned char posid, char *postag);

/**2007/12/09
* º¯Êı½éÉÜ£º	µÃµ½¸Ãterm
* ·µ»ØÖµ  £º	>=0			³É¹¦
*							<0			Ê§°Ü
*             TermInfo_touser¿Õ¼äÓÉÓÃ»§Ìá¹©
*/
int getTermInfo(LEXICON *lexicon, unsigned short *word, int wordlen, TermInfo_touser *pTerm);

#ifdef __cplusplus
}
#endif

#define MAX_LEXICON_NUM 10					//ÏµÍ³¿ÉÒÔÊ¹ÓÃµÄ´ÇµäÊıÄ¿

#ifndef MAX_WORDSEGMENT_NUM
#define MAX_WORDSEGMENT_NUM 1			//·Ö´Ê¿â×î´ó¸öÊı
#endif

#ifndef MAX_WORDLIB_NUM
#define MAX_WORDLIB_NUM 8				//Ò»¸ö·Ö´Ê¿âÖĞ×î´óÔØÈë´Ê¿â¸öÊı
#endif

#ifndef MAX_IWORDSEGMENT
#define MAX_IWORDSEGMENT 1			//×î´ó¿ÉÓÃ·Ö´Ê½Ó¿Ú¸öÊı£¬¼´Ïß³ÌÊı
#endif

#ifndef MAX_WORDNUM
#define MAX_WORDNUM 16777216			//Ò»¸ö·Ö´Ê¿âÖĞ×î´ó´ÊÊı
#endif

#ifndef MAX_WORD_ID
#define MAX_WORD_ID 16777215			//´ÊµäÖĞµÄ×î´ówordID
#endif

#ifndef STATIC_DIC_MAX_WORD_ID
#define STATIC_DIC_MAX_WORD_ID 2621439	//´Ê¿âÖĞµÄ´ÊÌõ·ÖÎªÁ½¸öÇø:[0~2621440); [2621440~16777216)
#endif


#ifdef __cplusplus

/* ·Ö´ÊÏµÍ³µ¥Ò»×ÖÌå·ÖÎöÊı¾İµÄÊı¾İ½á¹¹¶¨Òå */
typedef struct {
	int exist;					//ÊÇ·ñ´æÔÚ£¬0--²»´æÔÚ£¬1--´æÔÚ
	char fn_libgpws[100];		//·Ö´Ê½Ó¿Úº¯Êı¿â
	HINSTANCE hWS;				//DLL¾ä±ú
	ENDWORDSEGMENT EndWordSegment_d;
	COINITWORDSEGMENT CoInitWordSegment_d;
	
	CREATEWORDLIBINSTANCE CreateWordLibInstance_d;
	CREATEWORDSEGMENTINSTANCE CreateWordSegmentInstance_d;
	CREATEPOSTAGGERINSTANCE   CreatePOSTaggerInstance_d;
	CREATEPHRASETERMINSTANCE  CreatePhraseTermInstance_d;
	CREATEIASKEXKEYWORDSINSTANCE  CreateKeywordsInstance_d;

	CREATECLASSIFYKNNINSTANCE CreateClassifyKnnInstance_d;
	//CREATECLASSIFYDOMAININSTANCE  CreateClassifydomainInstance_d;	
	CREATECLASSIFYINSTANCE  CreateClassifierInstance_d;
	
	RELEASEWORDLIB ReleaseWordLib_d;
	RELEASESEGMENT ReleaseSegment_d;
	RELEASEPOSTAGGER ReleasePOSTagger_d;
	RELEASEPHRASETERM ReleasePhraseTerm_d;
	RELEASEIASKEXKEYWORDS ReleaseKeywords_d;
	//RELEASECLASSIFYDOMAIN ReleaseClassifydomain_d;
	RELEASECLASSIFYKNN ReleaseClassifyKnn_d;
	RELEASECLASSIFY ReleaseClassifier_d;
	
	char dir_segdata[100];	//´Ê¿âÎ»ÖÃ
	int wordlibnum;				//´Ê¿â¸öÊı
	
	IWordLib* wl[MAX_WORDLIB_NUM];	//´Ê¿âÁĞ±í
	IWordSegment* ws[MAX_IWORDSEGMENT];	//·Ö´Ê½Ó¿Ú
	IPOSTagger* postagger[MAX_IWORDSEGMENT];	//´ÊĞÔ±ê×¢½Ó¿Ú
	IPhraseTerm* chunker[MAX_IWORDSEGMENT];	//¶ÌÓïºÏ²¢½Ó¿Ú
	IKeywords* keyworder[MAX_IWORDSEGMENT];	//¹Ø¼ü´ÊÌáÈ¡½Ó¿Ú
	Classify_KNN *pknnclassifier[MAX_IWORDSEGMENT];

	//ClassifyDomain* classifydomainer[MAX_IWORDSEGMENT];	//ÍøÒ³ÁìÓò·ÖÀà½Ó¿Ú
	Classify* classifier[MAX_IWORDSEGMENT];	//ÍøÒ³·ÖÀàµÄ½Ó¿Ú
	
	int ws_stat[MAX_IWORDSEGMENT];		//·Ö´Ê½Ó¿Ú×´Ì¬ 0:±íÊ¾Ã»ÓĞ±»Õ¼ÓÃ£»£±±íÊ¾ÒÑ¾­±»Õ¼ÓÃ
	
	WORD_PROPERTY *wordprop;		//´ÊÊôĞÔ
	
	IWordLib* lexicon_level_wordLib;
	//20070327 ÓÃÓÚÆÁ±Î·Ö´Ê½á¹ûµÄid£¬ËùÒÔÔÚÕâÒ»²ãÎ¬»¤ÁËÒ»¸ö´Ê->wordidµÄÓ³Éä, Õâ¸öwordid¾ÍÓÃÓÚË÷ÒıÖĞ
	
} WORD_SEGMENT_LIB;

/* ´ÊµäÃèÊöĞÅÏ¢Êı¾İ²¿·ÖÊı¾İ½á¹¹¶¨Òå */
typedef struct {
	int wordsegment_num;			//·Ö´ÊÏµÍ³ÓïÖÖÊıÄ¿
	WORD_SEGMENT_LIB wordsegment[MAX_WORDSEGMENT_NUM];	//¼òÌå£¨0£©ºÍ·±Ìå£¨1£©·ÖÎöÏµÍ³Êı¾İ
}LEXICON_DATA;

extern LEXICON_DATA *gp_lexdata[MAX_LEXICON_NUM];		//´ÊµäÊı¾İÖ¸Õë

#endif //end __cplusplus

#endif //end LEXICON_H
