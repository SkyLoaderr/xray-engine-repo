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
	SetHeaderColumnText(0, "Name");
	SetHeaderColumnText(1, "Frags");
	SetHeaderColumnText(2, "Deaths");
	SetHeaderColumnText(3, "Ping");

	Show();
}

CUIDMFragList::~CUIDMFragList()
{
}

//--------------------------------------------------------------------
CUIStatsListItem*		CUIDMFragList::GetItem			(int index)
{
	return (smart_cast<CUIStatsListItem*> (UIStatsList.GetItem(index)));
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

};

//--------------------------------------------------------------------
