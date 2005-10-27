#include "StdAfx.h"
#include "UIStatsPlayerList.h"
#include "../game_cl_base.h"

//#include "UIDMFragList.h"
//#include "hudmanager.h"
//#include "game_cl_base.h"
#include "../level.h"
//#include "game_cl_deathmatch.h"

CUIStatsPlayerList::CUIStatsPlayerList(){

}

CUIStatsPlayerList::~CUIStatsPlayerList(){

}

void CUIStatsPlayerList::Update(){

	game_cl_GameState::PLAYERS_MAP_IT I=Game().players.begin();
	game_cl_GameState::PLAYERS_MAP_IT E=Game().players.end();

//	u32 n = Game().players.size();



	// create temporary map (sort by kills)
//	items.clear			();
//	for (;I!=E;++I)		
//	{
//		game_PlayerState* PS = (game_PlayerState*) I->second;
//		if (Game().Phase() == GAME_PHASE_INPROGRESS)
//		{
//			if (PS->testFlag(GAME_PLAYER_FLAG_SPECTATOR)) continue;
//		}		
//		items.push_back(I->second);
//	};
//	std::sort			(items.begin(),items.end(),DM_Compare_Players);

}