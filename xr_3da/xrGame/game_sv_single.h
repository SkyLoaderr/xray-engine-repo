#pragma once

#include "game_sv_base.h"

class	game_sv_Single				: public game_sv_GameState
{
protected:
public:
	virtual		void				Create					(LPCSTR options)				{};

	// Events
	virtual		void				OnStartRound			()								{};		// ����� ������
	virtual		void				OnTeamScore				(u32 team)						{};		// ������� ��������
	virtual		void				OnTeamsInDraw			()								{};		// �����
	virtual		void				OnTargetTouched			(u32 id_who, u32 id_target)		{};
	virtual		void				OnPlayerKillPlayer		(u32 id_killer, u32 id_killed)	{};
	virtual		void				OnTimeElapsed			()								{};

	// Main
	virtual		void				Update					()								{};
};
