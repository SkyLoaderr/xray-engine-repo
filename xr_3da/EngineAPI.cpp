// EngineAPI.cpp: implementation of the CEngineAPI class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EngineAPI.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEngineAPI::CEngineAPI()
{
	hGame			= 0;
	hRender			= 0;
	pCreate			= 0;
	pDestroy		= 0;
}

CEngineAPI::~CEngineAPI()
{
}

void CEngineAPI::Initialize(void)
{
	//
	LPCSTR			r_name	= "xrRender_R1.dll";
	if (strstr(Core.Params,"-r2"))	r_name	= "xrRender_R2.dll";
	Log				("Loading DLL:",r_name);
	hRender			= LoadLibrary	(r_name);
	if (0==hRender)	R_CHK			(GetLastError());
	R_ASSERT		(hRender);

	//
	LPCSTR			g_name	= "xrGame.dll";
	Log				("Loading DLL:",g_name);
	hGame			= LoadLibrary	(g_name);
	if (0==hGame)	R_CHK			(GetLastError());
	R_ASSERT2		(hGame,"Game DLL raised exception during loading or there is no game DLL at all");
	pCreate			= (Factory_Create*)		GetProcAddress(hGame,"xrFactory_Create"		);	R_ASSERT(pCreate);
	pDestroy		= (Factory_Destroy*)	GetProcAddress(hGame,"xrFactory_Destroy"	);	R_ASSERT(pDestroy);
}

void CEngineAPI::Destroy	(void)
{
	if (hGame)		{ FreeLibrary(hGame);	hGame	= 0; }
	if (hRender)	{ FreeLibrary(hRender); hRender = 0; }
	pCreate			= 0;
	pDestroy		= 0;
}
