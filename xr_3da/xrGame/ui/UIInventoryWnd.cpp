// UIInventoryWnd.h:  диалог инвентаря
// 
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "UIInventoryWnd.h"

#include "xrXMLParser.h"
#include "UIXmlInit.h"

#include "../actor.h"
#include "../uigamesp.h"
#include "../hudmanager.h"


#include "../WeaponAmmo.h"
#include "../CustomOutfit.h"
#include "../ArtifactMerger.h"

#include "../weapon.h"
#include "../silencer.h"
#include "../scope.h"
#include "../grenadelauncher.h"

#include "../weapon.h"
#include "../silencer.h"
#include "../scope.h"
#include "../grenadelauncher.h"

#include "../script_space.h"
#include "../script_process.h"

#include "../eatable_item.h"
#include "../inventory.h"

#include "UIInventoryUtilities.h"
using namespace InventoryUtilities;


#include "../InfoPortion.h"
#include "../level.h"
#include "../game_cl_base.h"

#include "../string_table.h"

#define MAX_ITEMS	70

const char * const INVENTORY_ITEM_XML		= "inventory_item.xml";
const char * const INVENTORY_XML			= "inventory_new.xml";
const char * const INVENTORY_CHARACTER_XML	= "inventory_character.xml";



#define CANT_SLEEP_ENEMIES "cant sleep near enemies"
#define CANT_SLEEP_GROUND "cant sleep not on solid ground"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUIInventoryWnd::CUIInventoryWnd()
{
	m_iCurrentActiveSlot = NO_ACTIVE_SLOT;
	Hide();

	SetCurrentItem(NULL);

	m_pCurrentDragDropItem = NULL;
	m_pItemToUpgrade = NULL;

	Init();

	SetFont(HUD().pFontMedium);
}

CUIInventoryWnd::~CUIInventoryWnd()
{
	ClearDragDrop(m_vDragDropItems);
}

void CUIInventoryWnd::Init()
{
	CUIXml uiXml;
	bool xml_result = uiXml.Init("$game_data$", INVENTORY_XML);
	R_ASSERT3(xml_result, "xml file not found", INVENTORY_XML);

	CUIXmlInit xml_init;

	CUIWindow::Init(CUIXmlInit::ApplyAlignX(0, alCenter),
					CUIXmlInit::ApplyAlignY(0, alCenter),
					UI_BASE_WIDTH, UI_BASE_HEIGHT);

	AttachChild(&UIStaticTop);
	UIStaticTop.Init("ui\\ui_inv_quick_slots", 0,0,UI_BASE_WIDTH,128);
	AttachChild(&UIStaticBottom);
	UIStaticBottom.Init("ui\\ui_bottom_background", 0, UI_BASE_HEIGHT-32, UI_BASE_WIDTH,32);


	AttachChild(&UIStaticBelt);
	xml_init.InitStatic(uiXml, "static", 0, &UIStaticBelt);
	
	AttachChild(&UIBagWnd);
	xml_init.InitStatic(uiXml, "bag_static", 0, &UIBagWnd);

	////////////////////////////////////////
	//окно с описанием активной вещи
	
	//для работы с артефактами
	AttachChild(&UIArtefactMergerWnd);
	xml_init.InitWindow(uiXml, "frame_window", 0, &UIArtefactMergerWnd);
	UIArtefactMergerWnd.Hide();

	AttachChild(&UIDescrWnd);
	xml_init.InitStatic(uiXml, "descr_static", 0, &UIDescrWnd);

	//информация о предмете
	UIDescrWnd.AttachChild(&UIItemInfo);
	UIItemInfo.Init(0, 0, UIDescrWnd.GetWidth(), UIDescrWnd.GetHeight(), INVENTORY_ITEM_XML);

	// Кнопка Drop
	UIDescrWnd.AttachChild(&UIDropButton);
	xml_init.InitButton(uiXml, "drop_button", 0, &UIDropButton);
	UIDropButton.SetMessageTarget(this);
	UIDropButton.SetTextAlign(CGameFont::alLeft);

	////////////////////////////////////
	//Окно с информации о персонаже
	AttachChild(&UIPersonalWnd);
	xml_init.InitFrameWindow(uiXml, "frame_window", 1, &UIPersonalWnd);

	//Полосы прогресса
	UIPersonalWnd.AttachChild(&UIProgressBarHealth);
	xml_init.InitProgressBar(uiXml, "progress_bar", 0, &UIProgressBarHealth);
	
	UIPersonalWnd.AttachChild(&UIProgressBarSatiety);
	xml_init.InitProgressBar(uiXml, "progress_bar", 1, &UIProgressBarSatiety);

	UIPersonalWnd.AttachChild(&UIProgressBarPower);
	xml_init.InitProgressBar(uiXml, "progress_bar", 2, &UIProgressBarPower);

	UIPersonalWnd.AttachChild(&UIProgressBarRadiation);
	xml_init.InitProgressBar(uiXml, "progress_bar", 3, &UIProgressBarRadiation);


	UIPersonalWnd.AttachChild(&UIStaticPersonal);
	UIStaticPersonal.Init("ui\\ui_inv_personal_over_b", -1, UIPersonalWnd.GetHeight() - 175, 260, 260);

	//информация о персонаже
	UIStaticPersonal.AttachChild(&UICharacterInfo);
	UICharacterInfo.Init(0, 0, UIStaticPersonal.GetWidth(), UIStaticPersonal.GetHeight(), INVENTORY_CHARACTER_XML);
	


	//Элементы автоматического добавления
	xml_init.InitAutoStatic(uiXml, "auto_static", this);


	//окошко для диалога параметров сна
	AttachChild(&UISleepWnd);
	xml_init.InitStatic(uiXml, "sleep_window", 0, &UISleepWnd);
	UISleepWnd.Init();

	//Списки Drag&Drop
	AttachChild(&UIBeltList);
	xml_init.InitDragDropList(uiXml, "dragdrop_list", 0, &UIBeltList);
		
	UIBagWnd.AttachChild(&UIBagList);	
	xml_init.InitDragDropList(uiXml, "dragdrop_list", 1, &UIBagList);
	UIBagList.SetItemsScale(TRADE_ICONS_SCALE);

	AttachChild(&UITopList[0]);
	xml_init.InitDragDropList(uiXml, "dragdrop_list", 2, &UITopList[0]);
	UITopList[0].BlockCustomPlacement();

	AttachChild(&UITopList[1]);
	xml_init.InitDragDropList(uiXml, "dragdrop_list", 3, &UITopList[1]);
	UITopList[1].BlockCustomPlacement();

	AttachChild(&UITopList[2]);
	xml_init.InitDragDropList(uiXml, "dragdrop_list", 4, &UITopList[2]);
	UITopList[2].BlockCustomPlacement();

	AttachChild(&UITopList[3]);
	xml_init.InitDragDropList(uiXml, "dragdrop_list", 5, &UITopList[3]);
	UITopList[3].BlockCustomPlacement();

	AttachChild(&UITopList[4]);
	xml_init.InitDragDropList(uiXml, "dragdrop_list", 6, &UITopList[4]);
	UITopList[4].BlockCustomPlacement();
		
	AttachChild(&UIOutfitSlot);
	xml_init.InitDragDropList(uiXml, "dragdrop_list", 7, &UIOutfitSlot);
	UIOutfitSlot.BlockCustomPlacement();


	UITopList[0].SetCheckProc(SlotProc0);
	UITopList[1].SetCheckProc(SlotProc1);
	UITopList[2].SetCheckProc(SlotProc2);
	UITopList[3].SetCheckProc(SlotProc3);
	UITopList[4].SetCheckProc(SlotProc4);
	UIOutfitSlot.SetCheckProc(OutfitSlotProc);

	UIBeltList.SetCheckProc(BeltProc);
	UIBagList.SetCheckProc(BagProc);


	//pop-up menu
	AttachChild(&UIPropertiesBox);
	UIPropertiesBox.Init("ui\\ui_pop_up",0,0,300,300);
	UIPropertiesBox.Hide();

	// Time indicator
	AttachChild(&UIStaticTime);
	xml_init.InitStatic(uiXml, "time_static", 0, &UIStaticTime);

	// Exit button
	AttachChild(&UIExitButton);
	xml_init.InitButton(uiXml, "exit_button", 0, &UIExitButton);
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


//при вызове проверки необходимо помнить 
//иерархию окон, чтоб знать какой именно из
//родителей является CUIInventoryWnd и 
//содержит свойство GetInventory()

//проверка на помещение инвентаря в слоты
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

		EActorSleep result = pActor->GoSleep(*reinterpret_cast<u32*>(pData));
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

		Game().StartStopMenu(this);
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
		Game().StartStopMenu(this);
	}

	CUIWindow::SendMessage(pWnd, msg, pData);
}


void CUIInventoryWnd::OnMouse(int x, int y, EUIMessages mouse_action)
{
	//вызов дополнительного меню по правой кнопке
	if(mouse_action == WINDOW_RBUTTON_DOWN)
	{
		if(UIPropertiesBox.IsShown())
		{
			UIPropertiesBox.Hide();
		}
	}

	CUIWindow::OnMouse(x, y, mouse_action);
}

void CUIInventoryWnd::Draw()
{
	CUIWindow::Draw();
}


void CUIInventoryWnd::Update()
{
	//CActor *pActor = smart_cast<CActor*>(Level().CurrentEntity());
	CEntityAlive *pEntityAlive = smart_cast<CEntityAlive*>(Level().CurrentEntity());

	if(pEntityAlive) 
	{
		UIProgressBarHealth.SetProgressPos(s16(pEntityAlive->GetHealth()*1000));
		UIProgressBarSatiety.SetProgressPos(s16(pEntityAlive->GetSatiety()*1000));
		UIProgressBarPower.SetProgressPos(s16(pEntityAlive->GetPower()*1000));
		UIProgressBarRadiation.SetProgressPos(s16(pEntityAlive->GetRadiation()*1000));

		
		//убрать объект drag&drop для уже использованной вещи
		for(u32 i = 0; i <m_vDragDropItems.size()/*m_iUsedItems*/; i++) 
		{
			CInventoryItem* pItem = (CInventoryItem*)m_vDragDropItems[i]->GetData();
			if(pItem && !pItem->Useful())
			{
				m_vDragDropItems[i]->GetParent()->DetachChild(m_vDragDropItems[i]);
				m_vDragDropItems[i]->SetData(NULL);
				m_vDragDropItems[i]->SetCustomUpdate(NULL);
				
				if(m_pCurrentItem == pItem)
				{	
					SetCurrentItem(NULL);
					m_pCurrentDragDropItem = NULL;
				}

				xr_delete(m_vDragDropItems[i]);
				m_vDragDropItems.erase(m_vDragDropItems.begin()+i);
			}
		}
	}

	// Update time indicator
	UpdateTime();


	CUIWindow::Update();
}

void CUIInventoryWnd::DropItem()
{
	CActor *pActor = smart_cast<CActor*>(Level().CurrentEntity());
	if(!pActor) return;

//	if (smart_cast<CCustomOutfit*>(m_pCurrentItem))
//		SendMessage(NULL, CUIOutfitSlot::UNDRESS_OUTFIT, NULL);
	if (m_pCurrentDragDropItem == UIOutfitSlot.GetDragDropItemsList().front())
		SendMessage(NULL, UNDRESS_OUTFIT, NULL);

	m_pCurrentItem->Drop();
	m_pCurrentDragDropItem->Highlight(false);
	
	(smart_cast<CUIDragDropList*>(m_pCurrentDragDropItem->GetParent()))->
										DetachChild(m_pCurrentDragDropItem);
	
	DD_ITEMS_VECTOR_IT it = std::find(m_vDragDropItems.begin(), m_vDragDropItems.end(),m_pCurrentDragDropItem);
	VERIFY(it != m_vDragDropItems.end());
	m_vDragDropItems.erase(it);

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

	pActor->inventory().Eat(m_pCurrentItem);
	
	if(!m_pCurrentItem->Useful())
	{
		(smart_cast<CUIDragDropList*>(m_pCurrentDragDropItem->GetParent()))->
			DetachChild(m_pCurrentDragDropItem);

		DD_ITEMS_VECTOR_IT it = std::find(m_vDragDropItems.begin(), m_vDragDropItems.end(),m_pCurrentDragDropItem);
		VERIFY(it != m_vDragDropItems.end());
		m_vDragDropItems.erase(it);

		//-----------------------------------------------------------------------
		//SendEvent_ItemDrop(m_pCurrentItem);
		//-----------------------------------------------------------------------
		SetCurrentItem(NULL);
		m_pCurrentDragDropItem = NULL;
	}

	/*m_pCurrentItem->Drop();
	
	NET_Packet P;
	pActor->u_EventGen(P,GE_DESTROY,m_pCurrentItem->ID());
	P.w_u16(u16(m_pCurrentItem->ID()));
	pActor->u_EventSend(P);*/
}


void CUIInventoryWnd::Show() 
{ 
/*	//спрятать вещь из активного слота в инвентарь на время вызова менюшки
	CActor *pActor = smart_cast<CActor*>(Level().CurrentEntity());
	if(pActor)
	{
		m_iCurrentActiveSlot = pActor->inventory().GetActiveSlot();

		if(pActor->inventory().ActiveItem())
		{
			CWeapon* pWeapon = smart_cast<CWeapon*>(pActor->inventory().ActiveItem());
			if(pWeapon) pWeapon->ResetPending();
		}
		pActor->inventory().Activate(NO_ACTIVE_SLOT);
	}


*/
	InitInventory();
	inherited::Show();
	m_pCurrentDragDropItem = NULL;

	if (GameID() != GAME_SINGLE)
	{
		CActor *pActor = smart_cast<CActor*>(Level().CurrentEntity());
		if(!pActor) return;

		NET_Packet	P;
		pActor->u_EventGen(P, GEG_PLAYER_INVENTORYMENU_OPEN, pActor->ID());
		pActor->u_EventSend(P);
	}

	Update();
}

void CUIInventoryWnd::Hide()
{
	inherited::Hide();
	//достать вещь в активный слот
	CActor *pActor = smart_cast<CActor*>(Level().CurrentEntity());
	if(pActor && m_iCurrentActiveSlot != NO_ACTIVE_SLOT && 
		pActor->inventory().m_slots[m_iCurrentActiveSlot].m_pIItem)
	{
		pActor->inventory().Activate(m_iCurrentActiveSlot);
		m_iCurrentActiveSlot = NO_ACTIVE_SLOT;
	}

	if (GameID() != GAME_SINGLE)
	{
		CActor *pActor = smart_cast<CActor*>(Level().CurrentEntity());
		if(!pActor) return;

		NET_Packet	P;
		pActor->u_EventGen(P, GEG_PLAYER_INVENTORYMENU_CLOSE, pActor->ID());
		pActor->u_EventSend(P);
	}
}


void CUIInventoryWnd::ActivatePropertiesBox()
{
	int x,y;
	// Флаг-признак для невлючения пункта контекстного меню: Dreess Outfit, если костюм уже надет
	bool bAlreadyDressed = false; 

	RECT rect = GetAbsoluteRect();
	HUD().GetUI()->GetCursor()->GetPos(x,y);
		
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


bool CUIInventoryWnd::ToSlot()
{
	// Если целевой слот - слот с одеждой, то попробуем убрать текущую одежду
	if (OUTFIT_SLOT == m_pCurrentItem->GetSlot()) UndressOutfit();

	// Убираем текущую вещь в слоте, если это не одежда, и текущая вещь и вешь в слоте не одно и то же
	if (OUTFIT_SLOT != m_pCurrentItem->GetSlot() && m_pCurrentItem->GetSlot() < SLOTS_NUM)
	{
		DRAG_DROP_LIST &DDList = UITopList[m_pCurrentItem->GetSlot()].GetDragDropItemsList();

		if (!DDList.empty() &&
			m_pCurrentDragDropItem != *DDList.begin())
		{
			// Берем текущую вещь в слоте...
			(*DDList.begin())->MoveOnNextDrop();
			// ...и посылаем слоту сообщение переместить эту вещь в себя
			UIBagList.SendMessage(
				*DDList.begin(), 
				DRAG_DROP_ITEM_DROP,
				NULL);
		}
	}

	// Можно ли засунуть новую вещь в предназначеный для нее слот?
	if(!GetInventory()->CanPutInSlot(m_pCurrentItem)) return false;

	//попытаться закинуть элемент в соответствующий слот
	bool result = GetInventory()->Slot(m_pCurrentItem);

	if(!result) return false;

	((CUIDragDropList*)m_pCurrentDragDropItem->GetParent())->DetachChild(m_pCurrentDragDropItem);

	if(m_pCurrentItem->GetSlot() == OUTFIT_SLOT)
		UIOutfitSlot.AttachChild(m_pCurrentDragDropItem);
	else
		UITopList[m_pCurrentItem->GetSlot()].AttachChild(m_pCurrentDragDropItem);
			
	m_pMouseCapturer = NULL;

	//---------------------------------------------------------------------------
	SendEvent_Item2Slot(m_pCurrentItem);
	//---------------------------------------------------------------------------
	return true;
}

bool CUIInventoryWnd::ToBag()
{
	if(!GetInventory()->CanPutInRuck(m_pCurrentItem)) return false;

	//попытаться закинуть элемент в соответствующий слот
	bool result = GetInventory()->Ruck(m_pCurrentItem);

	if(!result) return false;

	((CUIDragDropList*)m_pCurrentDragDropItem->GetParent())->DetachChild(m_pCurrentDragDropItem);
	UIBagList.AttachChild(m_pCurrentDragDropItem);
			
	m_pMouseCapturer = NULL;

	//---------------------------------------------------------------------------
	SendEvent_Item2Ruck(m_pCurrentItem);
	//---------------------------------------------------------------------------
	return true;
}

bool CUIInventoryWnd::ToBelt()
{
	if(!GetInventory()->CanPutInBelt(m_pCurrentItem)) return false;
	
	bool result = GetInventory()->Belt(m_pCurrentItem);

	if(!result) return false;

	((CUIDragDropList*)m_pCurrentDragDropItem->GetParent())->DetachChild(m_pCurrentDragDropItem);
	UIBeltList.AttachChild(m_pCurrentDragDropItem);
			
	m_pMouseCapturer = NULL;
	//---------------------------------------------------------------------------
	SendEvent_Item2Belt(m_pCurrentItem);
	//---------------------------------------------------------------------------
	return true;
}

//запуск и остановка меню работы с артефактами
void CUIInventoryWnd::StartArtefactMerger()
{
	UIArtefactMergerWnd.InitArtefactMerger(smart_cast<CArtefactMerger*>(m_pCurrentItem));
	UIArtefactMergerWnd.Show();
}
void CUIInventoryWnd::StopArtefactMerger()
{
	UIArtefactMergerWnd.Hide();
	
	//скинуть все элементы из усторйства артефактов в рюкзак
	for(DRAG_DROP_LIST_it it = UIArtefactMergerWnd.UIArtefactList.GetDragDropItemsList().begin(); 
 						  UIArtefactMergerWnd.UIArtefactList.GetDragDropItemsList().end() != it;
						  ++it)
	{
		CUIDragDropItem* pDragDropItem = *it;
		UIBagList.AttachChild(pDragDropItem);
	}

	//((CUIDragDropList*)pDragDropItem->GetParent())->DetachChild(pDragDropItem);
	UIArtefactMergerWnd.UIArtefactList.DropAll();
}

//для работы с сочетателем артефактом извне
void CUIInventoryWnd::AddArtefactToMerger(CArtefact* pArtefact)
{
	m_vDragDropItems.push_back(xr_new<CUIWpnDragDropItem>());
	CUIDragDropItem& UIDragDropItem = *m_vDragDropItems.back();
	//CUIDragDropItem& UIDragDropItem = m_vDragDropItems[m_iUsedItems];
	//++m_iUsedItems; R_ASSERT(m_iUsedItems<MAX_ITEMS);

	UIDragDropItem.CUIStatic::Init(0, 0, INV_GRID_WIDTH, INV_GRID_HEIGHT);
	UIDragDropItem.SetShader(GetEquipmentIconsShader());
	UIDragDropItem.SetGridHeight(pArtefact->GetGridHeight());
	UIDragDropItem.SetGridWidth(pArtefact->GetGridWidth());
	UIDragDropItem.GetUIStaticItem().SetOriginalRect(
										pArtefact->GetXPos()*INV_GRID_WIDTH,
										pArtefact->GetYPos()*INV_GRID_HEIGHT,
										pArtefact->GetGridWidth()*INV_GRID_WIDTH,
										pArtefact->GetGridHeight()*INV_GRID_HEIGHT);
	UIDragDropItem.SetData(pArtefact);
	UIArtefactMergerWnd.UIArtefactList.AttachChild(&UIDragDropItem);
}

void CUIInventoryWnd::AddItemToBag(PIItem pItem)
{
	m_vDragDropItems.push_back(xr_new<CUIWpnDragDropItem>());
	CUIDragDropItem& UIDragDropItem = *m_vDragDropItems.back();

//	CUIDragDropItem& UIDragDropItem = m_vDragDropItems[m_iUsedItems];
//	m_iUsedItems++; R_ASSERT(m_iUsedItems<MAX_ITEMS);

	UIDragDropItem.CUIStatic::Init(0, 0, INV_GRID_WIDTH, INV_GRID_HEIGHT);
	UIDragDropItem.SetShader(GetEquipmentIconsShader());
	UIDragDropItem.SetGridHeight(pItem->GetGridHeight());
	UIDragDropItem.SetGridWidth(pItem->GetGridWidth());
	UIDragDropItem.GetUIStaticItem().SetOriginalRect(
										pItem->GetXPos()*INV_GRID_WIDTH,
										pItem->GetYPos()*INV_GRID_HEIGHT,
										pItem->GetGridWidth()*INV_GRID_WIDTH,
										pItem->GetGridHeight()*INV_GRID_HEIGHT);
	if(m_pInv->Get(pItem->ID(), false))
        UIBeltList.AttachChild(&UIDragDropItem);
	else if(m_pInv->Get(pItem->ID(), true))
		UIBagList.AttachChild(&UIDragDropItem);

	
	UIDragDropItem.SetData(pItem);
}

void CUIInventoryWnd::AttachAddon()
{
	if (OnClient())
	{
		NET_Packet P;
		m_pItemToUpgrade->u_EventGen(P, GE_ADDON_ATTACH, m_pItemToUpgrade->ID());
		P.w_u32(m_pCurrentItem->ID());
		m_pItemToUpgrade->u_EventSend(P);
	};

	R_ASSERT(m_pItemToUpgrade);
	m_pItemToUpgrade->Attach(m_pCurrentItem);


	//спрятать вещь из активного слота в инвентарь на время вызова менюшки
	CActor *pActor = smart_cast<CActor*>(Level().CurrentEntity());
	if(pActor && m_pItemToUpgrade == pActor->inventory().ActiveItem())
	{
					m_iCurrentActiveSlot = pActor->inventory().GetActiveSlot();
					pActor->inventory().Activate(NO_ACTIVE_SLOT);
//		m_iCurrentActiveSlot = pActor->HideActiveItem();
	}

	(smart_cast<CUIDragDropList*>(m_pCurrentDragDropItem->GetParent()))->
									DetachChild(m_pCurrentDragDropItem);
	SetCurrentItem(NULL);
	m_pCurrentDragDropItem = NULL;

	m_pItemToUpgrade = NULL;
}
void CUIInventoryWnd::DetachAddon(const char* addon_name)
{
	if (OnClient())
	{
		NET_Packet P;
		m_pCurrentItem->u_EventGen(P, GE_ADDON_DETACH, m_pCurrentItem->ID());
		P.w_stringZ(addon_name);
		m_pCurrentItem->u_EventSend(P);
	};
	m_pCurrentItem->Detach(addon_name);

	//спрятать вещь из активного слота в инвентарь на время вызова менюшки
	CActor *pActor = smart_cast<CActor*>(Level().CurrentEntity());
	if(pActor && m_pCurrentItem == pActor->inventory().ActiveItem())
	{
			m_iCurrentActiveSlot = pActor->inventory().GetActiveSlot();
			pActor->inventory().Activate(NO_ACTIVE_SLOT);
//			m_iCurrentActiveSlot = pActor->HideActiveItem();
	}
}

void CUIInventoryWnd::SetCurrentItem(CInventoryItem* pItem)
{
	m_pCurrentItem = pItem;
	const int offset = -10;

	UIItemInfo.InitItem(m_pCurrentItem);
	UIItemInfo.AlignRight(UIItemInfo.UIWeight, offset);
	UIItemInfo.AlignRight(UIItemInfo.UICost, offset);
	UIItemInfo.AlignRight(UIItemInfo.UICondition, offset);
}

//-----------------------------------------------------------------------------/
//  Снять костюм если есть
//-----------------------------------------------------------------------------/
bool CUIInventoryWnd::UndressOutfit()
{
	bool status = true;
	// Получить текущий костюм
	CUIDragDropItem *pDDItem = UIOutfitSlot.GetCurrentOutfit();
	if (pDDItem)
	{
		// Переместить его в сумку
		PIItem pInvItem = (PIItem)(UIOutfitSlot.GetCurrentOutfit())->GetData();

		// Запоминаем текущий активный элемент
		PIItem pCurrentItem = m_pCurrentItem;
		CUIDragDropItem *pCurrDDItem = m_pCurrentDragDropItem;

		SetCurrentItem(pInvItem);
		m_pCurrentDragDropItem = UIOutfitSlot.GetCurrentOutfit();

		status = ToBag();

		// восстанавливам предыдущее состояние
		SetCurrentItem(pCurrentItem);
		m_pCurrentDragDropItem = pCurrDDItem;
	}
	return status;
}

//-----------------------------------------------------------------------------/
//	Проверить принадлежность вещи к нужному слоту и попробовать освободить для 
//	нее место, переместив текущую вещь в слоте в сумку
//-----------------------------------------------------------------------------/
bool CUIInventoryWnd::SlotToBag(PIItem pItem, CUIDragDropList *pList, const u32 SlotNum)
{
	// Проверяем возможность перенесения  айтема в нужный слот
	if (SlotNum != pItem->GetSlot()) return false;
	// Выкидываем вещь из слота если есть.
	if (!pList->GetDragDropItemsList().empty())
	{
		// Берем текущее оружие в слоте...
		(*pList->GetDragDropItemsList().begin())->MoveOnNextDrop();
		// ...и посылаем ему сообщение переместиться в сумку
		GetBag()->SendMessage((*pList->GetDragDropItemsList().begin()), 
			DRAG_DROP_ITEM_DROP, NULL);
	}
	return true;
}

void	CUIInventoryWnd::SendEvent_Item2Slot			(PIItem	pItem)
{
	NET_Packet	P;
	pItem->u_EventGen(P, GEG_PLAYER_ITEM2SLOT, pItem->H_Parent()->ID());
	P.w_u16		(pItem->ID());
	pItem->u_EventSend(P);
};

void	CUIInventoryWnd::SendEvent_Item2Belt			(PIItem	pItem)
{
	NET_Packet	P;
	pItem->u_EventGen(P, GEG_PLAYER_ITEM2BELT, pItem->H_Parent()->ID());
	P.w_u16		(pItem->ID());
	pItem->u_EventSend(P);

};

void	CUIInventoryWnd::SendEvent_Item2Ruck			(PIItem	pItem)
{
	NET_Packet	P;
	pItem->u_EventGen(P, GEG_PLAYER_ITEM2RUCK, pItem->H_Parent()->ID());
	P.w_u16		(pItem->ID());
	pItem->u_EventSend(P);
};

void	CUIInventoryWnd::SendEvent_ItemDrop			(PIItem	pItem)
{
	NET_Packet	P;
	pItem->u_EventGen(P, GEG_PLAYER_ITEMDROP, pItem->H_Parent()->ID());
	P.w_u16		(pItem->ID());
	pItem->u_EventSend(P);
};

//////////////////////////////////////////////////////////////////////////

void CUIInventoryWnd::UpdateTime()
{
	static ref_str prevStrTime;
	const ref_str strTime = InventoryUtilities::GetGameTimeAsString(InventoryUtilities::etpTimeToMinutes);

	if (strTime != prevStrTime)
	{
		string64	buf;
		strconcat(buf, "Current Time:      ", *strTime);
		UIStaticTime.SetText(buf);
		prevStrTime = strTime;
	}
}