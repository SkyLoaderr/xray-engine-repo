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
	
	
	BOOL							m_delayedRoundEnd;
	u32								m_roundEndDelay;

	int								m_iReinforcementTime;		//0 - Immediate, -1 - after artefact spawn , other - reinforcement
	u32								m_dwNextReinforcementTime;

	u32								m_dwArtefactRespawnDelta;
	u32								m_dwArtefactStayTime;

	u32								m_dwArtefactSpawnTime;
	u32								m_dwArtefactRemoveTime;

	u16								m_ArtefactsSpawnedTotal;
	u16								m_dwArtefactID;	

	ARTEFACT_STATE					m_eAState;

	xr_vector<RPoint>				Artefact_rpoints;
	xr_vector<u8>					ArtefactsRPoints_ID;
	u8								m_LastRespawnPointID;

	u8								artefactsNum;//ah
	u16								artefactBearerID;//ah,ZoneMap
	u8								teamInPossession;//ah,ZoneMap

	bool							bNoLostMessage;

	void							Artefact_PrepareForSpawn	();
	void							Artefact_PrepareForRemove	();

	bool							Artefact_NeedToSpawn	();
	bool							Artefact_NeedToRemove	();
	bool							Artefact_MissCheck		();

	void							CheckForAnyAlivePlayer	();
    	
public:

									game_sv_ArtefactHunt	(){type = GAME_ARTEFACTHUNT;}
	virtual		void				Create					(ref_str& options);

	virtual		LPCSTR				type_name			() const { return "artefacthunt";};
	// Events	
	virtual		void				OnEvent					(NET_Packet &tNetPacket, u16 type, u32 time, ClientID sender );
	virtual		void				OnRoundStart			();							// старт раунда
	virtual		void				OnPlayerKillPlayer		(ClientID id_killer, ClientID id_killed);
	virtual		void				OnPlayerReady			(ClientID id_who);

	virtual		void				OnTimelimitExceed		();

	virtual		u32					RP_2_Use				(CSE_Abstract* E);
	virtual		void				RespawnPlayer			(ClientID id_who, bool NoSpectator);


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

				void				OnDelayedRoundEnd		(LPCSTR /**reason/**/);
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
};
