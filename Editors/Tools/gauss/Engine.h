// Engine.h: interface for the CEngine class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ENGINE_H__22802DD7_D7EB_4234_9781_E237657471AC__INCLUDED_)
#define AFX_ENGINE_H__22802DD7_D7EB_4234_9781_E237657471AC__INCLUDED_
#pragma once

#ifndef	M_BORLAND
    // xr_malloc
    IC void*	xr_malloc	(size_t size)
    {	return	_aligned_malloc(size,16); }

    // xr_free
    IC void		xr_free		(void *P)
    {	_aligned_free(P); }

    // xr_realloc
    IC void*	xr_realloc	(void* P, size_t size)
    {	return _aligned_realloc(P,size,16); }

    // xr_strdup
    IC char*	xr_strdup	(const char* string)
    {	
        VERIFY	(string);
        int		len			= strlen(string)+1;
        char *	memory		= (char *) xr_malloc( len );
        if (PSGP.memCopy)	PSGP.memCopy	(memory,string,len);
        else				CopyMemory		(memory,string,len);
        return memory;
    }
#else
	#define		xr_malloc	malloc
	#define		xr_free		free
	#define		xr_realloc	realloc
	#define		xr_strdup	strdup
#endif

#include "FS.h"
#include "FileSystem.h"
#include "xrCPU_Pipe.h"
                      
class ENGINE_API CEngine
{
	HMODULE				hPSGP;
public:
	CFileSystem			FS;
    string256			Params;
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
