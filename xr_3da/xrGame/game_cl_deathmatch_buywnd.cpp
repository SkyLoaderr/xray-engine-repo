#include "stdafx.h"
#include "game_cl_deathmatch.h"
#include "level.h"
#include "ui/UIBuyWeaponWnd.h"
#include "actor.h"
#include "inventory.h"
#include "xrServer_Objects_ALife_Items.h"

static	u16 SlotsToCheck [] = {
		KNIFE_SLOT		,			// 0
		PISTOL_SLOT		,		// 1
		RIFLE_SLOT		,		// 2
		GRENADE_SLOT	,		// 3
		APPARATUS_SLOT	,		// 4
		OUTFIT_SLOT		,		// 5
};


void game_cl_Deathmatch::OnBuyMenu_Ok	()
{
	if (!m_bBuyEnabled) return;
	CObject *l_pObj = Level().CurrentEntity();

	CGameObject *l_pPlayer = smart_cast<CGameObject*>(l_pObj);
	if(!l_pPlayer) return;

	game_PlayerState* Pl = local_player;
	if (!Pl) return;

	NET_Packet		P;
	l_pPlayer->u_EventGen		(P,GE_GAME_EVENT,l_pPlayer->ID()	);
	P.w_u16(GAME_EVENT_PLAYER_BUY_FINISHED);
	//-------------------------------------------------------------------------------
	pCurPresetItems->clear();

	for (u8 s =0; s<6; s++)
	{
		u8 ItemID = pCurBuyMenu->GetWeaponIndex(SlotsToCheck[s]);
		if (ItemID == 0xff) continue;
		u16 SlotID = SlotsToCheck[s];
		if (SlotID == OUTFIT_SLOT) SlotID = APPARATUS_SLOT;
		s16	ID = (s16(SlotID) << 0x08) | s16(ItemID);
		pCurPresetItems->push_back(ID);
	}

	for (u8 i=0; i<pCurBuyMenu->GetBeltSize(); i++)
	{
		u8 SectID, ItemID;
		pCurBuyMenu->GetWeaponIndexInBelt(i, SectID, ItemID);
		s16	ID = (s16(SectID) << 0x08) | s16(ItemID);
		pCurPresetItems->push_back(ID);
	};	
	//-------------------------------------------------------------------------------
	P.w_s16		(s16(pCurBuyMenu->GetMoneyAmount()) - Pl->money_for_round);
	P.w_u8		(u8(pCurPresetItems->size()));
	for (s=0; s<pCurPresetItems->size(); s++)
	{
		P.w_s16((*pCurPresetItems)[s]);
	}
	//-------------------------------------------------------------------------------
	l_pPlayer->u_EventSend		(P);
};

void game_cl_Deathmatch::SetBuyMenuItems		()
{
	game_PlayerState* P = local_player;
	if (!P) return;
	//---------------------------------------------------------
	xr_vector <s16>			TmpPresetItems;
	PRESET_ITEMS_it		It = pCurPresetItems->begin();
	PRESET_ITEMS_it		Et = pCurPresetItems->end();
	for ( ; It != Et; ++It) 
	{
		TmpPresetItems.push_back(*It);
	};
	//---------------------------------------------------------
	ClearBuyMenu			();
	//---------------------------------------------------------
	pCurBuyMenu->IgnoreMoney(true);
	//---------------------------------------------------------
	CActor* pCurActor = smart_cast<CActor*> (Level().Objects.net_Find	(P->GameID));
	if (pCurActor)
	{
		//проверяем предметы которые есть у игрока

		/*
		TIItemSet::const_iterator	I = pCurActor->inventory().m_all.begin();
		TIItemSet::const_iterator	E = pCurActor->inventory().m_all.end();
		
		for ( ; I != E; ++I) 
		{
			CheckItem((*I), &TmpPresetItems);
		};
		*/

		//проверяем пояс
		TIItemList::const_iterator	IBelt = pCurActor->inventory().m_belt.begin();
		TIItemList::const_iterator	EBelt = pCurActor->inventory().m_belt.end();

		for ( ; IBelt != EBelt; ++IBelt) 
		{
			CheckItem((*IBelt), &TmpPresetItems);
		};

		//проверяем слоты
		TISlotArr::const_iterator	ISlot = pCurActor->inventory().m_slots.begin();
		TISlotArr::const_iterator	ESlot = pCurActor->inventory().m_slots.end();

		for ( ; ISlot != ESlot; ++ISlot) 
		{
			CheckItem((*ISlot).m_pIItem, &TmpPresetItems);
		};
	};
	//---------------------------------------------------------
	It = TmpPresetItems.begin();
	Et = TmpPresetItems.end();
	for ( ; It != Et; ++It) 
	{
		s16	ItemID = (*It);
		
		pCurBuyMenu->SectionToSlot(u8((ItemID&0xff00)>>0x08), u8(ItemID&0x00ff), false);
	};
	//---------------------------------------------------------
	pCurBuyMenu->IgnoreMoney(false);

	pCurBuyMenu->SetMoneyAmount(P->money_for_round);
	pCurBuyMenu->CheckBuyAvailabilityInSlots();
};

void game_cl_Deathmatch::CheckItem			(PIItem pItem, PRESET_ITEMS* pPresetItems)
{
	if (!pItem || !pPresetItems) return;
	if (!pItem || pItem->getDestroy() || pItem->m_drop) return;

	u8 SlotID, ItemID;
	pCurBuyMenu->GetWeaponIndexByName(*pItem->cNameSect(), SlotID, ItemID);
	if (SlotID == 0xff || ItemID == 0xff) return;
	//-----------------------------------------------------
	pCurBuyMenu->SectionToSlot(SlotID, ItemID, true);
	//-----------------------------------------------------
	s16 BigID = (s16(SlotID) << 0x08) | s16(ItemID);
	s16 DesiredAddons = 0;
	PRESET_ITEMS_it It = pPresetItems->begin();
	PRESET_ITEMS_it Et = pPresetItems->end();
	for ( ; It != Et; ++It) 
	{
		if (BigID == ((*It)& 0xff1f))
		{
			DesiredAddons = ((*It)&0x00ff)>>5;
			pPresetItems->erase(It);
			break;
		}
	}
	//-----------------------------------------------------
	CWeapon* pWeapon = smart_cast<CWeapon*> (pItem);
	if (pWeapon)
	{
		if (pWeapon->ScopeAttachable())
		{
			pCurBuyMenu->GetWeaponIndexByName(*pWeapon->GetScopeName(), SlotID, ItemID);
			if (SlotID != 0xff && ItemID != 0xff)
			{
				if (pWeapon->IsScopeAttached())
					pCurBuyMenu->SectionToSlot(SlotID, ItemID, true);
				else
				{
					if (DesiredAddons & CSE_ALifeItemWeapon::eWeaponAddonScope)
						pCurBuyMenu->SectionToSlot(SlotID, ItemID, false);
				}
			}
		};

		if (pWeapon->GrenadeLauncherAttachable())
		{
			pCurBuyMenu->GetWeaponIndexByName(*pWeapon->GetGrenadeLauncherName(), SlotID, ItemID);
			if (SlotID != 0xff && ItemID != 0xff)
			{
				if (pWeapon->IsGrenadeLauncherAttached())
					pCurBuyMenu->SectionToSlot(SlotID, ItemID, true);
				else
				{
					if (DesiredAddons & CSE_ALifeItemWeapon::eWeaponAddonGrenadeLauncher)
						pCurBuyMenu->SectionToSlot(SlotID, ItemID, false);
				}
			}
		};

		if (pWeapon->SilencerAttachable())
		{
			pCurBuyMenu->GetWeaponIndexByName(*pWeapon->GetSilencerName(), SlotID, ItemID);
			if (SlotID != 0xff && ItemID != 0xff)
			{
				if (pWeapon->IsSilencerAttached())
					pCurBuyMenu->SectionToSlot(SlotID, ItemID, true);
				else
				{
					if (DesiredAddons & CSE_ALifeItemWeapon::eWeaponAddonSilencer)
						pCurBuyMenu->SectionToSlot(SlotID, ItemID, false);
				}
			}
		};
	};
};
