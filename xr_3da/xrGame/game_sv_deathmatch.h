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

	// массив в котором хратнятся названия секций для оружия 
	DEF_VECTOR(WPN_SECT_NAMES, std::string);
	// Вектор массивов с именами секций для оружия
	DEF_VECTOR(WPN_LISTS, WPN_SECT_NAMES);
	// Вектор массивов оборудования для комманд
	DEF_VECTOR(TEAM_LISTS, WPN_LISTS);

	TEAM_LISTS	wpnTeamsSectStorage;

	// Вектор имен скинов комманды
	DEF_VECTOR(SKINS_NAMES, std::string);
	// Структура скинов

	struct		SkinsStruct
	{
		string256		caSection;
		SKINS_NAMES		Skins;
	};

	// Вектор скинов комманд
	DEF_DEQUE(SKINS_LISTS, SkinsStruct);

	SKINS_LISTS	SkinsTeamSectStorage;

	

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
	virtual		void				OnRoundStart			();										// старт раунда

	virtual		void				OnTeamScore				(u32 /**team/**/)						{};		// команда выиграла
	virtual		void				OnTeamsInDraw			()								{};		// ничья
	virtual		void				OnPlayerHitPlayer		(u16 id_hitter, u16 id_hitted, NET_Packet& P); //игрок получил Hit

	virtual		BOOL				OnTouch					(u16 eid_who, u16 eid_what);
	virtual		BOOL				OnDetach				(u16 eid_who, u16 eid_what);

	virtual		void				OnPlayerConnect			(u32 id_who);
	virtual		void				OnPlayerDisconnect		(u32 id_who);
	virtual		void				OnPlayerReady			(u32 id_who);
	virtual		void				OnPlayerKillPlayer		(u32 id_killer, u32 id_killed);
	virtual		void				OnPlayerWantsDie		(u32 id_who);

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
	virtual		void				SpawnWeaponsForActor	(CSE_Abstract* pE, game_PlayerState*	ps);
	virtual		const char * 		GetItemForSlot			(u8 SlotNum, u8 ItemID, game_PlayerState* ps);
	virtual		u8 					GetItemAddonsForSlot	(u8 SlotNum, u8 ItemID, game_PlayerState* ps);

	virtual		void				LoadWeaponsForTeam		(WPN_LISTS *pTeamList, char* caSection);
	virtual		void				LoadSkinsForTeam		(SkinsStruct *pTeamList, char* caSection);

	virtual		void				LoadWeapons				();
	virtual		void				LoadSkins				();

	virtual		void				SendPlayerKilledMessage	(u32 id_killer, u32 id_killed);
};
