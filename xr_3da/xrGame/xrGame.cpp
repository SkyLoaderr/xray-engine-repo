// xrGame.cpp : Defines the entry point for the DLL application.
//
 
#include "stdafx.h"

#pragma comment(lib,"ode.lib" )

#include "..\xr_ioconsole.h"
#include "..\xr_ioc_cmd.h"
#include "..\customhud.h"
#include "ai_console.h"
#include "Actor_Flags.h"
#include "ai\\crow\\ai_crow.h"
#include "ai\\rat\\ai_rat.h"
#include "ai\\soldier\\ai_soldier.h"
#include "ai\\zombie\\ai_zombie.h"
#include "car.h"
#include "dummyobject.h"
#include "customtarget.h"
    
ENGINE_API extern u32		psAlwaysRun;
ENGINE_API extern float		psHUD_FOV;
extern int	 				psPhysicsFPS;
extern float				psSqueezeVelocity;

// console commands
class CCC_Spawn : public CConsoleCommand
{
public:
	CCC_Spawn(LPCSTR N) : CConsoleCommand(N)  { };
	virtual void Execute(LPCSTR args) {
		R_ASSERT(pCreator);

		char	Name[128];	Name[0]=0;
		sscanf	(args,"%s", Name);
		Level().g_cl_Spawn	(Name,0xff,M_SPAWN_OBJECT_ACTIVE | M_SPAWN_OBJECT_LOCAL);
	}
	virtual void	Info	(TInfo& I)		
	{
		strcpy(I,"name,team,squad,group"); 
	}
};
class CCC_EndRound : public CConsoleCommand {
public:
	CCC_EndRound(LPCSTR N) : CConsoleCommand(N)  { bEmptyArgsHandled = true; };
	virtual void Execute(LPCSTR args) {
		if(Level().Server) {
			Level().Server->game->OnRoundEnd("GAME_restarted");
			// Level().Server->game->OnRoundStart();
		}
	}
	virtual void	Info	(TInfo& I)		
	{
		strcpy(I,"end round"); 
	}
};

BOOL APIENTRY DllMain( HANDLE hModule, 
                       u32  ul_reason_for_call, 
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
		CMD1(CCC_EndRound,	"g_end_round"				);

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
#include "customitem.h"
#include "flyer.h"
#include "customevent.h"
#include "dummyobject.h"
#include "mercuryball.h"
#include "customdoor.h"
#include "customlift.h"
#include "demoactor.h"
#include "hudmanager.h"
#include "targetassault.h"
#include "targetcsbase.h"
#include "targetcs.h"
#include "targetcscask.h"

#include "weaponM134.h"
#include "weaponAK74.h"
#include "weaponLR300.h"
#include "weaponFN2000.h"
#include "weaponHPSA.h"
#include "weaponPM.h"
#include "weaponFORT.h"
#include "weaponBINOCULARS.h"
#include "weaponShotgun.h"

extern "C" {
	DLL_API DLL_Pure*	__cdecl xrFactory_Create		(CLASS_ID cls)
	{
		DLL_Pure*	P	= 0;
		switch (cls)
		{
		case CLSID_LEVEL:				P = new CLevel();				break;
		case CLSID_HUDMANAGER:			P = new CHUDManager();			break;
		case CLSID_OBJECT_ACTOR:		P = new CActor();				break;
		case CLSID_TARGET:				P = new CCustomTarget();		break;

		case CLSID_AI_RAT:				P = new CAI_Rat();				break;
		case CLSID_AI_SOLDIER:			P =	new CAI_Soldier();			break;
		case CLSID_AI_ZOMBIE:			P = new CAI_Zombie();			break;
		case CLSID_AI_CROW:				P = new CAI_Crow();				break;
		case CLSID_CAR_NIVA:			P = new CCar();					break;

		// Artifacts
		case CLSID_AF_MERCURY_BALL:		P = new CMercuryBall();			break;

		// Targets
		case CLSID_TARGET_ASSAULT:		P = new CTargetAssault();		break;
		case CLSID_TARGET_CS_BASE:		P = new CTargetCSBase();		break;
		case CLSID_TARGET_CS_CASK:		P = new CTargetCSCask();		break;
		case CLSID_TARGET_CS:			P = new CTargetCS();			break;

		case CLSID_OBJECT_DUMMY:		P = new CDummyObject();			break;

		case CLSID_OBJECT_W_FN2000:		P = new CWeaponFN2000();		break;
		case CLSID_OBJECT_W_AK74:		P = new CWeaponAK74();			break;
		case CLSID_OBJECT_W_LR300:		P = new CWeaponLR300();			break;
		case CLSID_OBJECT_W_HPSA:		P = new CWeaponHPSA	();			break;
		case CLSID_OBJECT_W_PM:			P = new CWeaponPM	();			break;
		case CLSID_OBJECT_W_FORT:		P = new CWeaponFORT	();			break;
		case CLSID_OBJECT_W_BINOCULAR:	P = new CWeaponBinoculars();	break;
		case CLSID_OBJECT_W_SHOTGUN:	P = new CWeaponShotgun();		break;
		}
		R_ASSERT		(P);
		P->SUB_CLS_ID	= cls;
		return P;
	}
	DLL_API void		__cdecl	xrFactory_Destroy		(DLL_Pure* O)
	{
		delete O;
	}
};

/*
//		case CLSID_OBJECT_DUMMY:		P = new CDummyObject();			break;
//		case CLSID_EVENT:				P = new CCustomEvent();			break;
//		case CLSID_OBJECT_W_M134:		P = new CWeaponM134();			break;
//		case CLSID_OBJECT_W_RAIL:	P = new	CWeaponRail();		break;
//		case CLSID_OBJECT_ITEM_STD:	P = new CCustomItem();		break;
//		case CLSID_OBJECT_FLYER:	P = new CFlyer();			break;
//		case CLSID_OBJECT_DOOR:		P = new CCustomDoor();		break;
//		case CLSID_OBJECT_LIFT:		P = new CCustomLift();		break;
//		case CLSID_OBJECT_DACTOR:	P = new CDemoActor();		break;
//		case CLSID_AI_HUMAN:		P = new CAI_Human();		break;
*/
