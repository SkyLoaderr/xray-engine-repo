#include "stdafx.h"
#include "UIDMPlayerList.h"
#include "hudmanager.h"
#include "game_cl_base.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CUIDMPlayerList::CUIDMPlayerList()
{
	SetHeaderColumnText(0, "Name");
	SetHeaderColumnText(1, "Frags");
	SetHeaderColumnText(2, "Status");
	SetHeaderColumnText(3, "");
}

CUIDMPlayerList::~CUIDMPlayerList()
{
}

bool	CUIDMPlayerList::SetItemData		(u32 ItemID, CUIStatsListItem *pItem)
{
	if (ItemID>= items.size()) return false;

	game_PlayerState* P = (game_PlayerState*)(items[ItemID]);//(game_cl_GameState::Player*)*mI;

	if (P->testFlag(GAME_PLAYER_FLAG_LOCAL)) SelectItem(ItemID);

	char Text[1024];
	pItem->FieldsVector[0]->SetText(P->name);
	sprintf(Text, "%d", P->kills); pItem->FieldsVector[1]->SetText(Text);
	if (P->testFlag(GAME_PLAYER_FLAG_READY) )
		pItem->FieldsVector[2]->SetText("READY");
	else
		pItem->FieldsVector[2]->SetText("");
	return true;
};
/*
void	CUIDMPlayerList::Update()
{
	CUIStatsWnd::Update();

	UpdateItemsList();

	char Text[1024];
	int ItemIDX = 0;
	for (ItemIt mI=items.begin(); items.end() != mI; ++mI)
	{
		game_cl_GameState::Player* P = (game_cl_GameState::Player*)*mI;
		//		u32	color = 0;
		//		if (P->flags&GAME_PLAYER_FLAG_LOCAL)	color = 0xf0a0ffa0; //H->SetColor(0xf0a0ffa0);
		//		else									color = 0xf0a0a0ff; //H->SetColor(0xe0a0eea0);

		if (P->flags&GAME_PLAYER_FLAG_LOCAL)	SelectItem(ItemIDX);
		CUIStatsListItem *pItem = GetItem(ItemIDX++);
		if (!pItem) continue;

		pItem->FieldsVector[0]->SetText(P->name);
		sprintf(Text, "%d", P->kills); pItem->FieldsVector[1]->SetText(Text);
//		sprintf(Text, "%d", P->ping); pItem->FieldsVector[2]->SetText(Text);
		if (P->flags&GAME_PLAYER_FLAG_READY)
			pItem->FieldsVector[2]->SetText("READY");
		else
			pItem->FieldsVector[2]->SetText("");
	}
};
*/
/*
void	CUIDMPlayerList::Show()
{
	CUIStatsWnd::Show();

//	HUD().GetUI()->ShowIndicators();
	HUD().GetUI()->HideCursor();

	CUIStatsWnd::Enable(false);
};
*/
/*
CUIDMPlayerList::CUIDMPlayerList()
{
}
//--------------------------------------------------------------------

void CUIDMPlayerList::Init()
{
	inherited::Init		("ui\\ui_hud_frame",0,0,500,400,alCenter);
}
//--------------------------------------------------------------------
IC bool	pred_player		(LPVOID v1, LPVOID v2)
{
	return ((game_cl_GameState::Player*)v1)->kills>((game_cl_GameState::Player*)v2)->kills;
}
void CUIDMPlayerList::OnFrame()
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
	for (ItemIt mI=items.begin(); items.end() != mI; ++mI){
		game_cl_GameState::Player* P = (game_cl_GameState::Player*)*mI;
		if (P->flags&GAME_PLAYER_FLAG_LOCAL)	H->SetColor(0xf0a0ffa0);
		else									H->SetColor(0xb0a0a0a0);
		H->OutNext		("%3d: %-20s %-5d %s",k++,P->name,P->kills,(P->flags&GAME_PLAYER_FLAG_READY)?"ready":"");
	}
}
//--------------------------------------------------------------------
*/