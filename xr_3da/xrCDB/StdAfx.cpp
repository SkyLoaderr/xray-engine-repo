// stdafx.cpp : source file that includes just the standard includes
//	xrCDB.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"
#pragma hdrstop

#ifdef __BORLANDC__
	#pragma comment(lib,"xrCoreB.lib")
	#pragma comment(lib,"winmm.lib")
#else
	#pragma comment(lib,"x:\\xrCore")
	#pragma comment(lib,"winmm")
#endif
// TODO: reference any additional headers you need in STDAFX.H
// and not in this file
