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
#include "game_sv_single.h"
#include "gamepersistent.h"
#include "actor.h"
#include "Actor_Flags.h"
#include "customzone.h"
#include "script_engine.h"
#include "script_process.h"
#include "xrServer_Objects.h"
#include "ui/UIMainIngameWnd.h"
#include "PhysicsGamePars.h"
#include "string_table.h"

extern void show_smart_cast_stats		();
extern void clear_smart_cast_stats		();
extern void release_smart_cast_stats	();

ENGINE_API
extern	float	psHUD_FOV;
extern	float	psSqueezeVelocity;
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
extern	BOOL	net_cl_inputguaranteed		;
extern	int		g_dwInputUpdateDelta		;

		BOOL	g_bCheckTime			= FALSE;
		int		g_dwEventDelay			= 0	;
		int		net_cl_inputupdaterate	= 50;
#ifdef DEBUG
		Flags32	dbg_net_Draw_Flags			= {0};
#endif

#ifdef DEBUG
		BOOL	g_bDebugNode			= FALSE;
		u32		g_dwDebugNodeSource		= 0;
		u32		g_dwDebugNodeDest		= 0;
#endif

// console commands
class CCC_Spawn : public IConsole_Command
{
public:
	CCC_Spawn(LPCSTR N) : IConsole_Command(N)  { };
	virtual void Execute(LPCSTR args) {
		R_ASSERT(g_pGameLevel);

		char	Name[128];	Name[0]=0;
		sscanf	(args,"%s", Name);
		Level().g_cl_Spawn	(Name,0xff,M_SPAWN_OBJECT_LOCAL);
	}
	virtual void	Info	(TInfo& I)		
	{
		strcpy(I,"name,team,squad,group"); 
	}
};
class CCC_Restart : public IConsole_Command {
public:
	CCC_Restart(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = true; };
	virtual void Execute(LPCSTR /**args/**/) {
		if(Level().Server) {
			Level().Server->game->OnRoundEnd("GAME_restarted");
//			Level().Server->game->round = -1;
			// Level().Server->game->OnRoundStart();
		}
	}
	virtual void	Info	(TInfo& I)		
	{
		strcpy(I,"restart game"); 
	}
};
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

class CCC_Kill : public IConsole_Command {
public:
	CCC_Kill(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = true; };
	virtual void Execute(LPCSTR args) {
		CObject *l_pObj = Level().CurrentEntity();
		CActor *l_pPlayer = smart_cast<CActor*>(l_pObj);
		if(l_pPlayer) {
			NET_Packet		P;
//			l_pPlayer->u_EventGen		(P,GEG_PLAYER_KILL,l_pPlayer->ID()	);
			l_pPlayer->u_EventGen		(P,GE_GAME_EVENT,l_pPlayer->ID()	);
			P.w_u16(GAME_EVENT_PLAYER_KILL);

			P.w_u16			(u16(l_pPlayer->ID())	);
			//			P.w_s16			(s16(l_team));
			//			P.w_s16			((s16)0);
			//			P.w_u32			(0);
			l_pPlayer->u_EventSend		(P);
		}
	}
	virtual void	Info	(TInfo& I)		
	{
		strcpy(I,"player kill"); 
	}
};

class CCC_Dbg_NumObjects : public IConsole_Command {
public:
	CCC_Dbg_NumObjects(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = true; };
	virtual void Execute(LPCSTR args) 
	{
		u32 SVObjNum = Level().Server->GetEntitiesNum();
		u32 CLObjNum = Level().Objects.objects.size();

		Msg("Client Objects : %d", CLObjNum);
		Msg("Server Objects : %d", SVObjNum);

		for (u32 CO= 0; CO<max(CLObjNum, SVObjNum); CO++)
		{
			if (CO < CLObjNum && CO < SVObjNum)
			{
				CSE_Abstract* pEntity = Level().Server->GetEntity(CO);
				char color = (Level().Objects.objects[CO]->ID() == pEntity->ID) ? '-' : '!';
				Msg("%c%4d: Client - %20s[%5d] <===> Server - %s [%d]", color, CO+1, 
					*(Level().Objects.objects[CO]->cNameSect()), Level().Objects.objects[CO]->ID(),
					pEntity->s_name, pEntity->ID);
			}
			else
			{
				if (CO<CLObjNum)
				{
					Msg("! %2d: Client - %s [%d] <===> Server - -----------------", CO+1, 
						*(Level().Objects.objects[CO]->cNameSect()), Level().Objects.objects[CO]->ID());
				}
				else
				{
					CSE_Abstract* pEntity = Level().Server->GetEntity(CO);
					Msg("! %2d: Client - ----- <===> Server - %s [%d]", CO+1, 
						pEntity->s_name, pEntity->ID);
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
						Sleep				(1);
						u64 t1x = CPU::GetCycleCount();
						//						float fValue = ai().m_tpAStar->ffFindMinimalPath(id1,id2);
						u64 t2x = CPU::GetCycleCount();
						t2x -= t1x;
						//						Msg("* %7.2f[%d] : %11I64u cycles (%.3f microseconds)",fValue,ai().m_tpAStar->m_tpaNodes.size(),t2x,CPU::cycles2microsec*t2x);
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
					ALife::_GRAPH_ID id = ALife::_GRAPH_ID(id1);
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
			Log("!Not a single player game!");
	}
};


//-----------------------------------------------------------------------
class CCC_DemoRecord : public IConsole_Command
{
public:

	CCC_DemoRecord(LPCSTR N) : IConsole_Command(N) {};
	virtual void Execute(LPCSTR args) {
		//#ifndef	DEBUG
		if (GameID() != GAME_SINGLE) 
		{
			Msg("For this game type Demo Record is disabled.");
			return;
		};
		//#endif
		Console->Hide	();
		char fn[256]; strconcat(fn,args,".xrdemo");
		g_pGameLevel->Cameras.AddEffector(xr_new<CDemoRecord> (fn));
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
			  Console->Hide				();
			  char fn[256]; strconcat	(fn,args,".xrdemo");
			  g_pGameLevel->Cameras.AddEffector(xr_new<CDemoPlay> (fn,1.0f));
		  }
	  }
};

class CCC_ALifeSave : public IConsole_Command {
public:
	CCC_ALifeSave(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = true; };
	virtual void Execute(LPCSTR args) {
		string256	S;
		S[0]		= 0;
		sscanf		(args ,"%s",S);
		if (!xr_strlen(S)) {
			NET_Packet			net_packet;
			net_packet.w_begin	(M_SAVE_GAME);
			net_packet.w_stringZ("quick_save");
			Level().Send		(net_packet,net_flags(TRUE));
			return;
		}

		NET_Packet			net_packet;
		net_packet.w_begin	(M_SAVE_GAME);
		net_packet.w_stringZ(S);
		Level().Send		(net_packet,net_flags(TRUE));
	}
};

class CCC_ALifeReload : public IConsole_Command {
public:
	CCC_ALifeReload(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = true; };
	virtual void Execute(LPCSTR /**args/**/) {
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
		Msg		("! Log file has been saved successfully!");
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
			if (Level().Server->GetServer_client() == l_pC)
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
				if (Level().Server->GetServer_client() == l_pC)
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

class CCC_BanPlayer : public IConsole_Command {
public:
	CCC_BanPlayer(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = false; };
	virtual void Execute(LPCSTR args) 
	{
		if (!OnServer())	return;
		
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
			if (l_pC != Level().Server->GetServer_client())
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
			if (l_pC && BanByName && (l_pC != Level().Server->GetServer_client())) Level().Server->DisconnectClient(l_pC);
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
		Msg("- Total Players : %d", cnt);
		for(u32 it=0; it<cnt; it++)	
		{
			xrClientData *l_pC = (xrClientData*)	Level().Server->client_Get	(it);
			if (!l_pC) continue;
			Msg("%d : %s", it+1, l_pC->ps->getName());
		};
		Msg("------------------------");
	};

	virtual void	Info	(TInfo& I)		
	{
		strcpy(I,"List Players"); 
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
			if (ai().script_engine().script_process("level")) {
				ai().script_engine().script_process("level")->add_script(S);
				ai().script_engine().reload_modules(true);
			}
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
			if (ai().script_engine().script_process("level")) {
				ai().script_engine().script_process("level")->add_string(args);
				ai().script_engine().reload_modules(true);
			}
		}
	}
};

class CCC_ScriptDbg : public IConsole_Command {
public:
	CCC_ScriptDbg(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = true; };
	virtual void Execute(LPCSTR args) {
		CScriptDebugger* d = CScriptDebugger::GetDebugger();
		if(d){
			if(d->Active())
				d->initiateDebugBreak();
			else
				Msg("Script debugger not active.");
		}else
			Msg("Script debugger not present.");
	}

	virtual void	Info	(TInfo& I)		
	{
		strcpy(I,"script debugger <DebugBreak> command"); 
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
		p_zone->m_effector.SetParam(type,value);
	}
};

#ifdef DEBUG
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
#endif

class CCC_PHIterations : public CCC_Integer {
private: 
int	value;
public:
		CCC_PHIterations(LPCSTR N) :
		CCC_Integer(N,&value,5,50)
		{};
	  virtual void	Execute	(LPCSTR args)
	  {
		  CCC_Integer::Execute(args);
		  dxWorld w;
		  dWorldSetQuickStepNumIterations(&w,value);
		  phIterations=value;
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

		CGameGraph::LEVEL_MAP::const_iterator	I = ai().game_graph().header().levels().begin();
		CGameGraph::LEVEL_MAP::const_iterator	E = ai().game_graph().header().levels().end();
		for ( ; I != E; ++I)
			if (!xr_strcmp((*I).second.name(),level)) {
				ai().alife().jump_to_level(level);
				return;
			}
		Msg							("! There is no level \"%s\" in the game graph!",level);
	}
};

void CCC_RegisterCommands()
{
	// game
	CMD3(CCC_Mask,				"g_always_run",			&psActorFlags,	AF_ALWAYSRUN);
	CMD3(CCC_Mask,				"g_god",				&psActorFlags,	AF_GODMODE	);
	CMD1(CCC_Spawn,				"g_spawn"				);
	CMD1(CCC_Restart,			"g_restart"				);
	CMD1(CCC_Money,				"g_money"				);
	CMD1(CCC_Team,				"g_change_team"			);
	CMD1(CCC_Kill,				"g_kill"				);
	CMD3(CCC_Mask,				"g_backrun",			&psActorFlags,	AF_RUN_BACKWARD);

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
	CMD3(CCC_Mask,				"hud_crosshair",		&psHUD_Flags,	HUD_CROSSHAIR);
	CMD3(CCC_Mask,				"hud_crosshair_dist",	&psHUD_Flags,	HUD_CROSSHAIR_DIST);
	CMD3(CCC_Mask,				"hud_weapon",			&psHUD_Flags,	HUD_WEAPON);
	CMD3(CCC_Mask,				"hud_info",				&psHUD_Flags,	HUD_INFO);
	CMD3(CCC_Mask,				"hud_draw",				&psHUD_Flags,	HUD_DRAW);
	CMD2(CCC_Float,				"hud_fov",				&psHUD_FOV);

	// Demo
	CMD1(CCC_DemoRecord,		"demo_record"			);
	CMD1(CCC_DemoPlay,			"demo_play"				);

	// ai
	CMD3(CCC_Mask,				"ai_debug",				&psAI_Flags,	aiDebug);
	CMD3(CCC_Mask,				"ai_dbg_brain",			&psAI_Flags,	aiBrain);
	CMD3(CCC_Mask,				"ai_dbg_motion",		&psAI_Flags,	aiMotion);
	CMD3(CCC_Mask,				"ai_dbg_frustum",		&psAI_Flags,	aiFrustum);
	CMD3(CCC_Mask,				"ai_dbg_funcs",			&psAI_Flags,	aiFuncs);
	CMD3(CCC_Mask,				"ai_dbg_alife",			&psAI_Flags,	aiALife);
	CMD3(CCC_Mask,				"ai_dbg_lua",			&psAI_Flags,	aiLua);
	CMD3(CCC_Mask,				"ai_dbg_goap",			&psAI_Flags,	aiGOAP);
	CMD3(CCC_Mask,				"ai_dbg_cover",			&psAI_Flags,	aiCover);
	CMD3(CCC_Mask,				"ai_dbg_anim",			&psAI_Flags,	aiAnimation);
	CMD3(CCC_Mask,				"ai_dbg_vision",		&psAI_Flags,	aiVision);
	CMD3(CCC_Mask,				"ai_dbg_monster",		&psAI_Flags,	aiMonsterDebug);
	CMD3(CCC_Mask,				"ai_stats",				&psAI_Flags,	aiStats);
	
	
#ifdef DEBUG
	CMD1(CCC_DebugNode,			"ai_dbg_node");
#endif // DEBUG
	

	CMD1(CCC_Script,			"run_script");
	CMD1(CCC_ScriptCommand,		"run_string");

	CMD1(CCC_ScriptDbg,			"script_debug_break");

	CMD1(CCC_PostprocessTest,	"pp_test");

	// Physics
	CMD4(CCC_Integer,			"ph_fps",						&phFPS						,			10,		100				);
	CMD1(CCC_PHIterations,		"ph_iterations"																					);
	CMD4( CCC_FloatBlock,		"ph_timefactor",				&phTimefactor				,			0.0001f	,1000.f			);
	CMD4( CCC_FloatBlock,		"ph_break_common_factor",		&phBreakCommonFactor		,			0.f		,1000000000.f	);
	CMD4( CCC_FloatBlock,		"ph_rigid_break_weapon_factor",	&phRigidBreakWeaponFactor	,			0.f		,1000000000.f	);
	CMD4(CCC_FloatBlock,		"snd_collide_max_volume",		&collide_volume_max			,			1.f		,5000.f			);
	CMD4(CCC_FloatBlock,		"snd_collide_min_volume",		&collide_volume_min 		,			0.f		,5000.f			);
	// Mad Max
	// Net Interpolation
	CMD4(CCC_Float,					"net_cl_interpolation",		&g_cl_lvInterp,				-1,1);
	CMD4(CCC_Integer,				"net_cl_icurvetype",		&g_cl_InterpolationType,	0, 2)	;
	CMD4(CCC_Integer,				"net_cl_icurvesize",		(int*)&g_cl_InterpolationMaxPoints,	0, 2000)	;
	
	CMD1(CCC_Net_CL_Resync,			"net_cl_resync" );
	CMD1(CCC_Net_CL_ClearStats,		"net_cl_clearstats" );
	CMD1(CCC_Net_SV_ClearStats,		"net_sv_clearstats" );
	CMD4(CCC_Integer,				"net_cl_inputguaranteed",	&net_cl_inputguaranteed,	0, 1)	;
	CMD4(CCC_Net_CL_InputUpdateRate,"net_cl_inputupdaterate",	&net_cl_inputupdaterate,	1, 100)	;


	CMD3(CCC_Mask,		"g_unlimitedammo",			&psActorFlags,	AF_UNLIMITEDAMMO);

	// adjust mode support
	CMD4(CCC_Integer,	"hud_adjust_mode",			&g_bHudAdjustMode,	0, 3);
	CMD4(CCC_Float,		"hud_adjust_value",			&g_fHudAdjustValue,	0.0f, 1.0f);

	// Enable/disable news
	CMD4(CCC_Integer,	"hud_news_disable",			&g_bNewsDisable,	0, 1);

	// Network
	CMD4(CCC_Integer,	"net_cl_update_rate",	&psNET_ClientUpdate,1,		100				);
	CMD4(CCC_Integer,	"net_cl_pending_lim",	&psNET_ClientPending,0,		10				);
	CMD4(CCC_Integer,	"net_sv_update_rate",	&psNET_ServerUpdate,1,		100				);
	CMD4(CCC_Integer,	"net_sv_pending_lim",	&psNET_ServerPending,0,		10				);
//	CMD4(CCC_Integer,	"net_port",				&psNET_Port,		5400,	5500			);
	CMD3(CCC_String,	"net_name",				psNET_Name,			32						);
	CMD3(CCC_Mask,		"net_dump_size",		&psNET_Flags,		NETFLAG_DBG_DUMPSIZE	);
	CMD1(CCC_Dbg_NumObjects,	"net_dbg_objects"				);

	//Recon params
	CMD4(CCC_Float,		"recon_min_dist",		&g_fMinReconDist,	1.0f, 50.0f);
	CMD4(CCC_Float,		"recon_max_dist",		&g_fMaxReconDist,	50.0f, 100.0f);
	CMD4(CCC_Float,		"recon_min_speed",		&g_fMinReconSpeed,	1.0f, 180.0f);
	CMD4(CCC_Float,		"recon_max_speed",		&g_fMaxReconSpeed,	1.0f, 180.0f);

	CMD4(CCC_Integer,	"g_checktime",			&g_bCheckTime,		0,	1);
	CMD4(CCC_Integer,	"g_eventdelay",			&g_dwEventDelay,	0,	1000);

	CMD4(CCC_Integer,	"g_corpsenum",			(int*)&g_dwMaxCorpses,		0,	100);

#ifdef DEBUG
	CMD1(CCC_LuaHelp,				"lua_help");
	CMD1(CCC_ShowSmartCastStats,	"show_smart_cast_stats");
	CMD1(CCC_ClearSmartCastStats,	"clear_smart_cast_stats");

	CMD4(CCC_Integer,	"center_x",				&x_m_x,	-1000,	1000);
	CMD4(CCC_Integer,	"center_y",				&x_m_z,	-1000,	1000);

	CMD3(CCC_Mask,		"dbg_draw_actor_alive",	&dbg_net_Draw_Flags,	(1<<0));
	CMD3(CCC_Mask,		"dbg_draw_actor_dead",	&dbg_net_Draw_Flags,	(1<<1));
	CMD3(CCC_Mask,		"dbg_draw_customzone",	&dbg_net_Draw_Flags,	(1<<2));
	CMD3(CCC_Mask,		"dbg_draw_teamzone",	&dbg_net_Draw_Flags,	(1<<3));
	CMD3(CCC_Mask,		"dbg_draw_invitem",		&dbg_net_Draw_Flags,	(1<<4));
	CMD3(CCC_Mask,		"dbg_draw_actor_phys",	&dbg_net_Draw_Flags,	(1<<5));
	CMD3(CCC_Mask,		"dbg_draw_customdetector",	&dbg_net_Draw_Flags,	(1<<6));

#endif

#ifdef DEBUG
	CMD4(CCC_Integer,	"string_table_error_msg",	&CStringTable::m_bWriteErrorsToLog,	0,	1);
#endif

	CMD1(CCC_KickPlayer,	"sv_kick"					);		
	CMD1(CCC_BanPlayer,	"sv_banplayer"					);
	CMD1(CCC_ListPlayers,	"sv_listplayers"				);		
	
	CMD1(CCC_ChangeGameType,	"sv_changegametype"			);
	CMD1(CCC_ChangeLevel,		"sv_changelevel"				);
	CMD1(CCC_ChangeLevelGameType,		"sv_changelevelgametype"				);	
}

