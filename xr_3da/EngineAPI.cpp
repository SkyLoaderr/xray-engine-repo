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

}

CEngineAPI::~CEngineAPI()
{
}

void CEngineAPI::Initialize(void)
{
	Log("Loading game: xrGame.dll");
	hModule		= LoadLibrary("xrGame.dll");
	if (0==hModule) Log("* HR=%d",GetLastError());
	R_ASSERT(hModule);
	pCreate		= (Factory_Create*)		GetProcAddress(hModule,"xrFactory_Create"	);	R_ASSERT(pCreate);
	pDestroy	= (Factory_Destroy*)	GetProcAddress(hModule,"xrFactory_Destroy"	);	R_ASSERT(pDestroy);
}

void CEngineAPI::Destroy	(void)
{
	if (hModule)	{ FreeLibrary(hModule); hModule = 0; }
	pCreate			= 0;
	pDestroy		= 0;
}
