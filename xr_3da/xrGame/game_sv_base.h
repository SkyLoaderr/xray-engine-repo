#pragma once

#include "game_base.h"

// [OLES] Policy:
// it_*		- means ordinal number of player
// id_*		- means player-id
// eid_*	- means entity-id

class	game_sv_GameState	: public game_GameState
{
public:
	BOOL							sv_force_sync;
public:
	// Main accessors
	virtual		void				Lock					();
	virtual		void				Unlock					();
	virtual		game_PlayerState*	get_it					(u32 it);
	virtual		game_PlayerState*	get_id					(u32 id);								// DPNID
	virtual		string64*			get_name_it				(u32 it);
	virtual		string64*			get_name_id				(u32 id);								// DPNID
	virtual		u32					get_it_2_id				(u32 it);
	virtual		u32					get_count				();
	
	// Signals
	virtual		void				signal_Syncronize		();
	virtual		void				switch_Phase			(u32 new_phase);

	// Utilities
	virtual		u32					get_alive_count			(u32 team);
	virtual		s32					get_option_i			(LPCSTR lst, LPCSTR name, s32 def = 0);
	virtual		string64&			get_option_s			(LPCSTR lst, LPCSTR name, LPCSTR def = 0);

	// Events
	virtual		void				OnPlayerConnect			(u32 id_who);
	virtual		void				OnPlayerDisconnect		(u32 id_who);
	virtual		void				OnPlayerReady			(u32 id_who)					{};
	virtual		void				OnPlayerKillPlayer		(u32 id_killer, u32 id_killed)	= 0;
	virtual		BOOL				OnTargetTouched			(u32 id_who, u32 eid_target)	= 0;	// TRUE=allow ownership, FALSE=denied
	virtual		BOOL				OnTargetDetouched		(u32 id_who, u32 eid_target)	{};		// TRUE=allow ownership, FALSE=denied

	virtual		void				OnRoundStart			();										// старт раунда
	virtual		void				OnRoundEnd				(LPCSTR reason);						// конец раунда

	virtual		void				OnTeamScore				(u32 team)						{};		// команда выиграла
	virtual		void				OnTeamsInDraw			()								{};		// ничья

	// Main
	virtual		void				Update					()								= 0;
	virtual		void				net_Export_State		(NET_Packet& P, u32 id_to);				// full state
	virtual		void				net_Export_Update		(NET_Packet& P, u32 id_to, u32 id);		// just incremental update for specific client
};
