#pragma once

#include "game_sv_mp.h"
#include "inventory_space.h"



class	game_sv_Deathmatch			: public game_sv_mp
{
	typedef game_sv_mp inherited;
protected:
	struct		RPointData
	{
		u32		PointID;
		float	MinEnemyDist;

		RPointData(u32 ID, float Dist):
		PointID(ID),MinEnemyDist(Dist){};
		bool operator<(const RPointData &x)	{return MinEnemyDist < x.MinEnemyDist;};
	};



	ref_str							m_sBaseWeaponCostSection;
	
	s32								fraglimit; //dm,tdm,ah
	s32								timelimit; //dm
	u32								damageblocklimit;//dm,tdm
	xr_vector<game_TeamState>		teams;//dm,tdm,ah
	/////////////////////////////////////////////////////////////
	DEF_VECTOR(ANOMALIES, std::string);
	DEF_VECTOR(ANOMALY_SETS, ANOMALIES);

	ANOMALY_SETS					m_AnomalySetsList;
	xr_vector<u8>					m_AnomalySetID;
	u32								m_dwLastAnomalySetID;
	bool							m_bAnomaliesEnabled;
	//--------------------------------------------------
	bool							m_bSpectatorMode;
	u32								m_dwSM_SwitchDelta;
	u32								m_dwSM_LastSwitchTime;
	u32								m_dwSM_CurViewEntity;
	void							SM_SwitchOnNextActivePlayer			();
	void							SM_SwitchOnPlayer		(CObject* pNewObject);//(game_PlayerState* ps);

protected:


	virtual		bool				checkForTimeLimit		();
	virtual		bool				checkForFragLimit		();
	virtual		bool				checkForRoundStart		();
	virtual		bool				checkForRoundEnd		();

//	virtual		void				OnPlayerChangeSkin		(ClientID id_who, u8 skin);
//	virtual		void				OnPlayerWantsDie		(ClientID id_who);
	virtual		void				OnPlayerBuyFinished		(ClientID id_who, NET_Packet& P);

//	void							SpawnWeapon4Actor		(u32 actorId,  LPCSTR N, u8 Addons = 0);
//	void							KillPlayer				(ClientID id_who);


	virtual	void					CheckItem				(game_PlayerState*	ps, PIItem pItem, xr_vector<s16> *pItemsDesired, xr_vector<u16> *pItemsToDelete);
public:
									game_sv_Deathmatch		(){type = GAME_DEATHMATCH;};
	virtual		void				Create					(ref_str &options);

	virtual		LPCSTR				type_name				() const { return "deathmatch";};
	virtual		void				net_Export_State		(NET_Packet& P, ClientID id_to);

	virtual		void				OnEvent					(NET_Packet &tNetPacket, u16 type, u32 time, ClientID sender );

	virtual		void				OnTeamScore				(u32 /**team/**/, bool)						;		// команда выиграла
	virtual		void				OnTeamsInDraw			()								{};		// ничья

	// Events
	virtual		void				OnRoundStart			();												// старт раунда

	virtual		void				OnPlayerHitPlayer		(u16 id_hitter, u16 id_hitted, NET_Packet& P); //игрок получил Hit

	virtual		BOOL				OnTouch					(u16 eid_who, u16 eid_what);
	virtual		BOOL				OnDetach				(u16 eid_who, u16 eid_what);

	virtual		void				OnPlayerConnect			(ClientID id_who);
	virtual		void				OnPlayerDisconnect		(ClientID id_who, LPSTR Name);
	virtual		void				OnPlayerReady			(ClientID id_who);
	virtual		void				OnPlayerKillPlayer		(ClientID id_killer, ClientID id_killed);
				void				OnPlayerChangeSkin		(ClientID id_who, u8 skin);
	
	virtual		void				OnFraglimitExceed		();
	virtual		void				OnTimelimitExceed		();
	virtual		void				OnDestroyObject			(u16 eid_who);
	// Main
	virtual		void				Update					();
				BOOL				AllPlayers_Ready		();

	virtual		void				assign_RP				(CSE_Abstract* E);
	virtual		u32					RP_2_Use				(CSE_Abstract* E);

	virtual		void				SetSkin					(CSE_Abstract* E, u16 Team, u16 ID);//	{};

	virtual		void				ClearPlayerState		(game_PlayerState* ps);
	virtual		void				ClearPlayerItems		(game_PlayerState* ps);
	virtual		void				SetPlayersDefItems		(game_PlayerState* ps);

	virtual		void				SpawnWeaponsForActor	(CSE_Abstract* pE, game_PlayerState*	ps);

	
	virtual		void				LoadTeams				();
	virtual		void				LoadTeamData			(char* caSection);
	virtual		void				LoadWeaponsForTeam		(char* caSection, TEAM_WPN_LIST *pTeamWpnList);
	virtual		void				LoadSkinsForTeam		(char* caSection, TEAM_SKINS_NAMES* pTeamSkins);
	virtual		void				LoadDefItemsForTeam		(char* caSection, TEAM_WPN_LIST *pWpnList, DEF_ITEMS_LIST* pDefItems);

	virtual		char*				GetAnomalySetBaseName	() {return "deathmatch_game_anomaly_sets";};
	virtual		void				LoadAnomalySets			();
	virtual		void				StartAnomalies			();

//	virtual		void				SendPlayerKilledMessage	(u32 id_killer, u32 id_killed);
	virtual		bool				IsBuyableItem			(LPCSTR	ItemName);
	void							RemoveItemFromActor		(CSE_Abstract* pItem);
	//----- Money routines -----------------------------------------------------------------
	virtual		void				Money_SetStart			(ClientID	id_who);
	virtual		void				Player_AddMoney			(game_PlayerState* ps, s32 MoneyAmount);
	virtual		s16					GetMoneyAmount			(char* caSection, char* caMoneyStr);
//	virtual		s16					GetItemCost				(u32 id_who, s16 ItemID);
				int					GetTeamScore			(u32 idx);
				void				SetTeamScore			(u32 idx, int val);
				

	DECLARE_SCRIPT_REGISTER_FUNCTION
};
add_to_type_list(game_sv_Deathmatch)
#undef script_type_list
#define script_type_list save_type_list(game_sv_Deathmatch)
