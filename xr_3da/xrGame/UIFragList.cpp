#include "stdafx.h"
#include "uiFragList.h"
#include "hudmanager.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUIFragList::CUIFragList()
{
}
//--------------------------------------------------------------------

void CUIFragList::Init()
{
	inherited::Init		("ui\\ui_hud_frame",150,150,400,300,alLeft|alTop);
}
//--------------------------------------------------------------------

void CUIFragList::OnFrame()
{
	inherited::OnFrame();
	map<u32,game_cl_GameState::Player>::iterator I=Game().players.begin();
	map<u32,game_cl_GameState::Player>::iterator E=Game().players.end();

	// create temporary map (sort by kills)
	multimap<u32,game_cl_GameState::Player> temp;
	for (;I!=E;I++)		temp.insert(make_pair(I->second.kills,I->second));

	multimap<u32,game_cl_GameState::Player>::iterator mI=Game().players.begin();
	multimap<u32,game_cl_GameState::Player>::iterator mE=Game().players.end();
	// out info
	mI					= temp.begin();
	mE					= temp.end();
	CGameFont* H		= Level().HUD()->pHUDFont;
	H->OutSet			(float(list_rect.lt.x),float(list_rect.lt.y));
	int k=1;
	for (;mI!=mE;mI++){
		if (mI->second.flags&GAME_PLAYER_FLAG_LOCAL)	H->Color(0xf0a0ffa0);
		else											H->Color(0xb0a0a0a0);
		H->OutNext		("%3d: %-20s %-5d",k++,mI->second.name,mI->second.kills);
	}
}
//--------------------------------------------------------------------
