#include "stdafx.h"
#include "UIDMFragList.h"
#include "hudmanager.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUIDMFragList::CUIDMFragList()
{
}

CUIDMFragList::~CUIDMFragList()
{
}

void	CUIDMFragList::Show()
{
	inherited::Show();

	HUD().GetUI()->ShowIndicators();
	HUD().GetUI()->HideCursor();

	inherited::Enable(false);
};

//--------------------------------------------------------------------
CUIStatsListItem*		CUIDMFragList::GetItem			(int index)
{
	return (dynamic_cast<CUIStatsListItem*> (UIStatsList.GetItem(index)));
};

IC bool	pred_player		(LPVOID v1, LPVOID v2)
{
	return ((game_cl_GameState::Player*)v1)->kills>((game_cl_GameState::Player*)v2)->kills;
};

void	CUIDMFragList::Update()
{
	inherited::Update();

	xr_map<u32,game_cl_GameState::Player>::iterator I=Game().players.begin();
	xr_map<u32,game_cl_GameState::Player>::iterator E=Game().players.end();

	// create temporary map (sort by kills)
	items.clear			();
	for (;I!=E;++I)		items.push_back(&I->second);
	std::sort			(items.begin(),items.end(),pred_player);


	while (GetItemCount() < items.size())
	{
		AddItem();
	};

	char Text[1024];
	int ItemIDX = 0;
	HighlightItem(0xffffffff);
	for (ItemIt mI=items.begin(); items.end() != mI; ++mI)
	{
		game_cl_GameState::Player* P = (game_cl_GameState::Player*)*mI;
//		u32	color = 0;
//		if (P->flags&GAME_PLAYER_FLAG_LOCAL)	color = 0xf0a0ffa0; //H->SetColor(0xf0a0ffa0);
//		else									color = 0xf0a0a0ff; //H->SetColor(0xe0a0eea0);

		if (P->flags&GAME_PLAYER_FLAG_LOCAL)	HighlightItem(ItemIDX);
		CUIStatsListItem *pItem = GetItem(ItemIDX++);
		if (!pItem) continue;

		pItem->FieldsVector[0]->SetText(P->name);
		sprintf(Text, "%d", P->kills); pItem->FieldsVector[1]->SetText(Text);
		sprintf(Text, "%d", P->ping); pItem->FieldsVector[2]->SetText(Text);
	}
};
//--------------------------------------------------------------------
/*
void CUIDMFragList::Init()
{
	inherited::Init		("ui\\ui_hud_frame",0,0,500,400,alCenter);
}
//--------------------------------------------------------------------
IC bool	pred_player		(LPVOID v1, LPVOID v2)
{
	return ((game_cl_GameState::Player*)v1)->kills>((game_cl_GameState::Player*)v2)->kills;
}
void CUIDMFragList::OnFrame()
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
	// global info
	if (Game().fraglimit)	H->OutNext	("Frag Limit:  %3d",Game().fraglimit);
	else					H->OutNext	("Frag Limit:  unlimited");
	if (Game().timelimit)	H->OutNext	("Time remain: %3d (sec)",(Game().timelimit-(Level().timeServer()-Game().start_time))/1000);
	else					H->OutNext	("Time remain: unlimited");
	H->OutSkip			(1.5f);
	int k=1;
	for (ItemIt mI=items.begin(); items.end() != mI; ++mI){
		game_cl_GameState::Player* P = (game_cl_GameState::Player*)*mI;
		u32	color = 0;
		if (P->flags&GAME_PLAYER_FLAG_LOCAL)	color = 0xf0a0ffa0; //H->SetColor(0xf0a0ffa0);
		else									color = 0xf0a0a0ff; //H->SetColor(0xe0a0eea0);		
		H->SetColor(color);
		H->OutNext		("%3d: %-20s %-20d %-5d",k++,P->name,P->kills, P->ping);
	}
}
*/
//--------------------------------------------------------------------
