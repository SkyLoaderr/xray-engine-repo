// xrGame.cpp : Defines the entry point for the DLL application.
//
 
#include "stdafx.h"
#include "gamepersistent.h"

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
#include "..\fdemorecord.h"
#include "..\fdemoplay.h"
#include "a_star.h"
#include "game_sv_single.h"
    
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
class CCC_Restart : public CConsoleCommand {
public:
	CCC_Restart(LPCSTR N) : CConsoleCommand(N)  { bEmptyArgsHandled = true; };
	virtual void Execute(LPCSTR args) {
		if(Level().Server) {
			Level().Server->game->OnRoundEnd("GAME_restarted");
			Level().Server->game->round = -1;
			// Level().Server->game->OnRoundStart();
		}
	}
	virtual void	Info	(TInfo& I)		
	{
		strcpy(I,"restart game"); 
	}
};
class CCC_Team : public CConsoleCommand {
public:
	CCC_Team(LPCSTR N) : CConsoleCommand(N)  { };
	virtual void Execute(LPCSTR args) {
		u32 l_team = 2;
		sscanf(args, "%d", &l_team);
		CObject *l_pObj = Level().CurrentEntity();
		CActor *l_pActor = dynamic_cast<CActor*>(l_pObj);
		if(l_pActor) {
			if(l_pActor->g_Team() == s32(l_team)) return;
			Fvector l_dir; l_dir.set(0, -1.f, 0);
			Fvector position_in_bone_space;
			position_in_bone_space.set(0.f,0.f,0.f);
			NET_Packet		P;
			l_pActor->u_EventGen		(P,GE_HIT,l_pActor->ID());
			P.w_u16			(u16(l_pActor->ID()));
			P.w_dir			(l_dir);
			P.w_float		(10000.f);
			P.w_s16			((s16)0);
			P.w_vec3		(position_in_bone_space);
			l_pActor->u_EventSend		(P);
			//NET_Packet		P;
			//l_pActor->u_EventGen		(P,GE_DIE,l_pActor->ID()	);
			//P.w_u16			(u16(l_pActor->ID())	);
			//P.w_u32			(0);
			//l_pActor->u_EventSend		(P);
		}
		CGameObject *l_pPlayer = dynamic_cast<CGameObject*>(l_pObj);
		if(l_pPlayer) {
			NET_Packet		P;
			l_pPlayer->u_EventGen		(P,GEG_PLAYER_CHANGE_TEAM,l_pPlayer->ID()	);
			P.w_u16			(u16(l_pPlayer->ID())	);
			P.w_s16			(s16(l_team));
			P.w_s16			((s16)0);
			//P.w_u32			(0);
			l_pPlayer->u_EventSend		(P);
		}
	}
	virtual void	Info	(TInfo& I)		
	{
		strcpy(I,"change team"); 
	}
};
class CCC_Money : public CConsoleCommand {
public:
	CCC_Money(LPCSTR N) : CConsoleCommand(N)  { };
	virtual void Execute(LPCSTR args) {
		s32 l_money;
		if(sscanf(args, "%d", &l_money) == 1) {
			if(Level().Server) {
				u32 l_cnt = Level().Server->game->get_count();
				while(l_cnt--) {
					game_PlayerState *l_pPs = Level().Server->game->get_it(l_cnt);
					l_pPs->money_total = s16(l_money);
				}
			}
		}
	}
	virtual void Info(TInfo& I) {
		strcpy(I,"give money"); 
	}
};

class CCC_Path : public CConsoleCommand {
public:
	CCC_Path(LPCSTR N) : CConsoleCommand(N)  { };
	virtual void Execute(LPCSTR args) {
		int id1=-1, id2=-1;
		sscanf(args ,"%d %d",&id1,&id2);
		if (!Level().AI.bfCheckIfGraphLoaded())
			Msg("! there is no graph!");
		else
			if ((id1 != -1) && (id2 != -1))
				if (max(id1,id2) > (int)Level().AI.GraphHeader().dwVertexCount - 1)
					Msg("! there are only %d vertexes!",Level().AI.GraphHeader().dwVertexCount);
				else
					if (min(id1,id2) < 0)
						Msg("! invalid vertex number (%d)!",min(id1,id2));
					else {
//						SetPriorityClass	(GetCurrentProcess(),REALTIME_PRIORITY_CLASS);
//						SetThreadPriority	(GetCurrentThread(),THREAD_PRIORITY_TIME_CRITICAL);
						Sleep				(1);
						u64 t1x = CPU::GetCycleCount();
						float fValue = Level().AI.m_tpAStar->ffFindMinimalPath(id1,id2);
						u64 t2x = CPU::GetCycleCount();
//						SetThreadPriority	(GetCurrentThread(),THREAD_PRIORITY_NORMAL);
//						SetPriorityClass	(GetCurrentProcess(),NORMAL_PRIORITY_CLASS);
						t2x -= t1x;
						Msg("* %7.2f[%d] : %11I64u cycles (%.3f microseconds)",fValue,Level().AI.m_tpAStar->m_tpaNodes.size(),t2x,CPU::cycles2microsec*t2x);
					}
			else
				Msg("! not enough parameters!");
	}
};

class CCC_SaveALife : public CConsoleCommand {
public:
	CCC_SaveALife(LPCSTR N) : CConsoleCommand(N)  { };
	virtual void Execute(LPCSTR args) {
		if (Level().game.type == GAME_SINGLE) {
			game_sv_Single *tpGame = dynamic_cast<game_sv_Single *>(Level().Server->game);
			if (tpGame && tpGame->m_tALife.m_bLoaded) {
				tpGame->m_tALife.Save();
				Log("* ALife simulation is successfully saved!");
			}
			else
				Log("!ALife simulation cannot be saved!");
		}
		else
			Log("!Not a single player game!");
	}
};
//-----------------------------------------------------------------------
class CCC_DemoRecord : public CConsoleCommand
{
public:
	CCC_DemoRecord(LPCSTR N) : CConsoleCommand(N) {};
	virtual void Execute(LPCSTR args) {
		Console.Hide	();
		char fn[256]; strconcat(fn,args,".xrdemo");
		pCreator->Cameras.AddEffector(new CDemoRecord(fn));
	}
};
class CCC_DemoPlay : public CConsoleCommand
{
public:
	CCC_DemoPlay(LPCSTR N) : 
	  CConsoleCommand(N) 
	  { bEmptyArgsHandled = TRUE; };
	  virtual void Execute(LPCSTR args) {
		  Console.Hide();
		  char fn[256]; strconcat(fn,args,".xrdemo");
		  pCreator->Cameras.AddEffector(new CDemoPlay(fn,1.3f));
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
		CMD1(CCC_Restart,	"g_restart"				);
		CMD1(CCC_Money,		"g_money"				);
		CMD1(CCC_Team,		"g_change_team"			);
		CMD1(CCC_Path,		"path"					);
		CMD1(CCC_SaveALife,	"asave"					);

		// hud
		CMD3(CCC_Mask,		"hud_crosshair",		&psHUD_Flags,	HUD_CROSSHAIR);
		CMD3(CCC_Mask,		"hud_crosshair_dist",	&psHUD_Flags,	HUD_CROSSHAIR_DIST);
		CMD3(CCC_Mask,		"hud_weapon",			&psHUD_Flags,	HUD_WEAPON);
		CMD3(CCC_Mask,		"hud_info",				&psHUD_Flags,	HUD_INFO);
		CMD3(CCC_Mask,		"hud_draw",				&psHUD_Flags,	HUD_DRAW);
		CMD2(CCC_Float,		"hud_fov",				&psHUD_FOV);

		// Demo
#ifdef DEBUG
		CMD1(CCC_DemoRecord,"demo_record"			);
#endif
		CMD1(CCC_DemoPlay,	"demo_play"				);

		// ai
		CMD3(CCC_Mask,		"ai_debug",				&psAI_Flags,	aiDebug);
		CMD3(CCC_Mask,		"ai_dbg_brain",			&psAI_Flags,	aiBrain);
		CMD3(CCC_Mask,		"ai_dbg_motion",		&psAI_Flags,	aiMotion);
		CMD3(CCC_Mask,		"ai_dbg_frustum",		&psAI_Flags,	aiFrustum);

		// physics
		CMD4(CCC_Integer,	"ph_fps",				&psPhysicsFPS,	10,1000);
		CMD4(CCC_Float,		"ph_squeeze_velocity",	&psHUD_FOV,		0,500);
		}
		GamePersistent		= new CGamePersistent();
		break;
	case DLL_PROCESS_DETACH:
		_DELETE(GamePersistent);
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
#include "Spectator.h"

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
		case CLSID_SPECTATOR:			P = new CSpectator();			break;

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
