#pragma once

#include "game_sv_base.h"
#include "ai_alife.h"

class	game_sv_Single				: public game_sv_GameState
{
private:
public:
	CAI_ALife						*m_tpALife;
	xrServer						*m_tpServer;
	bool							m_bALife;
									game_sv_Single			(xrServer *tpServer) : game_sv_GameState()
	{
		m_tpServer					= tpServer;
		m_tpALife					= 0;
		m_bALife					= false;
	};

	virtual							~game_sv_Single			()
	{
		xr_delete					(m_tpALife);
	};

	virtual		void				Create					(LPCSTR options);

	// Events
	virtual		void				OnRoundStart			();										// ����� ������
	virtual		void				OnRoundEnd				()								{};		// ����� ������
	virtual		void				OnTeamScore				(u32 team)						{};		// ������� ��������
	virtual		void				OnTeamsInDraw			()								{};		// �����
	virtual		void				OnPlayerKillPlayer		(u32 id_killer, u32 id_killed)	{};

	virtual		BOOL				OnTouch					(u16 eid_who, u16 eid_what);
	virtual		BOOL				OnDetach				(u16 eid_who, u16 eid_what);

	virtual		void				OnFraglimitExceed		()								{};
	virtual		void				OnTimelimitExceed		()								{};

	// Main
	virtual		void				Update					();
};
