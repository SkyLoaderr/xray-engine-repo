#pragma once

#include "game_base_space.h"
#include "script_export_space.h"

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
	u16			lasthitter;
	u16			lasthitweapon;
	u8			skin;
	//---------------------------
	u32			RespawnTime;
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
	DECLARE_SCRIPT_REGISTER_FUNCTION_STRUCT
};
add_to_type_list(game_PlayerState)
#undef script_type_list
#define script_type_list save_type_list(game_PlayerState)


struct	game_TeamState
{
	int			score;
	u16			num_targets;

	game_TeamState();
	DECLARE_SCRIPT_REGISTER_FUNCTION_STRUCT
};
add_to_type_list(game_TeamState)
#undef script_type_list
#define script_type_list save_type_list(game_TeamState)

#pragma pack(pop)

class	game_GameState : public DLL_Pure
{
public:
	s32								type;
	u16								phase;
	s32								round;
	u32								start_time;
	u32								buy_time;
	s32								fraglimit;
	s32								timelimit;
	u32								damageblocklimit;
	xr_vector<game_TeamState>		teams;
	// for Artefact Hunt
	u8								artefactsNum;
	u16								artefactBearerID;
	u8								teamInPossession;

public:
	game_GameState();

	virtual		void				Create					(LPSTR &options){};
	virtual		LPCSTR				type_name				() const {return "base game";};
//for scripting enhancement
	static		CLASS_ID			getCLASS_ID(LPCSTR game_type_name, bool bServer);

	DECLARE_SCRIPT_REGISTER_FUNCTION
};
add_to_type_list(game_GameState)
#undef script_type_list
#define script_type_list save_type_list(game_GameState)
