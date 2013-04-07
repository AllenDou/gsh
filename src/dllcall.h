/**
 * DLL使用。可以在Unix和Win32平台通用
 *
 * 使用方法：
 *    HINSTANCE hLib;                  //DLL句柄
 *    void (*dll_bar)(const char* s);; //DLL中的函数原型，由实际的DLL中的函数为准
 *
 *    // 加载DLL
 *    hLib = LoadLibrary("DLL名称");
 *    if (hLib == NULL)
 *    {
 *         //加载DLL失败
 *    }
 *
 *    dll_bar = (void (*)(const char*))GetProcAddress(hLib, "函数名");
 *    if (dll_bar == NULL)
 *    {
 *         //取函数地址失败
 *    }
 *    
 *    // 调用函数
 *    dll_bar("string");
 *
 *    // 卸载DLL
 *    FreeLibrary(hLib);
 *
 * Xu Lubing
 * Dec. 23, 2003
 */
#ifndef DLL_CALL_H
#define DLL_CALL_H

#ifndef WIN32
/* Unix平台 */
typedef void* HINSTANCE;
typedef void* HMODULE;
#else
/* Win32平台 */
#include <windows.h>
#endif /* WIN32 */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef WIN32
/**
 * 加载DLL
 * 参数:
 *   lpLibFileName: DLL的名称
 * 返回:
 *   NULL: 加载给定的DLL失败
 *   非NULL: 加载DLL成功，返回值存放DLL的句柄
 */
HINSTANCE LoadLibrary(const char* lpLibFileName);

/**
 * 获得DLL中某个函数的指针
 * 参数:
 *   hModule: 调用LoadLibrary返回的DLL句柄
 *   lpProcName: 需要获取的函数的名称
 * 返回:
 *   NULL: 取函数指针失败
 *   非NULL: 成功，返回值存放所需的函数指针
 */
void* GetProcAddress(HINSTANCE hModule, const char* lpProcName);

/**
 * 卸载DLL
 * 参数:
 *   hModule: 调用LoadLibrary返回的DLL句柄
 * 返回:
 *   0: 失败
 *   非0: 卸载成功
 */
int FreeLibrary(HINSTANCE hLibModule);
#endif

/**
 * 取上一次调用DLL函数的错误信息
 * 参数:
 *   lpErrMsg: 用于存放错误信息
 * [注意] 调用该函数获得的错误信息，请务必调用FreeErrMsg释放
 */
void LastDLLError(char** lpErrMsg);

/**
 * 释放错误信息
 * 参数：
 *   lpErrMsg: 调用LastDLLError得到的错误信息
 */
void FreeErrMsg(char* lpErrMsg);

#ifdef __cplusplus
}
#endif

#endif
