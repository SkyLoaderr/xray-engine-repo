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
#include "ai\\stalker\\ai_stalker.h"
#include "ai\\zombie\\ai_zombie.h"
#include "ai\\idol\\ai_idol.h"
#include "ai\\flesh\\ai_flesh.h"
#include "ai\\chimera\\ai_chimera.h"
#include "ai\\bloodsucker\\ai_bloodsucker.h"
#include "ai\\dog\\ai_dog.h"
#include "ai\\boar\\ai_boar.h"
#include "ai\\trader\\ai_trader.h"
#include "car.h"
#include "customtarget.h"
#include "..\fdemorecord.h"
#include "..\fdemoplay.h"
#include "a_star.h"
#include "game_sv_single.h"
#include "ai_alife.h"
#include "HangingLamp.h"
#include "trade.h"

ENGINE_API extern float		psHUD_FOV;
extern	float				psSqueezeVelocity;
extern	int					phFPS;
extern	int					phIterations;

// console commands
class CCC_Spawn : public CConsoleCommand
{
public:
	CCC_Spawn(LPCSTR N) : CConsoleCommand(N)  { };
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
				P.w_float		(.0f);
				P.w_u16			(eHitTypeWound);	//hit type
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
					l_pPs->money_total = l_money;
				}
			}
		}
	}
	virtual void Info(TInfo& I) {
		strcpy(I,"give money"); 
	}
};

class CCC_ALifePath : public CConsoleCommand {
public:
	CCC_ALifePath(LPCSTR N) : CConsoleCommand(N)  { };
	virtual void Execute(LPCSTR args) {
		if (!getAI().bfCheckIfGraphLoaded())
			Msg("! there is no graph!");
		else {
			int id1=-1, id2=-1;
			sscanf(args ,"%d %d",&id1,&id2);
			if ((id1 != -1) && (id2 != -1))
				if (_max(id1,id2) > (int)getAI().GraphHeader().dwVertexCount - 1)
					Msg("! there are only %d vertexes!",getAI().GraphHeader().dwVertexCount);
				else
					if (_min(id1,id2) < 0)
						Msg("! invalid vertex number (%d)!",_min(id1,id2));
					else {
						Sleep				(1);
						u64 t1x = CPU::GetCycleCount();
						float fValue = getAI().m_tpAStar->ffFindMinimalPath(id1,id2);
						u64 t2x = CPU::GetCycleCount();
						t2x -= t1x;
						Msg("* %7.2f[%d] : %11I64u cycles (%.3f microseconds)",fValue,getAI().m_tpAStar->m_tpaNodes.size(),t2x,CPU::cycles2microsec*t2x);
					}
			else
				Msg("! not enough parameters!");
		}
	}
};

#ifdef ALIFE_SUPPORT_CONSOLE_COMMANDS
class CCC_ALifeListAll : public CConsoleCommand {
public:
	CCC_ALifeListAll(LPCSTR N) : CConsoleCommand(N)  { bEmptyArgsHandled = true; };
	virtual void Execute(LPCSTR args) {
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

class CCC_ALifeListObjects : public CConsoleCommand {
public:
	CCC_ALifeListObjects(LPCSTR N) : CConsoleCommand(N)  { bEmptyArgsHandled = true; };
	virtual void Execute(LPCSTR args) {
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

class CCC_ALifeListEvents : public CConsoleCommand {
public:
	CCC_ALifeListEvents(LPCSTR N) : CConsoleCommand(N)  { bEmptyArgsHandled = true; };
	virtual void Execute(LPCSTR args) {
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

class CCC_ALifeListTasks : public CConsoleCommand {
public:
	CCC_ALifeListTasks(LPCSTR N) : CConsoleCommand(N)  { bEmptyArgsHandled = true; };
	virtual void Execute(LPCSTR args) {
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

class CCC_ALifeListTerrain : public CConsoleCommand {
public:
	CCC_ALifeListTerrain(LPCSTR N) : CConsoleCommand(N)  { bEmptyArgsHandled = true; };
	virtual void Execute(LPCSTR args) {
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

class CCC_ALifeListSpawns : public CConsoleCommand {
public:
	CCC_ALifeListSpawns(LPCSTR N) : CConsoleCommand(N)  { bEmptyArgsHandled = true; };
	virtual void Execute(LPCSTR args) {
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

class CCC_ALifeObjectInfo : public CConsoleCommand {
public:
	CCC_ALifeObjectInfo(LPCSTR N) : CConsoleCommand(N)  { };
	virtual void Execute(LPCSTR args) {
		if (Level().game.type == GAME_SINGLE) {
			game_sv_Single *tpGame = dynamic_cast<game_sv_Single *>(Level().Server->game);
			if (tpGame && tpGame->m_tpALife->m_bLoaded) {
				int id1 = -1;
				sscanf(args ,"%d",&id1);
				tpGame->m_tpALife->vfObjectInfo(_OBJECT_ID(id1));
			}
			else
				Log("!ALife simulator is not loaded!");
		}
		else
			Log("!Not a single player game!");
	}
};

class CCC_ALifeEventInfo : public CConsoleCommand {
public:
	CCC_ALifeEventInfo(LPCSTR N) : CConsoleCommand(N)  { };
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

class CCC_ALifeTaskInfo : public CConsoleCommand {
public:
	CCC_ALifeTaskInfo(LPCSTR N) : CConsoleCommand(N)  { };
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

class CCC_ALifeSpawnInfo : public CConsoleCommand {
public:
	CCC_ALifeSpawnInfo(LPCSTR N) : CConsoleCommand(N)  { };
	virtual void Execute(LPCSTR args) {
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

class CCC_ALifeGraphInfo : public CConsoleCommand {
public:
	CCC_ALifeGraphInfo(LPCSTR N) : CConsoleCommand(N)  { };
	virtual void Execute(LPCSTR args) {
		if (Level().game.type == GAME_SINGLE) {
			game_sv_Single *tpGame = dynamic_cast<game_sv_Single *>(Level().Server->game);
			if (getAI().bfCheckIfGraphLoaded()) {
				u32 id1 = u32(-1);
				sscanf(args ,"%d",&id1);
				if (id1 >= getAI().GraphHeader().dwVertexCount)
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

class CCC_ALifeTimeFactor : public CConsoleCommand {
public:
	CCC_ALifeTimeFactor(LPCSTR N) : CConsoleCommand(N)  { };
	virtual void Execute(LPCSTR args) {
		if (Level().game.type == GAME_SINGLE) {
			game_sv_Single *tpGame = dynamic_cast<game_sv_Single *>(Level().Server->game);
			if (tpGame && tpGame->m_tpALife->m_bLoaded) {
				float id1 = 0.0f;
				sscanf(args ,"%f",&id1);
				if (id1 < EPS_L)
					Msg("Invalid time factor! (%.4f)",id1);
				else
					tpGame->m_tpALife->vfSetTimeFactor(id1);
			}
			else
				Log("!ALife simulator is not loaded!");
		}
		else
			Log("!Not a single player game!");
	}
};

class CCC_ALifeSwitchDistance : public CConsoleCommand {
public:
	CCC_ALifeSwitchDistance(LPCSTR N) : CConsoleCommand(N)  { };
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

class CCC_ALifeProcessTime : public CConsoleCommand {
public:
	CCC_ALifeProcessTime(LPCSTR N) : CConsoleCommand(N)  { };
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

class CCC_ALifeSwitchFactor : public CConsoleCommand {
public:
	CCC_ALifeSwitchFactor(LPCSTR N) : CConsoleCommand(N)  { };
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

class CCC_ALifeScheduleMin : public CConsoleCommand {
public:
	CCC_ALifeScheduleMin(LPCSTR N) : CConsoleCommand(N)  { };
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

class CCC_ALifeScheduleMax : public CConsoleCommand {
public:
	CCC_ALifeScheduleMax(LPCSTR N) : CConsoleCommand(N)  { };
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
class CCC_DemoRecord : public CConsoleCommand
{
public:
	CCC_DemoRecord(LPCSTR N) : CConsoleCommand(N) {};
	virtual void Execute(LPCSTR args) {
		Console->Hide	();
		char fn[256]; strconcat(fn,args,".xrdemo");
		g_pGameLevel->Cameras.AddEffector(xr_new<CDemoRecord> (fn));
	}
};
class CCC_DemoPlay : public CConsoleCommand
{
public:
	CCC_DemoPlay(LPCSTR N) : 
	  CConsoleCommand(N) 
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
class CCC_Trader : public CConsoleCommand {
public:
	CCC_Trader(LPCSTR N) : CConsoleCommand(N)  { };
	virtual void Execute(LPCSTR args) {
		CActor *pActor = dynamic_cast<CActor *>(Level().CurrentEntity());
		if (!pActor) return;
		
		string128 param1, param2;
		_GetItem(args,0,param1,' ');
		_GetItem(args,1,param2,' ');
		if (strcmp(param1,"show_items") == 0) {
			if (pActor->GetTrade()->IsInTradeState()) 
				pActor->GetTrade()->pPartner.inv_owner->GetTrade()->ShowItems();
		} else if (strcmp(param1,"show_my_items") == 0) {
			if (pActor->GetTrade()->IsInTradeState()) 
				pActor->GetTrade()->ShowItems();
		} else if (strcmp(param1,"show_money") == 0) {
			if (pActor->GetTrade()->IsInTradeState()) 
				pActor->GetTrade()->ShowMoney();
		} else if (strcmp(param1,"show_your_money") == 0) {
			if (pActor->GetTrade()->IsInTradeState()) 
				pActor->GetTrade()->pPartner.inv_owner->GetTrade()->ShowMoney();
		} else if (strcmp(param1,"show_artifact_prices") == 0) {
			if (pActor->GetTrade()->IsInTradeState()) 
				pActor->GetTrade()->pPartner.inv_owner->GetTrade()->ShowArtifactPrices();
		} else if (strcmp(param1,"show_all") == 0) {
			if (pActor->GetTrade()->IsInTradeState()) {
				pActor->GetTrade()->ShowItems();
				pActor->GetTrade()->ShowMoney();
				pActor->GetTrade()->pPartner.inv_owner->GetTrade()->ShowItems();
				pActor->GetTrade()->pPartner.inv_owner->GetTrade()->ShowArtifactPrices();
			}
		} else if (strcmp(param1,"sell") == 0) {
			if (pActor->GetTrade()->IsInTradeState()) {
				int id = 0;
				sscanf(param2 ,"%d",&id);
				pActor->GetTrade()->SellItem(id);
			}
		} else if (strcmp(param1,"buy") == 0) {
			if (pActor->GetTrade()->IsInTradeState()) {
				int id = 0;
				sscanf(param2 ,"%d",&id);
				pActor->GetTrade()->pPartner.inv_owner->GetTrade()->SellItem(id);
			}
		} else if (strcmp(param1,"help") == 0) {
			Msg("	show_all");		Msg("	show_items");		Msg("	show_my_items"); 
			Msg("	show_money");	Msg("	show_your_money");	Msg("	show_artifact_prices");
			Msg("	sell");			Msg("	buy");			
		}
	}
};

class CCC_ALifeSave : public CConsoleCommand {
public:
	CCC_ALifeSave(LPCSTR N) : CConsoleCommand(N)  { bEmptyArgsHandled = true; };
	virtual void Execute(LPCSTR args) {
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

class CCC_ALifeSaveTo : public CConsoleCommand {
public:
	CCC_ALifeSaveTo(LPCSTR N) : CConsoleCommand(N)  { bEmptyArgsHandled = true; };
	virtual void Execute(LPCSTR args) {
		if (Level().game.type == GAME_SINGLE) {
			game_sv_Single *tpGame = dynamic_cast<game_sv_Single *>(Level().Server->game);
			if (tpGame && tpGame->m_tpALife->m_bLoaded) {
				string256	S;
				S[0]		= 0;
				sscanf		(args ,"%s",S);
				if (!strlen(S))
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

class CCC_ALifeReload : public CConsoleCommand {
public:
	CCC_ALifeReload(LPCSTR N) : CConsoleCommand(N)  { bEmptyArgsHandled = true; };
	virtual void Execute(LPCSTR args) {
		if (Level().game.type == GAME_SINGLE)
			Level().IR_OnKeyboardPress(DIK_F7);
		else
			Log("!Not a single player game!");
	}
};

class CCC_ALifeLoadFrom : public CConsoleCommand {
public:
	CCC_ALifeLoadFrom(LPCSTR N) : CConsoleCommand(N)  { bEmptyArgsHandled = true; };
	virtual void Execute(LPCSTR args) {
		if (Level().game.type == GAME_SINGLE) {
			string256	S;
			S[0]		= 0;
			sscanf		(args ,"%s",S);
			if (!strlen(S))
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

class CCC_ALifeLoadALifeFrom : public CConsoleCommand {
public:
	CCC_ALifeLoadALifeFrom(LPCSTR N) : CConsoleCommand(N)  { bEmptyArgsHandled = true; };
	virtual void Execute(LPCSTR args) {
		if (Level().game.type == GAME_SINGLE) {
			string256	S;
			S[0]		= 0;
			sscanf		(args ,"%s",S);
			if (!strlen(S))
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

class CCC_FlushLog : public CConsoleCommand {
public:
	CCC_FlushLog(LPCSTR N) : CConsoleCommand(N)  { bEmptyArgsHandled = true; };
	virtual void Execute(LPCSTR args) {
		FlushLog();
		Msg		("! Log file has been saved successfully!");
	}
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////


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
		CMD3(CCC_Mask,				"g_always_run",			&psActorFlags,	AF_ALWAYSRUN);
		CMD3(CCC_Mask,				"g_god",				&psActorFlags,	AF_GODMODE	);
		CMD1(CCC_Spawn,				"g_spawn"				);
		CMD1(CCC_Restart,			"g_restart"				);
		CMD1(CCC_Money,				"g_money"				);
		CMD1(CCC_Team,				"g_change_team"			);
		
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

		// Trader
		CMD1(CCC_Trader,			"trade");		

		// Physics
		CMD4(CCC_Integer,			"ph_fps",				&phFPS,			10,100);
		CMD4(CCC_Integer,			"ph_iterations",		&phIterations,	1,50);

		// keyboard binding
		CCC_RegisterInput			();
		}
		break;
	case DLL_PROCESS_DETACH:
		xr_delete			(tpAI_Space);
		break;
	}
    return TRUE;
}

// Factory
#include "actor.h"
#include "customitem.h"
#include "flyer.h"
#include "customevent.h"
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
#include "weaponsvd.h"
#include "weaponsvu.h"
#include "weaponrpg7.h"
#include "weaponval.h"
#include "weaponvintorez.h"
#include "weaponwalther.h"
#include "weaponusp45.h"
#include "weapongroza.h"
#include "weaponmagazinedwgrenade.h"

#include "bolt.h"
#include "medkit.h"
#include "antirad.h"
#include "bread.h"

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

#include "customzone.h"
#include "mosquitobald.h"
#include "mincer.h"
#include "radioactivezone.h"
#include "level_changer.h"

#include "simpledetector.h"
#include "pda.h"

#include "physicobject.h"

extern "C" {
	DLL_API DLL_Pure*	__cdecl xrFactory_Create		(CLASS_ID cls)
	{
		DLL_Pure*	P	= 0;
		switch (cls)
		{
		case CLSID_GAME_LEVEL:			P = xr_new<CLevel>();				break;
		case CLSID_GAME_PERSISTANT:		P = xr_new<CGamePersistent>();		break;
		case CLSID_HUDMANAGER:			P = xr_new<CHUDManager>();			break;
		case CLSID_OBJECT_ACTOR:		P = xr_new<CActor>();				break;
		case CLSID_TARGET:				P = xr_new<CCustomTarget>();		break;
		case CLSID_SPECTATOR:			P = xr_new<CSpectator>();			break;

		case CLSID_AI_GRAPH:												break;
		case CLSID_AI_RAT:				P = xr_new<CAI_Rat>();				break;
		case CLSID_AI_FLESH:			P = xr_new<CAI_Flesh>();			break;
		case CLSID_AI_CHIMERA:			P = xr_new<CAI_Chimera>();			break;
		case CLSID_AI_DOG_RED:			P = xr_new<CAI_Dog>();				break;
		case CLSID_AI_SOLDIER:			P =	xr_new<CAI_Soldier>();			break;
		case CLSID_AI_STALKER:			P =	xr_new<CAI_Stalker>();			break;
		case CLSID_AI_ZOMBIE:			P = xr_new<CAI_Zombie>();			break;
		case CLSID_AI_IDOL:				P = xr_new<CAI_Idol>();				break;
		case CLSID_AI_BLOODSUCKER:		P = xr_new<CAI_Bloodsucker>();		break;
		case CLSID_AI_BOAR:				P = xr_new<CAI_Boar>();				break;

		// Trader
		case CLSID_AI_TRADER:			P = xr_new<CAI_Trader>();			break;

		case CLSID_AI_CROW:				P = xr_new<CAI_Crow>();				break;
		case CLSID_CAR_NIVA:			P = xr_new<CCar>();					break;

		// Artifacts
		case CLSID_AF_MERCURY_BALL:		P = xr_new<CMercuryBall>();			break;
		case CLSID_ARTEFACT:			P = xr_new<CArtifact>();			break;

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

		// Inventory
		case CLSID_IITEM_BOLT:			P = xr_new<CBolt>();				break;
		case CLSID_IITEM_MEDKIT:		P = xr_new<CMedkit>();				break;
		case CLSID_IITEM_ANTIRAD:		P = xr_new<CAntirad>();				break;
		case CLSID_IITEM_BREAD:			P = xr_new<CBread>();				break;

		//Equipment outfit
		case CLSID_EQUIPMENT_SCIENTIFIC:P = xr_new<CScientificOutfit>();	break;
		case CLSID_EQUIPMENT_STALKER:	P = xr_new<CStalkerOutfit>();		break;
		case CLSID_EQUIPMENT_MILITARY:	P = xr_new<CMilitaryOutfit>();		break;
		case CLSID_EQUIPMENT_EXO:		P = xr_new<CExoOutfit>();			break;

		// Grenades
		case CLSID_GRENADE_F1:			P = xr_new<CF1>();					break;
		case CLSID_OBJECT_G_RPG7:		P = xr_new<CWeaponRPG7Grenade>();	break;
		case CLSID_GRENADE_RGD5:		P = xr_new<CRGD5>();				break;
		case CLSID_OBJECT_G_FAKE:		P = xr_new<CWeaponFakeGrenade>();	break;

		// Zones
		case CLSID_ZONE:				P = xr_new<CCustomZone>();			break;
		case CLSID_Z_MBALD:				P = xr_new<CMosquitoBald>();		break;
		case CLSID_Z_MINCER:			P = xr_new<CMincer>();				break;
		case CLSID_Z_ACIDF:				P = xr_new<CMosquitoBald>();		break;
		case CLSID_Z_GALANT:			P = xr_new<CMosquitoBald>();		break;
		case CLSID_Z_RADIO:				P = xr_new<CRadioactiveZone>();		break;
		case CLSID_Z_BFUZZ:				P = xr_new<CMosquitoBald>();		break;
		case CLSID_Z_RUSTYH:			P = xr_new<CMosquitoBald>();		break;
		case CLSID_Z_DEAD:				P = xr_new<CMosquitoBald>();		break;
		case CLSID_LEVEL_CHANGER:		P = xr_new<CLevelChanger>();		break;

		// Detectors
		case CLSID_DETECTOR_SIMPLE:		P = xr_new<CSimpleDetector>();		break;

		// Devices
		case CLSID_DEVICE_TORCH:		P = xr_new<CTorch>();				break;
		case CLSID_DEVICE_PDA:			P = xr_new<CPda>();					break;

		// entity
		case CLSID_OBJECT_HLAMP:		P = xr_new<CHangingLamp>();			break;
		case CLSID_OBJECT_PHYSIC:		P = xr_new<CPhysicObject>();			break;
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
