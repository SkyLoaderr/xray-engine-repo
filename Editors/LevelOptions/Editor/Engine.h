// Engine.h: interface for the CEngine class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ENGINE_H__22802DD7_D7EB_4234_9781_E237657471AC__INCLUDED_)
#define AFX_ENGINE_H__22802DD7_D7EB_4234_9781_E237657471AC__INCLUDED_
#pragma once

#include "FS.h"
#include "FileSystem.h"
#include "xrCPU_Pipe.h"
                      
class ENGINE_API CEngine
{
	HMODULE				hPSGP;
public:
	CFileSystem			FS;
public:
						CEngine		    ();
						~CEngine	    ();

	void				Initialize	    ();
	void				Destroy		    ();
    LPCSTR              LastWindowsError();
};

ENGINE_API extern xrDispatchTable	PSGP;
ENGINE_API extern CEngine			Engine;

#endif // !defined(AFX_ENGINE_H__22802DD7_D7EB_4234_9781_E237657471AC__INCLUDED_)
