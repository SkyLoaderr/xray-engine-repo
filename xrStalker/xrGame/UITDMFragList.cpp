#include "stdafx.h"
#include "UITDMFragList.h"
#include "hudmanager.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------

void CUITDMFragList::Init(u8 Team)
{
	m_CurTeam = Team;	
}
//--------------------------------------------------------------------
IC bool	pred_player		(LPVOID v1, LPVOID v2)
{
	return ((game_cl_GameState::Player*)v1)->kills>((game_cl_GameState::Player*)v2)->kills;
};

void	CUITDMFragList::UpdateItemsList ()
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
/*
void	CUITDMFragList::Update()
{
	inherited::Update();

	CUIStatsListItem *pItem = GetItem(GetItemCount()-1);
	if (!pItem) return;

	pItem->FieldsVector[0]->SetText("TOTAL");

	char Text[1024];
	sprintf(Text, "%d", Game().teams[m_CurTeam-1].score); pItem->FieldsVector[1]->SetText(Text);
};
*/

//--------------------------------------------------------------------
