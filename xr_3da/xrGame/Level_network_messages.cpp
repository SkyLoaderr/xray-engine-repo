#include "stdafx.h"
#include "entity.h"
#include "xrserver_objects.h"
#include "level.h"
#include "xrmessages.h"
#include "game_cl_base.h"
#include "net_queue.h"
#include "Physics.h"
#include "xrServer.h"

void CLevel::ClientReceive()
{
	for (NET_Packet* P = net_msg_Retreive(); P; P=net_msg_Retreive())
	{
		u16			m_type;
		u16			ID;
		P->r_begin	(m_type);
		switch (m_type)
		{
		case M_SPAWN:
			{
				// Begin analysis
				string64			s_name;
				P->r_stringZ		(s_name);

				// Create DC (xrSE)
				CSE_Abstract*		E	= F_entity_Create	(s_name);
				VERIFY				(E);
				E->Spawn_Read		(*P);
				if (E->s_flags.is(M_SPAWN_UPDATE))
					E->UPDATE_Read	(*P);

				//force object to be local for server client
				if (OnServer())
				{
					E->s_flags.set(M_SPAWN_OBJECT_LOCAL, TRUE);
				};

				game_spawn_queue.push_back(E);
			}
			break;
		case M_UPDATE:
			{
//				if (m_dwPingLastSendTime == 0) m_dwPingLastSendTime = timeServer_Async();

				game->net_import_update	(*P);
				Objects.net_Import		(P);

				if (OnClient()) UpdateDeltaUpd(timeServer());
				//-------------------------------------------
				if (OnServer()) break;
				//-------------------------------------------
				IClientStatistic pStat = Level().GetStatistic();
				u32 dTime = Level().timeServer() - P->timeReceive + pStat.getPing();
				u32 NumSteps = ph_world->CalcNumSteps(dTime);
				SetNumCrSteps(NumSteps);
			}break;
		//----------- for E3 -----------------------------
		case M_CL_UPDATE:
			{
				P->r_u16		(ID);
				u32 Ping = P->r_u32();
				CGameObject*	O	= smart_cast<CGameObject*>(Objects.net_Find		(ID));
				if (0 == O)		break;
				O->net_Import(*P);
		//---------------------------------------------------
				UpdateDeltaUpd(timeServer());
				if (pObjects4CrPr.empty() && pActors4CrPr.empty())
					break;

				u32 dTime = Level().timeServer() - P->timeReceive + Ping;
				u32 NumSteps = ph_world->CalcNumSteps(dTime);
				SetNumCrSteps(NumSteps);

				O->CrPr_SetActivationStep(u32(ph_world->m_steps_num) - NumSteps);
				AddActor_To_Actors4CrPr(O);

				if (pStatGraph)
				{
					pStatGraph->AppendItem(float(Ping), 0xff00ff00, 2);
					pStatGraph->AppendItem(float(dTime), 0xffffff00, 1);
					pStatGraph->AppendItem(float(NumSteps*20), 0xffff00ff, 0);
				}
			}break;
		//------------------------------------------------
		case M_CL_INPUT:
			{
				P->r_u16		(ID);
				CObject*	O	= Objects.net_Find		(ID);
				if (0 == O)		break;
				O->net_ImportInput(*P);
			}break;
		//---------------------------------------------------
		case M_CL_PING_CHALLENGE:
			{
				NET_Packet NewP;
				NewP.w_begin(m_type+1);//M_CL_PING_CHALLENGE_RESPOND);
				NewP.w_u32(P->r_u32());
				NewP.w_u32(P->r_u32());
				NewP.w_u32(P->r_u32());
				Send(NewP, net_flags(TRUE, TRUE));
			}break;
		case M_CL_PING_CHALLENGE_RESPOND:
			{
				u32 Time = P->r_u32();
				u32 Ping = timeServer_Async() - Time;
				m_dwRealPing = (m_dwRealPing*9+Ping)/10;
			}break;
		//---------------------------------------------------
		case 	M_SV_CONFIG_NEW_CLIENT:
			InitializeClientGame(*P);
			break;
		case M_SV_CONFIG_GAME:
			game->net_import_state	(*P);
			break;
		case M_SV_CONFIG_FINISHED:
			game_configured			= TRUE;
			break;
		case M_EVENT:
			game_events->insert		(*P);
			if (g_bDebugEvents)		ProcessGameEvents();
			break;
		case M_EVENT_PACK:
			NET_Packet	tmpP;
			while (!P->r_eof())
			{
				tmpP.B.count = P->r_u8();
                P->r(&tmpP.B.data, tmpP.B.count);

				game_events->insert		(tmpP);
				if (g_bDebugEvents)		ProcessGameEvents();
			};			
			break;
		case M_MIGRATE_DEACTIVATE:	// TO:   Changing server, just deactivate
			{
				P->r_u16		(ID);
				CObject*	O	= Objects.net_Find		(ID);
				if (0 == O)		break;
				O->net_MigrateInactive	(*P);
				if (bDebug)		Log("! MIGRATE_DEACTIVATE",*O->cName());
			}
			break;
		case M_MIGRATE_ACTIVATE:	// TO:   Changing server, full state
			{
				P->r_u16		(ID);
				CObject*	O	= Objects.net_Find		(ID);
				if (0 == O)		break;
				O->net_MigrateActive	(*P);
				if (bDebug)		Log("! MIGRATE_ACTIVATE",*O->cName());
			}
			break;
		case M_CHAT:
			{
				char	buffer[256];
				P->r_stringZ(buffer);
				Msg		("- %s",buffer);
			}
			break;
		case M_GAMEMESSAGE:
			{
				Game().OnGameMessage(*P);
			}break;
		case M_RELOAD_GAME:
		case M_LOAD_GAME:
		case M_CHANGE_LEVEL:
			{
				Engine.Event.Defer	("KERNEL:disconnect");
				Engine.Event.Defer	("KERNEL:start",size_t(xr_strdup(*m_caServerOptions)),size_t(xr_strdup(*m_caClientOptions)));
			}break;
		case M_SAVE_GAME:
			{
				ClientSave			();
			}break;
		case M_GAMESPY_CDKEY_VALIDATION_CHALLENGE:
			{
				OnGameSpyChallenge(P);
			}break;
		case M_AUTH_CHALLENGE:
			{
				OnBuildVersionChallenge();
			}break;
		case M_CLIENT_CONNECT_RESULT:
			{
				OnConnectResult(P);
			}break;
		case M_CHAT_MESSAGE:
			{
				Game().OnChatMessage(P);
			}break;
		case M_CHANGE_LEVEL_GAME:
			{
				if (OnClient())
				{
					Engine.Event.Defer	("KERNEL:disconnect");
					Engine.Event.Defer	("KERNEL:start",size_t(xr_strdup(*m_caServerOptions)),size_t(xr_strdup(*m_caClientOptions)));
				}
				else
				{
					const char* m_SO = m_caServerOptions.c_str();
//					const char* m_CO = m_caClientOptions.c_str();

					m_SO = strchr(m_SO, '/'); if (m_SO) m_SO++;
					m_SO = strchr(m_SO, '/'); 

					string128 LevelName = "";
					string128 GameType = "";

					P->r_stringZ(LevelName);
					P->r_stringZ(GameType);

					string4096 NewServerOptions = "";
					sprintf(NewServerOptions, "%s/%s", LevelName, GameType);

					if (m_SO) strcat(NewServerOptions, m_SO);
					m_caServerOptions = NewServerOptions;

					Engine.Event.Defer	("KERNEL:disconnect");
					Engine.Event.Defer	("KERNEL:start",size_t(xr_strdup(*m_caServerOptions)),size_t(xr_strdup(*m_caClientOptions)));
				};
			}break;
		case M_PAUSE_GAME:
			{
				u8 Pause = P->r_u8();
				Device.Pause(!(Pause == 0));
			}break;
		}

		net_msg_Release();
	}

	while (!game_spawn_queue.empty())
	{
//		u32			m0	= Memory.stat_calls;
//		CTimer		T;	T.Start		();

		g_sv_Spawn					(game_spawn_queue.front());
		F_entity_Destroy			(game_spawn_queue.front());
		game_spawn_queue.pop_front	();

//		T.Stop		();
//		u32			m1	= Memory.stat_calls;
		//Msg			("--spawn--TOTAL: %f ms, %d mo",1000.f*T.Get(),m1-m0);
	}
}
