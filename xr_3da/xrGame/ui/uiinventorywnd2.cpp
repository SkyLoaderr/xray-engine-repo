#include "stdafx.h"
#include "UIInventoryWnd.h"
#include "../level.h"
#include "../WeaponAmmo.h"
#include "../WeaponMagazined.h"
#include "../inventoryowner.h"
#include "../game_base_space.h"
#include "../game_cl_mp.h"
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
#include "../actor.h"
#include "../Artifact.h"
#include "../xr_level_controller.h"
#include "UISleepWnd.h"

using namespace InventoryUtilities;


void CUIInventoryWnd::ActivatePropertiesBox()
{
	float x,y;
	// Флаг-признак для невлючения пункта контекстного меню: Dreess Outfit, если костюм уже надет
	bool bAlreadyDressed = false; 

	Frect rect = GetAbsoluteRect();
	GetUICursor()->GetPos(x,y);
		
	UIPropertiesBox.RemoveAll();
	
	CEatableItem* pEatableItem			= smart_cast<CEatableItem*>(m_pCurrentItem);
	CCustomOutfit* pOutfit				= smart_cast<CCustomOutfit*>(m_pCurrentItem);
//	CArtefactMerger* pArtefactMerger	= smart_cast<CArtefactMerger*>(m_pCurrentItem);
	CArtefact* pArtefact				= smart_cast<CArtefact*>(m_pCurrentItem);
	CWeapon* pWeapon					= smart_cast<CWeapon*>(m_pCurrentItem);
	CScope* pScope						= smart_cast<CScope*>(m_pCurrentItem);
	CSilencer* pSilencer				= smart_cast<CSilencer*>(m_pCurrentItem);
	CGrenadeLauncher* pGrenadeLauncher	= smart_cast<CGrenadeLauncher*>(m_pCurrentItem);
    

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
		if(smart_cast<CWeaponMagazined*>(pWeapon) && GameID() == GAME_SINGLE)
		{
/*			if(pWeapon->GetAmmoElapsed()!=pWeapon->GetAmmoMagSize())
				if(	m_pInv->GetAny(*pWeapon->m_ammoTypes[pWeapon->m_ammoType] ) )
					UIPropertiesBox.AddItem("Reload magazine",  NULL, INVENTORY_RELOAD_MAGAZINE);
				else{
					for(u32 i = 0; i < pWeapon->m_ammoTypes.size(); ++i) 
						if( m_pInv->GetAny(*pWeapon->m_ammoTypes[i]) ){
							UIPropertiesBox.AddItem("Reload magazine",  NULL, INVENTORY_RELOAD_MAGAZINE);
							break;						
						}
				
				}
*/
			if(	pWeapon->GetAmmoElapsed() )
					UIPropertiesBox.AddItem("Unload magazine",  NULL, INVENTORY_UNLOAD_MAGAZINE);

		}


	}
	
	//присоединение аддонов к активному слоту (2 или 3)
	if(pScope)
	{
		if(m_pInv->m_slots[PISTOL_SLOT].m_pIItem != NULL &&
		   m_pInv->m_slots[PISTOL_SLOT].m_pIItem->CanAttach(pScope))
		 {
			PIItem tgt = m_pInv->m_slots[PISTOL_SLOT].m_pIItem;
			 UIPropertiesBox.AddItem("Attach scope to pistol",  (void*)tgt, INVENTORY_ATTACH_ADDON);
//			 m_pItemToUpgrade = m_pInv->m_slots[PISTOL_SLOT].m_pIItem;
		 }
		 if(m_pInv->m_slots[RIFLE_SLOT].m_pIItem != NULL &&
			m_pInv->m_slots[RIFLE_SLOT].m_pIItem->CanAttach(pScope))
		 {
			PIItem tgt = m_pInv->m_slots[RIFLE_SLOT].m_pIItem;
			 UIPropertiesBox.AddItem("Attach scope to rifle",  (void*)tgt, INVENTORY_ATTACH_ADDON);
//			 m_pItemToUpgrade = m_pInv->m_slots[RIFLE_SLOT].m_pIItem;
		 }
	}
	else if(pSilencer)
	{
		 if(m_pInv->m_slots[PISTOL_SLOT].m_pIItem != NULL &&
		   m_pInv->m_slots[PISTOL_SLOT].m_pIItem->CanAttach(pSilencer))
		 {
			PIItem tgt = m_pInv->m_slots[PISTOL_SLOT].m_pIItem;
			 UIPropertiesBox.AddItem("Attach silencer to pistol",  (void*)tgt, INVENTORY_ATTACH_ADDON);
			 m_pItemToUpgrade = m_pInv->m_slots[PISTOL_SLOT].m_pIItem;
		 }
		 if(m_pInv->m_slots[RIFLE_SLOT].m_pIItem != NULL &&
			m_pInv->m_slots[RIFLE_SLOT].m_pIItem->CanAttach(pSilencer))
		 {
			PIItem tgt = m_pInv->m_slots[RIFLE_SLOT].m_pIItem;
			 UIPropertiesBox.AddItem("Attach silencer to rifle",  (void*)tgt, INVENTORY_ATTACH_ADDON);
//			 m_pItemToUpgrade = m_pInv->m_slots[RIFLE_SLOT].m_pIItem;
		 }
	}
	else if(pGrenadeLauncher)
	{
		 if(m_pInv->m_slots[RIFLE_SLOT].m_pIItem != NULL &&
			m_pInv->m_slots[RIFLE_SLOT].m_pIItem->CanAttach(pGrenadeLauncher))
		 {
			PIItem tgt = m_pInv->m_slots[RIFLE_SLOT].m_pIItem;
			 UIPropertiesBox.AddItem("Attach grenade launcher to rifle",  (void*)tgt, INVENTORY_ATTACH_ADDON);
//			 m_pItemToUpgrade = m_pInv->m_slots[RIFLE_SLOT].m_pIItem;
		 }

	}
	
	
	if(pEatableItem)
	{
		UIPropertiesBox.AddItem("Eat",  NULL, INVENTORY_EAT_ACTION);
	}

	if(pArtefact&&pArtefact->CanBeActivated()&&!GetInventory()->isSlotsBlocked())
		UIPropertiesBox.AddItem("Activate artefact",  NULL, INVENTORY_ACTIVATE_ARTEFACT_ACTION);

	if(!m_pCurrentItem->IsQuestItem())
		UIPropertiesBox.AddItem("Drop", NULL, INVENTORY_DROP_ACTION);

	if (GameID() == GAME_ARTEFACTHUNT && Game().local_player && 
		Game().local_player->testFlag(GAME_PLAYER_FLAG_ONBASE) && 
		!Game().local_player->testFlag(GAME_PLAYER_FLAG_VERY_VERY_DEAD)
		)
	{
		UIPropertiesBox.AddItem("Sell Item",  NULL, INVENTORY_SELL_ITEM);	
	}

	UIPropertiesBox.AutoUpdateSize();
	UIPropertiesBox.BringAllToTop();
	UIPropertiesBox.Show(x-rect.left, y-rect.top);
	PlaySnd				(eInvProperties);
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
	SendEvent_Item_Drop(m_pCurrentItem);
	//-----------------------------------------------------------------------
	SetCurrentItem(NULL);
	m_pCurrentDragDropItem = NULL;

	UpdateWeight		(UIBagWnd, true);
}

void	CUIInventoryWnd::Activate_Artefact()
{
	CActor *pActor = smart_cast<CActor*>(Level().CurrentEntity());
	if(!pActor) return;

	SendEvent_ActivateArtefact(m_pCurrentItem);
};

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
		m_pCurrentDragDropItem->Rescale(1.0f,1.0f);
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
					CUIWindow* w = m_pCurrentDragDropItem->GetParent();
					w->DetachChild(m_pCurrentDragDropItem);
					w->AttachChild(m_pCurrentDragDropItem);
				}
		m_pCurrentDragDropItem->Rescale(((CUIDragDropList*)m_pCurrentDragDropItem->GetParent())->GetItemsScaleX(),
										((CUIDragDropList*)m_pCurrentDragDropItem->GetParent())->GetItemsScaleY());
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
			case INVENTORY_SELL_ITEM:
				SellItem();
				break;

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
			case INVENTORY_ATTACH_ADDON:{
					m_pItemToUpgrade = (PIItem)pData;
					AttachAddon();
				}break;
			case INVENTORY_DETACH_SCOPE_ADDON:
				DetachAddon(*(smart_cast<CWeapon*>(m_pCurrentItem))->GetScopeName());
				break;
			case INVENTORY_DETACH_SILENCER_ADDON:
				DetachAddon(*(smart_cast<CWeapon*>(m_pCurrentItem))->GetSilencerName());
				break;
			case INVENTORY_DETACH_GRENADE_LAUNCHER_ADDON:
				DetachAddon(*(smart_cast<CWeapon*>(m_pCurrentItem))->GetGrenadeLauncherName());
				break;
			case INVENTORY_ACTIVATE_ARTEFACT_ACTION:
				Activate_Artefact();
//				(smart_cast<CArtefact*>(m_pCurrentItem))->ActivateArtefact();
				break;
			case INVENTORY_RELOAD_MAGAZINE:
				(smart_cast<CWeapon*>(m_pCurrentItem))->Action(kWPN_RELOAD, CMD_START);
				break;
			case INVENTORY_UNLOAD_MAGAZINE:
				(smart_cast<CWeaponMagazined*>(m_pCurrentItem))->UnloadMagazine();
				break;
			}
		}
	}
	else if(pWnd == UISleepWnd && msg == SLEEP_WND_PERFORM_BUTTON_CLICKED)
	{
		CActor *pActor = smart_cast<CActor*>(Level().CurrentEntity());
		if(!pActor) return;
		
		if(GameID() != GAME_SINGLE)
			return;

		ACTOR_DEFS::EActorSleep result = pActor->conditions().GoSleep(*reinterpret_cast<u32*>(pData));
		LPCSTR sleep_msg = NULL;
		sleep_msg = *CStringTable().translate(result);
/*		switch(result)
		{
		case easEnemies:
			sleep_msg = *CStringTable()(CANT_SLEEP_ENEMIES);
			break;
		case easNotSolidGround:
			sleep_msg = *CStringTable()(CANT_SLEEP_GROUND);
			break;
		}
*/
		if(sleep_msg)
			HUD().GetUI()->AddInfoMessage(sleep_msg);

		Game().StartStopMenu(this,true);
	}
	else if (UNDRESS_OUTFIT == msg)
	{
		UndressOutfit();
	}
	else if (&UIDropButton == pWnd && BUTTON_CLICKED == msg)
	{
		if (m_pCurrentDragDropItem && m_pCurrentItem && !m_pCurrentItem->IsQuestItem()) DropItem();
	}
	else if (DRAG_DROP_REFRESH_ACTIVE_ITEM == msg)
	{
		if (m_pCurrentDragDropItem) m_pCurrentDragDropItem->Highlight(true);
	}
	else if (&UIExitButton == pWnd && BUTTON_CLICKED == msg)
	{
		Game().StartStopMenu(this,true);
	}

	if (UISellAll == pWnd && BUTTON_CLICKED == msg)
	{
		game_cl_mp* pGame_MP = smart_cast<game_cl_mp*> (Level().game);
		if (pGame_MP) pGame_MP->OnSellItemsFromRuck();
	}

	CUIWindow::SendMessage(pWnd, msg, pData);
}

void CUIInventoryWnd::SellItem(){
	CActor *pActor = smart_cast<CActor*>(Level().CurrentEntity());
	if(!pActor) return;

	if (m_pCurrentDragDropItem == UIOutfitSlot.GetDragDropItemsList().front())
		SendMessage(NULL, UNDRESS_OUTFIT, NULL);

	m_pCurrentDragDropItem->Highlight(false);

	(smart_cast<CUIDragDropList*>(m_pCurrentDragDropItem->GetParent()))->
		DetachChild(m_pCurrentDragDropItem);

	DD_ITEMS_VECTOR_IT it = std::find(m_vDragDropItems.begin(), m_vDragDropItems.end(),m_pCurrentDragDropItem);
	VERIFY(it != m_vDragDropItems.end());

	//-----------------------------------------------------------------------
#pragma todo("SATAN -> MAD_MAX: i'm waiting for you (: ")
	// change to sell item
	SendEvent_Item_Sell(m_pCurrentItem);
	//-----------------------------------------------------------------------
	SetCurrentItem(NULL);
	m_pCurrentDragDropItem = NULL;

	UpdateWeight		(UIBagWnd, true);
}

void CUIInventoryWnd::InitInventory_delayed()
{
	m_b_need_reinit = true;
}

void CUIInventoryWnd::InitInventory() 
{
	CInventoryOwner *pInvOwner = smart_cast<CInventoryOwner*>(Level().CurrentEntity());

	if(!pInvOwner) return;


	CInventory* pInv = &pInvOwner->inventory();
	
	m_pMouseCapturer = NULL;

	SetCurrentItem(NULL);
	
	UIPropertiesBox.Hide();
//	UIArtefactMergerWnd.Hide();

	m_pInv = pInv;

	//инициализировать информацию о персонаже
	//UICharacterInfo.InitCharacter(pInvOwner);

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
									float(pInv->m_slots[i].m_pIItem->GetXPos()*INV_GRID_WIDTH),
									float(pInv->m_slots[i].m_pIItem->GetYPos()*INV_GRID_HEIGHT),
									float(pInv->m_slots[i].m_pIItem->GetGridWidth()*INV_GRID_WIDTH),
									float(pInv->m_slots[i].m_pIItem->GetGridHeight()*INV_GRID_HEIGHT));

			UITopList[i].AttachChild(&UIDragDropItem);
			UIDragDropItem.Rescale(UITopList[i].GetItemsScaleX(), UITopList[i].GetItemsScaleY());
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
									float(pInv->m_slots[OUTFIT_SLOT].m_pIItem->GetXPos()*INV_GRID_WIDTH),
									float(pInv->m_slots[OUTFIT_SLOT].m_pIItem->GetYPos()*INV_GRID_HEIGHT),
									float(pInv->m_slots[OUTFIT_SLOT].m_pIItem->GetGridWidth()*INV_GRID_WIDTH),
									float(pInv->m_slots[OUTFIT_SLOT].m_pIItem->GetGridHeight()*INV_GRID_HEIGHT));


		UIDragDropItem.SetData(pInv->m_slots[OUTFIT_SLOT].m_pIItem);
		UIOutfitSlot.AttachChild(&UIDragDropItem);
		UIDragDropItem.Show(false);

//		++m_iUsedItems;
//		R_ASSERT(m_iUsedItems<MAX_ITEMS);
		UIOutfitSlot.HighlightAllCells(false);
	}

	//Пояс
	for(TIItemContainer::iterator it =  pInv->m_belt.begin(); pInv->m_belt.end() != it; ++it) 
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
									float((*it)->GetXPos()*INV_GRID_WIDTH),
									float((*it)->GetYPos()*INV_GRID_HEIGHT),
									float((*it)->GetGridWidth()*INV_GRID_WIDTH),
									float((*it)->GetGridHeight()*INV_GRID_HEIGHT));


			CWeaponAmmo* pWeaponAmmo  = smart_cast<CWeaponAmmo*>((*it));

			// Не отображаем патроны в инвентаре если они посечены как "бесконечные"
			// Применимио только к режиму мультиплеера
			if(pWeaponAmmo)
			{
				UIDragDropItem.SetCustomDraw(AmmoDrawProc);
//				if (GameID() != GAME_SINGLE && pWeaponAmmo->m_bCanBeUnlimited)	
//					continue;
			}

			CEatableItem* pEatableItem = smart_cast<CEatableItem*>((*it));
			if(pEatableItem) UIDragDropItem.SetCustomDraw(FoodDrawProc);

			UIBeltList.AttachChild(&UIDragDropItem);
			UIDragDropItem.Rescale(UIBeltList.GetItemsScaleX(), UIBeltList.GetItemsScaleY());
			UIDragDropItem.SetData(*it);
			UIDragDropItem.Show(true);

			UIBeltList.HighlightAllCells(false);
//			++m_iUsedItems;
//			R_ASSERT(m_iUsedItems<MAX_ITEMS);
		}
	}


	ruck_list		= pInv->m_ruck;
	std::sort		(ruck_list.begin(),ruck_list.end(),GreaterRoomInRuck);

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
								float((*it)->GetXPos()*INV_GRID_WIDTH),
								float((*it)->GetYPos()*INV_GRID_HEIGHT),
								float((*it)->GetGridWidth()*INV_GRID_WIDTH),
								float((*it)->GetGridHeight()*INV_GRID_HEIGHT));
				
			CWeaponAmmo* pWeaponAmmo  = smart_cast<CWeaponAmmo*>((*it));

			// Не отображаем патроны в инвентаре если они посечены как "бесконечные"
			// Применимио только к режиму мультиплеера
			if(pWeaponAmmo)
			{
				UIDragDropItem.SetCustomDraw(AmmoDrawProc);
//				if (GameID() != GAME_SINGLE && pWeaponAmmo->m_bCanBeUnlimited)
//					continue;
			}

			CEatableItem* pEatableItem = smart_cast<CEatableItem*>((*it));
			if(pEatableItem) UIDragDropItem.SetCustomDraw(FoodDrawProc);

			UIBagList.AttachChild(&UIDragDropItem);
			UIDragDropItem.Rescale(UIBagList.GetItemsScaleX(), UIBagList.GetItemsScaleY());
			UIDragDropItem.SetData((*it));
			UIDragDropItem.Show(true);
			UIBagList.HighlightAllCells(false);

		//	m_iUsedItems++;
		//	R_ASSERT(m_iUsedItems<MAX_ITEMS);

		}
	}
	UpdateWeight(UIBagWnd, true);

	m_b_need_reinit		= false;
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

	this_inventory->SlotToBelt(pInvItem, pList, GRENADE_SLOT);
	this_inventory->SlotToBag(pInvItem, pList, GRENADE_SLOT);

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
//	if(pItem->GetParent() == &this_inventory->UIArtefactMergerWnd.UIArtefactList)
//		return true;


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
