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
