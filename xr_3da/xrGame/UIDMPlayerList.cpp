#include "stdafx.h"
#include "UIDMPlayerList.h"
#include "hudmanager.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUIDMPlayerList::CUIDMPlayerList()
{
}
//--------------------------------------------------------------------

void CUIDMPlayerList::Init()
{
	inherited::Init		("ui\\ui_hud_frame",150,150,400,300,alLeft|alTop);
}
//--------------------------------------------------------------------

void CUIDMPlayerList::OnFrame()
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
		H->OutNext		("%3d: %-20s %-5d %s",k++,I->second.name,I->first,(I->second.flags&GAME_PLAYER_FLAG_READY)?"ready":"");
	}
}
//--------------------------------------------------------------------
