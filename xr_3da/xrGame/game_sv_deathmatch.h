#pragma once

#include "game_sv_base.h"

class	game_sv_Deathmatch			: public game_sv_GameState
{
protected:
public:
	virtual		void				Create					(LPCSTR options);

	// Events
	virtual		void				OnRoundStart			()								{};		// старт раунда

	virtual		void				OnTeamScore				(u32 team)						{};		// команда выиграла
	virtual		void				OnTeamsInDraw			()								{};		// ничья
	virtual		void				OnTargetTouched			(u32 id_who, u32 id_target)		{};

	virtual		void				OnPlayerKillPlayer		(u32 id_killer, u32 id_killed);

	virtual		void				OnFraglimitExceed		();
	virtual		void				OnTimelimitExceed		();

	// Main
	virtual		void				Update					();
};
