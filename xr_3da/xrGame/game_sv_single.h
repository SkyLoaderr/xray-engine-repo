#pragma once

#include "game_sv_base.h"

class	game_sv_Single				: public game_sv_GameState
{
protected:
public:
	virtual		void				Create					(LPCSTR options);

	// Events
	virtual		void				OnRoundStart			()								{};		// ����� ������
	virtual		void				OnRoundEnd				()								{};		// ����� ������
	virtual		void				OnTeamScore				(u32 team)						{};		// ������� ��������
	virtual		void				OnTeamsInDraw			()								{};		// �����
	virtual		void				OnPlayerKillPlayer		(u32 id_killer, u32 id_killed)	{};

	virtual		BOOL				OnTouch					(u16 eid_who, u16 eid_what);
	virtual		BOOL				OnDetouch				(u16 eid_who, u16 eid_what);

	virtual		void				OnFraglimitExceed		()								{};
	virtual		void				OnTimelimitExceed		()								{};

	// Main
	virtual		void				Update					()								{};
};
