#pragma once

#include "game_base.h"
#include "alife_space.h"
#include "script_export_space.h"

class CSE_Abstract;
class xrServer;
// [OLES] Policy:
// it_*		- means ordinal number of player
// id_*		- means player-id
// eid_*	- means entity-id

class	game_sv_GameState	: public game_GameState
{
protected:
	struct		RPoint
	{
		Fvector	P;
		Fvector A;
	};
	xrServer*						m_server;
public:

	BOOL							sv_force_sync;
	xr_vector<RPoint>				rpoints	[4];
	// scripts
	u64								m_qwStartProcessorTime;
	u64								m_qwStartGameTime;
	float							m_fTimeFactor;

public:
									game_sv_GameState		();
	virtual							~game_sv_GameState		();
	// Main accessors
	virtual		game_PlayerState*	get_it					(u32 it);
	virtual		game_PlayerState*	get_id					(u32 id);								// DPNID
	virtual		LPCSTR				get_name_it				(u32 it);
	virtual		LPCSTR				get_name_id				(u32 id);								// DPNID
	virtual		u16					get_id_2_eid			(u32 id);
	virtual		u32					get_it_2_id				(u32 it);
	virtual		u32					get_count				();
	virtual		CSE_Abstract*		get_entity_from_eid		(u16 id);

	virtual		void				OnEvent					(NET_Packet &tNetPacket, u16 type, u32 time, u32 sender );

	// Signals
	virtual		void				switch_Phase			(u32 new_phase);
	virtual		void				signal_Syncronize		();
	virtual		void				assign_RP				(CSE_Abstract* E);

	
				CSE_Abstract*		spawn_begin				(LPCSTR N);
				CSE_Abstract*		spawn_end				(CSE_Abstract* E, u32 id);

	// Utilities
	virtual		s32					get_option_i			(LPCSTR lst, LPCSTR name, s32 def = 0);
	virtual		string64&			get_option_s			(LPCSTR lst, LPCSTR name, LPCSTR def = 0);
	virtual		u32					get_alive_count			(u32 team);
	virtual		xr_vector<u16>*		get_children			(u32 id_who);
	void							u_EventGen				(NET_Packet& P, u16 type, u16 dest	);
	void							u_EventSend				(NET_Packet& P);

	// Events
	virtual		void				OnPlayerConnect			(u32 id_who);
	virtual		void				OnPlayerDisconnect		(u32 id_who);
	virtual		void				OnPlayerReady			(u32 id_who)							{};
//	virtual		void				OnPlayerChangeSkin		(u32 id_who, u8 skin)					{};
//	virtual		void				OnPlayerWantsDie		(u32 id_who)							{};
	virtual		void				OnPlayerBuy				(u32 id_who, u16 eid_who, LPCSTR what)	{};
	virtual		void				OnPlayerKillPlayer		(u32 id_killer, u32 id_killed)	= 0;
	virtual		void				OnPlayerKilled			(u32 id_killed)	= 0;
	virtual		void				OnCreate				(u16 id_who)					{}
	virtual		BOOL				OnTouch					(u16 eid_who, u16 eid_target)	= 0;			// TRUE=allow ownership, FALSE=denied
	virtual		BOOL				OnDetach				(u16 eid_who, u16 eid_target)	= 0;			// TRUE=allow ownership, FALSE=denied
	virtual		void				OnDestroyObject			(u16 eid_who)							{};			


//	virtual		void				OnPlayerBuyFinished		(u32 id_who, NET_Packet& P)				{};

	virtual		void				OnRoundStart			();												// старт раунда
	virtual		void				OnRoundEnd				(LPCSTR reason);								// конец раунда

	virtual		void				OnTeamScore				(u32 team)						{};				// команда выиграла
	virtual		void				OnTeamsInDraw			()								{};				// ничья

	virtual		void				OnHit					(u16 id_hitter, u16 id_hitted, NET_Packet& P);	//кто-то получил Hit
	virtual		void				OnPlayerHitPlayer		(u16 id_hitter, u16 id_hitted, NET_Packet& P) = 0; //игрок получил Hit

	virtual		void				OnObjectEnterTeamBase	(u16 id, u16 id_zone)	{};
	virtual		void				OnObjectLeaveTeamBase	(u16 id, u16 id_zone)	{};
	// Main
	virtual		void				Create					(ref_str& options);
	virtual		void				Update					();
	virtual		void				net_Export_State		(NET_Packet& P, u32 id_to);				// full state
	virtual		void				net_Export_Update		(NET_Packet& P, u32 id_to, u32 id);		// just incremental update for specific client
	virtual		void				net_Export_GameTime		(NET_Packet& P);						// update GameTime only for remote clients
	virtual		ALife::_TIME_ID		GetGameTime				();
	virtual		float				GetGameTimeFactor		();
	virtual		void				SetGameTime				(ALife::_TIME_ID GameTime);
	virtual		void				SetGameTimeFactor		(const float fTimeFactor);
	virtual		bool				change_level			(NET_Packet &net_packet, DPNID sender);
	virtual		void				save_game				(NET_Packet &net_packet, DPNID sender);
	virtual		bool				load_game				(NET_Packet &net_packet, DPNID sender);
	virtual		void				reload_game				(NET_Packet &net_packet, DPNID sender);
	virtual		void				switch_distance			(NET_Packet &net_packet, DPNID sender);
	DECLARE_SCRIPT_REGISTER_FUNCTION
};
add_to_type_list(game_sv_GameState)
#undef script_type_list
#define script_type_list save_type_list(game_sv_GameState)
