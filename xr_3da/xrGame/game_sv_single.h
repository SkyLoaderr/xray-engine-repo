#pragma once

#include "game_sv_base.h"
#include "ai_alife.h"

class	game_sv_Single				: public game_sv_GameState
{
private:
public:
	CAI_ALife						m_tALife;
									game_sv_Single			(){};
	virtual							~game_sv_Single			(){};
	virtual		void				Create					(LPCSTR options);

	// Events
	virtual		void				OnRoundStart			();										// старт раунда
	virtual		void				OnRoundEnd				()								{};		// старт раунда
	virtual		void				OnTeamScore				(u32 team)						{};		// команда выиграла
	virtual		void				OnTeamsInDraw			()								{};		// ничья
	virtual		void				OnPlayerKillPlayer		(u32 id_killer, u32 id_killed)	{};

	virtual		BOOL				OnTouch					(u16 eid_who, u16 eid_what);
	virtual		BOOL				OnDetach				(u16 eid_who, u16 eid_what);

	virtual		void				OnFraglimitExceed		()								{};
	virtual		void				OnTimelimitExceed		()								{};

	// Main
	virtual		void				Update					();
};
