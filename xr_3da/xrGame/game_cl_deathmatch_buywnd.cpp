#include "stdafx.h"
#include "game_cl_deathmatch.h"
#include "level.h"
#include "ui/UIBuyWeaponWnd.h"
#include "actor.h"
#include "inventory.h"
#include "xrServer_Objects_ALife_Items.h"

static	u16 SlotsToCheck [] = {
		APPARATUS_SLOT	,		// 4
		OUTFIT_SLOT		,		// 5
		KNIFE_SLOT		,		// 0
		GRENADE_SLOT	,		// 3
		PISTOL_SLOT		,		// 1
		RIFLE_SLOT		,		// 2
};

s16	game_cl_Deathmatch::GetBuyMenuItemIndex		(u8 SlotID, u8 ItemID)
{
	R_ASSERT2(SlotID != 0xff && ItemID != 0xff, "Bad Buy Manu Item");
	if (SlotID == OUTFIT_SLOT) SlotID = APPARATUS_SLOT;
	s16	ID = (s16(SlotID) << 0x08) | s16(ItemID);
	return ID;
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
		s16 ID = GetBuyMenuItemIndex(u8(SlotID), ItemID);
		pCurPresetItems->push_back(ID);
	}

	for (u8 i=0; i<pCurBuyMenu->GetBeltSize(); i++)
	{
		u8 SectID, ItemID;
		pCurBuyMenu->GetWeaponIndexInBelt(i, SectID, ItemID);
//		s16	ID = (s16(SectID) << 0x08) | s16(ItemID);
		s16 ID = GetBuyMenuItemIndex(SectID, ItemID);
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
		//��������� �������� ������� ���� � ������

		/*
		TIItemSet::const_iterator	I = pCurActor->inventory().m_all.begin();
		TIItemSet::const_iterator	E = pCurActor->inventory().m_all.end();
		
		for ( ; I != E; ++I) 
		{
			CheckItem((*I), &TmpPresetItems);
		};
		*/

		//��������� ����
		TIItemList::const_iterator	IBelt = pCurActor->inventory().m_belt.begin();
		TIItemList::const_iterator	EBelt = pCurActor->inventory().m_belt.end();

		for ( ; IBelt != EBelt; ++IBelt) 
		{
			CheckItem((*IBelt), &TmpPresetItems);
		};

		//��������� �����
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
	if (!pItem || pItem->object().getDestroy() || pItem->m_drop) return;

	u8 SlotID, ItemID;
	pCurBuyMenu->GetWeaponIndexByName(*pItem->object().cNameSect(), SlotID, ItemID);
	if (SlotID == 0xff || ItemID == 0xff) return;
	//-----------------------------------------------------
	pCurBuyMenu->SectionToSlot(SlotID, ItemID, true);
	//-----------------------------------------------------
//	s16 BigID = (s16(SlotID) << 0x08) | s16(ItemID);
	s16 BigID = GetBuyMenuItemIndex(SlotID, ItemID);
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

void	game_cl_Deathmatch::LoadTeamDefaultPresetItems	(LPCSTR caSection, CUIBuyWeaponWnd* BuyMenu, PRESET_ITEMS* pPresetItems)
{
	if (!pSettings->line_exist(caSection, "default_items")) return;

	pPresetItems->clear();

	string256			ItemName;
	string4096			DefItems;
	// ������ ������ ����� ����
	std::strcpy(DefItems, pSettings->r_string(caSection, "default_items"));
	u32 count	= _GetItemCount(DefItems);
	// ������ ��� ������ ��� ������, ����������� ��������, ������� � ������
	for (u32 i = 0; i < count; ++i)
	{
		_GetItem(DefItems, i, ItemName);

		u8 SlotID, ItemID;
		BuyMenu->GetWeaponIndexByName(ItemName, SlotID, ItemID);
		if (SlotID == 0xff || ItemID == 0xff) continue;
		s16 ID = GetBuyMenuItemIndex(SlotID, ItemID);
		pPresetItems->push_back(ID);
	};
};