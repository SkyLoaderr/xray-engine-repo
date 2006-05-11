#include "stdafx.h"
#include "../xr_ioconsole.h"
#include "../xr_ioc_cmd.h"
#include "../customhud.h"
#include "../fdemorecord.h"
#include "../fdemoplay.h"
#include "xrMessages.h"
#include "xrserver.h"
#include "level.h"
#include "script_debugger.h"
#include "ai_debug.h"
#include "alife_simulator.h"
#include "game_cl_base.h"
#include "game_cl_single.h"
#include "game_sv_single.h"
#include "gamepersistent.h"
#include "hit.h"
#include "PHDestroyable.h"
#include "actor.h"
#include "Actor_Flags.h"
#include "customzone.h"
#include "script_engine.h"
#include "script_engine_space.h"
#include "script_process.h"
#include "xrServer_Objects.h"
#include "ui/UIMainIngameWnd.h"
#include "PhysicsGamePars.h"
#include "phworld.h"
#include "string_table.h"
#include "autosave_manager.h"
#include "ai_space.h"
#include "ai/monsters/BaseMonster/base_monster.h"
#include "game_sv_deathmatch.h"
#include "date_time.h"
#include "mt_config.h"
#include "ui/UIOptConCom.h"
#include "zone_effector.h"
#include "GameTask.h"
#include "MainMenu.h"

//#define MASTER_GOLD;

#ifdef DEBUG
#	include "PHDebug.h"
#	include "ui/UIDebugFonts.h" 
#endif
#	include "hudmanager.h"

extern void show_smart_cast_stats		();
extern void clear_smart_cast_stats		();
extern void release_smart_cast_stats	();

extern	u64		g_qwStartGameTime;
extern	float	g_fTimeFactor;
extern	u64		g_qwEStartGameTime;

ENGINE_API
extern	float	psHUD_FOV;
extern	float	psSqueezeVelocity;
extern	int		psLUA_GCSTEP;
extern	float	g_cl_lvInterp;
extern	int		g_cl_InterpolationType; //0 - Linear, 1 - BSpline, 2 - HSpline
extern	u32		g_cl_InterpolationMaxPoints;

extern	float	g_fMinReconDist;
extern	float	g_fMaxReconDist;
extern	float	g_fMinReconSpeed;
extern	float	g_fMaxReconSpeed;
extern	u32		g_dwMaxCorpses;
extern	int		x_m_x;
extern	int		x_m_z;
extern	BOOL	net_cl_inputguaranteed	;
extern	BOOL	net_sv_control_hit		;
extern	int		g_dwInputUpdateDelta	;
extern	BOOL	g_ShowAnimationInfo		;
extern	BOOL	g_bCalculatePing		;
extern	BOOL	g_bBearerCantSprint		;
extern	BOOL	g_bShildedBases			;
extern	BOOL	g_bAfReturnPlayersToBases;
extern	BOOL	g_b_COD_PickUpMode		;
extern	BOOL	g_bShowHitSectors		;
extern	INT		g_iWeaponRemove			;
extern	INT		g_iCorpseRemove			;
//extern	BOOL	g_bShowPlayerNames		;
extern	BOOL	g_bCollectStatisticData ;
extern	BOOL	g_bStatisticSaveAuto	;
extern	BOOL	g_SV_Disable_Auth_Check	;
extern	BOOL	g_bDebugDumpPhysicsStep	;
extern	BOOL	g_bLeaveTDemo			;
extern	ESingleGameDifficulty g_SingleGameDifficulty;
extern	BOOL	g_show_wnd_rect			;

#ifdef DEBUG
	extern	BOOL	g_SV_Force_Artefact_Spawn;
#endif

		BOOL	g_bCheckTime			= FALSE;
		int		g_dwEventDelay			= 0	;
		int		net_cl_inputupdaterate	= 50;
		Flags32	g_mt_config				= {mtLevelPath | mtDetailPath | mtObjectHandler | mtSoundPlayer | mtAiVision | mtBullets | mtLUA_GC | mtLevelSounds};
#ifdef DEBUG
		Flags32	dbg_net_Draw_Flags		= {0};
#endif

#ifdef DEBUG
		BOOL	g_bDebugNode			= FALSE;
		u32		g_dwDebugNodeSource		= 0;
		u32		g_dwDebugNodeDest		= 0;
extern	BOOL	g_bDrawBulletHit;

#endif

int g_AI_inactive_time = 0;
Flags32 g_uCommonFlags;
enum E_COMMON_FLAGS{
	flAiUseTorchDynamicLights = 1
};
BOOL g_use_scripts_in_goap = 1;

CUIOptConCom g_OptConCom;

// console commands
class CCC_Spawn : public IConsole_Command
{
public:
	CCC_Spawn(LPCSTR N) : IConsole_Command(N)  { };
	virtual void Execute(LPCSTR args) {
		R_ASSERT(g_pGameLevel);

#ifndef	DEBUG
		if (GameID() != GAME_SINGLE) 
		{
			Msg("For this game type entity-spawning is disabled.");
			return;
		};
#endif
		char	Name[128];	Name[0]=0;
		sscanf	(args,"%s", Name);
		Fvector pos = Actor()->Position();
		pos.y		+= 2.0f;
		Level().g_cl_Spawn	(Name,0xff,M_SPAWN_OBJECT_LOCAL, pos);
	}
	virtual void	Info	(TInfo& I)		
	{
		strcpy(I,"name,team,squad,group"); 
	}
};
class CCC_GameDifficulty : public CCC_Token {
public:
	CCC_GameDifficulty(LPCSTR N) : CCC_Token(N,(u32*)&g_SingleGameDifficulty,difficulty_type_token)  {};
	virtual void Execute(LPCSTR args) {
		CCC_Token::Execute(args);
		if (g_pGameLevel && Level().game){
//#ifndef	DEBUG
			if (GameID() != GAME_SINGLE){
				Msg("For this game type difficulty level is disabled.");
				return;
			};
//#endif

			game_cl_Single* game		= smart_cast<game_cl_Single*>(Level().game); VERIFY(game);
			game->OnDifficultyChanged	();
		}
	}
	virtual void	Info	(TInfo& I)		
	{
		strcpy(I,"game difficulty"); 
	}
};
class CCC_Restart : public IConsole_Command {
public:
	CCC_Restart(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = true; };
	virtual void Execute(LPCSTR /**args/**/) {
		if(Level().Server) {
			Level().Server->game->OnRoundEnd("GAME_restarted");
		}
	}
	virtual void	Info	(TInfo& I)		
	{
		strcpy(I,"restart game"); 
	}
};

class CCC_RestartFast : public IConsole_Command {
public:
	CCC_RestartFast(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = true; };
	virtual void Execute(LPCSTR /**args/**/) {
		if(Level().Server) {
			Level().Server->game->OnRoundEnd("GAME_restarted_fast");
		}
	}
	virtual void	Info	(TInfo& I)		
	{
		strcpy(I,"restart game fast"); 
	}
};

class CCC_SaveStatistic : public IConsole_Command {
public:
	CCC_SaveStatistic(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = true; };
	virtual void Execute(LPCSTR /**args/**/) {
		Game().m_WeaponUsageStatistic.SaveData();
	}
	virtual void	Info	(TInfo& I)		
	{
		strcpy(I,"saving statistic data"); 
	}
};
/*
class CCC_Team : public IConsole_Command {
public:
	CCC_Team(LPCSTR N) : IConsole_Command(N)  { };
	virtual void Execute(LPCSTR args) {
		u32 l_team = 2;
		sscanf(args, "%d", &l_team);
		CObject *l_pObj = Level().CurrentEntity();

		CGameObject *l_pPlayer = smart_cast<CGameObject*>(l_pObj);
		if(l_pPlayer) {
			NET_Packet		P;
		//	l_pPlayer->u_EventGen		(P,GEG_PLAYER_CHANGE_TEAM,l_pPlayer->ID()	);
			l_pPlayer->u_EventGen		(P,GE_GAME_EVENT,l_pPlayer->ID()	);
			P.w_u16(GAME_EVENT_PLAYER_CHANGE_TEAM);

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
*/
class CCC_Kill : public IConsole_Command {
public:
	CCC_Kill(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = true; };
	virtual void Execute(LPCSTR args) {
		CObject *l_pObj = Level().CurrentEntity();
		CActor *l_pPlayer = smart_cast<CActor*>(l_pObj);
		if(l_pPlayer) {
			NET_Packet		P;
			l_pPlayer->u_EventGen		(P,GE_GAME_EVENT,l_pPlayer->ID()	);
			P.w_u16(GAME_EVENT_PLAYER_KILL);
			P.w_u16			(u16(l_pPlayer->ID())	);
			l_pPlayer->u_EventSend		(P);
		}
	}
	virtual void	Info	(TInfo& I)		
	{
		strcpy(I,"player kill"); 
	}
};

class CCC_Name : public IConsole_Command {
public:
	CCC_Name(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = false; };
	virtual void Execute(LPCSTR args) 
	{
		if (GameID() == GAME_SINGLE) return;
		if (!Game().local_player) return;
		
		string1024 NewName = "";
		sscanf	(args,"%s", NewName);
		if (!xr_strlen(NewName)) return;
	
		NET_Packet		P;
		Game().u_EventGen		(P,GE_GAME_EVENT,Game().local_player->GameID);
		P.w_u16(GAME_EVENT_PLAYER_NAME);
		P.w_stringZ(NewName);
		Game().u_EventSend(P);
	}

	virtual void	Info	(TInfo& I)		
	{
		strcpy(I,"player name"); 
	}
};

class CCC_Dbg_NumObjects : public IConsole_Command {
public:
	CCC_Dbg_NumObjects(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = true; };
	virtual void Execute(LPCSTR args) 
	{
		u32 SVObjNum = Level().Server->GetEntitiesNum();
		xr_vector<u16>	SObjID;
		for (u32 i=0; i<SVObjNum; i++)
		{
			CSE_Abstract* pEntity = Level().Server->GetEntity(i);
			SObjID.push_back(pEntity->ID);
		};
		std::sort(SObjID.begin(), SObjID.end());

		u32 CLObjNum = Level().Objects.o_count();
		xr_vector<u16>	CObjID;
		for (i=0; i<CLObjNum; i++)
		{
			CObjID.push_back(Level().Objects.o_get_by_iterator(i)->ID());
		};
		std::sort(CObjID.begin(), CObjID.end());

		Msg("Client Objects : %d", CLObjNum);
		Msg("Server Objects : %d", SVObjNum);

		for (u32 CO= 0; CO<max(CLObjNum, SVObjNum); CO++)
		{
			if (CO < CLObjNum && CO < SVObjNum)
			{
//				CSE_Abstract* pEntity = Level().Server->GetEntity(CO);
//				char color = (Level().Objects.o_get_by_iterator(CO)->ID() == pEntity->ID) ? '-' : '!';
				CSE_Abstract* pEntity = Level().Server->ID_to_entity(SObjID[CO]);
				CObject* pObj = Level().Objects.net_Find(CObjID[CO]);
				char color = (pObj->ID() == pEntity->ID) ? '-' : '!';

				Msg("%c%4d: Client - %20s[%5d] <===> Server - %s [%d]", color, CO+1, 
					*(pObj->cNameSect()), pObj->ID(),
					pEntity->s_name.c_str(), pEntity->ID);
			}
			else
			{
				if (CO<CLObjNum)
				{
					CObject* pObj = Level().Objects.net_Find(CObjID[CO]);
					Msg("! %2d: Client - %s [%d] <===> Server - -----------------", CO+1, 
						*(pObj->cNameSect()), pObj->ID());
				}
				else
				{
					CSE_Abstract* pEntity = Level().Server->ID_to_entity(SObjID[CO]);
					Msg("! %2d: Client - ----- <===> Server - %s [%d]", CO+1, 
						pEntity->s_name.c_str(), pEntity->ID);
				}
			}
		};

		Msg("Client Objects : %d", CLObjNum);
		Msg("Server Objects : %d", SVObjNum);
	}
	virtual void	Info	(TInfo& I)		
	{
		strcpy(I,"dbg Num Objects"); 
	}
};
class CCC_Money : public IConsole_Command {
public:
	CCC_Money(LPCSTR N) : IConsole_Command(N)  { };
	virtual void Execute(LPCSTR args) {
		s32 l_money;
		if(sscanf(args, "%d", &l_money) == 1) {
			if(Level().Server) {
				u32 l_cnt = Level().Server->game->get_players_count();
				while(l_cnt--) {
					game_PlayerState *l_pPs = Level().Server->game->get_it(l_cnt);
					l_pPs->money_total = l_money;
				}
			}
		}
	}
	virtual void Info(TInfo& I) {
		strcpy(I,"give money"); 
	}
};

#include "game_graph.h"

class CCC_ALifePath : public IConsole_Command {
public:
	CCC_ALifePath(LPCSTR N) : IConsole_Command(N)  { };
	virtual void Execute(LPCSTR args) {
		if (!ai().get_level_graph())
			Msg("! there is no graph!");
		else {
			int id1=-1, id2=-1;
			sscanf(args ,"%d %d",&id1,&id2);
			if ((-1 != id1) && (-1 != id2))
				if (_max(id1,id2) > (int)ai().game_graph().header().vertex_count() - 1)
					Msg("! there are only %d vertexes!",ai().game_graph().header().vertex_count());
				else
					if (_min(id1,id2) < 0)
						Msg("! invalid vertex number (%d)!",_min(id1,id2));
					else {
//						Sleep				(1);
//						CTimer				timer;
//						timer.Start			();
//						float				fValue = ai().m_tpAStar->ffFindMinimalPath(id1,id2);
//						Msg					("* %7.2f[%d] : %11I64u cycles (%.3f microseconds)",fValue,ai().m_tpAStar->m_tpaNodes.size(),timer.GetElapsed_ticks(),timer.GetElapsed_ms()*1000.f);
					}
			else
				Msg("! not enough parameters!");
		}
	}
};

#ifdef ALIFE_SUPPORT_CONSOLE_COMMANDS
class CCC_ALifeListAll : public IConsole_Command {
public:
	CCC_ALifeListAll(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = true; };
	virtual void Execute(LPCSTR /**args/**/) {
		if (GameID() == GAME_SINGLE) {
			game_sv_Single *tpGame = smart_cast<game_sv_Single *>(Level().Server->game);
			if (tpGame && tpGame->alife().m_bLoaded) {
				tpGame->alife().vfListObjects();
				tpGame->alife().vfListEvents();
				tpGame->alife().vfListTasks();
				tpGame->alife().vfListTerrain();
				tpGame->alife().vfListSpawnPoints();
			}
			else
				Log("!ALife simulation cannot be saved!");
		}
		else
			Log("!Not a single player game!");
	}
};

class CCC_ALifeListObjects : public IConsole_Command {
public:
	CCC_ALifeListObjects(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = true; };
	virtual void Execute(LPCSTR /**args/**/) {
		if (GameID() == GAME_SINGLE) {
			game_sv_Single *tpGame = smart_cast<game_sv_Single *>(Level().Server->game);
			if (tpGame && tpGame->alife().m_bLoaded) {
				tpGame->alife().vfListObjects();
			}
			else
				Log("!ALife simulation cannot be saved!");
		}
		else
			Log("!Not a single player game!");
	}
};

class CCC_ALifeListEvents : public IConsole_Command {
public:
	CCC_ALifeListEvents(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = true; };
	virtual void Execute(LPCSTR /**args/**/) {
		if (GameID() == GAME_SINGLE) {
			game_sv_Single *tpGame = smart_cast<game_sv_Single *>(Level().Server->game);
			if (tpGame && tpGame->alife().m_bLoaded) {
				tpGame->alife().vfListEvents();
			}
			else
				Log("!ALife simulator is not loaded!");
		}
		else
			Log("!Not a single player game!");
	}
};

class CCC_ALifeListTasks : public IConsole_Command {
public:
	CCC_ALifeListTasks(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = true; };
	virtual void Execute(LPCSTR /**args/**/) {
		if (GameID() == GAME_SINGLE) {
			game_sv_Single *tpGame = smart_cast<game_sv_Single *>(Level().Server->game);
			if (tpGame && tpGame->alife().m_bLoaded) {
				tpGame->alife().vfListTasks();
			}
			else
				Log("!ALife simulator is not loaded!");
		}
		else
			Log("!Not a single player game!");
	}
};

class CCC_ALifeListTerrain : public IConsole_Command {
public:
	CCC_ALifeListTerrain(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = true; };
	virtual void Execute(LPCSTR /**args/**/) {
		if (GameID() == GAME_SINGLE) {
			game_sv_Single *tpGame = smart_cast<game_sv_Single *>(Level().Server->game);
			if (tpGame && tpGame->alife().m_bLoaded) {
				tpGame->alife().vfListTerrain();
			}
			else
				Log("!ALife simulator is not loaded!");
		}
		else
			Log("!Not a single player game!");
	}
};

class CCC_ALifeListSpawns : public IConsole_Command {
public:
	CCC_ALifeListSpawns(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = true; };
	virtual void Execute(LPCSTR /**args/**/) {
		if (GameID() == GAME_SINGLE) {
			game_sv_Single *tpGame = smart_cast<game_sv_Single *>(Level().Server->game);
			if (tpGame && tpGame->alife().m_bLoaded) {
				tpGame->alife().vfListSpawnPoints();
			}
			else
				Log("!ALife simulator is not loaded!");
		}
		else
			Log("!Not a single player game!");
	}
};

class CCC_ALifeObjectInfo : public IConsole_Command {
public:
	CCC_ALifeObjectInfo(LPCSTR N) : IConsole_Command(N)  { };
	virtual void Execute(LPCSTR args) {
		if (GameID() == GAME_SINGLE) {
			game_sv_Single *tpGame = smart_cast<game_sv_Single *>(Level().Server->game);
			if (tpGame && tpGame->alife().m_bLoaded) {
				int id1 = -1;
				sscanf(args ,"%d",&id1);
				tpGame->alife().vfObjectInfo(ALife::_OBJECT_ID(id1));
			}
			else
				Log("!ALife simulator is not loaded!");
		}
		else
			Log("!Not a single player game!");
	}
};

class CCC_ALifeEventInfo : public IConsole_Command {
public:
	CCC_ALifeEventInfo(LPCSTR N) : IConsole_Command(N)  { };
	virtual void Execute(LPCSTR args) {
		if (GameID() == GAME_SINGLE) {
			game_sv_Single *tpGame = smart_cast<game_sv_Single *>(Level().Server->game);
			if (tpGame && tpGame->alife().m_bLoaded) {
				int id1 = -1;
				sscanf(args ,"%d",&id1);
				if (id1 >= int(tpGame->alife().m_tEventID))
					Msg("Invalid event ID! (%d)",id1);
				else
					tpGame->alife().vfEventInfo(id1);
			}
			else
				Log("!ALife simulator is not loaded!");
		}
		else
			Log("!Not a single player game!");
	}
};

class CCC_ALifeTaskInfo : public IConsole_Command {
public:
	CCC_ALifeTaskInfo(LPCSTR N) : IConsole_Command(N)  { };
	virtual void Execute(LPCSTR args) {
		if (GameID() == GAME_SINGLE) {
			game_sv_Single *tpGame = smart_cast<game_sv_Single *>(Level().Server->game);
			if (tpGame && tpGame->alife().m_bLoaded) {
				int id1 = -1;
				sscanf(args ,"%d",&id1);
				if (id1 >= int(tpGame->alife().m_tTaskID))
					Msg("Invalid task ID! (%d)",id1);
				else
					tpGame->alife().vfTaskInfo(id1);
			}
			else
				Log("!ALife simulator is not loaded!");
		}
		else
			Log("!Not a single player game!");
	}
};

class CCC_ALifeSpawnInfo : public IConsole_Command {
public:
	CCC_ALifeSpawnInfo(LPCSTR N) : IConsole_Command(N)  { };
	virtual void Execute(LPCSTR /**args/**/) {
		//		if (GameID() == GAME_SINGLE) {
		//			game_sv_Single *tpGame = smart_cast<game_sv_Single *>(Level().Server->game);
		//			if (tpGame && tpGame->alife().m_bLoaded) {
		//				u32 id1 = u32(-1);
		//				sscanf(args ,"%d",&id1);
		//				if (id1 >= tpGame->alife().m_tpSpawnPoints.size())
		//					Msg("Invalid task ID! (%d)",id1);
		//				else {
		//					ALife::_SPAWN_ID id = ALife::_SPAWN_ID(id1);
		//					tpGame->alife().vfSpawnPointInfo(id);
		//				}
		//			}
		//			else
		//				Log("!ALife simulator is not loaded!");
		//		}
		//		else
		//			Log("!Not a single player game!");
	}
};

class CCC_ALifeGraphInfo : public IConsole_Command {
public:
	CCC_ALifeGraphInfo(LPCSTR N) : IConsole_Command(N)  { };
	virtual void Execute(LPCSTR args) {
		if (GameID() == GAME_SINGLE) {
			game_sv_Single *tpGame = smart_cast<game_sv_Single *>(Level().Server->game);
			if (ai().get_level_graph()) {
				u32 id1 = u32(-1);
				sscanf(args ,"%d",&id1);
				if (id1 >= ai().game_graph().header().vertex_count())
					Msg("Invalid task ID! (%d)",id1);
				else {
					GameGraph::_GRAPH_ID id = GameGraph::_GRAPH_ID(id1);
					tpGame->alife().vfGraphVertexInfo(id);
				}
			}
			else
				Log("!Game graph is not loaded!");
		}
		else
			Log("!Not a single player game!");
	}
};

class CCC_ALifeScheduleMin : public IConsole_Command {
public:
	CCC_ALifeScheduleMin(LPCSTR N) : IConsole_Command(N)  { };
	virtual void Execute(LPCSTR args) {
		if (GameID() == GAME_SINGLE) {
			game_sv_Single *tpGame = smart_cast<game_sv_Single *>(Level().Server->game);
			if (tpGame && tpGame->alife().m_bLoaded) {
				int id1 = 0;
				sscanf(args ,"%d",&id1);
				if (id1 < 0)
					Msg("Invalid schedule _min time! (%d)",id1);
				else
					tpGame->alife().vfSetScheduleMin(id1);
			}
			else
				Log("!ALife simulator is not loaded!");
		}
		else
			Log("!Not a single player game!");
	}
};

class CCC_ALifeScheduleMax : public IConsole_Command {
public:
	CCC_ALifeScheduleMax(LPCSTR N) : IConsole_Command(N)  { };
	virtual void Execute(LPCSTR args) {
		if (GameID() == GAME_SINGLE) {
			game_sv_Single *tpGame = smart_cast<game_sv_Single *>(Level().Server->game);
			if (tpGame && tpGame->alife().m_bLoaded) {
				int id1 = 0;
				sscanf(args ,"%d",&id1);
				if (id1 < 0)
					Msg("Invalid schedule max time! (%d)",id1);
				else
					tpGame->alife().vfSetScheduleMax(id1);
			}
			else
				Log("!ALife simulator is not loaded!");
		}
		else
			Log("!Not a single player game!");
	}
};
#endif

class CCC_ALifeTimeFactor : public IConsole_Command {
public:
	CCC_ALifeTimeFactor(LPCSTR N) : IConsole_Command(N)  { };
	virtual void Execute(LPCSTR args) {
		float id1 = 0.0f;
		sscanf(args ,"%f",&id1);
		if (id1 < EPS_L)
			Msg("Invalid time factor! (%.4f)",id1);
		else {
			if (!OnServer())
				return;

			Level().Server->game->SetGameTimeFactor(id1);
		}
	}
};

class CCC_ALifeSwitchDistance : public IConsole_Command {
public:
	CCC_ALifeSwitchDistance(LPCSTR N) : IConsole_Command(N)  { };
	virtual void Execute(LPCSTR args) {
		if ((GameID() == GAME_SINGLE)  &&ai().get_alife()) {
			float id1 = 0.0f;
			sscanf(args ,"%f",&id1);
			if (id1 < 2.0f)
				Msg("Invalid online distance! (%.4f)",id1);
			else {
				NET_Packet		P;
				P.w_begin		(M_SWITCH_DISTANCE);
				P.w_float		(id1);
				Level().Send	(P,net_flags(TRUE,TRUE));
			}
		}
		else
			Log("!Not a single player game!");
	}
};

class CCC_ALifeProcessTime : public IConsole_Command {
public:
	CCC_ALifeProcessTime(LPCSTR N) : IConsole_Command(N)  { };
	virtual void Execute(LPCSTR args) {
		if ((GameID() == GAME_SINGLE)  &&ai().get_alife()) {
			game_sv_Single	*tpGame = smart_cast<game_sv_Single *>(Level().Server->game);
			VERIFY			(tpGame);
			int id1 = 0;
			sscanf(args ,"%d",&id1);
			if (id1 < 1)
				Msg("Invalid process time! (%d)",id1);
			else
				tpGame->alife().set_process_time(id1);
		}
		else
			Log("!Not a single player game!");
	}
};

class CCC_ALifeSwitchFactor : public IConsole_Command {
public:
	CCC_ALifeSwitchFactor(LPCSTR N) : IConsole_Command(N)  { };
	virtual void Execute(LPCSTR args) {
		if ((GameID() == GAME_SINGLE)  &&ai().get_alife()) {
			game_sv_Single	*tpGame = smart_cast<game_sv_Single *>(Level().Server->game);
			VERIFY			(tpGame);
			float id1 = 0;
			sscanf(args ,"%f",&id1);
			clamp(id1,.1f,1.f);
			tpGame->alife().set_switch_factor(id1);
		}
		else
			Log		("!Not a single player game!");
	}
};


//-----------------------------------------------------------------------
class CCC_DemoRecord : public IConsole_Command
{
public:

	CCC_DemoRecord(LPCSTR N) : IConsole_Command(N) {};
	virtual void Execute(LPCSTR args) {
		#ifndef	DEBUG
		if (GameID() != GAME_SINGLE) 
		{
			Msg("For this game type Demo Record is disabled.");
			return;
		};
		#endif
		Console->Hide	();
		char fn[256]; strconcat(fn,args,".xrdemo");
		g_pGameLevel->Cameras().AddCamEffector(xr_new<CDemoRecord> (fn));
	}
};
class CCC_DemoPlay : public IConsole_Command
{
public:
	CCC_DemoPlay(LPCSTR N) : 
	  IConsole_Command(N) 
	  { bEmptyArgsHandled = TRUE; };
	  virtual void Execute(LPCSTR args) {
		  if (0==g_pGameLevel)
		  {
			  Msg	("! There are no level(s) started");
		  } else {
			  Console->Hide		();
			  string_path		fn;
			  u32		loops	=	0;
			  LPSTR		comma	=	strchr(args,',');
			  if (comma)	{
				  loops			=	atoi	(comma+1);
				  *comma		=	0;	//. :)
			  }
			  strconcat			(fn,args,".xrdemo");
			  g_pGameLevel->Cameras().AddCamEffector(xr_new<CDemoPlay> (fn,1.0f,loops));
		  }
	  }
};
#include "UIGameCustom.h"
#include "HUDManager.h"
class CCC_ALifeSave : public IConsole_Command {
public:
	CCC_ALifeSave(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = true; };
	virtual void Execute(LPCSTR args) {
		
#if 0
		if (!Level().autosave_manager().ready_for_autosave()) {
			Msg		("! Cannot save the game right now!");
			return;
		}
#endif
		if(!IsGameTypeSingle()){
			Msg("for single-mode only");
			return;
		}
		if(!Actor()->g_Alive()){
			Msg("cannot make saved game because actor is dead :(");
			return;
		}

		string_path				S,S1;
		S[0]					= 0;
		sscanf					(args ,"%s",S);
		
#ifdef DEBUG
		CTimer					timer;
		timer.Start				();
#endif
		if (!xr_strlen(S)) {
			strconcat			(S,Core.UserName,"_","quicksave");
			NET_Packet			net_packet;
			net_packet.w_begin	(M_SAVE_GAME);
			net_packet.w_stringZ(S);
			net_packet.w_u8		(0);
			Level().Send		(net_packet,net_flags(TRUE));
		}
		else {
			NET_Packet			net_packet;
			net_packet.w_begin	(M_SAVE_GAME);
			net_packet.w_stringZ(S);
			net_packet.w_u8		(1);
			Level().Send		(net_packet,net_flags(TRUE));
		}
#ifdef DEBUG
		Msg						("Game save overhead  : %f milliseconds",timer.GetElapsed_sec()*1000.f);
#endif
		SDrawStaticStruct* _s		= HUD().GetUI()->UIGame()->AddCustomStatic("game_saved", true);
		_s->m_endTime				= Device.fTimeGlobal+3.0f;// 3sec
		string_path					save_name;
		strconcat					(save_name,*CStringTable().translate("st_game_saved"),": ", S);
		_s->wnd()->SetText			(save_name);

		strcat					(S,".dds");
		FS.update_path			(S1,"$game_saves$",S);
		
#ifdef DEBUG
		timer.Start				();
#endif
		MainMenu()->Screenshot		(IRender_interface::SM_FOR_GAMESAVE,S1);

#ifdef DEBUG
		Msg						("Screenshot overhead : %f milliseconds",timer.GetElapsed_sec()*1000.f);
#endif
	}
};

class CCC_ALifeReload : public IConsole_Command {
public:
	CCC_ALifeReload(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = true; };
	virtual void Execute(LPCSTR /**args/**/) {
		if (GameID() != GAME_SINGLE) return;
		NET_Packet			net_packet;
		net_packet.w_begin	(M_RELOAD_GAME);
		Level().Send		(net_packet,net_flags(TRUE));
	}
};

class CCC_ALifeLoadFrom : public IConsole_Command {
public:
	CCC_ALifeLoadFrom(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = true; };
	virtual void Execute(LPCSTR args) {
		string256	S;
		S[0]		= 0;
		sscanf		(args ,"%s",S);
		if (!xr_strlen(S)) {
			Log("* Specify file name!");
			return;
		}

		NET_Packet			net_packet;
		net_packet.w_begin	(M_LOAD_GAME);
		net_packet.w_stringZ(S);
		Level().Send		(net_packet,net_flags(TRUE));
	}
};

class CCC_FlushLog : public IConsole_Command {
public:
	CCC_FlushLog(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = true; };
	virtual void Execute(LPCSTR /**args/**/) {
		FlushLog();
		Msg		("* Log file has been saved successfully!");
	}
};

class CCC_FloatBlock : public CCC_Float {
public:
	CCC_FloatBlock(LPCSTR N, float* V, float _min=0, float _max=1) :
	  CCC_Float(N,V,_min,_max)
	  {};

	  virtual void	Execute	(LPCSTR args)
	  {
#ifdef _DEBUG
		  CCC_Float::Execute(args);
#else
		  if (!g_pGameLevel || GameID() == GAME_SINGLE)
			  CCC_Float::Execute(args);
		  else
		  {
			  Msg ("! Command disabled for this type of game");
		  }
#endif
	  }
};
class CCC_Net_CL_Resync : public IConsole_Command {
public:
	CCC_Net_CL_Resync(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = true; };
	virtual void Execute(LPCSTR /**args/**/) {
		Level().net_Syncronize();
	}
	virtual void	Info	(TInfo& I)		
	{
		strcpy(I,"resyncronize client"); 
	}
};

class CCC_KickPlayer : public IConsole_Command {
public:
	CCC_KickPlayer(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = false; };
	virtual void Execute(LPCSTR args) 
	{
		if (!OnServer())	return;
		
		char	Name[128];	Name[0]=0;
		char	Number[128]; Number[0] = 0;
		sscanf	(args,"%s %s", Name, Number);
		
		if (Name[0] == '#')
		{
			u32 Num = atol(Number);
			xrClientData *l_pC = (xrClientData*)	Level().Server->client_Get	(Num-1);
			if (!l_pC) return;
			if (Level().Server->GetServerClient() == l_pC)
			{
				Msg("! Can't disconnect server's client");
				return;
			};
			Msg("Disconnecting : %s", l_pC->ps->getName());
			Level().Server->DisconnectClient(l_pC);
			return;
		};

		u32	cnt = Level().Server->game->get_players_count();
		for(u32 it=0; it<cnt; it++)	
		{
			xrClientData *l_pC = (xrClientData*)	Level().Server->client_Get	(it);
			if (!l_pC) continue;
			if (!_stricmp(l_pC->ps->getName(), Name))
			{
				if (Level().Server->GetServerClient() == l_pC)
				{
					Msg("! Can't disconnect server's client");
					return;
				};
				Msg("Disconnecting : %s", l_pC->ps->getName());
				Level().Server->DisconnectClient(l_pC);
				return;
			}
		};

		Msg("! No such player found : %s", Name);
	};

	virtual void	Info	(TInfo& I)		
	{
		strcpy(I,"Kick Player"); 
	}
};


class CCC_StartTeamMoney : public IConsole_Command {
public:
	CCC_StartTeamMoney(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = true; };
	virtual void Execute(LPCSTR args) 
	{
		if (!OnServer())	return;

		game_sv_mp* pGameMP = smart_cast<game_sv_Deathmatch *>(Level().Server->game);
		if (!pGameMP) return;

		char Team[128] = "";
		s32 TeamMoney = 0;
		sscanf	(args,"%s %i", Team, &TeamMoney);

		if (!Team[0])
		{
			Msg("- --------------------");
			Msg("Teams start money:");
			u32 TeamCount = pGameMP->GetTeamCount();
			for (u32 i=0; i<TeamCount; i++)
			{
				TeamStruct* pTS = pGameMP->GetTeamData(i);
				if (!pTS) continue;
				Msg ("Team %d: %d", i, pTS->m_iM_Start);
			}
			Msg("- --------------------");
			return;
		}
		else
		{
			u32 TeamID = 0;
			s32 TeamStartMoney = 0;
			sscanf	(args,"%i %i", &TeamID, &TeamStartMoney);
			TeamStruct* pTS = pGameMP->GetTeamData(TeamID);
			if (pTS) 
			{
				pTS->m_iM_Start = TeamStartMoney;
			}
		}
	};

	virtual void	Info	(TInfo& I)		
	{
		strcpy(I,"Set Start Team Money"); 
	}
};

class CCC_BanPlayer : public IConsole_Command {
public:
	CCC_BanPlayer(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = false; };
	virtual void Execute(LPCSTR args) 
	{
		char	Name[128] = {0};
		char	Number[128] = {0};
		char	Time[128] = {0};
		char	Kick[128] = {0};
		sscanf	(args,"%s %s %s %s", Name, Number, Time, Kick);

		bool NeedKick = _stricmp(Kick, "kick") == NULL;
		if (!_stricmp(Time, "kick")) NeedKick = true;
		xrClientData *l_pC = NULL;
		char Address[4] = {0, 0, 0, 0};
		u32 BanTime = 0;
		bool BanByName = true;

		if (Name[0] == '#') //индентификация по номеру клиента в списке
		{
			BanTime = atol(Time);
			u32 Num = atol(Number);
			if (Num < 1) return;
			xrClientData *pCl = (xrClientData*)	Level().Server->client_Get	(Num-1);
			if (!pCl) return;
			l_pC = pCl;
			BanByName = true;
		}
		else
		{
			if (!_stricmp(Name, "ip"))	//Напрямую задается IP адрес
			{
				BanTime = atol(Time);

				char a[4][4] = {0, 0, 0, 0};
				sscanf(Number, "%[^'.'].%[^'.'].%[^'.'].%s", a[0], a[1], a[2], a[3]);

				for (int i=0; i<4; i++)
				{
					Address[i] = char(atol(a[i]));
				};

				BanByName = false;
			}
			else						// идентификация по имени игрока
			{
				BanTime = atol(Number);
				u32	cnt = Level().Server->game->get_players_count();
				for(u32 it=0; it<cnt; it++)	
				{
					xrClientData *pCl = (xrClientData*)	Level().Server->client_Get	(it);
					if (!pCl) continue;
					if (!_stricmp(pCl->ps->getName(), Name))
					{
						l_pC = pCl;
						break;
					}
				};
				BanByName = true;
			};
		};

		if (l_pC && BanByName)
		{
			if (l_pC != Level().Server->GetServerClient())
			{
				Msg("Player %s Banned%s", l_pC->ps->getName(), (NeedKick)? " and Kicked!" : "!");
				Level().Server->BanClient(l_pC, BanTime);
			}
			else
			{
				Msg("! Can't Ban server's client");
			};
		}

		if (!BanByName)
		{
			Msg("Address %i.%i.%i.%i Banned%s", 
				unsigned char(Address[0]), 
				unsigned char(Address[1]), 
				unsigned char(Address[2]), 
				unsigned char(Address[3]), 
				(NeedKick)? " and Kicked!" : "!");
			Level().Server->BanAddress(Address, BanTime);
		};

		if (NeedKick) 
		{
			if (l_pC && BanByName && (l_pC != Level().Server->GetServerClient())) Level().Server->DisconnectClient(l_pC);
			if (!BanByName) Level().Server->DisconnectAddress(Address);
		};
	};

	virtual void	Info	(TInfo& I)		
	{
		strcpy(I,"Ban Player"); 
	}
};
class CCC_ListPlayers : public IConsole_Command {
public:
	CCC_ListPlayers(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = true; };
	virtual void Execute(LPCSTR args) 
	{
		if (!OnServer())	return;
		
		u32	cnt = Level().Server->game->get_players_count();
		Msg("------------------------");
		Msg("- Total Players : %d", cnt);
		for(u32 it=0; it<cnt; it++)	
		{
			xrClientData *l_pC = (xrClientData*)	Level().Server->client_Get	(it);
			if (!l_pC) continue;
			char Address[4];
			DWORD dwPort = 0;
			Level().Server->GetClientAddress(l_pC->ID, Address, &dwPort);
			Msg("%d : %s - %i.%i.%i.%i:%i", it+1, l_pC->ps->getName(),
				unsigned char(Address[0]), 
				unsigned char(Address[1]), 
				unsigned char(Address[2]), 
				unsigned char(Address[3]),
				dwPort);
		};
		Msg("------------------------");
	};

	virtual void	Info	(TInfo& I)		
	{
		strcpy(I,"List Players"); 
	}
};

class CCC_GetServerAddress : public IConsole_Command {
public:
	CCC_GetServerAddress(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = true; };
	virtual void Execute(LPCSTR args) 
	{
		char sAddress[4];
		DWORD dwPort = 0;
		
		
		Level().GetServerAddress(sAddress, &dwPort);
		

		Msg("Server Address - %i.%i.%i.%i:%i",
			unsigned char(sAddress[0]), 
			unsigned char(sAddress[1]), 
			unsigned char(sAddress[2]), 
			unsigned char(sAddress[3]),
			dwPort);
	};

	virtual void	Info	(TInfo& I)		
	{
		strcpy(I,"List Players"); 
	}
};

class CCC_AddMap : public IConsole_Command {
public:
	CCC_AddMap(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = false; };
	virtual void Execute(LPCSTR args) 
	{
		char	MapName[256] = {0};
		sscanf	(args,"%s", MapName);

		Level().Server->game->MapRotation_AddMap(MapName);
	};

	virtual void	Info	(TInfo& I)		
	{
		strcpy(I,"Adds map to map rotation list"); 
	}
};

class CCC_NextMap : public IConsole_Command {
public:
	CCC_NextMap(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = true; };
	virtual void Execute(LPCSTR args) 
	{
		if (!OnServer())	return;

		Level().Server->game->OnNextMap();
	};

	virtual void	Info	(TInfo& I)		
	{
		strcpy(I,"Switch to Next Map in map rotation list"); 
	}
};

class CCC_PrevMap : public IConsole_Command {
public:
	CCC_PrevMap(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = true; };
	virtual void Execute(LPCSTR args) 
	{
		if (!OnServer())	return;

		Level().Server->game->OnPrevMap();
	};

	virtual void	Info	(TInfo& I)		
	{
		strcpy(I,"Switch to Previous Map in map rotation list"); 
	}
};

class CCC_AnomalySet : public IConsole_Command {
public:
	CCC_AnomalySet(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = false; };
	virtual void Execute(LPCSTR args) 
	{
		if (!OnServer())	return;
		if (GameID() == GAME_SINGLE) return;

		game_sv_Deathmatch* gameDM = smart_cast<game_sv_Deathmatch *>(Level().Server->game);
		if (!gameDM) return;

		char	AnomalySet[256];		
		sscanf	(args,"%s", AnomalySet);
		gameDM->StartAnomalies(atol(AnomalySet));
	};

	virtual void	Info	(TInfo& I)		
	{
		strcpy(I,"Activating pointed Anomaly set"); 
	}
};

class CCC_SetWeather : public IConsole_Command {
public:
	CCC_SetWeather(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = false; };
	virtual void Execute(LPCSTR args) 
	{
		if (!g_pGamePersistent) return;
		if (!OnServer())	return;

		char	weather_name[256] = "";		
		sscanf	(args,"%s", weather_name);
		if (!weather_name[0]) return;
		g_pGamePersistent->Environment.SetWeather(weather_name);		
	};

	virtual void	Info	(TInfo& I)		
	{
		strcpy(I,"Set new weather"); 
	}
};

class CCC_ChangeLevelGameType : public IConsole_Command {
public:
	CCC_ChangeLevelGameType(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = false; };
	virtual void Execute(LPCSTR args) 
	{
		if (!OnServer())	return;
		char	LevelName[256];	LevelName[0]=0;
		char	GameType[256];	GameType[0]=0;
		sscanf	(args,"%s %s", LevelName, GameType);
		//  [7/5/2005]
		if (!xr_strcmp(GameType, "dm")) sprintf(GameType, "deathmatch");
		if (!xr_strcmp(GameType, "tdm")) sprintf(GameType, "teamdeathmatch");
		if (!xr_strcmp(GameType, "ah")) sprintf(GameType, "artefacthunt");
		//  [7/5/2005]

		NET_Packet P;
		P.w_begin(M_CHANGE_LEVEL_GAME);
		P.w_stringZ(LevelName);
		P.w_stringZ(GameType);
		Level().Send(P);
	};

	virtual void	Info	(TInfo& I)		
	{
		strcpy(I,"Changing level and game type"); 
	}
};

class CCC_ChangeGameType : public CCC_ChangeLevelGameType {
public:
	CCC_ChangeGameType(LPCSTR N) : CCC_ChangeLevelGameType(N)  { bEmptyArgsHandled = false; };
	virtual void Execute(LPCSTR args) 
	{

		if (!OnServer())	return;

		char	GameType[256];	GameType[0]=0;
		sscanf	(args,"%s", GameType);

		char	argsNew[1024];
		sprintf(argsNew, "%s %s", Level().name().c_str(), GameType);

		CCC_ChangeLevelGameType::Execute((LPCSTR)argsNew);
	};

	virtual void	Info	(TInfo& I)		
	{
		strcpy(I,"Changing Game Type"); 
	};
};

class CCC_ChangeLevel : public CCC_ChangeLevelGameType {
public:
	CCC_ChangeLevel(LPCSTR N) : CCC_ChangeLevelGameType(N)  { bEmptyArgsHandled = false; };
	virtual void Execute(LPCSTR args) 
	{
		if (!OnServer())	return;

		char	LevelName[256];	LevelName[0]=0;
		sscanf	(args,"%s", LevelName);

		char	argsNew[1024];
		sprintf(argsNew, "%s %s", LevelName, Level().Server->game->type_name());

		CCC_ChangeLevelGameType::Execute((LPCSTR)argsNew);
	};

	virtual void	Info	(TInfo& I)		
	{
		strcpy(I,"Changing Game Type"); 
	}
};

class CCC_Vote_Start : public IConsole_Command {
public:
	CCC_Vote_Start(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = false; };
	virtual void Execute(LPCSTR args) 
	{
		if (GameID() == GAME_SINGLE)
		{
			Msg("! Only for multiplayer games!");
			return;
		}

		if (!Game().IsVotingEnabled())
		{
			Msg("! Voting is disabled by server!");
			return;
		}
		if (Game().IsVotingActive())
		{
			Msg("! There is voting already!");
			return;
		}

		if (Game().Phase() != GAME_PHASE_INPROGRESS)
		{
			Msg("! Voting is allowed only when game is in progress!");
			return;
		};

		Game().SendStartVoteMessage(args);		
	};

	virtual void	Info	(TInfo& I)		
	{
		strcpy(I,"Starts Voting"); 
	};
};

class CCC_Vote_Stop : public IConsole_Command {
public:
	CCC_Vote_Stop(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = true; };
	virtual void Execute(LPCSTR args) 
	{
		if (!OnServer()) return;

		if (Level().Server->game->Type() == GAME_SINGLE)
		{
			Msg("! Only for multiplayer games!");
			return;
		}

		if (!Level().Server->game->IsVotingEnabled())
		{
			Msg("! Voting is disabled by server!");
			return;
		}

		if (!Level().Server->game->IsVotingActive())
		{
			Msg("! Currently there is no active voting!");
			return;
		}

		if (Level().Server->game->Phase() != GAME_PHASE_INPROGRESS)
		{
			Msg("! Voting is allowed only when game is in progress!");
			return;
		};

		Level().Server->game->OnVoteStop();
	};

	virtual void	Info	(TInfo& I)		
	{
		strcpy(I,"Stops Current Voting"); 
	};
};

class CCC_Vote_Yes : public IConsole_Command {
public:
	CCC_Vote_Yes(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = true; };
	virtual void Execute(LPCSTR args) 
	{
		if (GameID() == GAME_SINGLE)
		{
			Msg("! Only for multiplayer games!");
			return;
		}

		if (!Game().IsVotingEnabled())
		{
			Msg("! Voting is disabled by server!");
			return;
		}

		if (!Game().IsVotingActive())
		{
			Msg("! Currently there is no active voting!");
			return;
		}

		if (Game().Phase() != GAME_PHASE_INPROGRESS)
		{
			Msg("! Voting is allowed only when game is in progress!");
			return;
		};

		Game().SendVoteYesMessage();
	};

	virtual void	Info	(TInfo& I)		
	{
		strcpy(I,"Vote Yes"); 
	};
};

class CCC_Vote_No : public IConsole_Command {
public:
	CCC_Vote_No(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = true; };
	virtual void Execute(LPCSTR args) 
	{
		if (GameID() == GAME_SINGLE)
		{
			Msg("! Only for multiplayer games!");
			return;
		}

		if (!Game().IsVotingEnabled())
		{
			Msg("! Voting is disabled by server!");
			return;
		}

		if (!Game().IsVotingActive())
		{
			Msg("! Currently there is no active voting!");
			return;
		}

		if (Game().Phase() != GAME_PHASE_INPROGRESS)
		{
			Msg("! Voting is allowed only when game is in progress!");
			return;
		};

		Game().SendVoteNoMessage();
	};

	virtual void	Info	(TInfo& I)		
	{
		strcpy(I,"Vote No"); 
	};
};

class CCC_Net_CL_InputUpdateRate : public CCC_Integer {
protected:
	int		*value_blin;
public:
	CCC_Net_CL_InputUpdateRate(LPCSTR N, int* V, int _min=0, int _max=999) :
	  CCC_Integer(N,V,_min,_max),
		  value_blin(V)
	  {};

	  virtual void	Execute	(LPCSTR args)
	  {
		  CCC_Integer::Execute(args);
		  if ((*value_blin > 0) && g_pGameLevel)
		  {
			  g_dwInputUpdateDelta = 1000/(*value_blin);
		  };
	  }
};

class CCC_SvControlHit : public CCC_Integer {
protected:
	int		*value_blin;
public:
	CCC_SvControlHit(LPCSTR N, int* V, int _min=0, int _max=999) :
	  CCC_Integer(N,V,_min,_max),
		  value_blin(V)
	  {};

	  virtual void	Execute	(LPCSTR args)
	  {
		  CCC_Integer::Execute(args);
		  if (g_pGameLevel)
		  {
			  Level().Server->game->signal_Syncronize();
		  };
		  /*
		  if (*value_blin == 0)
			  Level().Server->game->SetServerControlHits(false);
		  else
			  Level().Server->game->SetServerControlHits(true);
			  */
	  }
};

class CCC_AuthCheck : public CCC_Integer {
public:
	CCC_AuthCheck(LPCSTR N, int* V, int _min=0, int _max=999) :
	  CCC_Integer(N,V,_min,_max)
	  {};

	  virtual void	Save	(IWriter *F)	{};
};


class CCC_Net_CL_ClearStats : public IConsole_Command {
public:
	CCC_Net_CL_ClearStats(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = true; };
	virtual void Execute(LPCSTR /**args/**/) {
		Level().ClearStatistic();
	}
	virtual void	Info	(TInfo& I)		
	{
		strcpy(I,"clear client net statistic"); 
	}
};

class CCC_Net_SV_ClearStats : public IConsole_Command {
public:
	CCC_Net_SV_ClearStats(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = true; };
	virtual void Execute(LPCSTR /**args/**/) {
		Level().Server->ClearStatistic();
	}
	virtual void	Info	(TInfo& I)		
	{
		strcpy(I,"clear server net statistic"); 
	}
};

#ifndef MASTER_GOLD
class CCC_Script : public IConsole_Command {
public:
	CCC_Script(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = true; };
	virtual void Execute(LPCSTR args) {
		string256	S;
		S[0]		= 0;
		sscanf		(args ,"%s",S);
		if (!xr_strlen(S))
			Log("* Specify script name!");
		else {
			// rescan pathes
			FS_Path* P = FS.get_path("$game_scripts$");
			P->m_Flags.set	(FS_Path::flNeedRescan,TRUE);
			FS.rescan_pathes();
			// run script
			if (ai().script_engine().script_process(ScriptEngine::eScriptProcessorLevel))
				ai().script_engine().script_process(ScriptEngine::eScriptProcessorLevel)->add_script(S,false,true);
		}
	}
};

class CCC_ScriptCommand : public IConsole_Command {
public:
	CCC_ScriptCommand	(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = true; };
	virtual void	Execute				(LPCSTR args) {
		if (!xr_strlen(args))
			Log("* Specify string to run!");
		else {
			if (ai().script_engine().script_process(ScriptEngine::eScriptProcessorLevel))
				ai().script_engine().script_process(ScriptEngine::eScriptProcessorLevel)->add_script(args,true,true);
		}
	}
};
#endif

#ifdef DEBUG
class CCC_ScriptDbg : public IConsole_Command {
public:
	CCC_ScriptDbg(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = true; };
	virtual void Execute(LPCSTR args) {
		
		if(strstr(cName,"script_debug_break")==cName ){
		
		CScriptDebugger* d = ai().script_engine().debugger();
		if(d){
			if(d->Active())
				d->initiateDebugBreak();
			else
				Msg("Script debugger not active.");
		}else
			Msg("Script debugger not present.");
		}
		else if(strstr(cName,"script_debug_stop")==cName ){
			ai().script_engine().stopDebugger();
		}
		else if(strstr(cName,"script_debug_restart")==cName ){
			ai().script_engine().restartDebugger();
		};
	};
	

	virtual void	Info	(TInfo& I)		
	{
		if(strstr(cName,"script_debug_break")==cName )
			strcpy(I,"initiate script debugger [DebugBreak] command"); 

		else if(strstr(cName,"script_debug_stop")==cName )
			strcpy(I,"stop script debugger activity"); 

		else if(strstr(cName,"script_debug_restart")==cName )
			strcpy(I,"restarts script debugger or start if no script debugger presents"); 
	}
};

class CCC_DumpInfos : public IConsole_Command {
public:
	CCC_DumpInfos	(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = true; };
	virtual void	Execute				(LPCSTR args) {
		CActor* A =smart_cast<CActor*>(Level().CurrentEntity());
		if(A)
			A->DumpInfo();
	}
	virtual void	Info	(TInfo& I)		
	{
		strcpy(I,"dumps all infoportions that actor have"); 
	}
};
#include "map_manager.h"
class CCC_DumpMap : public IConsole_Command {
public:
	CCC_DumpMap	(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = true; };
	virtual void	Execute				(LPCSTR args) {
		Level().MapManager().Dump();
	}
	virtual void	Info	(TInfo& I)		
	{
		strcpy(I,"dumps all currentmap locations"); 
	}

};

#include "alife_graph_registry.h"
class CCC_DumpCreatures : public IConsole_Command {
public:
	CCC_DumpCreatures	(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = true; };
	virtual void	Execute				(LPCSTR args) {
		
		typedef CSafeMapIterator<ALife::_OBJECT_ID,CSE_ALifeDynamicObject>::_REGISTRY::const_iterator const_iterator;

		const_iterator I = ai().alife().graph().level().objects().begin();
		const_iterator E = ai().alife().graph().level().objects().end();
		for ( ; I != E; ++I) {
			CSE_ALifeCreatureAbstract *obj = smart_cast<CSE_ALifeCreatureAbstract *>(I->second);
			if (obj) {
				Msg("\"%s\",",obj->name_replace());
			}
		}		

	}
	virtual void	Info	(TInfo& I)		
	{
		strcpy(I,"dumps all creature names"); 
	}

};



class CCC_DebugFonts : public IConsole_Command {
public:
	CCC_DebugFonts (LPCSTR N) : IConsole_Command(N) {bEmptyArgsHandled = true; }
	virtual void Execute				(LPCSTR args) {
		HUD().GetUI()->StartStopMenu( xr_new<CUIDebugFonts>(), true);		
	}
};

class CCC_PostprocessTest : public IConsole_Command {
public:
	CCC_PostprocessTest(LPCSTR N) : IConsole_Command(N)  { };

	virtual void Execute(LPCSTR args) {

		string128 param1, param2, param3;
		_GetItem(args,0,param1,' ');
		_GetItem(args,1,param2,' ');
		_GetItem(args,2,param3,' ');

		CObject			*obj = Level().Objects.FindObjectByName(param1);
		CCustomZone		*p_zone = smart_cast<CCustomZone *>(obj);
		if (!p_zone) return;

		u32 type = 0;
		if (xr_strcmp(param2, "d_v") == 0)		type = 0;
		else if (xr_strcmp(param2,"d_h") == 0)	type = 1;
		else if (xr_strcmp(param2,"n_i") == 0)	type = 2;
		else if (xr_strcmp(param2,"n_g") == 0)	type = 3;
		else if (xr_strcmp(param2,"n_f") == 0)	type = 4;
		else if (xr_strcmp(param2,"blur") == 0) type = 5;
		else if (xr_strcmp(param2,"gray") == 0) type = 6;
		else if (xr_strcmp(param2,"cb_r") == 0) type = 7;
		else if (xr_strcmp(param2,"cb_g") == 0) type = 8;
		else if (xr_strcmp(param2,"cb_b") == 0) type = 9;
		else if (xr_strcmp(param2,"cg_r") == 0) type = 10;
		else if (xr_strcmp(param2,"cg_g") == 0) type = 11;
		else if (xr_strcmp(param2,"cg_b") == 0) type = 12;
		else if (xr_strcmp(param2,"ca_r") == 0) type = 13;
		else if (xr_strcmp(param2,"ca_g") == 0) type = 14;
		else if (xr_strcmp(param2,"ca_b") == 0) type = 15;
		else if (xr_strcmp(param2,"d_min") == 0) type = 16;
		else if (xr_strcmp(param2,"d_max") == 0) type = 17;
		else return;

		float value;
		sscanf(param3 ,"%f",&value);
		p_zone->m_effector->SetParam	(type,value);
	}
};

class CCC_DebugNode : public IConsole_Command {
public:
	CCC_DebugNode(LPCSTR N) : IConsole_Command(N)  { };

	virtual void Execute(LPCSTR args) {

		string128 param1, param2;
		_GetItem(args,0,param1,' ');
		_GetItem(args,1,param2,' ');

		u32 value1;
		u32 value2;
		
		sscanf(param1,"%u",&value1);
		sscanf(param2,"%u",&value2);
		
		if ((value1 > 0) && (value2 > 0)) {
			g_bDebugNode		= TRUE;
			g_dwDebugNodeSource	= value1;
			g_dwDebugNodeDest	= value2;
		} else {
			g_bDebugNode = FALSE;
		}
	}
};

class CCC_ShowMonsterInfo : public IConsole_Command {
public:
				CCC_ShowMonsterInfo(LPCSTR N) : IConsole_Command(N)  { };

	virtual void Execute(LPCSTR args) {

		string128 param1, param2;
		_GetItem(args,0,param1,' ');
		_GetItem(args,1,param2,' ');

		CObject			*obj = Level().Objects.FindObjectByName(param1);
		CBaseMonster	*monster = smart_cast<CBaseMonster *>(obj);
		if (!monster)	return;
		
		u32				value2;
		
		sscanf			(param2,"%u",&value2);
		monster->set_show_debug_info (u8(value2));
	}
};
class CCC_DbgPhTrackObj : public IConsole_Command {
public:
	CCC_DbgPhTrackObj(LPCSTR N) : IConsole_Command(N)  { };
	virtual void Execute(LPCSTR args/**/) {
			ph_dbg_draw_mask1.set(ph_m1_DbgTrackObject,TRUE);
			PH_DBG_SetTrackObject(args);
			//CObject* O= Level().Objects.FindObjectByName(args);
			//if(O)
			//{
			//	PH_DBG_SetTrackObject(*(O->cName()));
			//	ph_dbg_draw_mask1.set(ph_m1_DbgTrackObject,TRUE);
			//}

		}
	
	//virtual void	Info	(TInfo& I)		
	//{
	//	strcpy(I,"restart game fast"); 
	//}
};
#endif

class CCC_PHIterations : public CCC_Integer {
public:
		CCC_PHIterations(LPCSTR N) :
		CCC_Integer(N,&phIterations,5,50)
		{};
	  virtual void	Execute	(LPCSTR args)
	  {
		  CCC_Integer::Execute	(args);
		  dWorldSetQuickStepNumIterations(NULL,phIterations);
	  }
};

class CCC_PHGravity : public IConsole_Command {
public:
		CCC_PHGravity(LPCSTR N) :
		IConsole_Command(N)
		{};
	  virtual void	Execute	(LPCSTR args)
	  {
		  if(!ph_world)	return;
#ifndef DEBUG
		  if (g_pGameLevel && Level().game && GameID() != GAME_SINGLE)
		  {
			  Msg("Command is not available in Multiplayer");
			  return;
		  }
#endif
		  ph_world->SetGravity(float(atof(args)));
	  }
	  virtual void	Status	(TStatus& S)
	{	
		if(ph_world)
			sprintf	(S,"%3.5f",ph_world->Gravity());
		else
			sprintf	(S,"%3.5f",default_world_gravity);
		while	(xr_strlen(S) && ('0'==S[xr_strlen(S)-1]))	S[xr_strlen(S)-1] = 0;
	}
	
};

#ifdef DEBUG
extern void print_help(lua_State *L);

struct CCC_LuaHelp : public IConsole_Command {
	CCC_LuaHelp(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = true; };

	virtual void Execute(LPCSTR args) {
		print_help(ai().script_engine().lua());
	}
};

struct CCC_ShowSmartCastStats : public IConsole_Command {
	CCC_ShowSmartCastStats(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = true; };

	virtual void Execute(LPCSTR args) {
		show_smart_cast_stats();
	}
};

struct CCC_ClearSmartCastStats : public IConsole_Command {
	CCC_ClearSmartCastStats(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = true; };

	virtual void Execute(LPCSTR args) {
		clear_smart_cast_stats();
	}
};
#endif

struct CCC_JumpToLevel : public IConsole_Command {
	CCC_JumpToLevel(LPCSTR N) : IConsole_Command(N)  {};

	virtual void Execute(LPCSTR args) {
		if (!ai().get_alife()) {
			Msg				("! ALife simulator is needed to perform specified command!");
			return;
		}
		string256		level;
		sscanf(args,"%s",level);

		GameGraph::LEVEL_MAP::const_iterator	I = ai().game_graph().header().levels().begin();
		GameGraph::LEVEL_MAP::const_iterator	E = ai().game_graph().header().levels().end();
		for ( ; I != E; ++I)
			if (!xr_strcmp((*I).second.name(),level)) {
				ai().alife().jump_to_level(level);
				return;
			}
		Msg							("! There is no level \"%s\" in the game graph!",level);
	}
};
#include "GamePersistent.h"


class CCC_MainMenu : public IConsole_Command {
public:
	CCC_MainMenu(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = true; };
	virtual void Execute(LPCSTR args) {

		bool bWhatToDo = TRUE;
		if( 0==xr_strlen(args) ){
			bWhatToDo = !MainMenu()->IsActive();
		};

		if( EQ(args,"on")||EQ(args,"1") )
			bWhatToDo = TRUE;

		if( EQ(args,"off")||EQ(args,"0") )
			bWhatToDo = FALSE;

		MainMenu()->Activate( bWhatToDo );
	}
};

struct CCC_StartTimeSingle : public IConsole_Command {
	CCC_StartTimeSingle(LPCSTR N) : IConsole_Command(N) {};
	virtual void	Execute	(LPCSTR args)
	{
		u32 year = 1, month = 1, day = 1, hours = 0, mins = 0, secs = 0, milisecs = 0;
		sscanf				(args,"%d.%d.%d %d:%d:%d.%d",&year,&month,&day,&hours,&mins,&secs,&milisecs);
		year				= _max(year,1);
		month				= _max(month,1);
		day					= _max(day,1);
		g_qwStartGameTime	= generate_time	(year,month,day,hours,mins,secs,milisecs);

		if (!g_pGameLevel)
			return;

		if (!Level().Server)
			return;

		if (!Level().Server->game)
			return;

		Level().Server->game->SetGameTimeFactor(g_qwStartGameTime,g_fTimeFactor);
	}

	virtual void	Status	(TStatus& S)
	{
		u32 year = 1, month = 1, day = 1, hours = 0, mins = 0, secs = 0, milisecs = 0;
		split_time	(g_qwStartGameTime, year, month, day, hours, mins, secs, milisecs);
		sprintf		(S,"%d.%d.%d %d:%d:%d.%d",year,month,day,hours,mins,secs,milisecs);
	}
};

struct CCC_TimeFactorSingle : public CCC_Float {
	CCC_TimeFactorSingle(LPCSTR N, float* V, float _min=0.f, float _max=1.f) : CCC_Float(N,V,_min,_max) {};

	virtual void	Execute	(LPCSTR args)
	{
		CCC_Float::Execute	(args);
		
		if (!g_pGameLevel)
			return;

		if (!Level().Server)
			return;

		if (!Level().Server->game)
			return;

		Level().Server->game->SetGameTimeFactor(g_fTimeFactor);
	}
};

struct CCC_StartTimeEnvironment: public IConsole_Command {
	CCC_StartTimeEnvironment(LPCSTR N) : IConsole_Command(N) {};
	virtual void	Execute	(LPCSTR args)
	{
		u32 year = 1, month = 1, day = 1, hours = 0, mins = 0, secs = 0, milisecs = 0;
		
		sscanf				(args,"%d:%d:%d.%d",&hours,&mins,&secs,&milisecs);
		u64 NewTime	= generate_time	(year,month,day,hours,mins,secs,milisecs);

		if (!g_pGameLevel)
			return;

		if (!Level().Server)
			return;

		if (!Level().Server->game)
			return;

		Level().Server->game->SetEnvironmentGameTimeFactor(NewTime,g_fTimeFactor);
		Level().Server->game->SetGameTimeFactor(NewTime,g_fTimeFactor);
	}
};
#ifdef DEBUG
class CCC_RadioGroupMask2;
class CCC_RadioMask :public CCC_Mask
{
	CCC_RadioGroupMask2		*group;
public:
	CCC_RadioMask(LPCSTR N, Flags32* V, u32 M):
	  CCC_Mask(N,V,M)
	 {
		group=NULL;
	 }
		void	SetGroup	(CCC_RadioGroupMask2		*G)
	{
		group=G													;
	}
virtual	void	Execute		(LPCSTR args)						;
	
IC		void	Set			(BOOL V)
	  {
		  value->set(mask,V)									;
	  }

};

class CCC_RadioGroupMask2 
{
	CCC_RadioMask *mask0;
	CCC_RadioMask *mask1;
public:
	CCC_RadioGroupMask2(CCC_RadioMask *m0,CCC_RadioMask *m1)
	  {
		mask0=m0;mask1=m1;
		mask0->SetGroup(this);
		mask1->SetGroup(this);
	  }
	void	Execute	(CCC_RadioMask& m,LPCSTR args)
	{
		BOOL value=m.GetValue();
		if(value)
		{
			mask0->Set(!value);mask1->Set(!value);
		}
		m.Set(value);
	}
};


void	CCC_RadioMask::Execute	(LPCSTR args)
{
	CCC_Mask::Execute(args);
	VERIFY2(group,"CCC_RadioMask: group not set");
	group->Execute(*this,args);
}

#define CMD_RADIOGROUPMASK2(p1,p2,p3,p4,p5,p6)		\
{\
static CCC_RadioMask x##CCC_RadioMask1(p1,p2,p3);		Console->AddCommand(&x##CCC_RadioMask1);\
static CCC_RadioMask x##CCC_RadioMask2(p4,p5,p6);		Console->AddCommand(&x##CCC_RadioMask2);\
static CCC_RadioGroupMask2 x##CCC_RadioGroupMask2(&x##CCC_RadioMask1,&x##CCC_RadioMask2);\
}

struct CCC_DbgBullets : public CCC_Integer {
	CCC_DbgBullets(LPCSTR N, int* V, int _min=0, int _max=999) : CCC_Integer(N,V,_min,_max) {};

	virtual void	Execute	(LPCSTR args)
	{
		extern FvectorVec g_hit[];
		g_hit[0].clear();
		g_hit[1].clear();
		g_hit[2].clear();
		CCC_Integer::Execute	(args);
	}
};

#include "attachable_item.h"
#include "attachment_owner.h"
class CCC_TuneAttachableItem : public IConsole_Command
{
public		:
	CCC_TuneAttachableItem(LPCSTR N):IConsole_Command(N){};
	virtual void	Execute	(LPCSTR args)
	{
		if( CAttachableItem::m_dbgItem){
			CAttachableItem::m_dbgItem = NULL;	
			Msg("CCC_TuneAttachableItem switched to off");
			return;
		};

		CObject* obj = Level().CurrentViewEntity();	VERIFY(obj);
		CAttachmentOwner* owner = smart_cast<CAttachmentOwner*>(obj);
		shared_str ssss = args;
		CAttachableItem* itm = owner->attachedItem(ssss);
		if(itm){
			CAttachableItem::m_dbgItem = itm;
			Msg("CCC_TuneAttachableItem switched to ON for [%s]",args);
		}else
			Msg("CCC_TuneAttachableItem cannot find attached item [%s]",args);
	}

	virtual void	Info	(TInfo& I)
	{	
		sprintf(I,"allows to change bind rotation and position offsets for attached item, <section_name> given as arguments");
	}
};


#endif

void CCC_RegisterCommands()
{
	// options
	g_OptConCom.Init();

	// game
	psActorFlags.set(AF_ALWAYSRUN, true);
	CMD3(CCC_Mask,				"g_always_run",			&psActorFlags,	AF_ALWAYSRUN);
	CMD3(CCC_Mask,				"g_god",				&psActorFlags,	AF_GODMODE	);
	CMD1(CCC_Spawn,				"g_spawn"				);
	CMD1(CCC_GameDifficulty,	"g_game_difficulty"		);
	CMD1(CCC_Restart,			"g_restart"				);
	CMD1(CCC_RestartFast,		"g_restart_fast"		);
	CMD1(CCC_Money,				"g_money"				);
//	CMD1(CCC_Team,				"g_change_team"			);
	CMD1(CCC_Kill,				"g_kill"				);
	CMD3(CCC_Mask,				"g_backrun",			&psActorFlags,	AF_RUN_BACKWARD);

	CMD3(CCC_Mask,				"g_gametask_auto_activate",&CGameTask::m_game_task_flags,CGameTask::eForceNewTaskActivation);
	
	// alife
	CMD1(CCC_ALifePath,			"al_path"				);		// build path
	CMD1(CCC_ALifeSave,			"save"					);		// save game
	CMD1(CCC_ALifeReload,		"reload"				);		// reload game
	CMD1(CCC_ALifeLoadFrom,		"load"				);		// load game from ...
	CMD1(CCC_FlushLog,			"flush"					);		// flush log
	CMD1(CCC_ALifeTimeFactor,	"al_time_factor"		);		// set time factor
	CMD1(CCC_ALifeSwitchDistance,"al_switch_distance"	);		// set switch distance
	CMD1(CCC_ALifeProcessTime,	"al_process_time"		);		// set process time
	CMD1(CCC_ALifeSwitchFactor,	"al_switch_factor"		);		// set switch factor
	CMD1(CCC_JumpToLevel,		"jump_to_level"			);
#ifdef ALIFE_SUPPORT_CONSOLE_COMMANDS
	CMD1(CCC_ALifeScheduleMin,	"al_schedule_min"		);		// set min schedule
	CMD1(CCC_ALifeScheduleMax,	"al_schedule_max"		);		// set max schedule
	CMD1(CCC_ALifeListAll,		"al_la"					);		// list all (objects, events and tasks)
	CMD1(CCC_ALifeListObjects,	"al_lo"					);		// list objects
	CMD1(CCC_ALifeListEvents,	"al_le"					);		// list events
	CMD1(CCC_ALifeListTasks,	"al_lt"					);		// list tasks
	CMD1(CCC_ALifeListTerrain,	"al_lr"					);		// list terrain
	CMD1(CCC_ALifeListSpawns,	"al_ls"					);		// list spawnpoints
	CMD1(CCC_ALifeObjectInfo,	"al_io"					);		// object info
	CMD1(CCC_ALifeEventInfo,	"al_ie"					);		// event info
	CMD1(CCC_ALifeTaskInfo,		"al_it"					);		// task info
	CMD1(CCC_ALifeSpawnInfo,	"al_is"					);		// spawn-point info
	CMD1(CCC_ALifeGraphInfo,	"al_ig"					);		// graph-point info
#endif

	// hud
	psHUD_Flags.set(HUD_CROSSHAIR, true);
	psHUD_Flags.set(HUD_WEAPON, true);
	psHUD_Flags.set(HUD_DRAW, true);
	psHUD_Flags.set(HUD_INFO, true);
	CMD3(CCC_Mask,				"hud_crosshair",		&psHUD_Flags,	HUD_CROSSHAIR);
	CMD3(CCC_Mask,				"hud_crosshair_dist",	&psHUD_Flags,	HUD_CROSSHAIR_DIST);
	CMD3(CCC_Mask,				"hud_weapon",			&psHUD_Flags,	HUD_WEAPON);
	CMD3(CCC_Mask,				"hud_info",				&psHUD_Flags,	HUD_INFO);
	CMD3(CCC_Mask,				"hud_draw",				&psHUD_Flags,	HUD_DRAW);
	CMD3(CCC_Mask,				"hud_animate_hit",		&psHUD_Flags,	HUD_CAM_ANIM_HIT);

	CMD2(CCC_Float,				"hud_fov",				&psHUD_FOV);

	// Demo
	CMD1(CCC_DemoRecord,		"demo_record"			);
	CMD1(CCC_DemoPlay,			"demo_play"				);

	// ai
	CMD3(CCC_Mask,				"mt_ai_vision",			&g_mt_config,	mtAiVision);
	CMD3(CCC_Mask,				"mt_level_path",		&g_mt_config,	mtLevelPath);
	CMD3(CCC_Mask,				"mt_detail_path",		&g_mt_config,	mtDetailPath);
	CMD3(CCC_Mask,				"mt_object_handler",	&g_mt_config,	mtObjectHandler);
	CMD3(CCC_Mask,				"mt_sound_player",		&g_mt_config,	mtSoundPlayer);
	CMD3(CCC_Mask,				"mt_bullets",			&g_mt_config,	mtBullets);
	CMD3(CCC_Mask,				"mt_script_gc",			&g_mt_config,	mtLUA_GC);
	CMD3(CCC_Mask,				"mt_level_sounds",		&g_mt_config,	mtLevelSounds);
	CMD4(CCC_Integer,			"lua_gcstep",			&psLUA_GCSTEP,	1, 1000);
#ifdef DEBUG
	CMD3(CCC_Mask,				"ai_debug",				&psAI_Flags,	aiDebug);
	CMD3(CCC_Mask,				"ai_dbg_brain",			&psAI_Flags,	aiBrain);
	CMD3(CCC_Mask,				"ai_dbg_motion",		&psAI_Flags,	aiMotion);
	CMD3(CCC_Mask,				"ai_dbg_frustum",		&psAI_Flags,	aiFrustum);
	CMD3(CCC_Mask,				"ai_dbg_funcs",			&psAI_Flags,	aiFuncs);
	CMD3(CCC_Mask,				"ai_dbg_alife",			&psAI_Flags,	aiALife);
	CMD3(CCC_Mask,				"ai_dbg_lua",			&psAI_Flags,	aiLua);
	CMD3(CCC_Mask,				"ai_dbg_goap",			&psAI_Flags,	aiGOAP);
	CMD3(CCC_Mask,				"ai_dbg_goap_script",	&psAI_Flags,	aiGOAPScript);
	CMD3(CCC_Mask,				"ai_dbg_goap_object",	&psAI_Flags,	aiGOAPObject);
	CMD3(CCC_Mask,				"ai_dbg_cover",			&psAI_Flags,	aiCover);
	CMD3(CCC_Mask,				"ai_dbg_anim",			&psAI_Flags,	aiAnimation);
	CMD3(CCC_Mask,				"ai_dbg_vision",		&psAI_Flags,	aiVision);
	CMD3(CCC_Mask,				"ai_dbg_monster",		&psAI_Flags,	aiMonsterDebug);
	CMD3(CCC_Mask,				"ai_dbg_stalker",		&psAI_Flags,	aiStalker);
	CMD3(CCC_Mask,				"ai_stats",				&psAI_Flags,	aiStats);
	CMD3(CCC_Mask,				"ai_dbg_destroy",		&psAI_Flags,	aiDestroy);
	CMD3(CCC_Mask,				"ai_dbg_serialize",		&psAI_Flags,	aiSerialize);
	CMD3(CCC_Mask,				"ai_dbg_dialogs",		&psAI_Flags,	aiDialogs);
	CMD3(CCC_Mask,				"ai_dbg_sectors",		&psAI_Flags,	aiSectors);
	CMD3(CCC_Mask,				"ai_dbg_infoportion",	&psAI_Flags,	aiInfoPortion);

	CMD4(CCC_Integer,			"ai_dbg_inactive_time",	&g_AI_inactive_time, 0, 1000000);
	
	CMD1(CCC_DebugNode,			"ai_dbg_node");
	CMD1(CCC_ScriptDbg,			"script_debug_break");
	CMD1(CCC_ScriptDbg,			"script_debug_stop");
	CMD1(CCC_ScriptDbg,			"script_debug_restart");
	
	CMD1(CCC_ShowMonsterInfo,	"ai_monster_info");
	CMD1(CCC_DebugFonts,		"debug_fonts");
	CMD1(CCC_PostprocessTest,	"pp_test");
	CMD1(CCC_TuneAttachableItem,"dbg_adjust_attachable_item");

	// adjust mode support
	CMD4(CCC_Integer,	"hud_adjust_mode",			&g_bHudAdjustMode,	0, 5);
	CMD4(CCC_Float,		"hud_adjust_value",			&g_fHudAdjustValue,	0.0f, 1.0f);

#endif // DEBUG
	
#ifndef MASTER_GOLD
	CMD1(CCC_Script,			"run_script");
	CMD1(CCC_ScriptCommand,		"run_string");
#endif

	// Physics
	CMD4(CCC_Integer,			"ph_fps",						&phFPS						,			10,		100				);
	CMD4(CCC_Integer,			"ph_tri_clear_disable_count",	&ph_tri_clear_disable_count	,			0,		255				);
	CMD1(CCC_PHGravity,			"ph_gravity"																					);
	CMD1(CCC_PHIterations,		"ph_iterations"																					);
	CMD4(CCC_FloatBlock,		"ph_timefactor",				&phTimefactor				,			0.0001f	,1000.f			);
	CMD4(CCC_FloatBlock,		"ph_break_common_factor",		&phBreakCommonFactor		,			0.f		,1000000000.f	);
	CMD4(CCC_FloatBlock,		"ph_rigid_break_weapon_factor",	&phRigidBreakWeaponFactor	,			0.f		,1000000000.f	);
	CMD4(CCC_FloatBlock,		"ph_tri_query_ex_aabb_rate",	&ph_tri_query_ex_aabb_rate	,			1.01f	,3.f			);
	//CMD4(CCC_FloatBlock,		"snd_collide_max_volume",		&collide_volume_max			,			1.f		,5000.f			);
	//CMD4(CCC_FloatBlock,		"snd_collide_min_volume",		&collide_volume_min 		,			0.f		,5000.f			);
	// Mad Max
	// Net Interpolation
	CMD4(CCC_Float,					"net_cl_interpolation",		&g_cl_lvInterp,				-1,1);
	CMD4(CCC_Integer,				"net_cl_icurvetype",		&g_cl_InterpolationType,	0, 2)	;
	CMD4(CCC_Integer,				"net_cl_icurvesize",		(int*)&g_cl_InterpolationMaxPoints,	0, 2000)	;
	
	CMD1(CCC_Net_CL_Resync,			"net_cl_resync" );
	CMD1(CCC_Net_CL_ClearStats,		"net_cl_clearstats" );
	CMD1(CCC_Net_SV_ClearStats,		"net_sv_clearstats" );
//	CMD4(CCC_Integer,				"net_cl_inputguaranteed",	&net_cl_inputguaranteed,	0, 1)	;
//	CMD4(CCC_Net_CL_InputUpdateRate,"net_cl_inputupdaterate",	&net_cl_inputupdaterate,	1, 100)	;


	CMD3(CCC_Mask,		"g_unlimitedammo",			&psActorFlags,	AF_UNLIMITEDAMMO);
	CMD3(CCC_Mask,		"g_autopickup",				&psActorFlags,	AF_AUTOPICKUP);


	// Enable/disable news
	CMD4(CCC_Integer,	"hud_news_disable",			&g_bNewsDisable,	0, 1);

	// Network
	CMD4(CCC_Integer,	"net_cl_update_rate",	&psNET_ClientUpdate,0,		100				);
	CMD4(CCC_Integer,	"net_cl_pending_lim",	&psNET_ClientPending,0,		10				);
	CMD4(CCC_Integer,	"net_sv_update_rate",	&psNET_ServerUpdate,0,		100				);
	CMD4(CCC_Integer,	"net_sv_pending_lim",	&psNET_ServerPending,0,		10				);
//	CMD3(CCC_String,	"net_name",				psNET_Name,			32						);
	CMD3(CCC_Mask,		"net_dump_size",		&psNET_Flags,		NETFLAG_DBG_DUMPSIZE	);
	CMD1(CCC_Dbg_NumObjects,	"net_dbg_objects"				);

	//Recon params
	CMD4(CCC_Float,		"recon_min_dist",		&g_fMinReconDist,	1.0f, 50.0f);
	CMD4(CCC_Float,		"recon_max_dist",		&g_fMaxReconDist,	50.0f, 100.0f);
	CMD4(CCC_Float,		"recon_min_speed",		&g_fMinReconSpeed,	0.01f, 180.0f);
	CMD4(CCC_Float,		"recon_max_speed",		&g_fMaxReconSpeed,	0.01f, 180.0f);

///	CMD4(CCC_Integer,	"g_checktime",			&g_bCheckTime,		0,	1);
	CMD4(CCC_Integer,	"g_eventdelay",			&g_dwEventDelay,	0,	1000);

	CMD4(CCC_Integer,	"g_corpsenum",			(int*)&g_dwMaxCorpses,		0,	100);

#ifdef DEBUG
	CMD1(CCC_LuaHelp,				"lua_help");
	CMD1(CCC_ShowSmartCastStats,	"show_smart_cast_stats");
	CMD1(CCC_ClearSmartCastStats,	"clear_smart_cast_stats");

	CMD3(CCC_Mask,		"dbg_draw_actor_alive",		&dbg_net_Draw_Flags,	(1<<0));
	CMD3(CCC_Mask,		"dbg_draw_actor_dead",		&dbg_net_Draw_Flags,	(1<<1));
	CMD3(CCC_Mask,		"dbg_draw_customzone",		&dbg_net_Draw_Flags,	(1<<2));
	CMD3(CCC_Mask,		"dbg_draw_teamzone",		&dbg_net_Draw_Flags,	(1<<3));
	CMD3(CCC_Mask,		"dbg_draw_invitem",			&dbg_net_Draw_Flags,	(1<<4));
	CMD3(CCC_Mask,		"dbg_draw_actor_phys",		&dbg_net_Draw_Flags,	(1<<5));
	CMD3(CCC_Mask,		"dbg_draw_customdetector",	&dbg_net_Draw_Flags,	(1<<6));
	CMD3(CCC_Mask,		"dbg_destroy",				&dbg_net_Draw_Flags,	(1<<7));
	CMD3(CCC_Mask,		"dbg_draw_autopickupbox",	&dbg_net_Draw_Flags,	(1<<8));
	CMD3(CCC_Mask,		"dbg_draw_rp",				&dbg_net_Draw_Flags,	(1<<9));
	CMD3(CCC_Mask,		"dbg_draw_climbable",		&dbg_net_Draw_Flags,	(1<<10));
	CMD3(CCC_Mask,		"dbg_draw_skeleton",		&dbg_net_Draw_Flags,	(1<<11));


	CMD3(CCC_Mask,		"dbg_draw_ph_contacts",			&ph_dbg_draw_mask,	phDbgDrawContacts);
	CMD3(CCC_Mask,		"dbg_draw_ph_enabled_aabbs",	&ph_dbg_draw_mask,	phDbgDrawEnabledAABBS);
	CMD3(CCC_Mask,		"dbg_draw_ph_intersected_tries",&ph_dbg_draw_mask,	phDBgDrawIntersectedTries);
	CMD3(CCC_Mask,		"dbg_draw_ph_saved_tries",		&ph_dbg_draw_mask,	phDbgDrawSavedTries);
	CMD3(CCC_Mask,		"dbg_draw_ph_tri_trace",		&ph_dbg_draw_mask,	phDbgDrawTriTrace);
	CMD3(CCC_Mask,		"dbg_draw_ph_positive_tries",	&ph_dbg_draw_mask,	phDBgDrawPositiveTries);
	CMD3(CCC_Mask,		"dbg_draw_ph_negative_tries",	&ph_dbg_draw_mask,	phDBgDrawNegativeTries);
	CMD3(CCC_Mask,		"dbg_draw_ph_tri_test_aabb",	&ph_dbg_draw_mask,	phDbgDrawTriTestAABB);
	CMD3(CCC_Mask,		"dbg_draw_ph_tries_changes_sign",&ph_dbg_draw_mask,	phDBgDrawTriesChangesSign);
	CMD3(CCC_Mask,		"dbg_draw_ph_tri_point"			,&ph_dbg_draw_mask,	phDbgDrawTriPoint);
	CMD3(CCC_Mask,		"dbg_draw_ph_explosion_position",&ph_dbg_draw_mask,	phDbgDrawExplosionPos);
	CMD3(CCC_Mask,		"dbg_draw_ph_statistics"		,&ph_dbg_draw_mask,	phDbgDrawObjectStatistics);
	CMD3(CCC_Mask,		"dbg_draw_ph_mass_centres"		,&ph_dbg_draw_mask,	phDbgDrawMassCenters);
	CMD3(CCC_Mask,		"dbg_draw_ph_death_boxes"		,&ph_dbg_draw_mask,	phDbgDrawDeathActivationBox);
	CMD3(CCC_Mask,		"dbg_draw_ph_hit_app_pos"		,&ph_dbg_draw_mask,	phHitApplicationPoints);
	CMD3(CCC_Mask,		"dbg_draw_ph_cashed_tries_stats",&ph_dbg_draw_mask,	phDbgDrawCashedTriesStat);
	CMD3(CCC_Mask,		"dbg_draw_ph_car_dynamics"		,&ph_dbg_draw_mask,	phDbgDrawCarDynamics);
	CMD3(CCC_Mask,		"dbg_draw_ph_car_plots"			,&ph_dbg_draw_mask,	phDbgDrawCarPlots);
	CMD3(CCC_Mask,		"dbg_ph_ladder"					,&ph_dbg_draw_mask,	phDbgLadder);
	CMD3(CCC_Mask,		"dbg_draw_ph_explosions"		,&ph_dbg_draw_mask,	phDbgDrawExplosions);
	CMD3(CCC_Mask,		"dbg_draw_car_plots_all_trans"	,&ph_dbg_draw_mask,	phDbgDrawCarAllTrnsm);
	CMD3(CCC_Mask,		"dbg_draw_ph_zbuffer_disable"	,&ph_dbg_draw_mask,	phDbgDrawZDisable);
	CMD3(CCC_Mask,		"dbg_ph_obj_collision_damage"	,&ph_dbg_draw_mask,	phDbgDispObjCollisionDammage);
	CMD_RADIOGROUPMASK2("dbg_ph_ai_always_phmove",&ph_dbg_draw_mask,phDbgAlwaysUseAiPhMove,"dbg_ph_ai_never_phmove",&ph_dbg_draw_mask,phDbgNeverUseAiPhMove);
	CMD3(CCC_Mask,		"dbg_ph_ik_anim_goal_only"		,&ph_dbg_draw_mask,	phDbgIKAnimGoalOnly);
	CMD3(CCC_Mask,		"dbg_draw_ph_ik_goal"			,&ph_dbg_draw_mask,	phDbgDrawIKGoal);
	CMD3(CCC_Mask,		"dbg_ph_ik_limits"				,&ph_dbg_draw_mask,	phDbgIKLimits);
	CMD3(CCC_Mask,		"dbg_ph_character_control"		,&ph_dbg_draw_mask,	phDbgCharacterControl);
	CMD3(CCC_Mask,		"dbg_draw_ph_ray_motions"		,&ph_dbg_draw_mask,	phDbgDrawRayMotions);
	CMD4(CCC_Float,		"dbg_ph_vel_collid_damage_to_display",&dbg_vel_collid_damage_to_display,	0.f, 1000.f);
	CMD4(CCC_DbgBullets,"dbg_draw_bullet_hit",			&g_bDrawBulletHit,	0, 1)	;
	CMD1(CCC_DbgPhTrackObj,"dbg_track_obj");
#endif



#ifdef DEBUG
	CMD4(CCC_Integer,	"string_table_error_msg",	&CStringTable::m_bWriteErrorsToLog,	0,	1);

	CMD1(CCC_DumpInfos,				"dump_infos");
	CMD1(CCC_DumpMap,				"dump_map");
	CMD1(CCC_DumpCreatures,			"dump_creatures");

#endif

	CMD1(CCC_KickPlayer,	"sv_kick"					);		
	CMD1(CCC_BanPlayer,	"sv_banplayer"					);
	CMD1(CCC_ListPlayers,	"sv_listplayers"				);		
	
	CMD1(CCC_ChangeGameType,	"sv_changegametype"			);
	CMD1(CCC_ChangeLevel,		"sv_changelevel"				);
	CMD1(CCC_ChangeLevelGameType,		"sv_changelevelgametype"				);	

	CMD1(CCC_AddMap,		"sv_addmap"				);	
	CMD1(CCC_NextMap,		"sv_nextmap"				);	
	CMD1(CCC_PrevMap,		"sv_prevmap"				);
	CMD1(CCC_AnomalySet,	"sv_nextanomalyset"			);

	CMD1(CCC_Vote_Start,	"cl_votestart"				);
	CMD1(CCC_Vote_Stop,		"sv_votestop"				);
	CMD1(CCC_Vote_Yes,		"cl_voteyes"				);
	CMD1(CCC_Vote_No,		"cl_voteno"				);

	CMD4(CCC_Integer,		"cl_calculateping",		&g_bCalculatePing,	0, 1)	;

 
//	CMD4(CCC_SvControlHit,	"net_sv_control_hit",	&net_sv_control_hit,	0, 1)	;

	CMD4(CCC_Integer,		"dbg_show_ani_info",	&g_ShowAnimationInfo,	0, 1)	;
	CMD3(CCC_Mask,			"cl_dynamiccrosshair",	&psHUD_Flags,	HUD_CROSSHAIR_DYNAMIC);

	CMD1(CCC_MainMenu,		"main_menu"				);

	CMD1(CCC_StartTimeSingle,	"start_time_single");
	CMD4(CCC_TimeFactorSingle,	"time_factor_single", &g_fTimeFactor, 0.f,flt_max);
	CMD1(CCC_StartTimeEnvironment,	"sv_setenvtime");

	CMD1(CCC_SetWeather,	"sv_setweather"			);

	CMD4(CCC_Integer,		"sv_bearercantsprint",	&g_bBearerCantSprint,	0, 1)	;
	CMD4(CCC_Integer,		"sv_shieldedbases",		&g_bShildedBases,		0, 1)	;
	CMD4(CCC_Integer,		"sv_returnplayers",		&g_bAfReturnPlayersToBases,		0, 1)	;

	CMD4(CCC_Integer,		"cl_cod_pickup_mode",	&g_b_COD_PickUpMode,	0, 1)	;
	CMD4(CCC_Integer,		"cl_show_hit_sectors",	&g_bShowHitSectors,	0, 1)	;

	CMD4(CCC_Integer,		"sv_remove_weapon",		&g_iWeaponRemove, -1, 1);
	CMD4(CCC_Integer,		"sv_remove_corpse",		&g_iCorpseRemove, -1, 1);
//	CMD4(CCC_Integer,		"cl_show_names",		&g_bShowPlayerNames, 0, 1);

	CMD4(CCC_Integer,		"sv_statistic_collect", &g_bCollectStatisticData, 0, 1);
	CMD1(CCC_SaveStatistic,	"sv_statistic_save");
	CMD4(CCC_Integer,		"sv_statistic_save_auto", &g_bStatisticSaveAuto, 0, 1);
	CMD4(CCC_Integer,		"dbg_dump_physics_step", &g_bDebugDumpPhysicsStep, 0, 1);
	CMD4(CCC_AuthCheck,		"sv_no_auth_check",		&g_SV_Disable_Auth_Check, 0, 1);
	CMD1(CCC_Name,			"name");
	
#ifdef DEBUG
	extern	INT	g_Dump_Update_Write;
	extern	INT	g_Dump_Update_Read;
	CMD4(CCC_Integer,	"net_dbg_dump_update_write",	&g_Dump_Update_Write, 0, 1);
	CMD4(CCC_Integer,	"net_dbg_dump_update_read",	&g_Dump_Update_Read, 0, 1);

	CMD4(CCC_Integer,	"sv_artefact_spawn_force",		&g_SV_Force_Artefact_Spawn, 0, 1);
#endif
	CMD4(CCC_Integer,	"cl_leave_tdemo",		&g_bLeaveTDemo, 0, 1);
	CMD1(CCC_GetServerAddress,	"get_server_address");		

#ifdef DEBUG
	extern	INT	g_sv_Skip_Winner_Waiting;
	CMD4(CCC_Integer,	"sv_skip_winner_waiting",		&g_sv_Skip_Winner_Waiting, 0, 1);

	extern	INT g_sv_Wait_For_Players_Ready;
	CMD4(CCC_Integer,	"sv_wait_for_players_ready",	&g_sv_Wait_For_Players_Ready, 0, 1);

	CMD1(CCC_StartTeamMoney,	"sv_startteammoney"		);		
		
#endif
	extern	INT G_DELAYED_ROUND_TIME;	
	CMD4(CCC_Integer,	"sv_hail_to_winner_time",		&G_DELAYED_ROUND_TIME, 0, 60000);

	extern INT	g_sv_Pending_Wait_Time;
	CMD4(CCC_Integer,	"sv_pending_wait_time",		&g_sv_Pending_Wait_Time, 0, 60000);

	extern INT	g_sv_Client_Reconnect_Time;
	CMD4(CCC_Integer,	"sv_client_reconnect_time",		&g_sv_Client_Reconnect_Time, 0, 60);

	g_uCommonFlags.zero();
	g_uCommonFlags.set(flAiUseTorchDynamicLights, TRUE);

	CMD3(CCC_Mask,		"ai_use_torch_dynamic_lights",	&g_uCommonFlags, flAiUseTorchDynamicLights);
	CMD4(CCC_Integer,	"use_scripts_in_goap",			&g_use_scripts_in_goap, 0, 1);
	CMD4(CCC_Integer,	"show_wnd_rect",				&g_show_wnd_rect, 0, 1);

	
}
