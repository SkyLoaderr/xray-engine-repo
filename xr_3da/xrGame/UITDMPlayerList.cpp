#include "stdafx.h"
#include "UITDMPlayerList.h"
#include "hudmanager.h"
#include "game_cl_base.h"
#include "level.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------

void CUITDMPlayerList::Init(u8 Team)
{
	m_CurTeam = Team;
}
//--------------------------------------------------------------------
IC bool	pred_player		(LPVOID v1, LPVOID v2)
{
	return ((game_cl_GameState::Player*)v1)->kills>((game_cl_GameState::Player*)v2)->kills;
}

void	CUITDMPlayerList::UpdateItemsList ()
{
	xr_map<u32,game_cl_GameState::Player>::iterator I=Game().players.begin();
	xr_map<u32,game_cl_GameState::Player>::iterator E=Game().players.end();

	// create temporary map (sort by kills)
	items.clear			();
	for (;I!=E;++I)		
	{
		game_cl_GameState::Player* P = (game_cl_GameState::Player*)(&I->second);
		if (!P || P->team != m_CurTeam) continue;

		items.push_back(&I->second);
	};
	std::sort			(items.begin(),items.end(),pred_player);
};
//--------------------------------------------------------------------
