#include "stdafx.h"
#include "UIInventoryWnd.h"
#include "../level.h"
#include "../WeaponAmmo.h"
#include "../inventoryowner.h"
#include "../game_base_space.h"
#include "UIInventoryUtilities.h"
#include "UIDragDropList.h"
#include "../inventory.h"
#include "../hudmanager.h"
#include "../eatable_item.h"
#include "../CustomOutfit.h"

using namespace InventoryUtilities;

void CUIInventoryWnd::InitInventory() 
{
	CInventoryOwner *pInvOwner = smart_cast<CInventoryOwner*>(Level().CurrentEntity());

	if(!pInvOwner) return;


	CInventory* pInv = &pInvOwner->inventory();
	
	m_pMouseCapturer = NULL;

	SetCurrentItem(NULL);
	
	UIPropertiesBox.Hide();
	UIArtefactMergerWnd.Hide();

	m_pInv = pInv;

	//инициализировать информацию о персонаже
	UICharacterInfo.InitCharacter(pInvOwner);

	if (GAME_SINGLE != GameID())
	{
		UIOutfitSlot.SetMPOutfit();
	}
	
	//очистить после предыдущего запуска
	UITopList[0].DropAll();
	UITopList[1].DropAll();
	UITopList[2].DropAll();
	UITopList[3].DropAll();
	UITopList[4].DropAll();
	UIOutfitSlot.DropAll();
		
	UIBeltList.DropAll();
	UIBagList.DropAll();


	/*for(u32 i = 0; i <MAX_ITEMS; ++i) 
	{
		m_vDragDropItems[i].SetData(NULL);
		m_vDragDropItems[i].SetWndRect(0,0,0,0);
		m_vDragDropItems[i].SetCustomUpdate(NULL);
	}
	m_iUsedItems = 0;*/
	u32 i;
	ClearDragDrop(m_vDragDropItems);

	//Slots
	for( i = 0; i < SLOTS_NUM; ++i) 
	{
        if(pInv->m_slots[i].m_pIItem) 
		{
			m_vDragDropItems.push_back(xr_new<CUIWpnDragDropItem>());
			CUIDragDropItem& UIDragDropItem = *m_vDragDropItems.back();
			//CUIDragDropItem& UIDragDropItem = m_vDragDropItems[m_iUsedItems];
		
			UIDragDropItem.CUIStatic::Init(0, 0, INV_GRID_WIDTH, INV_GRID_HEIGHT);
			UIDragDropItem.SetShader(GetEquipmentIconsShader());

			UIDragDropItem.SetGridHeight(pInv->m_slots[i].m_pIItem->GetGridHeight());
			UIDragDropItem.SetGridWidth(pInv->m_slots[i].m_pIItem->GetGridWidth());

			UIDragDropItem.SetFont(HUD().pFontLetterica16Russian);

			UIDragDropItem.GetUIStaticItem().SetOriginalRect(
									pInv->m_slots[i].m_pIItem->GetXPos()*INV_GRID_WIDTH,
									pInv->m_slots[i].m_pIItem->GetYPos()*INV_GRID_HEIGHT,
									pInv->m_slots[i].m_pIItem->GetGridWidth()*INV_GRID_WIDTH,
									pInv->m_slots[i].m_pIItem->GetGridHeight()*INV_GRID_HEIGHT);

			UITopList[i].AttachChild(&UIDragDropItem);
			UIDragDropItem.Rescale(UITopList[i].GetItemsScale());
			UIDragDropItem.SetData(pInv->m_slots[i].m_pIItem);
			UIDragDropItem.Show(true);

			//++m_iUsedItems;
			//R_ASSERT(m_iUsedItems<MAX_ITEMS);
		}
		UITopList[i].HighlightAllCells(false);
	}

	//Слот с костюмом
	if(pInv->m_slots[OUTFIT_SLOT].m_pIItem) 
	{
		m_vDragDropItems.push_back(xr_new<CUIWpnDragDropItem>());
		CUIDragDropItem& UIDragDropItem = *m_vDragDropItems.back();
//		CUIDragDropItem& UIDragDropItem = m_vDragDropItems[m_iUsedItems];		

		UIDragDropItem.CUIStatic::Init(0, 0, INV_GRID_WIDTH, INV_GRID_HEIGHT);
		UIDragDropItem.SetShader(GetEquipmentIconsShader());

		UIDragDropItem.SetGridHeight(pInv->m_slots[OUTFIT_SLOT].m_pIItem->GetGridHeight());
		UIDragDropItem.SetGridWidth(pInv->m_slots[OUTFIT_SLOT].m_pIItem->GetGridWidth());

		UIDragDropItem.SetFont(HUD().pFontLetterica16Russian);

		UIDragDropItem.GetUIStaticItem().SetOriginalRect(
									pInv->m_slots[OUTFIT_SLOT].m_pIItem->GetXPos()*INV_GRID_WIDTH,
									pInv->m_slots[OUTFIT_SLOT].m_pIItem->GetYPos()*INV_GRID_HEIGHT,
									pInv->m_slots[OUTFIT_SLOT].m_pIItem->GetGridWidth()*INV_GRID_WIDTH,
									pInv->m_slots[OUTFIT_SLOT].m_pIItem->GetGridHeight()*INV_GRID_HEIGHT);


		UIDragDropItem.SetData(pInv->m_slots[OUTFIT_SLOT].m_pIItem);
		UIOutfitSlot.AttachChild(&UIDragDropItem);
		UIDragDropItem.Show(false);

//		++m_iUsedItems;
//		R_ASSERT(m_iUsedItems<MAX_ITEMS);
		UIOutfitSlot.HighlightAllCells(false);
	}

	//Пояс
	for(PPIItem it =  pInv->m_belt.begin(); pInv->m_belt.end() != it; ++it) 
	{
		if((*it)) 
		{
			m_vDragDropItems.push_back(xr_new<CUIWpnDragDropItem>());
			CUIDragDropItem& UIDragDropItem = *m_vDragDropItems.back();
//			CUIDragDropItem& UIDragDropItem = m_vDragDropItems[m_iUsedItems];
		
			UIDragDropItem.CUIStatic::Init(0, 0, INV_GRID_WIDTH, INV_GRID_HEIGHT);
			UIDragDropItem.SetShader(GetEquipmentIconsShader());

			UIDragDropItem.SetGridHeight((*it)->GetGridHeight());
			UIDragDropItem.SetGridWidth((*it)->GetGridWidth());

			UIDragDropItem.SetFont(HUD().pFontLetterica16Russian);

			UIDragDropItem.GetUIStaticItem().SetOriginalRect(
									(*it)->GetXPos()*INV_GRID_WIDTH,
									(*it)->GetYPos()*INV_GRID_HEIGHT,
									(*it)->GetGridWidth()*INV_GRID_WIDTH,
									(*it)->GetGridHeight()*INV_GRID_HEIGHT);


			CWeaponAmmo* pWeaponAmmo  = smart_cast<CWeaponAmmo*>((*it));

			// Не отображаем патроны в инвентаре если они посечены как "бесконечные"
			// Применимио только к режиму мультиплеера
			if(pWeaponAmmo)
			{
				UIDragDropItem.SetCustomUpdate(AmmoUpdateProc);
				if (GameID() != GAME_SINGLE && pWeaponAmmo->m_bCanBeUnlimited)	
					continue;
			}

			CEatableItem* pEatableItem = smart_cast<CEatableItem*>((*it));
			if(pEatableItem) UIDragDropItem.SetCustomUpdate(FoodUpdateProc);

			UIBeltList.AttachChild(&UIDragDropItem);
			UIDragDropItem.Rescale(UIBeltList.GetItemsScale());
			UIDragDropItem.SetData(*it);
			UIDragDropItem.Show(true);

			UIBeltList.HighlightAllCells(false);
//			++m_iUsedItems;
//			R_ASSERT(m_iUsedItems<MAX_ITEMS);
		}
	}


	ruck_list = pInv->m_ruck;
	ruck_list.sort(GreaterRoomInRuck);

	//Рюкзак
	for(it =  ruck_list.begin(); ruck_list.end() != it; ++it) 
	{
		if((*it)) 
		{
			m_vDragDropItems.push_back(xr_new<CUIWpnDragDropItem>());
			CUIDragDropItem& UIDragDropItem = *m_vDragDropItems.back();
			//CUIDragDropItem& UIDragDropItem = m_vDragDropItems[m_iUsedItems];

			UIDragDropItem.CUIStatic::Init(0, 0, INV_GRID_WIDTH, INV_GRID_HEIGHT);
			UIDragDropItem.SetShader(GetEquipmentIconsShader());

			UIDragDropItem.SetGridHeight((*it)->GetGridHeight());
			UIDragDropItem.SetGridWidth((*it)->GetGridWidth());

			UIDragDropItem.SetFont(HUD().pFontLetterica16Russian);

			UIDragDropItem.GetUIStaticItem().SetOriginalRect(
								(*it)->GetXPos()*INV_GRID_WIDTH,
								(*it)->GetYPos()*INV_GRID_HEIGHT,
								(*it)->GetGridWidth()*INV_GRID_WIDTH,
								(*it)->GetGridHeight()*INV_GRID_HEIGHT);
				
			CWeaponAmmo* pWeaponAmmo  = smart_cast<CWeaponAmmo*>((*it));

			// Не отображаем патроны в инвентаре если они посечены как "бесконечные"
			// Применимио только к режиму мультиплеера
			if(pWeaponAmmo)
			{
				UIDragDropItem.SetCustomUpdate(AmmoUpdateProc);
				if (GameID() != GAME_SINGLE && pWeaponAmmo->m_bCanBeUnlimited)
					continue;
			}

			CEatableItem* pEatableItem = smart_cast<CEatableItem*>((*it));
			if(pEatableItem) UIDragDropItem.SetCustomUpdate(FoodUpdateProc);

			UIBagList.AttachChild(&UIDragDropItem);
			UIDragDropItem.Rescale(UIBagList.GetItemsScale());
			UIDragDropItem.SetData((*it));
			UIDragDropItem.Show(true);
			UIBagList.HighlightAllCells(false);

		//	m_iUsedItems++;
		//	R_ASSERT(m_iUsedItems<MAX_ITEMS);

		}
	}
	UpdateWeight(UIBagWnd, true);
}  

bool CUIInventoryWnd::SlotProc0(CUIDragDropItem* pItem, CUIDragDropList* pList)
{
	CUIInventoryWnd* this_inventory = smart_cast<CUIInventoryWnd*>(pList->GetParent());
	R_ASSERT2(this_inventory, "wrong parent addressed as inventory wnd");

	PIItem pInvItem = (PIItem)pItem->GetData();

	if(!this_inventory->GetInventory()->CanPutInSlot(pInvItem)) return false;

	if( pInvItem->GetSlot() == 0)
	{
		bool	result = this_inventory->GetInventory()->Slot(pInvItem);
		if (result) SendEvent_Item2Slot(pInvItem);
		return result;
	}
	else
		return false;
}

bool CUIInventoryWnd::SlotProc1(CUIDragDropItem* pItem, CUIDragDropList* pList)
{
	CUIInventoryWnd* this_inventory = smart_cast<CUIInventoryWnd*>(pList->GetParent());
	R_ASSERT2(this_inventory, "wrong parent addressed as inventory wnd");

	PIItem pInvItem = (PIItem)pItem->GetData();

	if (!this_inventory->SlotToBag(pInvItem, pList, PISTOL_SLOT)) return false;

	if(!this_inventory->GetInventory()->CanPutInSlot(pInvItem)) return false;

	if(pInvItem->GetSlot() == PISTOL_SLOT)
	{
		bool	result = this_inventory->GetInventory()->Slot(pInvItem);
		if (result) SendEvent_Item2Slot(pInvItem);
		return result;
	}
	else
		return false;
}

bool CUIInventoryWnd::SlotProc2(CUIDragDropItem* pItem, CUIDragDropList* pList)
{
	CUIInventoryWnd* this_inventory = smart_cast<CUIInventoryWnd*>(pList->GetParent());
	R_ASSERT2(this_inventory, "wrong parent addressed as inventory wnd");

	PIItem pInvItem = (PIItem)pItem->GetData();

	if (!this_inventory->SlotToBag(pInvItem, pList, RIFLE_SLOT)) return false;

	if (!this_inventory->GetInventory()->CanPutInSlot(pInvItem)) return false;

	if( pInvItem->GetSlot() == RIFLE_SLOT)
	{
		bool	result = this_inventory->GetInventory()->Slot(pInvItem);
		if (result) SendEvent_Item2Slot(pInvItem);
		return result;
	}
	else
		return false;
}

bool CUIInventoryWnd::SlotProc3(CUIDragDropItem* pItem, CUIDragDropList* pList)
{
	CUIInventoryWnd* this_inventory = smart_cast<CUIInventoryWnd*>(pList->GetParent());
	R_ASSERT2(this_inventory, "wrong parent addressed as inventory wnd");

	PIItem pInvItem = (PIItem)pItem->GetData();

	if (!this_inventory->SlotToBag(pInvItem, pList, GRENADE_SLOT)) return false;

	if(!this_inventory->GetInventory()->CanPutInSlot(pInvItem)) return false;

	if( pInvItem->GetSlot() == GRENADE_SLOT)
	{
		bool	result = this_inventory->GetInventory()->Slot(pInvItem);
		if (result) SendEvent_Item2Slot(pInvItem);
		return result;
	}
	else
		return false;
}

bool CUIInventoryWnd::SlotProc4(CUIDragDropItem* pItem, CUIDragDropList* pList)
{
	CUIInventoryWnd* this_inventory = smart_cast<CUIInventoryWnd*>(pList->GetParent());
	R_ASSERT2(this_inventory, "wrong parent addressed as inventory wnd");


	PIItem pInvItem = (PIItem)pItem->GetData();

	if(!this_inventory->GetInventory()->CanPutInSlot(pInvItem)) return false;

	if(pInvItem->GetSlot() == 4)
	{
		bool	result = this_inventory->GetInventory()->Slot(pInvItem);
		if (result) SendEvent_Item2Slot(pInvItem);
		return result;
	}
	else
		return false;
}

//при вызове проверки необходимо помнить 
//иерархию окон, чтоб знать какой именно из
//родителей является CUIInventoryWnd и 
//содержит свойство GetInventory()

//проверка на помещение инвентаря в слоты
//одеть костюм
bool CUIInventoryWnd::OutfitSlotProc(CUIDragDropItem* pItem, CUIDragDropList* pList)
{
	CUIInventoryWnd* this_inventory = smart_cast<CUIInventoryWnd*>(pList->GetParent());
	R_ASSERT2(this_inventory, "wrong parent addressed as inventory wnd");

	PIItem pInvItem = (PIItem)pItem->GetData();

	// Cнимаем текущий костюм.
	CUIInventoryWnd *pInvWnd = smart_cast<CUIInventoryWnd*>(pList->GetParent());
	// Нет костюма, или парент у листа не CUIInventoryWnd, чего быть не может.
	if (!pInvWnd) return false;

	// Проверка возможности надевания нового костюма
	
	if (smart_cast<CCustomOutfit*>(pInvItem))
		pInvWnd->SendMessage(NULL, UNDRESS_OUTFIT, NULL);

	if(!this_inventory->GetInventory()->CanPutInSlot(pInvItem)) return false;

	if(pInvItem->GetSlot() == OUTFIT_SLOT)
	{
		bool	result = this_inventory->GetInventory()->Slot(pInvItem);
		if (result) SendEvent_Item2Slot(pInvItem);
		return result;
	}
	else
		return false;

}

//в рюкзак
bool CUIInventoryWnd::BagProc(CUIDragDropItem* pItem, CUIDragDropList* pList)
{
	CUIInventoryWnd* this_inventory = smart_cast<CUIInventoryWnd*>(pList->GetParent()->GetParent());
	R_ASSERT2(this_inventory, "wrong parent addressed as inventory wnd");


	//если это артефакт из устройства то положить без всяких проверок
	if(pItem->GetParent() == &this_inventory->UIArtefactMergerWnd.UIArtefactList)
		return true;


	PIItem pInvItem = (PIItem)pItem->GetData();

	if(!this_inventory->GetInventory()->CanPutInRuck(pInvItem)) return false;
	bool	result = this_inventory->GetInventory()->Ruck(pInvItem);

	if (result)		SendEvent_Item2Ruck(pInvItem);

	return  result;
}

//на пояс
bool CUIInventoryWnd::BeltProc(CUIDragDropItem* pItem, CUIDragDropList* pList)
{
	CUIInventoryWnd* this_inventory = smart_cast<CUIInventoryWnd*>(pList->GetParent());
	R_ASSERT2(this_inventory, "wrong parent addressed as inventory wnd");


	PIItem pInvItem = (PIItem)pItem->GetData();

	if(!this_inventory->GetInventory()->CanPutInBelt(pInvItem)) return false;
	
	bool	result = this_inventory->GetInventory()->Belt(pInvItem);
	if (result) SendEvent_Item2Belt(pInvItem);
	return result;
}

