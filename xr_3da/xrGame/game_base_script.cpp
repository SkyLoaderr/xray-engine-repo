#include "stdafx.h"
#include "game_base.h"
#include "script_space.h"

using namespace luabind;

void game_PlayerState::script_register(lua_State *L)
{

	module(L)
		[
			luabind::class_<game_PlayerState>("game_PlayerState")
			.def(	constructor<>())
			.def_readwrite("team",				&game_PlayerState::team)
			.def_readwrite("kills",				&game_PlayerState::kills)
			.def_readwrite("deaths",			&game_PlayerState::deaths)
			.def_readwrite("money_total",		&game_PlayerState::money_total)
			.def_readwrite("money_for_round",	&game_PlayerState::money_for_round)
			.def_readwrite("flags",				&game_PlayerState::flags)
			.def_readwrite("ping",				&game_PlayerState::ping)
			.def_readwrite("GameID",			&game_PlayerState::GameID)
			.def_readwrite("Skip",				&game_PlayerState::Skip)
			.def_readwrite("lasthitter",		&game_PlayerState::lasthitter)
			.def_readwrite("lasthitweapon",		&game_PlayerState::lasthitweapon)
			.def_readwrite("skin",				&game_PlayerState::skin)
			.def_readwrite("RespawnTime",		&game_PlayerState::RespawnTime)
			.def_readwrite("money_delta",		&game_PlayerState::money_delta)

			.def_readwrite("pItemList",			&game_PlayerState::pItemList)
			.def_readwrite("LastBuyAcount",		&game_PlayerState::LastBuyAcount)
			
		];
}

void game_TeamState::script_register(lua_State *L)
{

	module(L)
		[
			luabind::class_<game_TeamState>("game_TeamState")
			.def(	constructor<>())
			.def_readwrite("score",				&game_TeamState::score)
			.def_readwrite("num_targets",		&game_TeamState::num_targets)
		];
}

void game_GameState::script_register(lua_State *L)
{

	module(L)
		[
			luabind::class_<DLL_Pure>("dll_pure"),

			luabind::class_< game_GameState, DLL_Pure >("game_GameState")
			.def(	constructor<>())

			.def_readwrite("type",				&game_GameState::type)
			.def_readwrite("phase",				&game_GameState::phase)
			.def_readwrite("round",				&game_GameState::round)
			.def_readwrite("start_time",		&game_GameState::start_time)
//			.def_readwrite("buy_time",			&game_GameState::buy_time)
//			.def_readwrite("fraglimit",			&game_GameState::fraglimit)
//			.def_readwrite("timelimit",			&game_GameState::timelimit)
//			.def_readwrite("damageblocklimit",	&game_GameState::damageblocklimit)
//			.def_readwrite("teams",				&game_GameState::teams)
//			.def_readwrite("artefactsNum",		&game_GameState::artefactsNum)
//			.def_readwrite("artefactBearerID",	&game_GameState::artefactBearerID)
//			.def_readwrite("teamInPosession",	&game_GameState::teamInPossession)

		];

}
