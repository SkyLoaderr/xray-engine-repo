// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//
#ifndef stdafxH
#define stdafxH
#pragma once

#define OPENAL                       

#include <xrCore.h>

#include <dsound.h>
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>
#include <eax.h>             

#define _RELEASE(x)			{ if(x) { (x)->Release();       (x)=NULL; } }

#include "xrCDB.h"
#include "sound.h"

// TODO: reference additional headers your program requires here
#endif

