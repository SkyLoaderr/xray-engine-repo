#pragma once

#include "game_sv_base.h"
#include "inventory_space.h"

class	game_sv_Deathmatch			: public game_sv_GameState
{
protected:
	struct		RPointData
	{
		u32		PointID;
		float	MinEnemyDist;

		RPointData(u32 ID, float Dist):
		PointID(ID),
		MinEnemyDist(Dist)
		{
		};

		bool operator<(const RPointData &x)
		{
			return MinEnemyDist < x.MinEnemyDist;
		};
	};

	//
//	typedef	std::pair<std::string, s16>					WPN_DATA;
	// ������ � ������� �������� �������� ������ ��� ������  � �����
//	DEF_MAP(WPN_SLOT_NAMES, s16, WPN_DATA);
//	typedef xr_map<s16, std::pair<std::string, s16> >	WPN_SLOT_NAMES;
	// ������ �������� � ������� ������ � ������
//	DEF_VECTOR(TEAM_WPN_LIST, WPN_SLOT_NAMES);	

	//��������� ������ �� ������
	struct		WeaponDataStruct
	{
		u16				SlotItem_ID		;    //SlotID << 8 | ItemID;
		std::string		WeaponName		;

		u16				Cost			;

		bool			operator	==		(s16 ID)
		{
			return		(SlotItem_ID == ID);
		}

		bool			operator	==		(LPCSTR name)
		{
			int res = xr_strcmp(WeaponName.c_str(), name);
			return	res	 == 0;
		}
	};

	DEF_VECTOR(TEAM_WPN_LIST, WeaponDataStruct);

	// ������ ���� ������ ��������
	DEF_VECTOR(TEAM_SKINS_NAMES, std::string);	

	// ������ ���� ������ ��������
	DEF_VECTOR(DEF_ITEMS_LIST, u16);	

	//��������� ������ �� �������
	struct		TeamStruct
	{
		string256			caSection;		// ��� ������ ��������
		TEAM_SKINS_NAMES	aSkins;			// ������ ������ ��� �������
		TEAM_WPN_LIST		aWeapons;		// ������ ������ ��� �������
		DEF_ITEMS_LIST		aDefaultItems;	// ������ ��������� �� ���������

		//---- Money -------------------
		s16					m_iM_Start			;
		s16					m_iM_Min			;
		
		s16					m_iM_KillRival		;
		s16					m_iM_KillSelf		;
		s16					m_iM_KillTeam		;

		s16					m_iM_TargetRival	;
		s16					m_iM_TargetTeam		;
		s16					m_iM_TargetSucceed	;
		s16					m_iM_TargetSucceedAll	;

		s16					m_iM_RoundWin		;
		s16					m_iM_RoundLoose		;
		s16					m_iM_RoundDraw		;		
	};

	//������ ������ �� ��������
	DEF_DEQUE(TEAM_DATA_LIST, TeamStruct);

	TEAM_DATA_LIST		TeamList;

	//������ ������ ��� ��������
	DEF_DEQUE(CORPSE_LIST, u16);

	CORPSE_LIST		m_CorpseList;

	ref_str			m_sBaseWeaponCostSection;

protected:
	void							AllowDeadBodyRemove		(u32 id);
	void							SpawnActor				(u32 id, LPCSTR N);
	bool							GetPosAngleFromActor	(u32 id, Fvector& Pos, Fvector &Angle);
//	void							SpawnItem4Actor			(u32 actorId, LPCSTR N);
	void							SpawnWeapon4Actor		(u32 actorId,  LPCSTR N, u8 Addons = 0);
	void							KillPlayer				(u32 id_who);

	game_sv_Deathmatch::TeamStruct*	GetTeamData				(u8 Team);

	virtual	void					CheckItem		(game_PlayerState*	ps, PIItem pItem, xr_vector<s16> *pItemsDesired, xr_vector<u16> *pItemsToDelete);
public:

	virtual		void				Create					(LPSTR &options);

	// Events
	virtual		void				OnRoundStart			();										// ����� ������

	virtual		void				OnTeamScore				(u32 /**team/**/)						;		// ������� ��������
	virtual		void				OnTeamsInDraw			()								{};		// �����
	virtual		void				OnPlayerHitPlayer		(u16 id_hitter, u16 id_hitted, NET_Packet& P); //����� ������� Hit

	virtual		BOOL				OnTouch					(u16 eid_who, u16 eid_what);
	virtual		BOOL				OnDetach				(u16 eid_who, u16 eid_what);
	virtual		void				OnDestroyObject			(u16 eid_who);			

	virtual		void				OnPlayerConnect			(u32 id_who);
	virtual		void				OnPlayerDisconnect		(u32 id_who);
	virtual		void				OnPlayerReady			(u32 id_who);
	virtual		void				OnPlayerKillPlayer		(u32 id_killer, u32 id_killed);
	virtual		void				OnPlayerWantsDie		(u32 id_who);
	virtual		void				OnPlayerChangeSkin		(u32 id_who, u8 skin);

	virtual		void				OnPlayerBuyFinished		(u32 id_who, NET_Packet& P);
	
	virtual		void				OnFraglimitExceed		();
	virtual		void				OnTimelimitExceed		();

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

	virtual		void				SendPlayerKilledMessage	(u32 id_killer, u32 id_killed);
	virtual		bool				IsBuyableItem			(LPCSTR	ItemName);
	void							RemoveItemFromActor		(CSE_Abstract* pItem);
	//----- Money routines -----------------------------------------------------------------
	virtual		void				Money_SetStart			(u32	id_who);
//	virtual		s16					GetItemCost				(u32 id_who, s16 ItemID);
	DECLARE_SCRIPT_REGISTER_FUNCTION
};
add_to_type_list(game_sv_Deathmatch)
#undef script_type_list
#define script_type_list save_type_list(game_sv_Deathmatch)
