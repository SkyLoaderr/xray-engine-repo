////////////////////////////////////////////////////////////////////////////
//	Module 		: xrGame.cpp
//	Created 	: 07.01.2001
//  Modified 	: 27.05.2004
//	Author		: Aleksandr Maksimchuk and Oles' Shyshkovtsov
//	Description : Defines the entry point for the DLL application.
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "gamepersistent.h"
#include "../xr_ioconsole.h"
#include "../xr_ioc_cmd.h"
#include "../customhud.h"
#include "../fdemorecord.h"
#include "../fdemoplay.h"
#include "ai_debug.h"
#include "Actor_Flags.h"
#include "game_sv_single.h"
#include "alife_simulator.h"
#include "trade.h"
#include "ai_space.h"
#include "script_engine.h"
#include "ai_script_processor.h"
#include "customzone.h"
#include "ui/UIMainIngameWnd.h"
#include "ui/xrXMLParser.h"
#include "level.h"
#include "xrserver.h"
#include "xr_level_controller.h"
#include "game_cl_base.h"
#include "object_factory.h"

#pragma comment(lib,"ode.lib" )

ENGINE_API
extern	float	psHUD_FOV;
extern	float	psSqueezeVelocity;
extern	int		lvInterp;
extern	float	g_fMaxDesyncLen;
extern	float	g_fMinReconDist;
extern	float	g_fMaxReconDist;
extern	float	g_fMinReconSpeed;
extern	float	g_fMaxReconSpeed;
extern	u32		g_dwMaxCorpses;
extern	int		x_m_x;
extern	int		x_m_z;

		BOOL	g_bCheckTime			= TRUE;
		int		g_dwEventDelay			= 0;
		BOOL	net_cl_inputguaranteed	= TRUE;
		int		net_cl_inputupdaterate	= 100;
		int		g_dwInputUpdateDelta	= 10;

extern "C" {
	DLL_API DLL_Pure*	__cdecl xrFactory_Create		(CLASS_ID clsid)
	{
		DLL_Pure			*object = object_factory().client_object(clsid);
		object->SUB_CLS_ID	= clsid;
		return				(object);
	}

	DLL_API void		__cdecl	xrFactory_Destroy		(DLL_Pure* O)
	{
		xr_delete(O);
	}
};

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
			Level().Server->game->round = -1;
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

class CCC_Kill : public IConsole_Command {
public:
	CCC_Kill(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = true; };
	virtual void Execute(LPCSTR args) {
		CObject *l_pObj = Level().CurrentEntity();
		CActor *l_pPlayer = dynamic_cast<CActor*>(l_pObj);
		if(l_pPlayer) {
			NET_Packet		P;
			l_pPlayer->u_EventGen		(P,GEG_PLAYER_KILL,l_pPlayer->ID()	);
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
				u32 l_cnt = Level().Server->game->get_count();
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
		if (Level().game->type == GAME_SINGLE) {
			game_sv_Single *tpGame = dynamic_cast<game_sv_Single *>(Level().Server->game);
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
		if (Level().game->type == GAME_SINGLE) {
			game_sv_Single *tpGame = dynamic_cast<game_sv_Single *>(Level().Server->game);
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
		if (Level().game->type == GAME_SINGLE) {
			game_sv_Single *tpGame = dynamic_cast<game_sv_Single *>(Level().Server->game);
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
		if (Level().game->type == GAME_SINGLE) {
			game_sv_Single *tpGame = dynamic_cast<game_sv_Single *>(Level().Server->game);
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
		if (Level().game->type == GAME_SINGLE) {
			game_sv_Single *tpGame = dynamic_cast<game_sv_Single *>(Level().Server->game);
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
		if (Level().game->type == GAME_SINGLE) {
			game_sv_Single *tpGame = dynamic_cast<game_sv_Single *>(Level().Server->game);
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
		if (Level().game->type == GAME_SINGLE) {
			game_sv_Single *tpGame = dynamic_cast<game_sv_Single *>(Level().Server->game);
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
		if (Level().game->type == GAME_SINGLE) {
			game_sv_Single *tpGame = dynamic_cast<game_sv_Single *>(Level().Server->game);
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
		if (Level().game->type == GAME_SINGLE) {
			game_sv_Single *tpGame = dynamic_cast<game_sv_Single *>(Level().Server->game);
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
//		if (Level().game->type == GAME_SINGLE) {
//			game_sv_Single *tpGame = dynamic_cast<game_sv_Single *>(Level().Server->game);
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
		if (Level().game->type == GAME_SINGLE) {
			game_sv_Single *tpGame = dynamic_cast<game_sv_Single *>(Level().Server->game);
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
		if (Level().game->type == GAME_SINGLE) {
			game_sv_Single *tpGame = dynamic_cast<game_sv_Single *>(Level().Server->game);
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
		if (Level().game->type == GAME_SINGLE) {
			game_sv_Single *tpGame = dynamic_cast<game_sv_Single *>(Level().Server->game);
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
		else
			Level().SetGameTimeFactor(id1);
	}
};

class CCC_ALifeSwitchDistance : public IConsole_Command {
public:
	CCC_ALifeSwitchDistance(LPCSTR N) : IConsole_Command(N)  { };
	virtual void Execute(LPCSTR args) {
		if ((Level().game->type == GAME_SINGLE)  &&ai().get_alife()) {
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
		if ((Level().game->type == GAME_SINGLE)  &&ai().get_alife()) {
			game_sv_Single	*tpGame = dynamic_cast<game_sv_Single *>(Level().Server->game);
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
		if ((Level().game->type == GAME_SINGLE)  &&ai().get_alife()) {
			game_sv_Single	*tpGame = dynamic_cast<game_sv_Single *>(Level().Server->game);
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
		if (Game().type != GAME_SINGLE) 
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

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// Trader
class CCC_Trader : public IConsole_Command {
public:
	CCC_Trader(LPCSTR N) : IConsole_Command(N)  { };
	virtual void Execute(LPCSTR args) {
		CActor *pActor = dynamic_cast<CActor *>(Level().CurrentEntity());
		if (!pActor) return;
		
		string128 param1, param2;
		_GetItem(args,0,param1,' ');
		_GetItem(args,1,param2,' ');
		if (xr_strcmp(param1,"show_items") == 0) {
			if (pActor->GetTrade()->IsInTradeState()) 
				pActor->GetTrade()->pPartner.inv_owner->GetTrade()->ShowItems();
		} else if (xr_strcmp(param1,"show_my_items") == 0) {
			if (pActor->GetTrade()->IsInTradeState()) 
				pActor->GetTrade()->ShowItems();
		} else if (xr_strcmp(param1,"show_money") == 0) {
			if (pActor->GetTrade()->IsInTradeState()) 
				pActor->GetTrade()->ShowMoney();
		} else if (xr_strcmp(param1,"show_your_money") == 0) {
			if (pActor->GetTrade()->IsInTradeState()) 
				pActor->GetTrade()->pPartner.inv_owner->GetTrade()->ShowMoney();
		} else if (xr_strcmp(param1,"show_artifact_prices") == 0) {
			if (pActor->GetTrade()->IsInTradeState()) 
				pActor->GetTrade()->pPartner.inv_owner->GetTrade()->ShowArtifactPrices();
		} else if (xr_strcmp(param1,"show_all") == 0) {
			if (pActor->GetTrade()->IsInTradeState()) {
				pActor->GetTrade()->ShowItems();
				pActor->GetTrade()->ShowMoney();
				pActor->GetTrade()->pPartner.inv_owner->GetTrade()->ShowItems();
				pActor->GetTrade()->pPartner.inv_owner->GetTrade()->ShowArtifactPrices();
			}
		} else if (xr_strcmp(param1,"sell") == 0) {
			if (pActor->GetTrade()->IsInTradeState()) {
				int id = 0;
				sscanf(param2 ,"%d",&id);
				pActor->GetTrade()->SellItem(id);
			}
		} else if (xr_strcmp(param1,"buy") == 0) {
			if (pActor->GetTrade()->IsInTradeState()) {
				int id = 0;
				sscanf(param2 ,"%d",&id);
				pActor->GetTrade()->pPartner.inv_owner->GetTrade()->SellItem(id);
			}
		} else if (xr_strcmp(param1,"help") == 0) {
			Msg("	show_all");		Msg("	show_items");		Msg("	show_my_items"); 
			Msg("	show_money");	Msg("	show_your_money");	Msg("	show_artifact_prices");
			Msg("	sell");			Msg("	buy");			
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
			net_packet.w_string	("quick_save");
			Level().Send		(net_packet,net_flags(TRUE));
			return;
		}

		NET_Packet			net_packet;
		net_packet.w_begin	(M_SAVE_GAME);
		net_packet.w_string	(S);
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
		net_packet.w_string	(S);
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

class CCC_Net_CL_Interpolation : public CCC_Integer {
protected:
	int		*value_blin;
public:
	CCC_Net_CL_Interpolation(LPCSTR N, int* V, int _min=0, int _max=999) :
		CCC_Integer(N,V,_min,_max),
		value_blin(V)
	{};

	virtual void	Execute	(LPCSTR args)
	{
		CCC_Integer::Execute(args);
		if ((*value_blin > 0) && g_pGameLevel)
			Level().SetInterpolationSteps(*value_blin);
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
		if (!g_pGameLevel || Game().type == GAME_SINGLE)
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
			if (ai().script_engine().script_processor("level")) {
				ai().script_engine().script_processor("level")->add_script(S);
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
			if (ai().script_engine().script_processor("level")) {
				ai().script_engine().script_processor("level")->add_string(args);
				ai().script_engine().reload_modules(true);
			}
		}
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
		CCustomZone		*p_zone = dynamic_cast<CCustomZone *>(obj);
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

BOOL APIENTRY DllMain(HANDLE hModule, u32 ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call) {
		case DLL_PROCESS_ATTACH: {
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
			CMD1(CCC_Script,			"run_script");
			CMD1(CCC_ScriptCommand,		"run_string");

			// Trader
			CMD1(CCC_Trader,			"trade");
			
			CMD1(CCC_PostprocessTest,	"pp_test");

			// Physics
			CMD4(CCC_Integer,			"ph_fps",				&phFPS,			10,		100);
			CMD4(CCC_Integer,			"ph_iterations",		&phIterations,	1,		50)	;

			CMD4( CCC_FloatBlock,			"ph_timefactor",		&phTimefactor,	0.0001f,1000.f);

			// Mad Max
			// Net Interpolation
			CMD4(CCC_Net_CL_Interpolation,		"net_cl_interpolation",	&lvInterp,			-1,100);
			CMD4(CCC_Float,				"net_cl_maxdesync",				&g_fMaxDesyncLen,		0, 10);
			CMD1(CCC_Net_CL_Resync,		"net_cl_resync" );
			CMD1(CCC_Net_CL_ClearStats,		"net_cl_clearstats" );
			CMD1(CCC_Net_SV_ClearStats,		"net_sv_clearstats" );
			CMD4(CCC_Integer,			"net_cl_inputguaranteed",		&net_cl_inputguaranteed,	0, 1)	;
			CMD4(CCC_Net_CL_InputUpdateRate,	"net_cl_inputupdaterate",		&net_cl_inputupdaterate, 1, 100)	;


			CMD3(CCC_Mask,				"g_unlimitedammo",				&psActorFlags,	AF_UNLIMITEDAMMO);
					
			// adjust mode support
			CMD4(CCC_Integer,			"hud_adjust_mode",			&g_bHudAdjustMode,	0, 3);
			CMD4(CCC_Float,				"hud_adjust_value",			&g_fHudAdjustValue,	0.0f, 1.0f);

			// Enable/disable news
			CMD4(CCC_Integer,			"hud_news_disable",			&g_bNewsDisable,	0, 1);

			// Network
			CMD4(CCC_Integer,	"net_cl_update_rate",	&psNET_ClientUpdate,1,		100				);
			CMD4(CCC_Integer,	"net_cl_pending_lim",	&psNET_ClientPending,0,		10				);
			CMD4(CCC_Integer,	"net_sv_update_rate",	&psNET_ServerUpdate,1,		100				);
			CMD4(CCC_Integer,	"net_sv_pending_lim",	&psNET_ServerPending,0,		10				);
			CMD4(CCC_Integer,	"net_port",				&psNET_Port,		5400,	5500			);
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
	#ifdef _DEBUG
			CMD4(CCC_Integer,	"center_x",				&x_m_x,	-1000,	1000);
			CMD4(CCC_Integer,	"center_y",				&x_m_z,	-1000,	1000);
	#endif


			// keyboard binding
			CCC_RegisterInput			();
			// xml parser options
			XML_DisableStringCaching	();
			// script registration
			ai().script_engine().export	();
			break;
		}

		case DLL_PROCESS_DETACH: {
			// xml parser options
			XML_CleanUpMemory			();
			break;
		}
	}
    return								(TRUE);
}
