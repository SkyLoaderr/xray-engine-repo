// xrGame.cpp : Defines the entry point for the DLL application.
//
 
#include "stdafx.h"
#include "..\xr_ioconsole.h"
#include "..\xr_ioc_cmd.h"
#include "..\customhud.h"
#include "ai_console.h"
#include "Actor_Flags.h"
  
extern int					psPhysicsFPS;
extern float				psSqueezeVelocity;
ENGINE_API extern DWORD		psAlwaysRun;
ENGINE_API extern float		psHUD_FOV;
  
// console commands
class CCC_Spawn : public CConsoleCommand
{
public:
	CCC_Spawn(LPCSTR N) : CConsoleCommand(N)  { };
	virtual void Execute(LPCSTR args) {
		R_ASSERT(pCreator);

		char	Name[128];	Name[0]=0;
		int		team=0,squad=0,group=0;

		sscanf	(args,"%[^,],%d,%d,%d", Name, &team, &squad, &group);
		Level().g_cl_Spawn	(Name,-1,team,squad,group);
	}
	virtual void	Info	(TInfo& I)		
	{
		strcpy(I,"name,team,squad,group"); 
	}
};

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		{
		// game
		CMD3(CCC_Mask,		"g_always_run",			&psActorFlags,	AF_ALWAYSRUN);
		CMD3(CCC_Mask,		"g_god",				&psActorFlags,	AF_GODMODE	);
		CMD1(CCC_Spawn,		"g_spawn"				);

		// hud
		CMD3(CCC_Mask,		"hud_crosshair",		&psHUD_Flags,	HUD_CROSSHAIR);
		CMD3(CCC_Mask,		"hud_crosshair_dist",	&psHUD_Flags,	HUD_CROSSHAIR_DIST);
		CMD3(CCC_Mask,		"hud_weapon",			&psHUD_Flags,	HUD_WEAPON);
		CMD3(CCC_Mask,		"hud_info",				&psHUD_Flags,	HUD_INFO);
		CMD3(CCC_Mask,		"hud_draw",				&psHUD_Flags,	HUD_DRAW);
		CMD2(CCC_Float,		"hud_fov",				&psHUD_FOV);

		// ai
		CMD3(CCC_Mask,		"ai_debug",				&psAI_Flags,	aiDebug);
		CMD3(CCC_Mask,		"ai_dbg_brain",			&psAI_Flags,	aiBrain);
		CMD3(CCC_Mask,		"ai_dbg_motion",		&psAI_Flags,	aiMotion);
		CMD3(CCC_Mask,		"ai_dbg_frustum",		&psAI_Flags,	aiFrustum);

		// physics
		CMD4(CCC_Integer,	"ph_fps",				&psPhysicsFPS,	10,1000);
		CMD4(CCC_Float,		"ph_squeeze_velocity",	&psHUD_FOV,		0,500);
		}
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
    return TRUE;
}

// Factory
#include "actor.h"
//#include "weaponrail.h"
#include "customitem.h"
#include "flyer.h"
#include "customevent.h"
#include "dummyobject.h"
#include "ai_human.h"
#include "customdoor.h"
#include "customlift.h"
#include "demoactor.h"
#include "hudmanager.h"

extern "C" {
	DLL_API DLL_Pure*	__cdecl xrFactory_Create		(CLASS_ID cls)
	{
		/*
		char N[20];
		CLSID2TEXT(cls,N);
		Log("CLS:",N);
		*/

		DLL_Pure*	P = 0;
		switch (cls)
		{
		case CLSID_LEVEL:			P = new CLevel();			break;
		case CLSID_HUDMANAGER:		P = new CHUDManager();		break;
		case CLSID_OBJECT_ACTOR:	P = new CActor();			break;
//		case CLSID_OBJECT_W_RAIL:	P = new	CWeaponRail();		break;
		case CLSID_OBJECT_ITEM_STD:	P = new CCustomItem();		break;
		case CLSID_OBJECT_FLYER:	P = new CFlyer();			break;
		case CLSID_OBJECT_DUMMY:	P = new CDummyObject();		break;
		case CLSID_EVENT:			P = new CCustomEvent();		break;
		case CLSID_AI_HUMAN:		P = new CAI_Human();		break;
		case CLSID_OBJECT_DOOR:		P = new CCustomDoor();		break;
		case CLSID_OBJECT_LIFT:		P = new CCustomLift();		break;
		case CLSID_OBJECT_DACTOR:	P = new CDemoActor();		break;
		}
		R_ASSERT(P);
		P->SUB_CLS_ID = cls;
		return P;
	}
	DLL_API void		__cdecl	xrFactory_Destroy		(DLL_Pure* O)
	{
		delete O;
	}
};
