// stdafx.cpp : source file that includes just the standard includes
// xrSound.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file

#ifdef __BORLANDC__
    #pragma comment(lib,	"xrCoreB.lib"		)
    #pragma comment(lib,	"xrCDBB.lib"		)
    #pragma comment(lib,	"dsound.lib" 		)
    #pragma comment(lib,	"msacm32.lib"		)
#else
    #pragma comment(lib,	"x:\\xrCore"		)
    #pragma comment(lib,	"x:\\xrCDB"			)
    #pragma comment(lib,	"dsound"			)
    #pragma comment(lib,	"msacm32"			)
#endif
