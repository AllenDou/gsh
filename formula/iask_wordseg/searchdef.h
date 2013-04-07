/**
 * Copyright (c) 2003, 新浪网研发中心
 * All rights reserved.
 *
 * 文件名称：searchdef.h
 * 摘    要：定义常用的数据类型，以便于夸平台编译
 * 作    者：徐绿兵 2003/9/24
 * 版    本：1.0
 * $Id: searchdef.h,v 1.1 2005/01/24 01:34:11 zhonghua Exp $
 */

#ifndef SEARCH_DEF_H
#define SEARCH_DEF_H

/* 标准宽度类型定义 */
typedef unsigned char      BYTE;
typedef unsigned short     WORD;
typedef unsigned long      DWORD;
typedef unsigned long long ULONGLONG;

/* 条目ID专用的数据类型，根据需要重新定义 */
#ifndef DOCID_T
#define DOCID_T DWORD
#endif

/* Value值专用的数据类型，根据需要重新定义 */
#ifndef VALUE_T
#define VALUE_T WORD
#endif
/* 常用类型定义 */
typedef int            INT;
typedef unsigned int   UINT;
typedef unsigned long  ULONG;
typedef unsigned short USHORT;
//typedef unsigned char  UCHAR;

/* 布尔型定义 */
typedef int    BOOL;

#ifndef FALSE
#define FALSE  0
#endif

#ifndef TRUE
#define TRUE  -1
#endif

#endif
