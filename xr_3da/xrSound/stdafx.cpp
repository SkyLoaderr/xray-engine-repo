// stdafx.cpp : source file that includes just the standard includes
// xrSound.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file

#ifdef __BORLANDC__
#	pragma comment(lib,	"x:\\eaxB.lib"			)
#	pragma comment(lib,	"x:\\oggB.lib"			)
#	pragma comment(lib,	"x:\\vorbisfileB.lib"	)
#	pragma comment(lib,	"x:\\xrCoreB.lib"		)
#	pragma comment(lib,	"x:\\xrCDBB.lib"		)
#	pragma comment(lib,	"x:\\wrap_oalB.lib"	)
#else
#	pragma comment(lib,	"x:\\eax.lib"			)
#	pragma comment(lib,	"x:\\vorbis.lib")
#	pragma comment(lib,	"x:\\vorbisfile.lib"	)
#	pragma comment(lib,	"x:\\xrCore"			)
#	pragma comment(lib,	"x:\\xrCDB"				)
#	pragma comment(lib,	"x:\\wrap_oal.lib"	)
#endif

#pragma comment(lib,	"dsound.lib" 		)
#pragma comment(lib,	"msacm32.lib"		)

