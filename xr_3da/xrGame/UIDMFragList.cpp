#include "stdafx.h"
#include "UIDMFragList.h"
#include "hudmanager.h"
#include "game_cl_base.h"
#include "level.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUIDMFragList::CUIDMFragList()
{
//	for (int i=0; i<32; i++)
//		AddItem();
	SetHeaderColumnText(0, "Name");
	SetHeaderColumnText(1, "Frags");
	SetHeaderColumnText(2, "Deaths");
	SetHeaderColumnText(3, "Ping");
}

CUIDMFragList::~CUIDMFragList()
{
}
/*
void	CUIDMFragList::Show()
{
	inherited::Show();

	HUD().GetUI()->ShowIndicators();
	HUD().GetUI()->HideCursor();

	inherited::Enable(false);
};
*/

//--------------------------------------------------------------------
CUIStatsListItem*		CUIDMFragList::GetItem			(int index)
{
	return (dynamic_cast<CUIStatsListItem*> (UIStatsList.GetItem(index)));
};

IC bool	pred_player		(LPVOID v1, LPVOID v2)
{
	return ((game_PlayerState*)v1)->kills>((game_PlayerState*)v2)->kills;
};

void	CUIDMFragList::UpdateItemsList ()
{
	game_cl_GameState::PLAYERS_MAP_IT I=Game().players.begin();
	game_cl_GameState::PLAYERS_MAP_IT E=Game().players.end();

	// create temporary map (sort by kills)
	items.clear			();
	for (;I!=E;++I)		items.push_back(I->second);
	std::sort			(items.begin(),items.end(),pred_player);
}

bool	CUIDMFragList::SetItemData		(u32 ItemID, CUIStatsListItem *pItem)
{
	if (ItemID>= items.size()) return false;

	game_PlayerState* P = (game_PlayerState*)(items[ItemID]);//(game_cl_GameState::Player*)*mI;

	if (P->testFlag(GAME_PLAYER_FLAG_LOCAL) ) SelectItem(ItemID);

	char Text[1024];
	pItem->FieldsVector[0]->SetText(P->name);
	sprintf(Text, "%d", P->kills); pItem->FieldsVector[1]->SetText(Text);
	sprintf(Text, "%d", P->deaths); pItem->FieldsVector[2]->SetText(Text);
	sprintf(Text, "%d", P->ping); pItem->FieldsVector[3]->SetText(Text);
//	sprintf(Text, "%d", P->ping); pItem->FieldsVector[2]->SetText(Text);

	return true;
};

void	CUIDMFragList::Update()
{
	inherited::Update();

	UpdateItemsList();

	HighlightItem(0xffffffff);
	SelectItem(0xffffffff);
	//---------------------------------------
	while (items.size() < GetItemCount())
	{
		RemoveItem(0);
	};
	//---------------------------------------
	while (items.size() > GetItemCount())
	{
		AddItem();
	};
	//---------------------------------------

	for (u32 i=0; i<GetItemCount(); i++)
	{
		CUIStatsListItem *pItem = GetItem(i);
		if (!pItem) continue;
		if (SetItemData(i, pItem)) continue;

		pItem->FieldsVector[0]->SetText(NULL);
		pItem->FieldsVector[1]->SetText(NULL);
		pItem->FieldsVector[2]->SetText(NULL);
	};
	/*
	for (ItemIt mI=items.begin(); items.end() != mI; ++mI)
	{
		game_cl_GameState::Player* P = (game_cl_GameState::Player*)*mI;

		if (P->flags&GAME_PLAYER_FLAG_LOCAL) SelectItem(ItemIDX);

		CUIStatsListItem *pItem = GetItem(ItemIDX++);
		if (!pItem) continue;

		pItem->FieldsVector[0]->SetText(P->name);
		sprintf(Text, "%d", P->kills); pItem->FieldsVector[1]->SetText(Text);
		sprintf(Text, "%d", P->ping); pItem->FieldsVector[2]->SetText(Text);
	}
	*/
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
