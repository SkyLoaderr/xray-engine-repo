#pragma once

#include "game_sv_deathmatch.h"

class	game_sv_TeamDeathmatch			: public game_sv_Deathmatch
{
private:
	typedef game_sv_Deathmatch inherited;

protected:
	BOOL		m_bAutoTeamBalance;
	BOOL		m_bFriendlyIndicators;
	float		m_fFriendlyFireModifier;


	virtual		bool				checkForFragLimit		();
	virtual		bool				HasChampion				();
		
	virtual		void				ReadOptions				(shared_str &options);
	virtual		void				ConsoleCommands_Create	();
	virtual		void				ConsoleCommands_Clear	();

public:	
									game_sv_TeamDeathmatch	(){type = GAME_TEAMDEATHMATCH;}
	virtual		void				Create					(shared_str& options);

	virtual		void				OnEvent					(NET_Packet &tNetPacket, u16 type, u32 time, ClientID sender );

	virtual		LPCSTR				type_name			() const { return "teamdeathmatch";};

	virtual		void				Update					();
	virtual		void				net_Export_State		(NET_Packet& P, ClientID id_to);				// full state
	// Events	
	virtual		void				OnPlayerConnect			(ClientID id_who);

	virtual		void				OnPlayerChangeTeam		(ClientID id_who, s16 team);
	virtual		void				OnPlayerKillPlayer		(game_PlayerState* ps_killer, game_PlayerState* ps_killed);

	virtual		void				OnPlayerHitPlayer		(u16 id_hitter, u16 id_hitted, NET_Packet& P);

	virtual		void				OnRoundStart			();												// ����� ������
	virtual		void				AutoBalanceTeams		();

	virtual		u8					AutoTeam				( );
	virtual		u32					RP_2_Use				(CSE_Abstract* E);

	virtual		void				LoadTeams				();

	virtual		char*				GetAnomalySetBaseName	()	{return "teamdeathmatch_game_anomaly_sets";};
	virtual		BOOL				isFriendlyFireEnabled	()	{return (m_fFriendlyFireModifier > 0.1f);};
	virtual		float				GetFriendlyFire			()	{ return (m_fFriendlyFireModifier > 0.1f) ? m_fFriendlyFireModifier : 0.0f;};
	virtual		BOOL				CanHaveFriendlyFire		()	{return TRUE;}
	virtual		void				OnFraglimitExceed		();
	virtual		void				OnTimelimitExceed		();

};
