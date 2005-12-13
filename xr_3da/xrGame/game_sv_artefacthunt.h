#pragma once

#include "game_sv_teamdeathmatch.h"

class	game_sv_ArtefactHunt			: public game_sv_TeamDeathmatch
{
private:
	typedef game_sv_TeamDeathmatch inherited;

	enum	ARTEFACT_STATE
	{
		NONE,
		NOARTEFACT,
		ON_FIELD,
		IN_POSSESSION,
	};

protected:
	int								m_iReinforcementTime;		//0 - Immediate, -1 - after artefact spawn , other - reinforcement
	u32								m_dwNextReinforcementTime;

	u32								m_dwArtefactRespawnDelta;
	u32								m_dwArtefactStayTime;

	u32								m_dwArtefactSpawnTime;
	u32								m_dwArtefactRemoveTime;

	u16								m_ArtefactsSpawnedTotal;
	u16								m_dwArtefactID;	

	ARTEFACT_STATE					m_eAState;
	bool							m_bWasTaken;

	xr_vector<RPoint>				Artefact_rpoints;
	xr_vector<u8>					ArtefactsRPoints_ID;
	u8								m_LastRespawnPointID;

	int								m_dwArtefactsNum;//ah
	u16								artefactBearerID;//ah,ZoneMap
	u16								m_iAfBearerMenaceID;
	u8								teamInPossession;//ah,ZoneMap

	bool							bNoLostMessage;
	bool							m_bArtefactWasBringedToBase;

	void							Artefact_PrepareForSpawn	();
	void							Artefact_PrepareForRemove	();

	bool							Artefact_NeedToSpawn	();
	bool							Artefact_NeedToRemove	();
	bool							Artefact_MissCheck		();

	void							CheckForAnyAlivePlayer	();

	virtual		void				check_Player_for_Invincibility	(game_PlayerState* ps);    	
	virtual		void				Check_ForClearRun		(game_PlayerState* ps);

	virtual		void				ReadOptions				(shared_str &options);
	virtual		void				ConsoleCommands_Create	();
	virtual		void				ConsoleCommands_Clear	();

	virtual		bool				Player_Check_Rank		(game_PlayerState* ps);

public:

									game_sv_ArtefactHunt	(){type = GAME_ARTEFACTHUNT;}
	virtual		void				Create					(shared_str& options);

	virtual		LPCSTR				type_name			() const { return "artefacthunt";};
	// Events	
	virtual		void				OnEvent					(NET_Packet &tNetPacket, u16 type, u32 time, ClientID sender );
	virtual		void				OnRoundStart			();							// ����� ������
	virtual		KILL_RES			GetKillResult			(game_PlayerState* pKiller, game_PlayerState* pVictim);
	virtual		bool				OnKillResult			(KILL_RES KillResult, game_PlayerState* pKiller, game_PlayerState* pVictim);
	virtual		void				OnGiveBonus				(KILL_RES KillResult, game_PlayerState* pKiller, game_PlayerState* pVictim, KILL_TYPE KillType, SPECIAL_KILL_TYPE SpecialKillType, CSE_Abstract* pWeaponA);
	virtual		void				OnPlayerHitPlayer		(u16 id_hitter, u16 id_hitted, NET_Packet& P);
	virtual		void				OnPlayerHitPlayer_Case	(game_PlayerState* ps_hitter, game_PlayerState* ps_hitted, SHit* pHitS);	
	virtual		void				OnPlayerKillPlayer		(game_PlayerState* ps_killer, game_PlayerState* ps_killed, KILL_TYPE KillType, SPECIAL_KILL_TYPE SpecialKillType, CSE_Abstract* pWeaponA);
	virtual		void				OnPlayerFire			(ClientID id_who, NET_Packet &P) {};
	virtual		void				Victim_Exp				(game_PlayerState* pVictim) {};
	virtual		void				UpdateTeamScore			(game_PlayerState* ps_killer) {};
	virtual		void				OnPlayerReady			(ClientID id_who);

	virtual		void				OnTimelimitExceed		();

	virtual		void				assign_RP				(CSE_Abstract* E, game_PlayerState* ps_who);
	virtual		u32					RP_2_Use				(CSE_Abstract* E);	
	virtual		void				CheckRPUnblock			();
	virtual		void				SetRP					(CSE_Abstract* E, RPoint* pRP);

	virtual		void				LoadTeams				();

	virtual		char*				GetAnomalySetBaseName	() {return "artefacthunt_game_anomaly_sets";};

//	virtual		void				OnPlayerBuyFinished		(u32 id_who, NET_Packet& P);

	virtual		void				OnObjectEnterTeamBase	(u16 id, u16 zone_team);
	virtual		void				OnObjectLeaveTeamBase	(u16 id, u16 zone_team);
	
///	bool							IsBuyableItem			(CSE_Abstract* pItem);
//	BOOL							CheckUpgrades			(CSE_Abstract* pItem, u8 IItem);	
//	void							RemoveItemFromActor		(CSE_Abstract* pItem);
	void							OnArtefactOnBase		(ClientID id_who);

	virtual		BOOL				OnTouch					(u16 eid_who, u16 eid_what);
	virtual		BOOL				OnDetach				(u16 eid_who, u16 eid_what);
	virtual		void				OnCreate				(u16 id_who);


	virtual		void				Update					();
				
				void				SpawnArtefact			();
				void				RemoveArtefact			();
				void				Assign_Artefact_RPoint	(CSE_Abstract* E);

	virtual		void				net_Export_State		(NET_Packet& P, ClientID id_to);				// full state
				bool				ArtefactSpawn_Allowed	();
	//-------------------------------------------------------------------------------
	virtual		void				RespawnAllNotAlivePlayers	();
	virtual		bool				CheckAlivePlayersInTeam		(s16 Team);
	virtual		void				MoveAllAlivePlayers			();
	virtual		void				CheckForTeamElimination		();
	virtual		void				CheckForTeamWin				();
	virtual		BOOL				CanHaveFriendlyFire		()	{return TRUE;}

	//  [7/5/2005]
#ifdef DEBUG
	virtual		void				OnRender				();
#endif
	//  [7/5/2005]

};
