// xrGame.cpp : Defines the entry point for the DLL application.
//
 
#include "stdafx.h"
#include "gamepersistent.h"

#pragma comment(lib,"ode.lib" )
 
#include "../xr_ioconsole.h"
#include "../xr_ioc_cmd.h"
#include "../customhud.h"
#include "ai_debug.h"
#include "Actor_Flags.h"
#include "ai/crow/ai_crow.h"
#include "ai/rat/ai_rat.h"
#include "ai/stalker/ai_stalker.h"
#include "ai/zombie/ai_zombie.h"
#include "ai/idol/ai_idol.h"
#include "ai/flesh/ai_flesh.h"
#include "ai/chimera/ai_chimera.h"
#include "ai/bloodsucker/ai_bloodsucker.h"
#include "ai/dog/ai_dog.h"
#include "ai/pseudodog/ai_pseudodog.h"
#include "ai/boar/ai_boar.h"
#include "ai/trader/ai_trader.h"
#include "ai/monsters/Burer/burer.h"
#include "ai/monsters/PseudoGigant/pseudo_gigant.h"
#include "ai/monsters/chimera/chimera.h"
#include "ai/monsters/controller/controller.h"
#include "car.h"
#include "customtarget.h"
#include "../fdemorecord.h"
#include "../fdemoplay.h"
#include "graph_engine.h"
#include "game_sv_single.h"
#include "ai_alife.h"
#include "HangingLamp.h"
#include "trade.h"
#include "actor.h"
#include "ai_script_processor.h"
#include "attachable_item.h"
#include "team_base_zone.h"

#include "customzone.h"
#include "mosquitobald.h"
#include "mincer.h"
#include "radioactivezone.h"
#include "PhysicsCommon.h"
#include "BreakableObject.h"
#include "WeaponMounted.h"

#include "ui/UIMainIngameWnd.h"


ENGINE_API extern float		psHUD_FOV;
extern	float				psSqueezeVelocity;

extern	int					lvInterp;
extern	float				g_fMaxDesyncLen;
extern	bool				g_bUnlimitedAmmo;

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
		if (Level().game.type == GAME_SINGLE) {
			game_sv_Single *tpGame = dynamic_cast<game_sv_Single *>(Level().Server->game);
			if (tpGame && tpGame->m_tpALife->m_bLoaded) {
				tpGame->m_tpALife->vfListObjects();
				tpGame->m_tpALife->vfListEvents();
				tpGame->m_tpALife->vfListTasks();
				tpGame->m_tpALife->vfListTerrain();
				tpGame->m_tpALife->vfListSpawnPoints();
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
		if (Level().game.type == GAME_SINGLE) {
			game_sv_Single *tpGame = dynamic_cast<game_sv_Single *>(Level().Server->game);
			if (tpGame && tpGame->m_tpALife->m_bLoaded) {
				tpGame->m_tpALife->vfListObjects();
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
		if (Level().game.type == GAME_SINGLE) {
			game_sv_Single *tpGame = dynamic_cast<game_sv_Single *>(Level().Server->game);
			if (tpGame && tpGame->m_tpALife->m_bLoaded) {
				tpGame->m_tpALife->vfListEvents();
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
		if (Level().game.type == GAME_SINGLE) {
			game_sv_Single *tpGame = dynamic_cast<game_sv_Single *>(Level().Server->game);
			if (tpGame && tpGame->m_tpALife->m_bLoaded) {
				tpGame->m_tpALife->vfListTasks();
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
		if (Level().game.type == GAME_SINGLE) {
			game_sv_Single *tpGame = dynamic_cast<game_sv_Single *>(Level().Server->game);
			if (tpGame && tpGame->m_tpALife->m_bLoaded) {
				tpGame->m_tpALife->vfListTerrain();
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
		if (Level().game.type == GAME_SINGLE) {
			game_sv_Single *tpGame = dynamic_cast<game_sv_Single *>(Level().Server->game);
			if (tpGame && tpGame->m_tpALife->m_bLoaded) {
				tpGame->m_tpALife->vfListSpawnPoints();
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
		if (Level().game.type == GAME_SINGLE) {
			game_sv_Single *tpGame = dynamic_cast<game_sv_Single *>(Level().Server->game);
			if (tpGame && tpGame->m_tpALife->m_bLoaded) {
				int id1 = -1;
				sscanf(args ,"%d",&id1);
				tpGame->m_tpALife->vfObjectInfo(ALife::_OBJECT_ID(id1));
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
		if (Level().game.type == GAME_SINGLE) {
			game_sv_Single *tpGame = dynamic_cast<game_sv_Single *>(Level().Server->game);
			if (tpGame && tpGame->m_tpALife->m_bLoaded) {
				int id1 = -1;
				sscanf(args ,"%d",&id1);
				if (id1 >= int(tpGame->m_tpALife->m_tEventID))
					Msg("Invalid event ID! (%d)",id1);
				else
					tpGame->m_tpALife->vfEventInfo(id1);
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
		if (Level().game.type == GAME_SINGLE) {
			game_sv_Single *tpGame = dynamic_cast<game_sv_Single *>(Level().Server->game);
			if (tpGame && tpGame->m_tpALife->m_bLoaded) {
				int id1 = -1;
				sscanf(args ,"%d",&id1);
				if (id1 >= int(tpGame->m_tpALife->m_tTaskID))
					Msg("Invalid task ID! (%d)",id1);
				else
					tpGame->m_tpALife->vfTaskInfo(id1);
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
//		if (Level().game.type == GAME_SINGLE) {
//			game_sv_Single *tpGame = dynamic_cast<game_sv_Single *>(Level().Server->game);
//			if (tpGame && tpGame->m_tpALife->m_bLoaded) {
//				u32 id1 = u32(-1);
//				sscanf(args ,"%d",&id1);
//				if (id1 >= tpGame->m_tpALife->m_tpSpawnPoints.size())
//					Msg("Invalid task ID! (%d)",id1);
//				else {
//					ALife::_SPAWN_ID id = ALife::_SPAWN_ID(id1);
//					tpGame->m_tpALife->vfSpawnPointInfo(id);
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
		if (Level().game.type == GAME_SINGLE) {
			game_sv_Single *tpGame = dynamic_cast<game_sv_Single *>(Level().Server->game);
			if (ai().get_level_graph()) {
				u32 id1 = u32(-1);
				sscanf(args ,"%d",&id1);
				if (id1 >= ai().game_graph().header().vertex_count())
					Msg("Invalid task ID! (%d)",id1);
				else {
					ALife::_GRAPH_ID id = ALife::_GRAPH_ID(id1);
					tpGame->m_tpALife->vfGraphVertexInfo(id);
				}
			}
			else
				Log("!Game graph is not loaded!");
		}
		else
			Log("!Not a single player game!");
	}
};

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
		if (Level().game.type == GAME_SINGLE) {
			game_sv_Single *tpGame = dynamic_cast<game_sv_Single *>(Level().Server->game);
			if (tpGame && tpGame->m_tpALife->m_bLoaded) {
				float id1 = 0.0f;
				sscanf(args ,"%f",&id1);
				if (id1 < 2.0f)
					Msg("Invalid online distance! (%.4f)",id1);
				else
					tpGame->m_tpALife->vfSetSwitchDistance(id1);
			}
			else
				Log("!ALife simulator is not loaded!");
		}
		else
			Log("!Not a single player game!");
	}
};

class CCC_ALifeProcessTime : public IConsole_Command {
public:
	CCC_ALifeProcessTime(LPCSTR N) : IConsole_Command(N)  { };
	virtual void Execute(LPCSTR args) {
		if (Level().game.type == GAME_SINGLE) {
			game_sv_Single *tpGame = dynamic_cast<game_sv_Single *>(Level().Server->game);
			if (tpGame && tpGame->m_tpALife->m_bLoaded) {
				int id1 = 0;
				sscanf(args ,"%d",&id1);
				if (id1 < 1)
					Msg("Invalid process time! (%d)",id1);
				else
					tpGame->m_tpALife->vfSetProcessTime(id1);
			}
			else
				Log("!ALife simulator is not loaded!");
		}
		else
			Log("!Not a single player game!");
	}
};

class CCC_ALifeSwitchFactor : public IConsole_Command {
public:
	CCC_ALifeSwitchFactor(LPCSTR N) : IConsole_Command(N)  { };
	virtual void Execute(LPCSTR args) {
		if (Level().game.type == GAME_SINGLE) {
			game_sv_Single *tpGame = dynamic_cast<game_sv_Single *>(Level().Server->game);
			if (tpGame && tpGame->m_tpALife->m_bLoaded) {
				float id1 = 0;
				sscanf(args ,"%f",&id1);
				clamp(id1,.1f,1.f);
				tpGame->m_tpALife->vfSetSwitchFactor(id1);
			}
			else
				Log("!ALife simulator is not loaded!");
		}
		else
			Log("!Not a single player game!");
	}
};

class CCC_ALifeScheduleMin : public IConsole_Command {
public:
	CCC_ALifeScheduleMin(LPCSTR N) : IConsole_Command(N)  { };
	virtual void Execute(LPCSTR args) {
		if (Level().game.type == GAME_SINGLE) {
			game_sv_Single *tpGame = dynamic_cast<game_sv_Single *>(Level().Server->game);
			if (tpGame && tpGame->m_tpALife->m_bLoaded) {
				int id1 = 0;
				sscanf(args ,"%d",&id1);
				if (id1 < 0)
					Msg("Invalid schedule _min time! (%d)",id1);
				else
					tpGame->m_tpALife->vfSetScheduleMin(id1);
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
		if (Level().game.type == GAME_SINGLE) {
			game_sv_Single *tpGame = dynamic_cast<game_sv_Single *>(Level().Server->game);
			if (tpGame && tpGame->m_tpALife->m_bLoaded) {
				int id1 = 0;
				sscanf(args ,"%d",&id1);
				if (id1 < 0)
					Msg("Invalid schedule max time! (%d)",id1);
				else
					tpGame->m_tpALife->vfSetScheduleMax(id1);
			}
			else
				Log("!ALife simulator is not loaded!");
		}
		else
			Log("!Not a single player game!");
	}
};

#endif
//-----------------------------------------------------------------------
class CCC_DemoRecord : public IConsole_Command
{
public:
	CCC_DemoRecord(LPCSTR N) : IConsole_Command(N) {};
	virtual void Execute(LPCSTR args) {
		if (Game().type != GAME_SINGLE) 
		{
			Msg("For this game type Demo Record is disabled.");
			return;
		};
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
	virtual void Execute(LPCSTR /**args/**/) {
		if (Level().game.type == GAME_SINGLE) {
			game_sv_Single *tpGame = dynamic_cast<game_sv_Single *>(Level().Server->game);
			if (tpGame && tpGame->m_tpALife->m_bLoaded)
				tpGame->m_tpALife->Save("quick_save");
			else
				Log("!ALife simulation cannot be saved!");
		}
		else
			Log("!Not a single player game!");
	}
};

class CCC_ALifeSaveTo : public IConsole_Command {
public:
	CCC_ALifeSaveTo(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = true; };
	virtual void Execute(LPCSTR args) {
		if (Level().game.type == GAME_SINGLE) {
			game_sv_Single *tpGame = dynamic_cast<game_sv_Single *>(Level().Server->game);
			if (tpGame && tpGame->m_tpALife->m_bLoaded) {
				string256	S;
				S[0]		= 0;
				sscanf		(args ,"%s",S);
				if (!xr_strlen(S))
					Log("* Specify file name!");
				else
					tpGame->m_tpALife->Save(S);
			}
			else
				Log("!ALife simulation cannot be saved!");
		}
		else
			Log("!Not a single player game!");
	}
};

class CCC_ALifeReload : public IConsole_Command {
public:
	CCC_ALifeReload(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = true; };
	virtual void Execute(LPCSTR /**args/**/) {
		if (Level().game.type == GAME_SINGLE)
			Level().IR_OnKeyboardPress(DIK_F7);
		else
			Log("!Not a single player game!");
	}
};

class CCC_ALifeLoadFrom : public IConsole_Command {
public:
	CCC_ALifeLoadFrom(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = true; };
	virtual void Execute(LPCSTR args) {
		if (Level().game.type == GAME_SINGLE) {
			string256	S;
			S[0]		= 0;
			sscanf		(args ,"%s",S);
			if (!xr_strlen(S))
				Log("* Specify file name!");
			else {
				strconcat(Level().m_caServerOptions,S,"/single");
				Level().IR_OnKeyboardPress(DIK_F7);
			}
		}
		else
			Log("!Not a single player game!");
	}
};

class CCC_ALifeLoadALifeFrom : public IConsole_Command {
public:
	CCC_ALifeLoadALifeFrom(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = true; };
	virtual void Execute(LPCSTR args) {
		if (Level().game.type == GAME_SINGLE) {
			string256	S;
			S[0]		= 0;
			sscanf		(args ,"%s",S);
			if (!xr_strlen(S))
				Log("* Specify file name!");
			else {
				strconcat(Level().m_caServerOptions,S,"/single/alife");
				Level().IR_OnKeyboardPress(DIK_F7);
			}
		}
		else
			Log("!Not a single player game!");
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


///////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ui/xrXMLParser.h"
#include "ai_space.h"

BOOL APIENTRY DllMain( HANDLE /**hModule/**/, 
                       u32  ul_reason_for_call, 
                       LPVOID /**lpReserved/**/
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
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
		CMD1(CCC_ALifeSaveTo,		"save_to"				);		// save game to ...
		CMD1(CCC_ALifeReload,		"reload"				);		// reload game
		CMD1(CCC_ALifeLoadFrom,		"load_from"				);		// load game from ...
		CMD1(CCC_ALifeLoadALifeFrom,"aload_from"			);		// load alife game from ...
		CMD1(CCC_FlushLog,			"flush"					);		// flush log
#ifdef ALIFE_SUPPORT_CONSOLE_COMMANDS
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
		CMD1(CCC_ALifeTimeFactor,	"al_time_factor"		);		// set time factor
		CMD1(CCC_ALifeSwitchDistance,"al_switch_distance"	);		// set switch distance
		CMD1(CCC_ALifeProcessTime,	"al_process_time"		);		// set process time
		CMD1(CCC_ALifeSwitchFactor,	"al_switch_factor"		);		// set switch factor
		CMD1(CCC_ALifeScheduleMin,	"al_schedule_min"		);		// set min schedule
		CMD1(CCC_ALifeScheduleMax,	"al_schedule_max"		);		// set max schedule
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
		CMD4( CCC_Float,			"ph_timefactor",		&phTimefactor,	0.0001f,1000.f);
		// Mad Max
		// Net Interpolation
		CMD4(CCC_Net_CL_Interpolation,		"net_cl_interpolation",	&lvInterp,			-1,100);
		CMD4(CCC_Float,				"net_cl_maxdesync",				&g_fMaxDesyncLen,		0, 10);
		CMD1(CCC_Net_CL_Resync,		"net_cl_resync" );
		CMD3(CCC_Mask,				"g_unlimitedammo",				&psActorFlags,	AF_UNLIMITEDAMMO);
				
		// adjust mode support
		CMD4(CCC_Integer,			"hud_adjust_mode",			&g_bHudAdjustMode,	0, 3);
		CMD4(CCC_Float,				"hud_adjust_value",			&g_fHudAdjustValue,	0.0f, 1.0f);

		// Network
		CMD4(CCC_Integer,	"net_cl_update_rate",	&psNET_ClientUpdate,1,		100	);
		CMD4(CCC_Integer,	"net_cl_pending_lim",	&psNET_ClientPending,0,		10	);
		CMD4(CCC_Integer,	"net_sv_update_rate",	&psNET_ServerUpdate,1,		100	);
		CMD4(CCC_Integer,	"net_sv_pending_lim",	&psNET_ServerPending,0,		10	);
		CMD4(CCC_Integer,	"net_port",				&psNET_Port,		5400,	5500);
		CMD3(CCC_String,	"net_name",				psNET_Name,			32			);
		CMD3(CCC_Mask,		"net_dump_size",		&psNET_Flags,		0x1			);
		CMD1(CCC_Dbg_NumObjects,	"net_dbg_objects"				);


		// keyboard binding
		CCC_RegisterInput			();
//		HMODULE h = LoadLibrary("x:/xrXMLParser.dll");
//		h=h;
		XML_DisableStringCaching	();
		VERIFY						(!g_ai_space);
		g_ai_space					= xr_new<CAI_Space>();
		ai().script_engine().export	();
		}
		break;
	case DLL_PROCESS_DETACH:
		XML_CleanUpMemory			();
		VERIFY						(g_ai_space);
		xr_delete					(g_ai_space);
		break;
	}
    return TRUE;
}

// Factory
#include "actor.h"
#include "customitem.h"
#include "flyer.h"
#include "customevent.h"
#include "customdoor.h"
#include "customlift.h"
#include "demoactor.h"
#include "hudmanager.h"
#include "targetassault.h"
#include "targetcsbase.h"
#include "targetcs.h"
#include "targetcscask.h"

#include "weaponAK74.h"
#include "weaponLR300.h"
#include "weaponFN2000.h"
#include "weaponHPSA.h"
#include "weaponPM.h"
#include "weaponFORT.h"
#include "weaponBINOCULARS.h"
#include "weaponShotgun.h"
#include "weaponsvd.h"
#include "weaponsvu.h"
#include "weaponrpg7.h"
#include "explosiverocket.h"
#include "weaponval.h"
#include "weaponvintorez.h"
#include "weaponwalther.h"
#include "weaponusp45.h"
#include "weapongroza.h"
#include "weaponmagazinedwgrenade.h"
#include "WeaponFakeGrenade.h"

#include "scope.h"
#include "silencer.h"
#include "grenadelauncher.h"

#include "bolt.h"
#include "medkit.h"
#include "antirad.h"
#include "fooditem.h"
#include "bottleitem.h"
#include "explosiveitem.h"
#include "infodocument.h"

#include "ScientificOutfit.h"
#include "StalkerOutfit.h"
#include "MilitaryOutfit.h"
#include "ExoOutfit.h"

#include "weaponknife.h"
#include "torch.h"
#include "f1.h"
#include "rgd5.h"
#include "Spectator.h"

#include "artifact.h"
#include "GraviArtifact.h"
#include "MercuryBall.h"
#include "BlackDrops.h"
#include "Needles.h"
#include "BastArtifact.h"
#include "BlackGraviArtifact.h"
#include "DummyArtifact.h"
#include "ZudaArtifact.h"
#include "ThornArtifact.h"
#include "FadedBall.h"
#include "ElectricBall.h"
#include "RustyHairArtifact.h"
#include "GalantineArtifact.h"

#include "level_changer.h"

#include "simpledetector.h"
#include "pda.h"
#include "artifactmerger.h"

#include "searchlight.h"

#include "physicobject.h"
#include "script_zone.h"
#include "gravizone.h"

#include "vehicle_helicopter.h"



extern "C" {
	DLL_API DLL_Pure*	__cdecl xrFactory_Create		(CLASS_ID cls)
	{
		DLL_Pure*	P	= 0;
		switch (cls) {
			case CLSID_GAME_LEVEL:			P = xr_new<CLevel>();				break;
			case CLSID_GAME_PERSISTANT:		P = xr_new<CGamePersistent>();		break;
			case CLSID_HUDMANAGER:			P = xr_new<CHUDManager>();			break;
			case CLSID_OBJECT_ACTOR:		P = xr_new<CActor>();				break;
			case CLSID_TARGET:				P = xr_new<CCustomTarget>();		break;
			case CLSID_SPECTATOR:			P = xr_new<CSpectator>();			break;

			case CLSID_AI_GRAPH:												break;
			case CLSID_AI_RAT:				P = xr_new<CAI_Rat>();				break;
			case CLSID_AI_FLESH:			P = xr_new<CAI_Flesh>();			break;
			case CLSID_AI_CHIMERA:			P = xr_new<CChimera>();				break;
			case CLSID_AI_DOG_RED:			P = xr_new<CAI_Dog>();				break;
			case CLSID_AI_SOLDIER:			P =	xr_new<CAI_Stalker>();			break;
			case CLSID_AI_STALKER:			P =	xr_new<CAI_Stalker>();			break;
			case CLSID_AI_ZOMBIE:			P = xr_new<CAI_Zombie>();			break;
			case CLSID_AI_IDOL:				P = xr_new<CAI_Idol>();				break;
			case CLSID_AI_BLOODSUCKER:		P = xr_new<CAI_Bloodsucker>();		break;
			case CLSID_AI_BOAR:				P = xr_new<CAI_Boar>();				break;
			case CLSID_AI_DOG_BLACK:		P = xr_new<CAI_PseudoDog>();		break;
			case CLSID_AI_BURER:			P = xr_new<CBurer>();				break;
			case CLSID_AI_GIANT:			P = xr_new<CPseudoGigant>();		break;
			case CLSID_AI_CONTROLLER:		P = xr_new<CController>();			break;

			// Trader
			case CLSID_AI_TRADER:			P = xr_new<CAI_Trader>();			break;

			case CLSID_AI_CROW:				P = xr_new<CAI_Crow>();				break;
			case CLSID_CAR:					P = xr_new<CCar>();					break;

			case CLSID_VEHICLE_HELICOPTER:	P = xr_new<CVehicleHelicopter>();	break;

			// Artifacts
			case CLSID_AF_MERCURY_BALL:		P = xr_new<CMercuryBall>();			break;
			case CLSID_AF_BLACKDROPS:		P = xr_new<CBlackDrops>();			break;
			//case CLSID_AF_NEEDLES:		P = xr_new<CNeedles>();				break;
			//case CLSID_AF_NEEDLES:		P = xr_new<CBastArtifact>();		break;
			case CLSID_AF_NEEDLES:			P = xr_new<CBlackGraviArtifact>();	break;

			case CLSID_AF_BAST:				P = xr_new<CBastArtifact>();		break;
			case CLSID_AF_BLACK_GRAVI:		P = xr_new<CBlackGraviArtifact>();	break;
			case CLSID_AF_DUMMY:			P = xr_new<CDummyArtifact>();		break;
			case CLSID_AF_ZUDA:				P = xr_new<CZudaArtifact>();		break;
			case CLSID_AF_THORN:			P = xr_new<CThornArtifact>();		break;
			case CLSID_AF_FADED_BALL:		P = xr_new<CFadedBall>();			break;
			case CLSID_AF_ELECTRIC_BALL:	P = xr_new<CElectricBall>();		break;	
			case CLSID_AF_RUSTY_HAIR:		P = xr_new<CRustyHairArtifact>();	break;
			case CLSID_AF_GALANTINE:		P = xr_new<CGalantineArtifact>();	break;

			case CLSID_AF_GRAVI:	
			case CLSID_ARTEFACT:			P = xr_new<CGraviArtifact>();		break;

			// Targets
			case CLSID_TARGET_ASSAULT:		P = xr_new<CTargetAssault>();		break;
			case CLSID_TARGET_CS_BASE:		P = xr_new<CTargetCSBase>();		break;
			case CLSID_TARGET_CS_CASK:		P = xr_new<CTargetCSCask>();		break;
			case CLSID_TARGET_CS:			P = xr_new<CTargetCS>();			break;
				
			case CLSID_OBJECT_W_FN2000:		P = xr_new<CWeaponFN2000>();		break;
			case CLSID_OBJECT_W_AK74:		P = xr_new<CWeaponAK74>();			break;
			case CLSID_OBJECT_W_LR300:		P = xr_new<CWeaponLR300>();			break;
			case CLSID_OBJECT_W_HPSA:		P = xr_new<CWeaponHPSA>	();			break;
			case CLSID_OBJECT_W_PM:			P = xr_new<CWeaponPM>	();			break;
			case CLSID_OBJECT_A_PM:			P = xr_new<CWeaponAmmo>	();			break;
			case CLSID_OBJECT_W_FORT:		P = xr_new<CWeaponFORT>	();			break;
			case CLSID_OBJECT_W_BINOCULAR:	P = xr_new<CWeaponBinoculars>();	break;
			case CLSID_OBJECT_W_SHOTGUN:	P = xr_new<CWeaponShotgun>();		break;
			case CLSID_OBJECT_W_SVD:		P = xr_new<CWeaponSVD>();			break;
			case CLSID_OBJECT_W_SVU:		P = xr_new<CWeaponSVU>();			break;
			case CLSID_OBJECT_W_RPG7:		P = xr_new<CWeaponRPG7>();			break;
			case CLSID_OBJECT_W_VAL:		P = xr_new<CWeaponVal>();			break;
			case CLSID_OBJECT_W_VINTOREZ:	P = xr_new<CWeaponVintorez>();		break;
			case CLSID_OBJECT_W_WALTHER:	P = xr_new<CWeaponWalther>();		break;
			case CLSID_OBJECT_W_USP45:		P = xr_new<CWeaponUSP45>();			break;
			case CLSID_OBJECT_W_GROZA:		P = xr_new<CWeaponGroza>();			break;
			case CLSID_OBJECT_W_KNIFE:		P = xr_new<CWeaponKnife>();			break;

			//Weapons Add-on
			case  CLSID_OBJECT_W_SCOPE:		P = xr_new<CScope>();				break;
			case  CLSID_OBJECT_W_SILENCER:	P = xr_new<CSilencer>();			break;
			case  CLSID_OBJECT_W_GLAUNCHER:	P = xr_new<CGrenadeLauncher>();		break;

			// Inventory
			case CLSID_IITEM_BOLT:			P = xr_new<CBolt>();				break;
			case CLSID_IITEM_MEDKIT:		P = xr_new<CMedkit>();				break;
			case CLSID_IITEM_ANTIRAD:		P = xr_new<CAntirad>();				break;
			case CLSID_IITEM_FOOD:			P = xr_new<CFoodItem>();			break;
			case CLSID_IITEM_BOTTLE:		P = xr_new<CBottleItem>();			break;
			case CLSID_IITEM_EXPLOSIVE:		P = xr_new<CExplosiveItem>();		break;
			
			//Info Document
			case CLSID_IITEM_DOCUMENT:		P = xr_new<CInfoDocument>();		break;
			case CLSID_IITEM_ATTACH:		P = xr_new<CAttachableItem>();		break;

			//Equipment outfit
			case CLSID_EQUIPMENT_SCIENTIFIC:P = xr_new<CScientificOutfit>();	break;
			case CLSID_EQUIPMENT_STALKER:	P = xr_new<CStalkerOutfit>();		break;
			case CLSID_EQUIPMENT_MILITARY:	P = xr_new<CMilitaryOutfit>();		break;
			case CLSID_EQUIPMENT_EXO:		P = xr_new<CExoOutfit>();			break;

			// Grenades
			case CLSID_GRENADE_F1:			P = xr_new<CF1>();					break;
			case CLSID_GRENADE_RGD5:		P = xr_new<CRGD5>();				break;
			// Rockets
			case CLSID_OBJECT_G_RPG7:		P = xr_new<CExplosiveRocket>();		break;
			case CLSID_OBJECT_G_FAKE:		P = xr_new<CExplosiveRocket>();		break;

			// Zones
			case CLSID_ZONE:				P = xr_new<CCustomZone>();			break;
			case CLSID_Z_MBALD:				P = xr_new<CMosquitoBald>();		break;
			case CLSID_Z_MINCER:			P = xr_new<CMincer>();				break;
			case CLSID_Z_ACIDF:				P = xr_new<CMosquitoBald>();		break;
			case CLSID_Z_GALANT:			P = xr_new<CGraviZone>();			break;
			case CLSID_Z_RADIO:				P = xr_new<CRadioactiveZone>();		break;
			case CLSID_Z_BFUZZ:				P = xr_new<CMosquitoBald>();		break;
			case CLSID_Z_RUSTYH:			P = xr_new<CMosquitoBald>();		break;
			case CLSID_Z_DEAD:				P = xr_new<CMosquitoBald>();		break;
			case CLSID_LEVEL_CHANGER:		P = xr_new<CLevelChanger>();		break;
			case CLSID_SCRIPT_ZONE:			P = xr_new<CScriptZone>();			break;
			case CLSID_Z_TEAM_BASE:			P = xr_new<CTeamBaseZone>();		break;

			// Detectors
			case CLSID_DETECTOR_SIMPLE:		P = xr_new<CSimpleDetector>();		break;

			// Devices
			case CLSID_DEVICE_TORCH:		P = xr_new<CTorch>();				break;
			case CLSID_DEVICE_PDA:			P = xr_new<CPda>();					break;
			case CLSID_DEVICE_AF_MERGER:	P = xr_new<CArtifactMerger>();		break;

			
			// objects
			case CLSID_OBJECT_PROJECTOR:	P = xr_new<CProjector>();			break;
			case CLSID_OBJECT_W_MOUNTED:	P = xr_new<CWeaponMounted>();		break;

			// entity
			case CLSID_OBJECT_HLAMP:		P = xr_new<CHangingLamp>();			break;
			case CLSID_OBJECT_PHYSIC:		P = xr_new<CPhysicObject>();		break;
			case CLSID_SCRIPT_OBJECT:		P = xr_new<CScriptMonster>();		break;
			case CLSID_OBJECT_BREAKABLE:	P = xr_new<CBreakableObject>();		break;
		}
		R_ASSERT2		(P,"Unknown object class");
		P->SUB_CLS_ID	= cls;
		return P;
	}

	DLL_API void		__cdecl	xrFactory_Destroy		(DLL_Pure* O)
	{
		xr_delete(O);
	}
};
