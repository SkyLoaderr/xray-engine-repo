#pragma once

#include "game_base.h"

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
public:
	BOOL							sv_force_sync;
	xr_vector<RPoint>				rpoints	[4];
public:
	virtual							~game_sv_GameState		()		{};
	// Main accessors
	virtual		game_PlayerState*	get_it					(u32 it);
	virtual		game_PlayerState*	get_id					(u32 id);								// DPNID
	virtual		LPCSTR				get_name_it				(u32 it);
	virtual		LPCSTR				get_name_id				(u32 id);								// DPNID
	virtual		u16					get_id_2_eid			(u32 id);
	virtual		u32					get_it_2_id				(u32 it);
	virtual		u32					get_count				();
	virtual		CSE_Abstract*		get_entity_from_eid		(u16 id);
	
	// Signals
	virtual		void				switch_Phase			(u32 new_phase);
	virtual		void				signal_Syncronize		();
	virtual		void				assign_RP				(CSE_Abstract* E);
	virtual		CSE_Abstract*		spawn_begin				(LPCSTR N);
	virtual		void				spawn_end				(CSE_Abstract* E, u32 id);

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
	virtual		void				OnPlayerChangeTeam		(u32 id_who, s16 team)							{};
	virtual		void				OnPlayerBuy				(u32 id_who, u16 eid_who, LPCSTR what)	{};
	virtual		void				OnPlayerKillPlayer		(u32 id_killer, u32 id_killed)	= 0;
	virtual		BOOL				OnTouch					(u16 eid_who, u16 eid_target)	= 0;			// TRUE=allow ownership, FALSE=denied
	virtual		BOOL				OnDetach				(u16 eid_who, u16 eid_target)	= 0;			// TRUE=allow ownership, FALSE=denied

	virtual		void				OnRoundStart			();												// ����� ������
	virtual		void				OnRoundEnd				(LPCSTR reason);								// ����� ������

	virtual		void				OnTeamScore				(u32 team)						{};				// ������� ��������
	virtual		void				OnTeamsInDraw			()								{};				// �����

	// Main
	virtual		void				Create					(LPSTR &options);
	virtual		void				Update					();
	virtual		void				net_Export_State		(NET_Packet& P, u32 id_to);				// full state
	virtual		void				net_Export_Update		(NET_Packet& P, u32 id_to, u32 id);		// just incremental update for specific client
	virtual		_TIME_ID			GetGameTime				();
};
