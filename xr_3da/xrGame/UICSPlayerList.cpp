#include "stdafx.h"
#include "UICSPlayerList.h"
#include "hudmanager.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUICSPlayerList::CUICSPlayerList()
{
}
//--------------------------------------------------------------------

void CUICSPlayerList::Init()
{
	inherited::Init		("ui\\ui_hud_frame",0,0,500,400,alCenter);
}
//--------------------------------------------------------------------
IC bool	pred_player		(LPVOID v1, LPVOID v2)
{
	return ((game_cl_GameState::Player*)v1)->kills>((game_cl_GameState::Player*)v2)->kills;
}
void CUICSPlayerList::OnFrame()
{
	inherited::OnFrame();
	map<u32,game_cl_GameState::Player>::iterator I=Game().players.begin();
	map<u32,game_cl_GameState::Player>::iterator E=Game().players.end();

	// create temporary map (sort by kills)
	items.clear			();
	for (;I!=E;I++)		items.push_back(&I->second);
	sort(items.begin(),items.end(),pred_player);

	// out info
	CGameFont* H		= Level().HUD()->pFontMedium;
	H->OutSet			(float(list_rect.lt.x),float(list_rect.lt.y));
	H->SetColor(0xf0ffa0a0); H->OutNext("Team 1:");
	int k=1;
	for (ItemIt mI=items.begin(); mI!=items.end(); mI++){
		game_cl_GameState::Player* P = (game_cl_GameState::Player*)*mI;
		if(P->team != 0) continue;
		if (P->flags&GAME_PLAYER_FLAG_LOCAL)	H->SetColor(0xf0a0ffa0);
		else									H->SetColor(0xb0a0a0a0);
		H->OutNext		("%3d. %-20s %-5d %s",k++,P->name,P->kills,(P->flags&GAME_PLAYER_FLAG_READY)?"ready":"");
	}
	H->OutSkip(2.f);
	H->SetColor(0xf0a0a0ff); H->OutNext("Team 2:");
	k=1;
	for (ItemIt mI=items.begin(); mI!=items.end(); mI++){
		game_cl_GameState::Player* P = (game_cl_GameState::Player*)*mI;
		if(P->team != 1) continue;
		if (P->flags&GAME_PLAYER_FLAG_LOCAL)	H->SetColor(0xf0a0ffa0);
		else									H->SetColor(0xb0a0a0a0);
		H->OutNext		("%3d. %-20s %-5d %s",k++,P->name,P->kills,(P->flags&GAME_PLAYER_FLAG_READY)?"ready":"");
	}
	H->OutSkip(3.f);
}
//--------------------------------------------------------------------
