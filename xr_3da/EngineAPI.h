// EngineAPI.h: interface for the CEngineAPI class.
//
//****************************************************************************
// Support for extension DLLs
//****************************************************************************

#if !defined(AFX_ENGINEAPI_H__CF21372B_C8B8_4891_82FC_D872C84E1DD4__INCLUDED_)
#define AFX_ENGINEAPI_H__CF21372B_C8B8_4891_82FC_D872C84E1DD4__INCLUDED_
#pragma once

#ifndef NO_ENGINE_API
// you must define ENGINE_BUILD then building the engine itself
// and not define it if you are about to build DLL
#ifdef ENGINE_BUILD
#define DLL_API			__declspec(dllimport)
#define ENGINE_API		__declspec(dllexport)
#else
#define DLL_API			__declspec(dllexport)
#define ENGINE_API		__declspec(dllimport)
#endif

#else
#define ENGINE_API
#define DLL_API
#endif // NO_ENGINE_API

#include "clsid.h"

// Abstract 'Pure' class for DLL interface
class ENGINE_API DLL_Pure {
public:
	CLASS_ID			CLS_ID;
	CLASS_ID			SUB_CLS_ID;

	DLL_Pure(void *params)	{CLS_ID=SUB_CLS_ID=0; };
	DLL_Pure()				{CLS_ID=SUB_CLS_ID=0; };
	virtual ~DLL_Pure()		{};
};

// Class creation/destroying interface
class ENGINE_API CObject;
extern "C" {
typedef DLL_API  DLL_Pure*	  __cdecl Factory_Create	(CLASS_ID	CLS_ID);
typedef DLL_API  void		  __cdecl Factory_Destroy	(DLL_Pure*	O);
};

class ENGINE_API CEngineAPI
{
private:
	HMODULE				hModule;
public:
	Factory_Create*		pCreate;
	Factory_Destroy*	pDestroy;

	void				Initialize	();
	void				Destroy		();

	CEngineAPI	();
	~CEngineAPI	();
};

#define NEW_INSTANCE(a)		Engine.External.pCreate(a)
#define DEL_INSTANCE(a)		{ Engine.External.pDestroy(a); a=NULL; }

#endif // !defined(AFX_ENGINEAPI_H__CF21372B_C8B8_4891_82FC_D872C84E1DD4__INCLUDED_)
