#pragma once

#include "game_sv_base.h"

class	game_sv_CS					: public game_sv_GameState
{
protected:
public:
	virtual		void				Create					(LPCSTR options);

	// Events
	virtual		void				OnRoundStart			();										// старт раунда
	virtual		void				OnRoundEnd				(LPCSTR reason);
	virtual		void				OnDelayedRoundEnd		(LPCSTR reason);

	virtual		void				OnTeamScore				(u32 team);								// команда выиграла
	virtual		void				OnTeamsInDraw			();										// ничья

	virtual		BOOL				OnTouch					(u16 eid_who, u16 eid_what);
	virtual		BOOL				OnDetach				(u16 eid_who, u16 eid_what);

	virtual		void				OnPlayerConnect			(u32 id_who);
	virtual		void				OnPlayerDisconnect		(u32 id_who);
	virtual		void				OnPlayerBuy				(u32 id_who, u16 eid_who, LPCSTR what);
	virtual		void				OnPlayerReady			(u32 id_who);
	virtual		void				OnPlayerChangeTeam		(u32 id_who);
	virtual		void				OnPlayerKillPlayer		(u32 id_killer, u32 id_killed);

	virtual		void				OnFraglimitExceed		()								{};
	virtual		void				OnTimelimitExceed		();

	// Mains
	virtual		void				Update					();

	u8 AutoTeam();

	BOOL m_delayedRoundEnd;
	u32 m_roundEndDelay;
};
