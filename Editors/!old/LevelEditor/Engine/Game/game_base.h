#pragma once

#include "game_base_space.h"

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
	u32			m_RespawnTime;
	//---------------------------
	s16			money_delta;

	game_PlayerState();
	~game_PlayerState();

#ifndef AI_COMPILER
	void	net_Export		(NET_Packet& P);
	void	net_Import		(NET_Packet& P);
#endif
	//---------------------------------------
//	struct PlayersItem
//	{
//		u16			ItemID;
//		s16			ItemCost;
//	};
	DEF_VECTOR(PLAYER_ITEMS_LIST, u16);

	PLAYER_ITEMS_LIST	pItemList;

	s16					LastBuyAcount;
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
