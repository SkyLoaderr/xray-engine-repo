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
	//
	Log				("Loading DLL: xrRender");
	hRender			= LoadLibrary	("xrRender_R1.dll");
	if (0==hRender)	R_CHK			(GetLastError());
	R_ASSERT		(hRender);

	//
	Log				("Loading DLL: xrGame");
	hGame			= LoadLibrary	("xrGame.dll");
	if (0==hGame)	R_CHK			(GetLastError());
	R_ASSERT		(hGame);
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
