// StdAfx.h: interface for the CStdAfx class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STDAFX_H__5B2D8104_8002_4D22_8DAF_9CB93D4C179C__INCLUDED_)
#define AFX_STDAFX_H__5B2D8104_8002_4D22_8DAF_9CB93D4C179C__INCLUDED_
#pragma once

#define WIN32_LEAN_AND_MEAN			// Exclude rarely-used stuff from Windows headers
#define STRICT						// Enable strict syntax
#define IDIRECTPLAY2_OR_GREATER

#define NOGDICAPMASKS
#define NOSYSMETRICS
#define NOMENUS
#define NOICONS
#define NOKEYSTATES
#define NOCLIPBOARD
#define NODRAWTEXT
#define NONLS
#define NOMEMMGR
#define NOMETAFILE
#define NOMINMAX
#define NOSERVICE
#define NOCOMM
#define NOHELP
#define NOPROFILER
#define NOMCX

#include "d3dx.h"

#include "stdlib.h"

#include "vector"
#include "limits"
using namespace std;

#define DEBUG
#define ENGINE_API
#define VERIFY
#define R_ASSERT(a) a


// maps unsigned 8 bits/channel to D3DCOLOR
#define D3DCOLOR_ARGB(a,r,g,b) \
    ((D3DCOLOR)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))
#define D3DCOLOR_RGBA(r,g,b,a) D3DCOLOR_ARGB(a,r,g,b)
#define D3DCOLOR_XRGB(r,g,b)   D3DCOLOR_ARGB(0xff,r,g,b)


#define NO_XR_LIGHT
#define NO_XR_MATERIAL
#define NO_XR_3DBUFFER
#define NO_XR_3DLISTENER
#define NO_XR_VERTEX
#define IC __forceinline
#include "vector.h"

#endif // !defined(AFX_STDAFX_H__5B2D8104_8002_4D22_8DAF_9CB93D4C179C__INCLUDED_)
