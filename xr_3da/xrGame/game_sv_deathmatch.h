#pragma once

#include "game_sv_base.h"

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

	// ������ � ������� ��������� �������� ������ ��� ������  � �����
	DEF_VECTOR(WPN_SLOT_NAMES, std::string);
	// ������ �������� � ������� ������ � ������
	DEF_VECTOR(TEAM_WPN_LIST, WPN_SLOT_NAMES);	

	// ������ ���� ������ ��������
	DEF_VECTOR(TEAM_SKINS_NAMES, std::string);	

	//��������� ������ �� �������
	struct		TeamStruct
	{
		string256			caSection;		// ��� ������ ��������
		TEAM_SKINS_NAMES	aSkins;			// ������ ������ ��� �������
		TEAM_WPN_LIST		aWeapons;		// ������ ������ ��� �������
		WPN_SLOT_NAMES		aDefaultItems;	// �������� �� ���������
	};

	//������ ������ �� ��������
	DEF_DEQUE(TEAM_DATA_LIST, TeamStruct);

	TEAM_DATA_LIST		TeamList;

protected:
	void							AllowDeadBodyRemove		(u32 id);
	void							SpawnActor				(u32 id, LPCSTR N);
	bool							GetPosAngleFromActor	(u32 id, Fvector& Pos, Fvector &Angle);
	void							SpawnItem4Actor			(u32 actorId, LPCSTR N);
	void							SpawnWeapon4Actor		(u32 actorId, LPCSTR N, u8 Addons = 0);
	void							KillPlayer				(u32 id_who);
public:
	virtual		void				Create					(LPSTR &options);

	// Events
	virtual		void				OnRoundStart			();										// ����� ������

	virtual		void				OnTeamScore				(u32 /**team/**/)						{};		// ������� ��������
	virtual		void				OnTeamsInDraw			()								{};		// �����
	virtual		void				OnPlayerHitPlayer		(u16 id_hitter, u16 id_hitted, NET_Packet& P); //����� ������� Hit

	virtual		BOOL				OnTouch					(u16 eid_who, u16 eid_what);
	virtual		BOOL				OnDetach				(u16 eid_who, u16 eid_what);

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

	virtual		void				SpawnWeaponsForActor	(CSE_Abstract* pE, game_PlayerState*	ps);
	virtual		const char * 		GetItemForSlot			(u8 SlotNum, u8 ItemID, game_PlayerState* ps);
	virtual		u8 					GetItemAddonsForSlot	(u8 SlotNum, u8 ItemID, game_PlayerState* ps);

	virtual		void				LoadTeams				();
	virtual		void				LoadTeamData			(char* caSection);
	virtual		void				LoadWeaponsForTeam		(char* caSection, TEAM_WPN_LIST *pTeamWpnList);
	virtual		void				LoadSkinsForTeam		(char* caSection, TEAM_SKINS_NAMES* pTeamSkins);
	virtual		void				LoadDefItemsForTeam		(char* caSection, WPN_SLOT_NAMES* pDefItems);

	virtual		void				SendPlayerKilledMessage	(u32 id_killer, u32 id_killed);
};
