#pragma once

#ifdef _EXPORTS_ //定义了该宏的才为导出，否则均为导入
#define DLL_EXPORT _declspec(dllexport)
#else
#define DLL_EXPORT _declspec(dllimport)
#endif