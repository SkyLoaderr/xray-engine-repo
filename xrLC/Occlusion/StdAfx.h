// StdAfx.h: interface for the CStdAfx class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STDAFX_H__5B2D8104_8002_4D22_8DAF_9CB93D4C179C__INCLUDED_)
#define AFX_STDAFX_H__5B2D8104_8002_4D22_8DAF_9CB93D4C179C__INCLUDED_
#pragma once

#include "d3dx.h"
#include <commctrl.h>
#include <shlobj.h>

#include "stdio.h"
#include "io.h"
#include "fcntl.h"
#include "sys\stat.h"
#include "stdlib.h"
#include "time.h"
#include "string.h"

#include "vector"
#include "algorithm"
using namespace std;

#define DEBUG
#define ENGINE_API
#include "defines.h"
#include "vector.h"
#include "log.h"
#include "vbmanager.h"

LPDIRECT3DDEVICE7	Device();
LPDIRECT3D7			D3D();

#endif // !defined(AFX_STDAFX_H__5B2D8104_8002_4D22_8DAF_9CB93D4C179C__INCLUDED_)
