#pragma once

#include "game_sv_base.h"

class	game_sv_CS					: public game_sv_GameState
{
protected:
public:
	virtual		void				Create					(LPCSTR options);

	// Events
	virtual		void				OnRoundStart			();										// ����� ������

	virtual		void				OnTeamScore				(u32 team);								// ������� ��������
	virtual		void				OnTeamsInDraw			();										// �����
	virtual		BOOL				OnTargetTouched			(u32 id_who, u32 eid_target);
	virtual		BOOL				OnTargetDetouched		(u32 id_who, u32 eid_target);			// TRUE=allow ownership, FALSE=denied

	virtual		void				OnPlayerReady			(u32 id_who);
	virtual		void				OnPlayerKillPlayer		(u32 id_killer, u32 id_killed);

	virtual		void				OnFraglimitExceed		()								{};
	virtual		void				OnTimelimitExceed		();

	// Main
	virtual		void				Update					();
};
