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
	virtual		void				Create					(shared_str& options);
	virtual		CSE_Abstract*		get_entity_from_eid		(u16 id);


	virtual		void				OnCreate				(u16 id_who);
	virtual		BOOL				OnTouch					(u16 eid_who, u16 eid_what);
	virtual		BOOL				OnDetach				(u16 eid_who, u16 eid_what);

	// Main
	virtual		void				Update					();
	virtual		ALife::_TIME_ID		GetGameTime				();
	virtual		float				GetGameTimeFactor		();
	virtual		void				SetGameTimeFactor		(const float fTimeFactor);
	virtual		bool				change_level			(NET_Packet &net_packet, ClientID sender);
	virtual		void				save_game				(NET_Packet &net_packet, ClientID sender);
	virtual		bool				load_game				(NET_Packet &net_packet, ClientID sender);
	virtual		void				reload_game				(NET_Packet &net_packet, ClientID sender);
	virtual		void				switch_distance			(NET_Packet &net_packet, ClientID sender);

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
