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
#include "../silencer.h"
#include "../scope.h"
#include "../grenadelauncher.h"
#include "../game_cl_base.h"
#include "../string_table.h"
#include "../actorcondition.h"
#include "../actor_defs.h"

using namespace InventoryUtilities;

#define CANT_SLEEP_ENEMIES "cant sleep near enemies"
#define CANT_SLEEP_GROUND "cant sleep not on solid ground"

void CUIInventoryWnd::ActivatePropertiesBox()
{
	int x,y;
	// Флаг-признак для невлючения пункта контекстного меню: Dreess Outfit, если костюм уже надет
	bool bAlreadyDressed = false; 

	RECT rect = GetAbsoluteRect();
	GetUICursor()->GetPos(x,y);
		
	UIPropertiesBox.RemoveAll();
	
	CEatableItem* pEatableItem = smart_cast<CEatableItem*>(m_pCurrentItem);
	CCustomOutfit* pOutfit = smart_cast<CCustomOutfit*>(m_pCurrentItem);
	CArtefactMerger* pArtefactMerger = smart_cast<CArtefactMerger*>(m_pCurrentItem);
	
	CWeapon* pWeapon = smart_cast<CWeapon*>(m_pCurrentItem);
	CScope* pScope = smart_cast<CScope*>(m_pCurrentItem);
	CSilencer* pSilencer = smart_cast<CSilencer*>(m_pCurrentItem);
	CGrenadeLauncher* pGrenadeLauncher = smart_cast<CGrenadeLauncher*>(m_pCurrentItem);
	

	if(m_pCurrentItem->GetSlot()<SLOTS_NUM && m_pInv->CanPutInSlot(m_pCurrentItem))
	{
		UIPropertiesBox.AddItem("Move to slot",  NULL, INVENTORY_TO_SLOT_ACTION);
	}
	if(m_pCurrentItem->Belt() && m_pInv->CanPutInBelt(m_pCurrentItem))
	{
		UIPropertiesBox.AddItem("Move on belt",  NULL, INVENTORY_TO_BELT_ACTION);
	}
	if(m_pCurrentItem->Ruck() && m_pInv->CanPutInRuck(m_pCurrentItem))
	{
		if(!pOutfit)
			UIPropertiesBox.AddItem("Move to bag",  NULL, INVENTORY_TO_BAG_ACTION);
		else
			UIPropertiesBox.AddItem("Undress outfit",  NULL, INVENTORY_TO_BAG_ACTION);
		bAlreadyDressed = true;
	}
	if(pOutfit  && !bAlreadyDressed /*&& m_pInv->CanPutInSlot(m_pCurrentItem)*/)
	{
		UIPropertiesBox.AddItem("Dress in outfit",  NULL, INVENTORY_TO_SLOT_ACTION);
	}
	
	//отсоединение аддонов от вещи
	if(pWeapon)
	{
		if(pWeapon->GrenadeLauncherAttachable() && pWeapon->IsGrenadeLauncherAttached())
		{
			UIPropertiesBox.AddItem("Detach grenade launcher",  NULL, INVENTORY_DETACH_GRENADE_LAUNCHER_ADDON);
		}
		if(pWeapon->ScopeAttachable() && pWeapon->IsScopeAttached())
		{
			UIPropertiesBox.AddItem("Detach scope",  NULL, INVENTORY_DETACH_SCOPE_ADDON);
		}
		if(pWeapon->SilencerAttachable() && pWeapon->IsSilencerAttached())
		{
			UIPropertiesBox.AddItem("Detach silencer",  NULL, INVENTORY_DETACH_SILENCER_ADDON);
		}
	}
	
	//присоединение аддонов к активному слоту (2 или 3)
	if(pScope)
	{
		if(m_pInv->m_slots[PISTOL_SLOT].m_pIItem != NULL &&
		   m_pInv->m_slots[PISTOL_SLOT].m_pIItem->CanAttach(pScope))
		 {
			 UIPropertiesBox.AddItem("Attach scope to pitol",  NULL, INVENTORY_ATTACH_ADDON);
			 m_pItemToUpgrade = m_pInv->m_slots[PISTOL_SLOT].m_pIItem;
		 }
		 if(m_pInv->m_slots[RIFLE_SLOT].m_pIItem != NULL &&
			m_pInv->m_slots[RIFLE_SLOT].m_pIItem->CanAttach(pScope))
		 {
			 UIPropertiesBox.AddItem("Attach scope to rifle",  NULL, INVENTORY_ATTACH_ADDON);
			 m_pItemToUpgrade = m_pInv->m_slots[RIFLE_SLOT].m_pIItem;
		 }
	}
	else if(pSilencer)
	{
		 if(m_pInv->m_slots[PISTOL_SLOT].m_pIItem != NULL &&
		   m_pInv->m_slots[PISTOL_SLOT].m_pIItem->CanAttach(pSilencer))
		 {
			 UIPropertiesBox.AddItem("Attach silencer to pitol",  NULL, INVENTORY_ATTACH_ADDON);
			 m_pItemToUpgrade = m_pInv->m_slots[PISTOL_SLOT].m_pIItem;
		 }
		 if(m_pInv->m_slots[RIFLE_SLOT].m_pIItem != NULL &&
			m_pInv->m_slots[RIFLE_SLOT].m_pIItem->CanAttach(pSilencer))
		 {
			 UIPropertiesBox.AddItem("Attach silencer to rifle",  NULL, INVENTORY_ATTACH_ADDON);
			 m_pItemToUpgrade = m_pInv->m_slots[RIFLE_SLOT].m_pIItem;
		 }
	}
	else if(pGrenadeLauncher)
	{
		 if(m_pInv->m_slots[RIFLE_SLOT].m_pIItem != NULL &&
			m_pInv->m_slots[RIFLE_SLOT].m_pIItem->CanAttach(pGrenadeLauncher))
		 {
			 UIPropertiesBox.AddItem("Attach grenade launcher to rifle",  NULL, INVENTORY_ATTACH_ADDON);
			 m_pItemToUpgrade = m_pInv->m_slots[RIFLE_SLOT].m_pIItem;
		 }

	}
	
	
	if(pEatableItem)
	{
		UIPropertiesBox.AddItem("Eat",  NULL, INVENTORY_EAT_ACTION);
	}

	if(pArtefactMerger)
	{
		if(!UIArtefactMergerWnd.IsShown())
		{
			UIPropertiesBox.AddItem("Activate Merger", NULL, 
									INVENTORY_ARTEFACT_MERGER_ACTIVATE);
			UIPropertiesBox.AddItem("Drop", NULL, INVENTORY_DROP_ACTION);
		}
		else
		{
			UIPropertiesBox.AddItem("Deactivate Merger", NULL, 
									INVENTORY_ARTEFACT_MERGER_DEACTIVATE);
		}
	}
	else
	{
		UIPropertiesBox.AddItem("Drop", NULL, INVENTORY_DROP_ACTION);
	}

	UIPropertiesBox.AutoUpdateSize();
	UIPropertiesBox.BringAllToTop();
	UIPropertiesBox.Show(x-rect.left, y-rect.top);
}

void CUIInventoryWnd::DropItem()
{
	CActor *pActor = smart_cast<CActor*>(Level().CurrentEntity());
	if(!pActor) return;

	//	if (smart_cast<CCustomOutfit*>(m_pCurrentItem))
	//		SendMessage(NULL, CUIOutfitSlot::UNDRESS_OUTFIT, NULL);
	if (m_pCurrentDragDropItem == UIOutfitSlot.GetDragDropItemsList().front())
		SendMessage(NULL, UNDRESS_OUTFIT, NULL);

//	m_pCurrentItem->Drop();
	m_pCurrentDragDropItem->Highlight(false);

	(smart_cast<CUIDragDropList*>(m_pCurrentDragDropItem->GetParent()))->
		DetachChild(m_pCurrentDragDropItem);

	DD_ITEMS_VECTOR_IT it = std::find(m_vDragDropItems.begin(), m_vDragDropItems.end(),m_pCurrentDragDropItem);
	VERIFY(it != m_vDragDropItems.end());
//	m_vDragDropItems.erase(it);

	//-----------------------------------------------------------------------
	SendEvent_ItemDrop(m_pCurrentItem);
	//-----------------------------------------------------------------------
	SetCurrentItem(NULL);
	m_pCurrentDragDropItem = NULL;

	UpdateWeight(UIBagWnd, true);
}

void CUIInventoryWnd::EatItem()
{
	CActor *pActor = smart_cast<CActor*>(Level().CurrentEntity());
	if(!pActor) return;

//	pActor->inventory().Eat(m_pCurrentItem);
	SendEvent_Item_Eat(m_pCurrentItem);

	if(!m_pCurrentItem->Useful())
	{
		(smart_cast<CUIDragDropList*>(m_pCurrentDragDropItem->GetParent()))->
			DetachChild(m_pCurrentDragDropItem);

		DD_ITEMS_VECTOR_IT it = std::find(m_vDragDropItems.begin(), m_vDragDropItems.end(),m_pCurrentDragDropItem);
		VERIFY(it != m_vDragDropItems.end());
//		m_vDragDropItems.erase(it);
		//-----------------------------------------------------------------------
		//SendEvent_ItemDrop(m_pCurrentItem);
		//-----------------------------------------------------------------------
		m_pCurrentDragDropItem->Highlight(false);
		SetCurrentItem(NULL);
		m_pCurrentDragDropItem = NULL;
	}
}

//------------------------------------------------
//как только подняли элемент, сделать его текущим
void CUIInventoryWnd::SendMessage(CUIWindow *pWnd, s16 msg, void *pData)
{
	if(msg == DRAG_DROP_ITEM_DRAG)
	{
		PIItem pInvItem = (PIItem)((CUIDragDropItem*)pWnd)->GetData();
				
		SetCurrentItem(pInvItem);
		// Гасим предыдущий активный элемент
		if (m_pCurrentDragDropItem) m_pCurrentDragDropItem->Highlight(false);
		m_pCurrentDragDropItem = (CUIDragDropItem*)pWnd;
		// Cкейлим и увеличиваем текстуру
		m_pCurrentDragDropItem->Rescale(1.0f);
	}
	else if(msg == DRAG_DROP_ITEM_DB_CLICK)
	{
		PIItem pInvItem = (PIItem)((CUIDragDropItem*)pWnd)->GetData();
		
		SetCurrentItem(pInvItem);
		m_pCurrentDragDropItem = (CUIDragDropItem*)pWnd;

		// "Поднять" вещь для освобождения занимаемого места
		SendMessage(m_pCurrentDragDropItem, DRAG_DROP_ITEM_DRAG, NULL);

		//попытаться закинуть элемент в слот, рюкзак или на пояс
		if(!ToSlot())
            if(!ToBelt())
                if(!ToBag())
                //если нельзя, то просто упорядочить элемент в своем списке
				{
					((CUIDragDropList*)m_pCurrentDragDropItem->GetParent())->
										DetachChild(m_pCurrentDragDropItem);
					((CUIDragDropList*)m_pCurrentDragDropItem->GetParent())->
										AttachChild(m_pCurrentDragDropItem);
				}
		m_pCurrentDragDropItem->Rescale(((CUIDragDropList*)m_pCurrentDragDropItem->GetParent())->GetItemsScale());
    }
	//по нажатию правой кнопки
	else if(msg == DRAG_DROP_ITEM_RBUTTON_CLICK)
	{
		PIItem pInvItem = (PIItem)((CUIDragDropItem*)pWnd)->GetData();
				
		SetCurrentItem(pInvItem);
		m_pCurrentDragDropItem = (CUIDragDropItem*)pWnd;
		
		ActivatePropertiesBox();
	}
	//сообщение от меню вызываемого правой кнопкой
	else if(pWnd == &UIPropertiesBox &&
			msg == PROPERTY_CLICKED)
	{
		
		if(UIPropertiesBox.GetClickedItem())
		{
			switch(UIPropertiesBox.GetClickedItem()->GetValue())
			{
			case INVENTORY_TO_SLOT_ACTION:	
				ToSlot();
				break;
			case INVENTORY_TO_BELT_ACTION:	
				ToBelt();
				break;
			case INVENTORY_TO_BAG_ACTION:	
				ToBag();
				break;
			case INVENTORY_DROP_ACTION:	//выкинуть объект
				DropItem();
				break;
			case INVENTORY_EAT_ACTION:	//съесть объект
				EatItem();
				break;
			case INVENTORY_ARTEFACT_MERGER_ACTIVATE:
				StartArtefactMerger();
				break;
			case INVENTORY_ARTEFACT_MERGER_DEACTIVATE:
				StopArtefactMerger();
				break;
			case INVENTORY_ATTACH_ADDON:
				AttachAddon();
				break;
			case INVENTORY_DETACH_SCOPE_ADDON:
				DetachAddon(*(smart_cast<CWeapon*>(m_pCurrentItem))->GetScopeName());
				break;
			case INVENTORY_DETACH_SILENCER_ADDON:
				DetachAddon(*(smart_cast<CWeapon*>(m_pCurrentItem))->GetSilencerName());
				break;
			case INVENTORY_DETACH_GRENADE_LAUNCHER_ADDON:
				DetachAddon(*(smart_cast<CWeapon*>(m_pCurrentItem))->GetGrenadeLauncherName());
				break;
			}
		}
	}
	//сообщения от ArtifactMerger
	else if(pWnd == &UIArtefactMergerWnd && msg == ARTEFACT_MERGER_PERFORM_BUTTON_CLICKED)
	{
	}
	else if(pWnd == &UIArtefactMergerWnd && msg == ARTEFACT_MERGER_CLOSE_BUTTON_CLICKED)
	{
		StopArtefactMerger();
	}
	else if(pWnd == &UISleepWnd && msg == SLEEP_WND_PERFORM_BUTTON_CLICKED)
	{
		CActor *pActor = smart_cast<CActor*>(Level().CurrentEntity());
		if(!pActor) return;
		
//		CUIGameSP* pGameSP = smart_cast<CUIGameSP*>(HUD().GetUI()->UIGame());
//		if(!pGameSP) return;
		if(GameID() != GAME_SINGLE)
			return;

		EActorSleep result = pActor->conditions().GoSleep(*reinterpret_cast<u32*>(pData));
		LPCSTR sleep_msg = NULL;
		switch(result)
		{
		case easEnemies:
			sleep_msg = *CStringTable()(CANT_SLEEP_ENEMIES);
			break;
		case easNotSolidGround:
			sleep_msg = *CStringTable()(CANT_SLEEP_GROUND);
			break;
		}

		if(sleep_msg)
			HUD().GetUI()->UIMainIngameWnd.AddInfoMessage(sleep_msg);

		Game().StartStopMenu(this,true);
	}
	else if (UNDRESS_OUTFIT == msg)
	{
		UndressOutfit();
	}
	else if (&UIDropButton == pWnd && BUTTON_CLICKED == msg)
	{
		if (m_pCurrentDragDropItem && m_pCurrentItem) DropItem();
	}
	else if (DRAG_DROP_REFRESH_ACTIVE_ITEM == msg)
	{
		if (m_pCurrentDragDropItem) m_pCurrentDragDropItem->Highlight(true);
	}
	else if (&UIExitButton == pWnd && BUTTON_CLICKED == msg)
	{
		Game().StartStopMenu(this,true);
	}

	CUIWindow::SendMessage(pWnd, msg, pData);
}

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

			UIDragDropItem.SetFont(HUD().Font().pFontLetterica16Russian);

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

		UIDragDropItem.SetFont(HUD().Font().pFontLetterica16Russian);

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

			UIDragDropItem.SetFont(HUD().Font().pFontLetterica16Russian);

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

			UIDragDropItem.SetFont(HUD().Font().pFontLetterica16Russian);

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

