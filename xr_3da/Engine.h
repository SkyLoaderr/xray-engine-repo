// Engine.h: interface for the CEngine class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ENGINE_H__22802DD7_D7EB_4234_9781_E237657471AC__INCLUDED_)
#define AFX_ENGINE_H__22802DD7_D7EB_4234_9781_E237657471AC__INCLUDED_
#pragma once

#include "engineAPI.h"
#include "debugkernel.h"
#include "scriptAPI.h"
#include "eventAPI.h"
#include "locatorAPI.h"
#include "xrCPU_Pipe.h"
#include "xrSheduler.h"

class ENGINE_API CEngine
{
	HMODULE				hPSGP;
public:
	// DLL api stuff
	char				Params		[512];
	CEngineAPI			External;
	CScriptAPI			Scripts;
	CEventAPI			Event;
	CLocatorAPI			FS;
	CSheduler			Sheduler;

	void				Initialize	();
	void				Destroy		();
	
	DWORD				mem_Usage	();
	void				mem_Compact ();

	CEngine();
	~CEngine();
};

ENGINE_API extern xrDispatchTable	PSGP;
ENGINE_API extern CEngine			Engine;

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


#endif // !defined(AFX_ENGINE_H__22802DD7_D7EB_4234_9781_E237657471AC__INCLUDED_)
