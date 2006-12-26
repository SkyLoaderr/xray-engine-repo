#include "StdAfx.h"
#include "GameSpy_Patching.h"
#include "GameSpy_Base_Defs.h"

CGameSpy_Patching::CGameSpy_Patching()
{
	hGameSpyDLL = NULL;

	LoadGameSpy();
};

CGameSpy_Patching::~CGameSpy_Patching()
{
	if (hGameSpyDLL)
	{
		FreeLibrary(hGameSpyDLL);
		hGameSpyDLL = NULL;
	}
};
void	CGameSpy_Patching::LoadGameSpy()
{
	LPCSTR			g_name	= "xrGameSpy.dll";
	Log				("Loading DLL:",g_name);
	hGameSpyDLL			= LoadLibrary	(g_name);
	if (0==hGameSpyDLL)	R_CHK			(GetLastError());
	R_ASSERT2		(hGameSpyDLL,"GameSpy DLL raised exception during loading or there is no game DLL at all");

	GAMESPY_LOAD_FN(xrGS_ptCheckForPatch);
}

void	CGameSpy_Patching::CheckForPatch()
{
};