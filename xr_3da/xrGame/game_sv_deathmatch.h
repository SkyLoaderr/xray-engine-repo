#pragma once

#include "game_sv_mp.h"
#include "inventory_space.h"
#include "client_id.h"

class	game_sv_Deathmatch			: public game_sv_mp
{
	typedef game_sv_mp inherited;
protected:
	struct		RPointData
	{
		u32		PointID;
		float	MinEnemyDist;
		bool	bFreezed;

		RPointData(u32 ID, float Dist, bool Freezed):
		PointID(ID),MinEnemyDist(Dist), bFreezed(Freezed){};
		bool operator<(const RPointData &x)	
		{
			if (bFreezed && !x.bFreezed) return false;
			if (!bFreezed && x.bFreezed) return true;
			return MinEnemyDist < x.MinEnemyDist;
		};
	};
	//---------------------------------------------------
	xr_vector<u32>					m_vFreeRPoints;
	u32								m_dwLastRPoint;
	//---------------------------------------------------

	BOOL							m_delayedRoundEnd;
	u32								m_roundEndDelay;

	shared_str							m_sBaseWeaponCostSection;
	
	s32								fraglimit; //dm,tdm,ah
	s32								timelimit; //dm
	u32								damageblocklimit;//dm,tdm
	BOOL							m_bDamageBlockIndicators;
	BOOL							m_bPDAHunt;
	xr_vector<game_TeamState>		teams;//dm,tdm,ah
	u32								m_u32ForceRespawn;

	LPCSTR							pWinnigPlayerName;

	virtual		void				ReadOptions				(shared_str &options);
	virtual		void				ConsoleCommands_Create	();
	virtual		void				ConsoleCommands_Clear	();
	/////////////////////////////////////////////////////////////
	DEF_VECTOR(ANOMALIES, xr_string);
	DEF_VECTOR(ANOMALY_SETS, ANOMALIES);

	ANOMALY_SETS					m_AnomalySetsList;
	xr_vector<u8>					m_AnomalySetID;
	u32								m_dwAnomalySetLengthTime;
	u32								m_dwLastAnomalySetID;
	u32								m_dwLastAnomalyStartTime;	
	BOOL							m_bAnomaliesEnabled;

	DEF_VECTOR(ANOMALIES_ID, u16);
	DEF_VECTOR(ANOMALY_SETS_ID, ANOMALIES_ID);

	ANOMALY_SETS_ID					m_AnomalyIDSetsList;
	//--------------------------------------------------
	bool							m_bSpectatorMode;
	u32								m_dwSM_SwitchDelta;
	u32								m_dwSM_LastSwitchTime;
	u32								m_dwSM_CurViewEntity;
	CObject	*						m_pSM_CurViewEntity;
	void							SM_SwitchOnNextActivePlayer			();
	void							SM_SwitchOnPlayer		(CObject* pNewObject);//(game_PlayerState* ps);

protected:


	virtual		bool				checkForTimeLimit		();
	virtual		bool				checkForFragLimit		();
	virtual		bool				checkForRoundStart		();
	virtual		bool				checkForRoundEnd		();
	virtual		bool				check_for_Anomalies		();
	virtual		void				check_for_WarmUp		();

				void				Send_Anomaly_States		(ClientID id_who);
				void				Send_EventPack_for_AnomalySet	(u32 AnomalySet, u8 Event);

//	virtual		void				OnPlayerChangeSkin		(ClientID id_who, u8 skin);
//	virtual		void				OnPlayerWantsDie		(ClientID id_who);
	virtual		void				OnPlayerBuyFinished		(ClientID id_who, NET_Packet& P);

//	void							SpawnWeapon4Actor		(u32 actorId,  LPCSTR N, u8 Addons = 0);
//	void							KillPlayer				(ClientID id_who);


	virtual		void				CheckItem				(game_PlayerState*	ps, PIItem pItem, xr_vector<s16> *pItemsDesired, xr_vector<u16> *pItemsToDelete);
	virtual		bool				HasChampion				();

	virtual		void				check_Player_for_Invincibility	(game_PlayerState* ps);

	virtual		void				Check_ForClearRun		(game_PlayerState* ps);


	u32								m_dwWarmUp_MaxTime;
	u32								m_dwWarmUp_CurTime;
public:
									game_sv_Deathmatch		();
									~game_sv_Deathmatch		();
	virtual		void				Create					(shared_str &options);

	virtual		LPCSTR				type_name				() const { return "deathmatch";};
	virtual		void				net_Export_State		(NET_Packet& P, ClientID id_to);

	virtual		void				OnEvent					(NET_Packet &tNetPacket, u16 type, u32 time, ClientID sender );

	virtual		void				OnTeamScore				(u32 /**team/**/, bool)						;		// ������� ��������
	virtual		void				OnTeamsInDraw			()								{};		// �����

	// Events
	virtual		void				OnRoundStart			();												// ����� ������
	virtual		void				OnRoundEnd				(LPCSTR reason);								// ����� ������
	virtual		void				OnDelayedRoundEnd		(LPCSTR /**reason/**/);

	virtual		void				OnPlayerHitPlayer		(u16 id_hitter, u16 id_hitted, NET_Packet& P); //����� ������� Hit

	virtual		BOOL				OnTouch					(u16 eid_who, u16 eid_what);
	virtual		BOOL				OnDetach				(u16 eid_who, u16 eid_what);
	virtual		void				OnCreate				(u16 eid_who);

	virtual		void				OnPlayerConnect			(ClientID id_who);
	virtual		void				OnPlayerDisconnect		(ClientID id_who, LPSTR Name, u16 GameID);
	virtual		void				OnPlayerReady			(ClientID id_who);
	virtual		KILL_RES			GetKillResult			(game_PlayerState* pKiller, game_PlayerState* pVictim);
	virtual		bool				OnKillResult			(KILL_RES KillResult, game_PlayerState* pKiller, game_PlayerState* pVictim);
	virtual		void				OnGiveBonus				(KILL_RES KillResult, game_PlayerState* pKiller, game_PlayerState* pVictim, KILL_TYPE KillType, SPECIAL_KILL_TYPE SpecialKillType, CSE_Abstract* pWeaponA);
	virtual		void				Processing_Victim		(game_PlayerState* pVictim, game_PlayerState* pKiller);
	virtual		void				Victim_Exp				(game_PlayerState* pVictim);
	virtual		bool				CheckTeams				() { return false; };
	virtual		void				OnPlayerKillPlayer		(game_PlayerState* ps_killer, game_PlayerState* ps_killed, KILL_TYPE KillType, SPECIAL_KILL_TYPE SpecialKillType, CSE_Abstract* pWeaponA);
	virtual		void				OnPlayerConnectFinished	(ClientID id_who);
	
	virtual		void				OnPlayerSelectSkin		(NET_Packet& P, ClientID sender);
	virtual		void				OnPlayerChangeSkin		(ClientID id_who, s8 skin);
	
	virtual		void				OnFraglimitExceed		();
	virtual		void				OnTimelimitExceed		();
				void				OnPlayerScores			();
	virtual		void				OnDestroyObject			(u16 eid_who);
	virtual		void				OnPlayerFire (ClientID id_who, NET_Packet &P);
	// Main
	virtual		void				Update					();
				BOOL				AllPlayers_Ready		();

	virtual		void				assign_RP				(CSE_Abstract* E, game_PlayerState* ps_who);
	virtual		u32					RP_2_Use				(CSE_Abstract* E);
	//  [7/5/2005]
#ifdef DEBUG
	virtual		void				OnRender				();
#endif
	//  [7/5/2005]

	virtual		void				SetSkin					(CSE_Abstract* E, u16 Team, u16 ID);//	{};

	virtual		void				SpawnWeaponsForActor	(CSE_Abstract* pE, game_PlayerState*	ps);

	
	virtual		void				LoadTeams				();
	virtual		void				LoadTeamData			(char* caSection);
	virtual		void				LoadWeaponsForTeam		(char* caSection, TEAM_WPN_LIST *pTeamWpnList);
	virtual		void				LoadSkinsForTeam		(char* caSection, TEAM_SKINS_NAMES* pTeamSkins);
	virtual		void				LoadDefItemsForTeam		(char* caSection, TEAM_WPN_LIST *pWpnList, DEF_ITEMS_LIST* pDefItems);

	virtual		char*				GetAnomalySetBaseName	() {return "deathmatch_game_anomaly_sets";};
	virtual		void				LoadAnomalySets			();
	virtual		void				StartAnomalies			(int AnomalySet = -1);

	virtual		bool				IsBuyableItem			(LPCSTR	ItemName);
	void							RemoveItemFromActor		(CSE_Abstract* pItem);
	//----- Money routines -----------------------------------------------------------------
	virtual		void				Money_SetStart			(ClientID	id_who);
	virtual		s32					GetMoneyAmount			(char* caSection, char* caMoneyStr);
//	virtual		s16					GetItemCost				(u32 id_who, s16 ItemID);
				int					GetTeamScore			(u32 idx);
				void				SetTeamScore			(u32 idx, int val);
				game_PlayerState*	GetWinningPlayer		();
	virtual		BOOL				CanHaveFriendlyFire		()	{return FALSE;}
	virtual		void				RespawnPlayer			(ClientID id_who, bool NoSpectator);
	virtual		void				check_InvinciblePlayers	();	
	virtual		void				check_ForceRespawn		();
	virtual		BOOL				IsDamageBlockIndEnabled	() {return m_bDamageBlockIndicators; };
	
	DECLARE_SCRIPT_REGISTER_FUNCTION
};
add_to_type_list(game_sv_Deathmatch)
#undef script_type_list
#define script_type_list save_type_list(game_sv_Deathmatch)
