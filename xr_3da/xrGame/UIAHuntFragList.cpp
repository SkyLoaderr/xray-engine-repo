#include "stdafx.h"
#include "UIAHuntFragList.h"
#include "hudmanager.h"
#include "game_cl_artefacthunt.h"

//--------------------------------------------------------------------
bool	CUIAHuntFragList::SetItemData		(u32 ItemID, CUIStatsListItem *pItem)
{
	if (ItemID>= items.size()) return false;

	game_PlayerState* PS = (game_PlayerState*)(items[ItemID]);//(game_cl_GameState::Player*)*mI;

	if (PS->testFlag(GAME_PLAYER_FLAG_LOCAL) ) SelectItem(ItemID);

	char Text[1024];
	pItem->FieldsVector[0]->SetText(PS->name);
	if (PS->testFlag(GAME_PLAYER_FLAG_VERY_VERY_DEAD))
		pItem->FieldsVector[1]->SetText("DEAD");
	else
	{
		game_cl_ArtefactHunt* pGameAHunt = smart_cast<game_cl_ArtefactHunt*>(&(Game()));
		if (pGameAHunt)
		{
			if (pGameAHunt->artefactBearerID == PS->GameID)
				pItem->FieldsVector[1]->SetText("@");
		}
		else
			pItem->FieldsVector[1]->SetText("");
	}
	sprintf(Text, "%d", PS->kills); pItem->FieldsVector[2]->SetText(Text);
	sprintf(Text, "%d", PS->deaths); pItem->FieldsVector[3]->SetText(Text);
	if (PS == Game().local_player)
		sprintf(Text, "%d/%d", PS->ping, Level().GetRealPing()); 
	else
		sprintf(Text, "%d/%d", PS->ping, PS->Rping); 
	pItem->FieldsVector[4]->SetText(Text);

	return true;
};