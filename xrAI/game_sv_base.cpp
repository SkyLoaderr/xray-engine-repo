#include "stdafx.h"
#include "xrServer.h"
#include "LevelGameDef.h"
#include "script_process.h"
#include "xrServer_Objects_ALife_Monsters.h"
#include "script_engine.h"
#include "level.h"
#include "xrserver.h"
#include "ai_space.h"
#include "game_sv_event_queue.h"

// Main
game_PlayerState*	game_sv_GameState::get_it					(u32 it)
{
	xrClientData*	C	= (xrClientData*)m_server->client_Get			(it);
	if (0==C)			return 0;
	else				return C->ps;
}

game_PlayerState*	game_sv_GameState::get_id					(ClientID id)							
{
	xrClientData*	C	= (xrClientData*)m_server->ID_to_client	(id);
	if (0==C)			return 0;
	else				return C->ps;
}

ClientID				game_sv_GameState::get_it_2_id				(u32 it)
{
	xrClientData*	C	= (xrClientData*)m_server->client_Get		(it);
	if (0==C){
		ClientID clientID;clientID.set(0);
		return clientID;
	}
	else				return C->ID;
}

LPCSTR				game_sv_GameState::get_name_it				(u32 it)
{
	xrClientData*	C	= (xrClientData*)m_server->client_Get		(it);
	if (0==C)			return 0;
	else				return *C->Name;
}

LPCSTR				game_sv_GameState::get_name_id				(ClientID id)							
{
	xrClientData*	C	= (xrClientData*)m_server->ID_to_client	(id);
	if (0==C)			return 0;
	else				return *C->Name;
}

LPCSTR				game_sv_GameState::get_player_name_id				(ClientID id)								
{
	xrClientData* xrCData	=	m_server->ID_to_client(id);
	if(xrCData)
		return get_option_s(*xrCData->Name,"name",*xrCData->Name);
	else 
		return "unknown";
}

u32					game_sv_GameState::get_players_count		()
{
	return				m_server->client_Count();
}

u16					game_sv_GameState::get_id_2_eid				(ClientID id)
{
	xrClientData*	C	= (xrClientData*)m_server->ID_to_client	(id);
	if (0==C)			return 0xffff;
	CSE_Abstract*	E	= C->owner;
	if (0==E)			return 0xffff;
	return E->ID;
}

game_PlayerState*	game_sv_GameState::get_eid (u16 id) //if exist
{
	CSE_Abstract* entity = get_entity_from_eid(id);
	if (entity && entity->owner->ps->GameID == id)
		return entity->owner->ps;
	return NULL;
}

CSE_Abstract*		game_sv_GameState::get_entity_from_eid		(u16 id)
{
	return				m_server->ID_to_entity(id);
}

// Utilities
u32					game_sv_GameState::get_alive_count			(u32 team)
{
	u32		cnt		= get_players_count	();
	u32		alive	= 0;
	for		(u32 it=0; it<cnt; ++it)	
	{
		game_PlayerState*	ps	=	get_it	(it);
		if (u32(ps->team) == team)	alive	+=	(ps->testFlag(GAME_PLAYER_FLAG_VERY_VERY_DEAD))?0:1;
	}
	return alive;
}

xr_vector<u16>*		game_sv_GameState::get_children				(ClientID id)
{
	xrClientData*	C	= (xrClientData*)m_server->ID_to_client	(id);
	if (0==C)			return 0;
	CSE_Abstract* E	= C->owner;
	if (0==E)			return 0;
	return	&(E->children);
}

s32					game_sv_GameState::get_option_i				(LPCSTR lst, LPCSTR name, s32 def)
{
	string64		op;
	strconcat		(op,"/",name,"=");
	if (strstr(lst,op))	return atoi	(strstr(lst,op)+xr_strlen(op));
	else				return def;
}

string64&			game_sv_GameState::get_option_s				(LPCSTR lst, LPCSTR name, LPCSTR def)
{
	static string64	ret;

	string64		op;
	strconcat		(op,"/",name,"=");
	LPCSTR			start	= strstr(lst,op);
	if (start)		
	{
		LPCSTR			begin	= start + xr_strlen(op); 
		sscanf			(begin, "%[^/]",ret);
	}
	else			
	{
		if (def)	strcpy		(ret,def);
		else		ret[0]=0;
	}
	return ret;
}
void				game_sv_GameState::signal_Syncronize		()
{
	sv_force_sync	= TRUE;
}

// Network
void game_sv_GameState::net_Export_State						(NET_Packet& P, ClientID to)
{
	// Generic
	P.w_clientID	(to);
	P.w_s32			(type);
	P.w_u16			(phase);
	P.w_s32			(round);
	P.w_u32			(start_time);

	// Players
//	u32	p_count			= get_players_count() - ((g_pGamePersistent->bDedicatedServer)? 1 : 0);
	u32 p_count = 0;
	for (u32 p_it=0; p_it<get_players_count(); ++p_it)
	{
		xrClientData*	C		=	(xrClientData*)	m_server->client_Get	(p_it);
		if (!C->net_Ready || C->ps->Skip) continue;
		p_count++;
	};

	P.w_u16				(u16(p_count));
	game_PlayerState*	Base	= get_id(to);
	for (u32 p_it=0; p_it<get_players_count(); ++p_it)
	{
		string64	p_name;
		xrClientData*	C		=	(xrClientData*)	m_server->client_Get	(p_it);
		if (0==C)	strcpy(p_name,"Unknown");
		else 
		{
			CSE_Abstract* C_e		= C->owner;
			if (0==C_e)		strcpy(p_name,"Unknown");
			else 
			{
				strcpy	(p_name,C_e->s_name_replace);
			}
		}

		game_PlayerState* A		=	get_it			(p_it);
		if (A->Skip || !C->net_Ready) continue;
		A->setName(p_name);
		u16 tmp_flags = A->flags;

		if (Base==A)	
			A->setFlag(GAME_PLAYER_FLAG_LOCAL);
/*
		if (A->Skip || !C->net_Ready)
		{
			A->flags = tmp_flags;
			continue;
		};		
*/
		ClientID clientID = get_it_2_id	(p_it);
		P.w_clientID			(clientID);
//		P.w_stringZ				(p_name);
		A->net_Export			(P);
		
		A->flags = tmp_flags;
	}

	P.w_u64(GetGameTime());
	P.w_float(GetGameTimeFactor());
}

void game_sv_GameState::net_Export_Update						(NET_Packet& P, ClientID id_to, ClientID id)
{
	game_PlayerState* A		= get_id		(id);
	if (A)
	{
		u16 bk_flags = A->flags;
		if (id==id_to)	
		{
			A->setFlag(GAME_PLAYER_FLAG_LOCAL);
		}

		P.w_clientID	(id);
		A->net_Export(P);
		A->flags = bk_flags;
	};
};

void game_sv_GameState::net_Export_GameTime						(NET_Packet& P)
{
	//Syncronize GameTime 
	P.w_u64(GetGameTime());
	P.w_float(GetGameTimeFactor());
};


void game_sv_GameState::OnPlayerConnect			(ClientID /**id_who/**/)
{
	signal_Syncronize	();
}

void game_sv_GameState::OnPlayerDisconnect		(ClientID /**id_who/**/, LPSTR, u16 )
{
	signal_Syncronize	();
}

void game_sv_GameState::Create					(ref_str &/**options/**/)
{
	string256	fn_game;
	if (FS.exist(fn_game, "$level$", "level.game")) 
	{
		IReader *F = FS.r_open	(fn_game);
		IReader *O = 0;

		// Load RPoints
		if (0!=(O = F->open_chunk	(RPOINT_CHUNK)))
		{ 
			for (int id=0; O->find_chunk(id); ++id)
			{
				RPoint					R;
				u8						team;
				u8						type;

				O->r_fvector3			(R.P);
				O->r_fvector3			(R.A);
				team					= O->r_u8	();	VERIFY(team>=0 && team<4);
				type					= O->r_u8	();
				//u16 res					= 
					O->r_u16	();
				switch (type)
				{
				case rptActorSpawn:
					{
						rpoints[team].push_back	(R);
					}break;
				};
			};
			O->close();
		}

		FS.r_close	(F);
	}

	// loading scripts
	ai().script_engine().remove_script_process("game");
	string256					S;
	FS.update_path				(S,"$game_data$","script.ltx");
	CInifile					*l_tpIniFile = xr_new<CInifile>(S);
	R_ASSERT					(l_tpIniFile);

	if( l_tpIniFile->section_exist( type_name() ) )
		if (l_tpIniFile->r_string(type_name(),"script"))
			ai().script_engine().add_script_process("game",xr_new<CScriptProcess>("game",l_tpIniFile->r_string(type_name(),"script")));
		else
			ai().script_engine().add_script_process("game",xr_new<CScriptProcess>("game",""));

	xr_delete					(l_tpIniFile);
}

void	game_sv_GameState::assign_RP				(CSE_Abstract* E)
{
	VERIFY				(E);

	u8					l_uc_team = u8(-1);
	CSE_Spectator		*tpSpectator = dynamic_cast<CSE_Spectator*>(E);
	if (tpSpectator)
		l_uc_team = tpSpectator->g_team();
	else {
		CSE_ALifeCreatureAbstract	*tpTeamed = dynamic_cast<CSE_ALifeCreatureAbstract*>(E);
		if (tpTeamed)
			l_uc_team = tpTeamed->g_team();
		else
			R_ASSERT2(tpTeamed,"Non-teamed object is assigning to respawn point!");
	}
	xr_vector<RPoint>&	rp	= rpoints[l_uc_team];
	RPoint&				r	= rp[::Random.randI((int)rp.size())];
	E->o_Position.set	(r.P);
	E->o_Angle.set		(r.A);
}

CSE_Abstract*		game_sv_GameState::spawn_begin				(LPCSTR N)
{
	CSE_Abstract*	A	= F_entity_Create(N);	R_ASSERT(A);	// create SE
	strcpy				(A->s_name,N);							// ltx-def
	A->s_gameid			=	u8(type);							// game-type
	A->s_RP				=	0xFE;								// use supplied
	A->ID				=	0xffff;								// server must generate ID
	A->ID_Parent		=	0xffff;								// no-parent
	A->ID_Phantom		=	0xffff;								// no-phantom
	A->RespawnTime		=	0;									// no-respawn
	return A;
}

CSE_Abstract*		game_sv_GameState::spawn_end				(CSE_Abstract* E, ClientID id)
{
	NET_Packet						P;
	u16								skip_header;
	E->Spawn_Write					(P,TRUE);
	P.r_begin						(skip_header);
	CSE_Abstract* N = m_server->Process_spawn	(P,id);
	F_entity_Destroy				(E);

	return N;
}

void game_sv_GameState::GenerateGameMessage (NET_Packet &P)
{ 
	P.w_begin(M_GAMEMESSAGE); 
};

void game_sv_GameState::u_EventGen(NET_Packet& P, u16 type, u16 dest)
{
	P.w_begin	(M_EVENT);
	P.w_u32		(Level().timeServer());//Device.TimerAsync());
	P.w_u16		(type);
	P.w_u16		(dest);
}

void game_sv_GameState::u_EventSend(NET_Packet& P)
{
	ClientID clientID; clientID.setBroadcast();
	m_server->SendBroadcast(clientID,P,net_flags(TRUE,TRUE));
}

void game_sv_GameState::Update		()
{
	for (u32 it=0; it<m_server->client_Count(); ++it)
	{
		xrClientData*	C		= (xrClientData*)	m_server->client_Get(it);
		C->ps->ping				= u16(C->stats.getPing());
	}
	
	if(ai().script_engine().script_process("game"))
		ai().script_engine().script_process("game")->update();
}

game_sv_GameState::game_sv_GameState()
{
/*	m_qwStartProcessorTime		= CPU::GetCycleCount();
	m_qwStartGameTime			= 12*60*60*1000;
	m_fTimeFactor				= pSettings->r_float("alife","time_factor");
*/
	m_server					= Level().Server;

	m_event_queue = xr_new<GameEventQueue>();
}

game_sv_GameState::~game_sv_GameState()
{
	ai().script_engine().remove_script_process("game");
	xr_delete(m_event_queue);
}
/*
ALife::_TIME_ID game_sv_GameState::GetGameTime()
{
	return			(m_qwStartGameTime + iFloor(m_fTimeFactor*float(CPU::GetCycleCount() - m_qwStartProcessorTime)*CPU::cycles2milisec));
}

float game_sv_GameState::GetGameTimeFactor()
{
	return			(m_fTimeFactor);
}

void game_sv_GameState::SetGameTimeFactor (const float fTimeFactor)
{
	m_qwStartGameTime			= GetGameTime();
	m_qwStartProcessorTime		= CPU::GetCycleCount();
	m_fTimeFactor				= fTimeFactor;
}

void game_sv_GameState::SetGameTime (ALife::_TIME_ID GameTime)
{
	m_qwStartGameTime			= GameTime;
}
*/
bool game_sv_GameState::change_level (NET_Packet &net_packet, ClientID sender)
{
	return						(true);
}

void game_sv_GameState::save_game (NET_Packet &net_packet, ClientID sender)
{
}

bool game_sv_GameState::load_game (NET_Packet &net_packet, ClientID sender)
{
	return						(true);
}

void game_sv_GameState::reload_game (NET_Packet &net_packet, ClientID sender)
{
}

void game_sv_GameState::switch_distance (NET_Packet &net_packet, ClientID sender)
{
}

void game_sv_GameState::OnHit (u16 id_hitter, u16 id_hitted, NET_Packet& P)
{
	CSE_Abstract*		e_hitter		= get_entity_from_eid	(id_hitter	);
	CSE_Abstract*		e_hitted		= get_entity_from_eid	(id_hitted	);
	if (!e_hitter || !e_hitted) return;

	CSE_Abstract*		a_hitter		= dynamic_cast <CSE_ALifeCreatureActor*> (e_hitter);
	CSE_Abstract*		a_hitted		= dynamic_cast <CSE_ALifeCreatureActor*> (e_hitted);

	if (a_hitted && a_hitter)
	{
		OnPlayerHitPlayer(id_hitter, id_hitted, P);
		return;
	};
};

void game_sv_GameState::OnEvent (NET_Packet &tNetPacket, u16 type, u32 time, ClientID sender )
{
	switch	(type)
	{	
	case GAME_EVENT_PLAYER_CONNECTED:
		{
			ClientID ID;
			tNetPacket.r_clientID(ID);
			OnPlayerConnect(ID);
		}break;

	case GAME_EVENT_PLAYER_DISCONNECTED:
		{
			ClientID ID;
			tNetPacket.r_clientID(ID);
			string64 PlayerName;
			tNetPacket.r_stringZ(PlayerName);
			u16		GameID = tNetPacket.r_u16();
			OnPlayerDisconnect(ID, get_option_s(PlayerName,"name",PlayerName), GameID);
		}break;

	case GAME_EVENT_PLAYER_KILLED:
		{
		}break;
	case GAME_EVENT_ON_HIT:
		{
			u16		id_dest = tNetPacket.r_u16();
			u16     id_src  = tNetPacket.r_u16();
			CSE_Abstract*	e_src	= get_entity_from_eid	(id_src	);
			if(!e_src) break;
			OnHit(id_src, id_dest, tNetPacket);
			ClientID clientID;clientID.setBroadcast();
			m_server->SendBroadcast		(clientID,tNetPacket,net_flags(TRUE,TRUE));
		}break;
	case GAME_EVENT_CREATE_CLIENT:
		{
			IClient* P = m_server->client_Create();
			VERIFY(P);
			P->ID = sender;
			//tNetPacket.r_clientID(P->ID);
			tNetPacket.r_stringZ(P->Name);
			P->flags.bLocal = !!tNetPacket.r_u8();
			P->flags.bConnected = TRUE;
			m_server->AttachNewClient(P);

		}break;
	default:
		R_ASSERT2	(0,"Game Event not implemented!!!");
	};
};

void game_sv_GameState::OnSwitchPhase(u32 old_phase, u32 new_phase)
{
	inherited::OnSwitchPhase(old_phase, new_phase);
	signal_Syncronize	(); 
}

void game_sv_GameState::AddDelayedEvent(NET_Packet &tNetPacket, u16 type, u32 time, ClientID sender )
{
//	OnEvent(tNetPacket,type,time,sender);
	m_event_queue->Create(tNetPacket,type,time,sender);
}

void game_sv_GameState::ProcessDelayedEvent		()
{
	GameEvent* ge = NULL;
	while ((ge = m_event_queue->Retreive()) != 0) {
		OnEvent(ge->P,ge->type,ge->time,ge->sender);
		m_event_queue->Release();
	}
}

u32 game_sv_GameState::getRPcount (u16 team_idx)
{
	if ( !(team_idx<TEAM_COUNT) )
		return 0;
	else
		return rpoints[team_idx].size();
}

RPoint game_sv_GameState::getRP (u16 team_idx, u32 rp_idx)
{
	if( (team_idx<TEAM_COUNT) && (rp_idx<rpoints[team_idx].size()) )
	return rpoints[team_idx][rp_idx];
	else 
		return RPoint();
};
