/**
 * 把Unicode转换为某种内码的字符串
 * Xu Lubing
 */
#ifndef _FROM_UNI_H
#define _FROM_UNI_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * 把Unicode转换成字节流
 * 参数:
 *   uni:    unicode串
 *   uniLen: uni的长度
 *   str:    存放转换后的字符串空间，内码由enc指定
 *   size:   str的个数，必须满足size >= uniLen*系数(GBK/Big5: 2; UTF-8: 3, 8859_1: 1)
 *   enc:    转换的内码，支持的内码有"GBK"/"Big5"/"UTF-8"/"8859_1"
 * 返回:
 *   <0: 不支持所给的内码
 *   =0: 所给的参数有问题
 *   >0: 在str中的可用的字符数。
 *   [注]从Unicode转换为GBK/Big5时，如果无对应的内码，相应的值为"?"
 */
int uniToBytes(const unsigned short* uni, int uniLen, char* str, int size, const char* enc);

#ifdef __cplusplus
}
#endif

#endif
