#include "stdafx.h"
#include "UITDMFragList.h"
#include "hudmanager.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUITDMFragList::CUITDMFragList()
{
	m_CurTeam = 0;
}
//--------------------------------------------------------------------

void CUITDMFragList::Init(u8 Team, u32 X, u32 Y, u32 Width, u32 Height, EUIItemAlign Align)
{
	inherited::Init		("ui\\ui_hud_frame",X, Y, Width, Height, Align);

	m_CurTeam = Team;

	switch (m_CurTeam)
	{
	case 1:
		{
			m_CurPlayerColor	= 0xfffff0f0;
			m_PlayerColor		= 0xffff0000;
		}break;
	case 2:
		{
			m_CurPlayerColor	= 0xfff0f0ff;
			m_PlayerColor		= 0xff0000ff;
		}break;
	}
}
//--------------------------------------------------------------------
IC bool	pred_player		(LPVOID v1, LPVOID v2)
{
	return ((game_cl_GameState::Player*)v1)->kills>((game_cl_GameState::Player*)v2)->kills;
}
void CUITDMFragList::OnFrame()
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

	H->SetColor(0xffffffff);

	// global info
	if (Game().fraglimit)	H->OutNext	("Frag Limit:  %3d",Game().fraglimit);
	else					H->OutNext	("Frag Limit:  unlimited");
	if (Game().timelimit)	H->OutNext	("Time remain: %3d (sec)",(Game().timelimit-(Level().timeServer()-Game().start_time))/1000);
	else					H->OutNext	("Time remain: unlimited");	

	H->OutNext		("");
	H->SetColor(m_PlayerColor);
	H->OutNext		("   %-40s %-8s %-10s","Name","Kills", "Ping");
	H->OutNext		("-------------------------------------------------------------");

	if (OnServer())
		H->OutNext		("   %-40s %-8d","Team Score",Level().Server->game->teams[m_CurTeam-1].score);
	else
		H->OutNext		("   %-40s %-8d","Team Score",Game().teams[m_CurTeam-1].score);
	H->OutNext		("-------------------------------------------------------------");


	H->OutSkip			(1.5f);
	int k=1;
	for (ItemIt mI=items.begin(); items.end() != mI; ++mI){
		game_cl_GameState::Player* P = (game_cl_GameState::Player*)*mI;
		if (P->team != m_CurTeam) continue;
		u32	color = 0;
		if (P->flags&GAME_PLAYER_FLAG_LOCAL)	color = m_CurPlayerColor;//H->SetColor(0xf0a0ffa0);
		else									color = m_PlayerColor; //H->SetColor(0xe0a0eea0);		
		H->SetColor(color);
		H->OutNext		("%3d: %-40s %-5d %-5d",k++,P->name,P->kills, P->ping);
	}
}
//--------------------------------------------------------------------
