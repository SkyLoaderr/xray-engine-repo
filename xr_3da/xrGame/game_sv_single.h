#pragma once

#include "game_sv_base.h"

class xrServer;
class CALifeSimulator;

class	game_sv_Single				: public game_sv_GameState
{
private:
	typedef game_sv_GameState inherited;

protected:
	CALifeSimulator					*m_alife_simulator;

public:
									game_sv_Single			();
	virtual							~game_sv_Single			();

	virtual		LPCSTR				type_name		() const { return "single";};
	virtual		void				Create					(ref_str& options);
	virtual		CSE_Abstract*		get_entity_from_eid		(u16 id);

	virtual		void				OnEvent					(NET_Packet &tNetPacket, u16 type, u32 time, u32 sender );

	// Events
	virtual		void				OnRoundStart			();										// старт раунда
	virtual		void				OnRoundEnd				()								{};		// старт раунда
	virtual		void				OnTeamScore				(u32 /**team/**/)				{};		// команда выиграла
	virtual		void				OnTeamsInDraw			()								{};		// ничья
	virtual		void				OnPlayerKillPlayer		(u32 id_killer, u32 id_killed)	;//{};
	virtual		void				OnPlayerKilled			(u32 id_killed)	{};

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
	virtual		void				save_game				(NET_Packet &net_packet, DPNID sender);
	virtual		bool				load_game				(NET_Packet &net_packet, DPNID sender);
	virtual		void				reload_game				(NET_Packet &net_packet, DPNID sender);
	virtual		void				switch_distance			(NET_Packet &net_packet, DPNID sender);

	IC			xrServer			&server					() const
	{
		VERIFY						(m_server);
		return						(*m_server);
	}

	IC			CALifeSimulator		&alife					() const
	{
		VERIFY						(m_alife_simulator);
		return						(*m_alife_simulator);
	}
};
