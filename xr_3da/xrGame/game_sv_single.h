#pragma once

#include "game_sv_base.h"

class CSE_ALifeSimulator;

class	game_sv_Single				: public game_sv_GameState
{
private:
	typedef game_sv_GameState inherited;
public:
	CSE_ALifeSimulator				*m_tpALife;
	xrServer						*m_tpServer;

									game_sv_Single			(xrServer *tpServer) : game_sv_GameState()
	{
		m_tpServer					= tpServer;
		m_tpALife					= 0;
	};

	virtual							~game_sv_Single			();

	virtual		void				Create					(LPSTR &options);
	virtual		CSE_Abstract*		get_entity_from_eid		(u16 id);

	// Events
	virtual		void				OnRoundStart			();										// старт раунда
	virtual		void				OnRoundEnd				()								{};		// старт раунда
	virtual		void				OnTeamScore				(u32 /**team/**/)				{};		// команда выиграла
	virtual		void				OnTeamsInDraw			()								{};		// ничья
	virtual		void				OnPlayerKillPlayer		(u32 id_killer, u32 id_killed)	;//{};

	virtual		void				OnCreate				(u16 id_who);
	virtual		BOOL				OnTouch					(u16 eid_who, u16 eid_what);
	virtual		BOOL				OnDetach				(u16 eid_who, u16 eid_what);

	virtual		void				OnFraglimitExceed		()								{};
	virtual		void				OnTimelimitExceed		()								{};
	virtual		void				OnPlayerHitPlayer		(u16 id_hitter, u16 id_hitted, NET_Packet& P) {}; //игрок получил Hit

	// Main
	virtual		void				Update					();
	virtual		ALife::_TIME_ID		GetGameTime				();
	virtual		float				GetGameTimeFactor		();
	virtual		void				SetGameTimeFactor		(const float fTimeFactor);
	virtual		bool				change_level			(NET_Packet &net_packet, DPNID sender);
};
