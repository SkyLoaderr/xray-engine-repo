#pragma once

#include "game_base.h"

// [OLES] Policy:
// it_*		- means ordinal number of player
// id_*		- means player-id
// eid_*	- means entity-id

class	game_sv_GameState	: public game_GameState
{
protected:
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

	// Utilities
	virtual		u32					get_alive_count			(u32 team);
	virtual		s32					get_option_i			(LPCSTR lst, LPCSTR name, s32 def = 0);

	// Events
	virtual		void				OnRoundStart			()								= 0;	// старт раунда
	virtual		void				OnRoundEnd				(LPCSTR reason)					{};		// конец раунда

	virtual		void				OnTeamScore				(u32 team)						= 0;	// команда выиграла
	virtual		void				OnTeamsInDraw			()								= 0;	// ничья
	virtual		BOOL				OnTargetTouched			(u32 id_who, u32 eid_target)	= 0;	// TRUE=allow ownership, FALSE=denie
	virtual		void				OnPlayerKillPlayer		(u32 id_killer, u32 id_killed)	= 0;

	// Main
	virtual		void				Update					()								= 0;
	virtual		void				net_Export_State		(NET_Packet& P, u32 id_to);				// full state
	virtual		void				net_Export_Update		(NET_Packet& P, u32 id_to, u32 id);		// just incremental update for specific client
};
