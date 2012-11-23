#ifndef __HASH_H__
#define __HASH_H__
#include "common.h"
#include <sys/types.h>
unsigned int dictIntHashFunction(unsigned int key);
unsigned int dictIdentityHashFunction(unsigned int key);
unsigned int dictGenHashFunction(const unsigned char *buf, int len);
unsigned int dictGenHashFunction_mur(const void *key, int len);
unsigned int dictGenCaseHashFunction(const unsigned char *buf, int len);
unsigned int ELFhash(char *key);
unsigned int lh_strhash(void *src);
unsigned long HashString(char *lpszFileName, unsigned long dwHashType);
unsigned int jhash2(const unsigned int *k, unsigned int length, unsigned int initval);
unsigned int ngx_murmur_hash2(unsigned char *data, size_t len);
unsigned int RSHash(char* str, unsigned int len); 
unsigned int JSHash(char* str, unsigned int len); 
unsigned int PJWHash(char* str, unsigned int len);  
unsigned int BKDRHash(char* str, unsigned int len);   
unsigned int SDBMHash(char* str, unsigned int len);   
unsigned int DJBHash(char* str, unsigned int len);  
unsigned int DEKHash(char* str, unsigned int len);  
unsigned int BPHash(char* str, unsigned int len); 
unsigned int FNVHash(char* str, unsigned int len);  
unsigned int APHash(char* str, unsigned int len); 
#endif
