// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//
#ifndef stdafxH
#define stdafxH
#pragma once

#include <xrCore.h>
			
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>
#include <eax.h>             

//#define _OPENAL

#ifdef _OPENAL
#	include <AL/al.h>
#	include <AL/alc.h>
#	include <AL/alut.h>
#	define A_CHK(expr)	{ expr; ALCenum error=alGetError(); VERIFY2(error==AL_NO_ERROR,(LPCSTR)alGetString(error)); }
#	define AC_CHK(expr)	{ expr; ALCenum error=alcGetError(pDevice); VERIFY2(error==AL_NO_ERROR,(LPCSTR)alcGetString(pDevice,error)); }
#else
#	include <dsound.h>
#	define _RELEASE(x)	{ if(x) { (x)->Release();       (x)=NULL; } }
#endif

#include "xrCDB.h"
#include "sound.h"

// TODO: reference additional headers your program requires here
#endif

