/**
 * 把某种内码的字符串转换为Unicode
 * Xu Lubing
 */
#ifndef _TO_UNI_H
#define _TO_UNI_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * 把字节流转换成Unicode
 * 参数:
 *   src:    源字符串，其内码由enc指定
 *   srcLen: src的长度
 *   uni:    存放转换后的Unicode的空间
 *   size:   uni的个数，[必须满足：size >= srcLen]
 *   enc:    src的内码，支持的内码有"GBK"/"Big5"/"UTF-8"/"8859_1"
 * 返回:
 *   <0: 不支持所给的内码
 *   =0: 所给的参数有问题
 *   >0: 在uni中的可用的Unicode字符数。
 *   [注]从UTF-8转换为Unicode时，如果存在非法UTF-8字符，相应的Unicode值为"?"
 */
int bytesToUni(const char* src, int srcLen, unsigned short* uni, int size, const char* enc);

/**
 * 把字符串转为Unicode字符串的[简易]函数，可用于需要Unicode串运算的各类函数
 * 参数:
 *   us:  保存Unicode串的地址，结果会以0结尾。空间必须可以放得下s对应的Unicode串，不做越界处理
 *   s:   对应的C语言普通字符串，以'\0'结尾
 *   enc: s对应的内码，支持的内码有"GBK"/"Big5"/"UTF-8"/"8859_1"，不支持的内码或NULL都视为"8859_1"
 * 返回:
 *   指向us的指针
 * [使用建议]
 *   本函数可以看作Unicode字符串常量，可以配合"ustring.h"里的函数使用
 *   例: #include "ustring.h"
 *       UCHAR u[1024];
 *       ustrlen(US(u, "中国", "GBK"));          // 对应以前的 strlen("中国")
 *       ustrstr(unistr, US(u, "中国", "GBK"));  // 对应以前的 strstr(str, "中国")
 *   当然如果一个串多次使用，为了减少转换次数，可以保存转换后的结果，多次使用
 *       US(u, "中国", "GBK");
 *       ustrlen(u);
 *       ustrstr(unistr, u);
 */
unsigned short* US(unsigned short* us, const char* s, const char* enc);

#ifdef __cplusplus
}
#endif

#endif
