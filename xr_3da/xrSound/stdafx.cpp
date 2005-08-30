// stdafx.cpp : source file that includes just the standard includes
// xrSound.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file

#ifdef __BORLANDC__
#	pragma comment(lib,	"eaxB.lib"			)
#	pragma comment(lib,	"oggB.lib"			)
#	pragma comment(lib,	"vorbisfileB.lib"	)
#	pragma comment(lib,	"xrCoreB.lib"		)
#	pragma comment(lib,	"EToolsB.lib"		)
#	pragma comment(lib,	"OpenAL32B.lib"		)
#	pragma comment(lib,	"dsoundb.lib" 		)
#else
#	pragma comment(lib,	"x:\\eax.lib"		)
#	pragma comment(lib,	"x:\\vorbis.lib"	)
#	pragma comment(lib,	"x:\\vorbisfile.lib")
#	pragma comment(lib,	"x:\\xrCore"		)
#	pragma comment(lib,	"x:\\xrCDB"			)
#	pragma comment(lib,	"OpenAL32.lib"		)
#	pragma comment(lib,	"dsound.lib" 		)
#endif

#pragma comment(lib,	"msacm32.lib"		)

