#pragma once

enum
{
	GAME_ANY							= 0,
	GAME_SINGLE							= 1,
	GAME_DEATHMATCH						= 2,
	GAME_CTF							= 3,
	GAME_ASSAULT						= 4,	// Team1 - assaulting, Team0 - Defending
	GAME_CS								= 5,
	GAME_TEAMDEATHMATCH					= 6,
	GAME_ARTEFACTHUNT					= 7,
	GAME_DUMMY							= 255	// temporary game type
};

enum
{
	GAME_PLAYER_FLAG_LOCAL				= (1<<0),
	GAME_PLAYER_FLAG_READY				= (1<<1),
	GAME_PLAYER_FLAG_VERY_VERY_DEAD		= (1<<2),
	GAME_PLAYER_FLAG_CS_ON_BASE			= (1<<3),
	GAME_PLAYER_FLAG_CS_ON_ENEMY_BASE	= (1<<4),
	GAME_PLAYER_FLAG_CS_HAS_ARTEFACT	= (1<<5),
	GAME_PLAYER_FLAG_CS_SPECTATOR		= (1<<6),

	GAME_PLAYER_FLAG_FORCEDWORD			= u32(-1)
};

enum
{
	GAME_PHASE_INPROGRESS		= 0,
	GAME_PHASE_PENDING,
	GAME_PHASE_TEAM1_SCORES,
	GAME_PHASE_TEAM2_SCORES,
	GAME_PHASE_TEAMS_IN_A_DRAW,

	GAME_PHASE_FORCEDWORD		= u32(-1)
};

#pragma pack(push,1)
struct	game_PlayerState
{
	s16			team;
	s16			kills;
	s16			deaths;
	s32			money_total;
	s16			money_for_round;
	u16			flags;

	u16			ping;

	u16			GameID;

	//------Dedicated-------------------
	bool		Skip;
	//---------------------------
	u16			m_lasthitter;
	u16			m_lasthitweapon;
	u8			m_skin;
	//---------------------------
	u32				m_RespawnTime;
	//---------------------------
	s16			money_delta;

	game_PlayerState();
	~game_PlayerState();

#ifndef AI_COMPILER
	void	net_Export		(NET_Packet& P);
	void	net_Import		(NET_Packet& P);
#endif

	//selected weapons & etc.
	u8			Slots[8];
	struct		BeltItem
	{
		u8		SlotID;
		u8		ItemID;

		BeltItem(u8 IDSlot, u8 IDItem):
			SlotID(IDSlot),
			ItemID(IDItem)
		{
		};
	};
	DEF_VECTOR	(BELT_ITEMS_LIST, BeltItem);

	BELT_ITEMS_LIST	BeltItems;
	//---------------------------------------
	
	struct PlayersItem
	{
		u16			ItemID;
		s16			ItemCost;
	};
	DEF_VECTOR(PLAYER_ITEMS_LIST, PlayersItem);

	PLAYER_ITEMS_LIST	pItemList;
};

struct	game_TeamState
{
	int			score;
	u16			num_targets;

	game_TeamState();
};
#pragma pack(pop)

class	game_GameState
{
public:
	s32								type;
	u16								phase;
	s32								round;
	u32								start_time;
	u32								buy_time;
	s32								fraglimit;
	s32								timelimit;
	u32								damgeblocklimit;
	xr_vector<game_TeamState>		teams;
	// for Artefact Hunt
	u8								m_ArtefactsNum;
	u16								m_ArtefactBearerID;
	u8								m_TeamInPosession;
public:
	game_GameState();

	virtual		void				Create					(LPSTR &options)	=	0;
};
