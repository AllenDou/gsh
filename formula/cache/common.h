#ifndef _COMMON_H_
#define _COMMON_H_

#include <error.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <time.h>
#include <stdarg.h>
#include <sys/types.h>
#include <syslog.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>
#include <math.h>
#include <sys/time.h>
#include "rbtree.h"
//#include "sds.h"
//#include "fmacros.h"
//#include "zmalloc.h"
//#include "datatype.h"
#include <unistd.h>
#include <uuid/uuid.h>

#define L fprintf(stderr,"%s %u\n",__FILE__,__LINE__);
#define ASSERT(x) \
		do{ \
				if(!(x)){ \
						printf("\nFailed ASSERT [%s] (%d):\n     %s\n\n", __FILE__,  __LINE__,  #x); \
						exit(1); \
				}\
		}while(0)
#define MAX_LOGMSG_LEN 4096
/* Log levels */
#define REDIS_DEBUG 0
#define REDIS_VERBOSE 1
#define REDIS_NOTICE 2
#define REDIS_WARNING 3
unsigned int aline(unsigned int n);
long long ustime(void);
long long memtoll(const char *p, int *err);
off_t get_file_size(char *fname);
void printLog(int level, const char *fmt, ...); 
int stringmatchlen(const char *p, int plen, const char *s, int slen, int nocase);
int stringmatch(const char *p, const char *s, int nocase);
int ll2string(char *s, size_t len, long long value);
int string2ll(char *s, size_t slen, long long *value);
int string2l(char *s, size_t slen, long *value);
int d2string(char *buf, size_t len, double value);
char *itoa_36ns_6c(unsigned int num, char *buf, int buf_len); 

#endif
