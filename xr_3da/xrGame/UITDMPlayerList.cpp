#include "stdafx.h"
#include "UITDMPlayerList.h"
#include "hudmanager.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUITDMPlayerList::CUITDMPlayerList()
{
	m_u8Team = 0;
}
//--------------------------------------------------------------------

void CUITDMPlayerList::Init(u8 Team)
{
	inherited::Init		("ui\\ui_hud_frame",(Team-1)*320,0,300,400,alCenter);

	m_u8Team = Team;
}
//--------------------------------------------------------------------
IC bool	pred_player		(LPVOID v1, LPVOID v2)
{
	return ((game_cl_GameState::Player*)v1)->kills>((game_cl_GameState::Player*)v2)->kills;
}
void CUITDMPlayerList::OnFrame()
{
	inherited::OnFrame();
	xr_map<u32,game_cl_GameState::Player>::iterator I=Game().players.begin();
	xr_map<u32,game_cl_GameState::Player>::iterator E=Game().players.end();

	// create temporary map (sort by kills)
	items.clear			();
	for (;I!=E;++I)		items.push_back(&I->second);
	std::sort			(items.begin(),items.end(),pred_player);

	// out info
	CGameFont* H		= HUD().pFontSmall;
	H->OutSet			(float(list_rect.lt.x),float(list_rect.lt.y));
	int k=1;
	for (ItemIt mI=items.begin(); items.end() != mI; ++mI)
	{
		game_cl_GameState::Player* P = (game_cl_GameState::Player*)*mI;
		if (P->team != m_u8Team) continue;
		if (P->flags&GAME_PLAYER_FLAG_LOCAL)	H->SetColor(0xf0a0ffa0);
		else									H->SetColor(0xb0a0a0a0);
		H->OutNext		("%3d: %-20s %-5d %s",k++,P->name,P->kills,(P->flags&GAME_PLAYER_FLAG_READY)?"ready":"");
	}
}
//--------------------------------------------------------------------
