#pragma once

#include "game_base.h"

class	game_sv_GameState	: public game_GameState
{
protected:
public:
	// Main
	virtual		void				Lock					();
	virtual		void				Unlock					();
	virtual		game_PlayerState*	get_it					(DWORD it);
	virtual		game_PlayerState*	get_id					(DWORD id);								// DPNID
	virtual		string64*			get_name_it				(DWORD it);
	virtual		string64*			get_name_id				(DWORD id);								// DPNID
	virtual		u32					get_it_2_id				(DWORD it);
	virtual		u32					get_count				();

	// Utilities
	virtual		u32					get_alive_count			(u32 team);
	virtual		s32					get_option_i			(LPCSTR lst, LPCSTR name, s32 def = 0);

	// Events
	virtual		void				OnRoundStart			()								= 0;	// старт раунда
	virtual		void				OnRoundEnd				(LPCSTR reason);						// конец раунда

	virtual		void				OnTeamScore				(u32 team)						= 0;	// команда выиграла
	virtual		void				OnTeamsInDraw			()								= 0;	// ничья
	virtual		void				OnTargetTouched			(u32 id_who, u32 id_target)		= 0;
	virtual		void				OnPlayerKillPlayer		(u32 id_killer, u32 id_killed)	= 0;

	virtual		void				OnFraglimitExceed		()								= 0;
	virtual		void				OnTimelimitExceed		()								= 0;

	// Main
	virtual		void				Update					()								= 0;
	virtual		void				net_Export_State		(NET_Packet& P);						// full state
	virtual		void				net_Export_Update		(NET_Packet& P, DWORD id);				// just incremental update for specific client
};
