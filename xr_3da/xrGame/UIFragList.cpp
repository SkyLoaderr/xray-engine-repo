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
	u32 align		= alLeft|alTop;	
	frame.Init		("ui\\ui_hud_frame",150,150,400,300,64,64,align);
	list_rect.set	(frame.rect);
	list_rect.shrink(40,40);
	Level().HUD()->ClientToScreen(list_rect, align);
}
//--------------------------------------------------------------------

void CUIFragList::Render()
{
	frame.Render	();
}
//--------------------------------------------------------------------

void CUIFragList::OnFrame()
{
	map<u32,game_cl_GameState::Player>::iterator I=Game().players.begin();
	map<u32,game_cl_GameState::Player>::iterator E=Game().players.end();

	// create temporary map (sort by kills)
	map<u32,game_cl_GameState::Player> temp;
	for (;I!=E;I++)		temp.insert(make_pair(I->second.kills,I->second));

	// out info
	I					= temp.begin();
	E					= temp.end();
	CGameFont* H		= Level().HUD()->pHUDFont;
	H->OutSet			(float(list_rect.lt.x),float(list_rect.lt.y));
	int k=1;
	for (;I!=E;I++){
		if (I->second.flags&GAME_PLAYER_FLAG_LOCAL)	H->Color(0xf0a0ffa0);
		else										H->Color(0xb0a0a0a0);
		H->OutNext		("%3d: %-20s %-5d",k++,I->second.name,I->first);
	}
}
//--------------------------------------------------------------------
