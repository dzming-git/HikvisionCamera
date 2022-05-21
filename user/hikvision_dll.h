#pragma once
#ifndef _HIKVISION_DLL_H_
#define _HIKVISION_DLL_H_
#ifdef LIBDLL
#define LIBDLL _declspec(dllimport)
#else
#define LIBDLL _declspec(dllexport)
#endif



#endif // _HIKVISION_DLL_H_

