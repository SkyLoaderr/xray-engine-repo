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

class ENGINE_API CEngine
{
	HMODULE				hPSGP;
public:
	// DLL api stuff
	CEngineAPI			External;
	CScriptAPI			Scripts;
	CEventAPI			Event;
	CLocatorAPI			FS;

	void				Initialize	();
	void				Destroy		();

	CEngine();
	~CEngine();
};

ENGINE_API extern xrDispatchTable	PSGP;
ENGINE_API extern CEngine			Engine;

#endif // !defined(AFX_ENGINE_H__22802DD7_D7EB_4234_9781_E237657471AC__INCLUDED_)
