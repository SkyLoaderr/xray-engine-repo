#pragma once

#include "game_base.h"

class	game_sv_GameState
{
	u32								round;
	vector<game_TeamState>			teams;
public:
	// Main
	virtual		void				Lock					()								= 0;
	virtual		void				Unlock					()								= 0;
	virtual		game_PlayerState*	get_it					(DWORD it)						= 0;
	virtual		game_PlayerState*	get_id					(DWORD id)						= 0;	// DPNID
	virtual		u32					get_count				()								= 0;

	// Utilities
	virtual		u32					get_alive_count			(u32 team);

	// Events
	virtual		void				OnStartRound			()								= 0;	// ����� ������
	virtual		void				OnTeamScore				(u32 team)						= 0;	// ������� ��������
	virtual		void				OnTeamsInDraw			()								= 0;	// �����
	virtual		void				OnTargetTouched			(u32 id_who, u32 id_target)		= 0;
	virtual		void				OnPlayerKillPlayer		(u32 id_killer, u32 id_killed)	= 0;
	virtual		void				OnTimeElapsed			()								= 0;

	// Main
	virtual		void				Update					()								= 0;
	virtual		void				net_Export_State		(NET_Packet& P);						// full state
	virtual		void				net_Export_Update		(NET_Packet& P, DWORD id);				// just incremental update for specific client
};
