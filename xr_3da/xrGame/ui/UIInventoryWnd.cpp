// UIInventoryWnd.h:  диалог инвентаря
// 
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "UIInventoryWnd.h"

#include "xrXMLParser.h"
#include "UIXmlInit.h"


#include "..\\actor.h"
#include "..\\WeaponAmmo.h"
#include "..\\CustomOutfit.h"
#include "..\\hudmanager.h"
#include "..\\ArtifactMerger.h"

#include "..\\weapon.h"
#include "..\\silencer.h"
#include "..\\scope.h"
#include "..\\grenadelauncher.h"

#include "..\\ai_script_space.h"
#include "..\\ai_script_processor.h"



#include "UIInventoryUtilities.h"
using namespace InventoryUtilities;


#include "..\\InfoPortion.h"

#define MAX_ITEMS	70

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUIInventoryWnd::CUIInventoryWnd()
{
	Hide();

	m_pCurrentItem = NULL;
	m_pCurrentDragDropItem = NULL;
	m_pItemToUpgrade = NULL;
	m_sAddonName = NULL;

	Init();

	SetFont(HUD().pFontMedium);

	m_vDragDropItems.clear();
	m_vDragDropItems.reserve(MAX_ITEMS);
}

CUIInventoryWnd::~CUIInventoryWnd()
{
//	m_vDragDropItems.erase(m_vDragDropItems.begin(), m_vDragDropItems.end());
	m_vDragDropItems.clear();
}

void CUIInventoryWnd::Init()
{

	/*CInfoPortion info;
	info.Load(1);

	char* s = info.GetText();*/


	CUIXml uiXml;
	uiXml.Init("$game_data$","inventory.xml");

	/*XML_NODE* pNode = uiXml.SearchForAttribute(uiXml.GetRoot(),
											 "test",
											 "index",
											 "12");
	char* str =  uiXml.Read(pNode, "no val");*/
	
	
	CUIXmlInit xml_init;

	CUIWindow::Init(0,0, Device.dwWidth, Device.dwHeight);


	AttachChild(&UIStaticTop);
	UIStaticTop.Init("ui\\ui_inv_quick_slots", 0,0,1024,128);
	AttachChild(&UIStaticBottom);
	UIStaticBottom.Init("ui\\ui_bottom_background", 0,Device.dwHeight-32,1024,32);

//	AttachChild(&UIStaticBelt);
//	UIStaticBelt.Init("ui\\ui_inv_belt", 0,140,1024,128);

	AttachChild(&UIStaticBelt);
	xml_init.InitStatic(uiXml, "static", 0, &UIStaticBelt);
	
	AttachChild(&UIBagWnd);
	xml_init.InitFrameWindow(uiXml, "frame_window", 0, &UIBagWnd);


	AttachChild(&UIDescWnd);
	UIDescWnd.Init("ui\\ui_frame", 440, 260, 267, 500);
	UIDescWnd.InitLeftTop("ui\\ui_inv_info_over_lt", 5,10);
	
	UIDescWnd.AttachChild(&UIStaticDesc);
	UIStaticDesc.Init("ui\\ui_inv_info_over_b", 
						5, UIDescWnd.GetHeight() - 310 ,260,310);
	
	//UIStaticDesc.SetText("\\n\\n\\nYou say %c99,99,255Yes \\n  %c9,255,9I say no\\n%c255,255,255You say stop And I say\\nGo    go    go...................\\nYou say goodbye and I say hello..");


	UIDescWnd.AttachChild(&UIStaticText);
	UIStaticText.Init(30, 197 ,250,50);

	UIDescWnd.AttachChild(&UI3dStatic);
	UI3dStatic.Init(5,10, 250,190);


//	AttachChild(&UI3dStatic);
//	UI3dStatic.Init(800, 160, 260, 130);


	//UIStaticText.SetText("Weapon Description");

	AttachChild(&UIPersonalWnd);
	UIPersonalWnd.Init("ui\\ui_frame", 730, 260, 268, 490);
	UIPersonalWnd.InitLeftTop("ui\\ui_inv_personal_over_t", 5,10);

	UIPersonalWnd.AttachChild(&UI3dCharacter);
	UI3dCharacter.Init(120,10, 170,380);


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
	UIStaticPersonal.Init("ui\\ui_inv_personal_over_b", 
						-3,UIPersonalWnd.GetHeight() - 209 ,260,260);


	//кнопки внизу
	AttachChild(&UIButton1);
	xml_init.InitButton(uiXml, "button", 0, &UIButton1);
	AttachChild(&UIButton2);
	xml_init.InitButton(uiXml, "button", 1, &UIButton2);
	AttachChild(&UIButton3);
	xml_init.InitButton(uiXml, "button", 2, &UIButton3);
	AttachChild(&UIButton4);
	xml_init.InitButton(uiXml, "button", 3, &UIButton4);
	AttachChild(&UIButton5);
	xml_init.InitButton(uiXml, "button", 4, &UIButton5);
	AttachChild(&UIButton6);
	xml_init.InitButton(uiXml, "button", 5, &UIButton6);



	

	//Списки Drag&Drop
	AttachChild(&UIBeltList);
	xml_init.InitDragDropList(uiXml, "dragdrop_list", 0, &UIBeltList);
		
	UIBagWnd.AttachChild(&UIBagList);	
	xml_init.InitDragDropList(uiXml, "dragdrop_list", 1, &UIBagList);

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
	UIPropertiesBox.Init("ui\\ui_frame",0,0,300,300);
	UIPropertiesBox.Hide();

	//для работы с артефактами
	AttachChild(&UIArtifactMergerWnd);
	//UIArtifactMergerWnd.Init(0,0,300,300);
	xml_init.InitWindow(uiXml, "frame_window", 1, &UIArtifactMergerWnd);
	UIArtifactMergerWnd.Hide();
}

void CUIInventoryWnd::InitInventory() 
{
	CInventoryOwner *pInvOwner = dynamic_cast<CInventoryOwner*>(Level().CurrentEntity());

	if(!pInvOwner) return;

	CEntityAlive *pCharacter = dynamic_cast<CEntityAlive*>(Level().CurrentEntity());
	UI3dCharacter.SetGameObject(pCharacter);

	CInventory* pInv = &pInvOwner->m_inventory;
	
	m_pMouseCapturer = NULL;

	m_pCurrentItem = NULL;
	UIStaticText.SetText(NULL);

	UIPropertiesBox.Hide();
	UIArtifactMergerWnd.Hide();

	m_pInv = pInv;

	//очистить после предыдущего запуска
	UITopList[0].DropAll();
	UITopList[1].DropAll();
	UITopList[2].DropAll();
	UITopList[3].DropAll();
	UITopList[4].DropAll();
	UIOutfitSlot.DropAll();
		
	UIBeltList.DropAll();
	UIBagList.DropAll();


	u32 i;
	/*for(u32 i = 0; i <MAX_ITEMS; i++) 
	{
		m_vDragDropItems[i].SetData(NULL);
		m_vDragDropItems[i].SetWndRect(0,0,0,0);
		m_vDragDropItems[i].SetCustomUpdate(NULL);
	}
	m_iUsedItems = 0;*/
	m_vDragDropItems.clear();


	//Slots
	for( i = 0; i < SLOTS_NUM; i++) 
	{
			if(pInv->m_slots[i].m_pIItem) 
			{
				m_vDragDropItems.push_back(CUIDragDropItem());
				CUIDragDropItem& UIDragDropItem = m_vDragDropItems.back();
	
				//UIDragDropItem.Init(pInv->m_slots[i].m_pIItem->m_sIconTexture, 0,0, 50,50);
				
				UIDragDropItem.CUIStatic::Init(0,0, 50,50);
				UIDragDropItem.SetShader(GetEquipmentIconsShader());

				UIDragDropItem.SetGridHeight(pInv->m_slots[i].m_pIItem->m_iGridHeight);
				UIDragDropItem.SetGridWidth(pInv->m_slots[i].m_pIItem->m_iGridWidth);

				UIDragDropItem.GetUIStaticItem().SetOriginalRect(
										pInv->m_slots[i].m_pIItem->m_iXPos*INV_GRID_WIDTH,
										pInv->m_slots[i].m_pIItem->m_iYPos*INV_GRID_HEIGHT,
										pInv->m_slots[i].m_pIItem->m_iGridWidth*INV_GRID_WIDTH,
										pInv->m_slots[i].m_pIItem->m_iGridHeight*INV_GRID_HEIGHT);


				UIDragDropItem.SetData(pInv->m_slots[i].m_pIItem);
				
				UITopList[i].AttachChild(&UIDragDropItem);
				
				//m_iUsedItems++;
				//m_vDragDropItems.push_back(UIDragDropItem);

			}
	}

	//Слот с костюмом
	if(pInv->m_slots[OUTFIT_SLOT].m_pIItem) 
	{
		m_vDragDropItems.push_back(CUIDragDropItem());
		CUIDragDropItem& UIDragDropItem = m_vDragDropItems.back();
		
//		UIDragDropItem.Init(pInv->m_slots[OUTFIT_SLOT].m_pIItem->m_sIconTexture, 0,0, 50,50);

		UIDragDropItem.CUIStatic::Init(0,0, 50,50);
		UIDragDropItem.SetShader(GetEquipmentIconsShader());

		UIDragDropItem.SetGridHeight(pInv->m_slots[OUTFIT_SLOT].m_pIItem->m_iGridHeight);
		UIDragDropItem.SetGridWidth(pInv->m_slots[OUTFIT_SLOT].m_pIItem->m_iGridWidth);

		UIDragDropItem.GetUIStaticItem().SetOriginalRect(
									pInv->m_slots[OUTFIT_SLOT].m_pIItem->m_iXPos*INV_GRID_WIDTH,
									pInv->m_slots[OUTFIT_SLOT].m_pIItem->m_iYPos*INV_GRID_HEIGHT,
									pInv->m_slots[OUTFIT_SLOT].m_pIItem->m_iGridWidth*INV_GRID_WIDTH,
									pInv->m_slots[OUTFIT_SLOT].m_pIItem->m_iGridHeight*INV_GRID_HEIGHT);


		UIDragDropItem.SetData(pInv->m_slots[OUTFIT_SLOT].m_pIItem);
				
		UIOutfitSlot.AttachChild(&UIDragDropItem);
		
		//m_iUsedItems++;
	}

	//Пояс
	for(PPIItem it =  pInv->m_belt.begin(); it !=  pInv->m_belt.end(); it++) 
	{
			if((*it)) 
			{
				m_vDragDropItems.push_back(CUIDragDropItem());
				CUIDragDropItem& UIDragDropItem = m_vDragDropItems.back();
			
//				UIDragDropItem.Init((*it)->m_sIconTexture, 0,0, 50,50);
//				UIDragDropItem.Init("ui\\ui_inv_quick_slots", 0,0, 50,50);
				UIDragDropItem.CUIStatic::Init(0,0, 50,50);
				UIDragDropItem.SetShader(GetEquipmentIconsShader());

				UIDragDropItem.SetGridHeight((*it)->m_iGridHeight);
				UIDragDropItem.SetGridWidth((*it)->m_iGridWidth);

				UIDragDropItem.GetUIStaticItem().SetOriginalRect(
										(*it)->m_iXPos*INV_GRID_WIDTH,
										(*it)->m_iYPos*INV_GRID_HEIGHT,
										(*it)->m_iGridWidth*INV_GRID_WIDTH,
										(*it)->m_iGridHeight*INV_GRID_HEIGHT);


				UIDragDropItem.SetData((*it));

				CWeaponAmmo* pWeaponAmmo  = dynamic_cast<CWeaponAmmo*>((*it));
				if(pWeaponAmmo)	UIDragDropItem.SetCustomUpdate(AmmoUpdateProc);

				CEatableItem* pEatableItem = dynamic_cast<CEatableItem*>((*it));
				if(pEatableItem) UIDragDropItem.SetCustomUpdate(FoodUpdateProc);
		
				UIBeltList.AttachChild(&UIDragDropItem);
				
				//m_iUsedItems++;
			}
	}


	ruck_list = pInv->m_ruck;
	ruck_list.sort(GreaterRoomInRuck);

	//Рюкзак
	for(it =  ruck_list.begin(); it !=  ruck_list.end(); it++) 
	{
			if((*it)) 
			{
				m_vDragDropItems.push_back(CUIDragDropItem());
				CUIDragDropItem& UIDragDropItem = m_vDragDropItems.back();

//				UIDragDropItem.Init((*it)->m_sIconTexture, 0,0, 50,50);
//				UIDragDropItem.Init("ui\\ui_inv_quick_slots", 0,0, 50,50);
				UIDragDropItem.CUIStatic::Init(0,0, 50,50);
				UIDragDropItem.SetShader(GetEquipmentIconsShader());

				UIDragDropItem.SetGridHeight((*it)->m_iGridHeight);
				UIDragDropItem.SetGridWidth((*it)->m_iGridWidth);

				UIDragDropItem.GetUIStaticItem().SetOriginalRect(
										(*it)->m_iXPos*INV_GRID_WIDTH,
										(*it)->m_iYPos*INV_GRID_HEIGHT,
										(*it)->m_iGridWidth*INV_GRID_WIDTH,
										(*it)->m_iGridHeight*INV_GRID_HEIGHT);
				
				UIDragDropItem.SetData((*it));

				CWeaponAmmo* pWeaponAmmo  = dynamic_cast<CWeaponAmmo*>((*it));
				if(pWeaponAmmo)	UIDragDropItem.SetCustomUpdate(AmmoUpdateProc);

				CEatableItem* pEatableItem = dynamic_cast<CEatableItem*>((*it));
				if(pEatableItem) UIDragDropItem.SetCustomUpdate(FoodUpdateProc);

				UIBagList.AttachChild(&UIDragDropItem);
				
				//m_iUsedItems++;
			}
	}
}  


//при вызове проверки необходимо помнить 
//иерархию окон, чтоб знать какой именно из
//родителей является CUIInventoryWnd и 
//содержит свойство GetInventory()

//проверка на помещение инвентаря в слоты
bool CUIInventoryWnd::SlotProc0(CUIDragDropItem* pItem, CUIDragDropList* pList)
{
	CUIInventoryWnd* this_inventory = dynamic_cast<CUIInventoryWnd*>(pList->GetParent());
	R_ASSERT2(this_inventory, "wrong parent addressed as inventory wnd");

	PIItem pInvItem = (PIItem)pItem->GetData();

	if(!this_inventory->GetInventory()->CanPutInSlot(pInvItem)) return false;

	if( pInvItem->m_slot == 0)
		return this_inventory->GetInventory()->Slot(pInvItem);
	else
		return false;
}

bool CUIInventoryWnd::SlotProc1(CUIDragDropItem* pItem, CUIDragDropList* pList)
{
	CUIInventoryWnd* this_inventory = dynamic_cast<CUIInventoryWnd*>(pList->GetParent());
	R_ASSERT2(this_inventory, "wrong parent addressed as inventory wnd");

	PIItem pInvItem = (PIItem)pItem->GetData();

	if(!this_inventory->GetInventory()->CanPutInSlot(pInvItem)) return false;

	if(pInvItem->m_slot == 1)
		return this_inventory->GetInventory()->Slot(pInvItem);
	else
		return false;
}
bool CUIInventoryWnd::SlotProc2(CUIDragDropItem* pItem, CUIDragDropList* pList)
{
	CUIInventoryWnd* this_inventory = dynamic_cast<CUIInventoryWnd*>(pList->GetParent());
	R_ASSERT2(this_inventory, "wrong parent addressed as inventory wnd");

	PIItem pInvItem = (PIItem)pItem->GetData();

	if(!this_inventory->GetInventory()->CanPutInSlot(pInvItem)) return false;

	if( pInvItem->m_slot == 2)
		return this_inventory->GetInventory()->Slot(pInvItem);
	else
		return false;
}
bool CUIInventoryWnd::SlotProc3(CUIDragDropItem* pItem, CUIDragDropList* pList)
{
	CUIInventoryWnd* this_inventory = dynamic_cast<CUIInventoryWnd*>(pList->GetParent());
	R_ASSERT2(this_inventory, "wrong parent addressed as inventory wnd");


	PIItem pInvItem = (PIItem)pItem->GetData();

	if(!this_inventory->GetInventory()->CanPutInSlot(pInvItem)) return false;

	if( pInvItem->m_slot == 3)
		return this_inventory->GetInventory()->Slot(pInvItem);
	else
		return false;
}
bool CUIInventoryWnd::SlotProc4(CUIDragDropItem* pItem, CUIDragDropList* pList)
{
	CUIInventoryWnd* this_inventory = dynamic_cast<CUIInventoryWnd*>(pList->GetParent());
	R_ASSERT2(this_inventory, "wrong parent addressed as inventory wnd");


	PIItem pInvItem = (PIItem)pItem->GetData();

	if(!this_inventory->GetInventory()->CanPutInSlot(pInvItem)) return false;

	if(pInvItem->m_slot == 4)
		return this_inventory->GetInventory()->Slot(pInvItem);
	else
		return false;
}

//одеть костюм
bool CUIInventoryWnd::OutfitSlotProc(CUIDragDropItem* pItem, CUIDragDropList* pList)
{
	CUIInventoryWnd* this_inventory = dynamic_cast<CUIInventoryWnd*>(pList->GetParent());
	R_ASSERT2(this_inventory, "wrong parent addressed as inventory wnd");

	PIItem pInvItem = (PIItem)pItem->GetData();

	if(!this_inventory->GetInventory()->CanPutInSlot(pInvItem)) return false;

	if(pInvItem->m_slot == OUTFIT_SLOT)
		return this_inventory->GetInventory()->Slot(pInvItem);
	else
		return false;

}

//в рюкзак
bool CUIInventoryWnd::BagProc(CUIDragDropItem* pItem, CUIDragDropList* pList)
{
	CUIInventoryWnd* this_inventory = dynamic_cast<CUIInventoryWnd*>(pList->GetParent()->GetParent());
	R_ASSERT2(this_inventory, "wrong parent addressed as inventory wnd");


	//если это артефакт из устройства то положить без всяких проверок
	if(pItem->GetParent() == &this_inventory->UIArtifactMergerWnd.UIArtifactList)
		return true;


	PIItem pInvItem = (PIItem)pItem->GetData();

	if(!this_inventory->GetInventory()->CanPutInRuck(pInvItem)) return false;
	return this_inventory->GetInventory()->Ruck(pInvItem);
}

//на пояс
bool CUIInventoryWnd::BeltProc(CUIDragDropItem* pItem, CUIDragDropList* pList)
{
	CUIInventoryWnd* this_inventory = dynamic_cast<CUIInventoryWnd*>(pList->GetParent());
	R_ASSERT2(this_inventory, "wrong parent addressed as inventory wnd");


	PIItem pInvItem = (PIItem)pItem->GetData();

	if(!this_inventory->GetInventory()->CanPutInBelt(pInvItem)) return false;
	return this_inventory->GetInventory()->Belt(pInvItem);
}

//------------------------------------------------
//как только подняли элемент, сделать его текущим
void CUIInventoryWnd::SendMessage(CUIWindow *pWnd, s16 msg, void *pData)
{
	if(msg == CUIDragDropItem::ITEM_DRAG)
	{
		PIItem pInvItem = (PIItem)((CUIDragDropItem*)pWnd)->GetData();
		UIStaticText.SetText(pInvItem->NameComplex());
		
		m_pCurrentItem = pInvItem;
		m_pCurrentDragDropItem = (CUIDragDropItem*)pWnd;
	}
	else if(msg == CUIDragDropItem::ITEM_DB_CLICK)
	{
		PIItem pInvItem = (PIItem)((CUIDragDropItem*)pWnd)->GetData();
		UIStaticText.SetText(pInvItem->NameComplex());

		m_pCurrentItem = pInvItem;
		m_pCurrentDragDropItem = (CUIDragDropItem*)pWnd;

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
    }
	//по нажатию правой кнопки
	else if(msg == CUIDragDropItem::ITEM_RBUTTON_CLICK)
	{
		PIItem pInvItem = (PIItem)((CUIDragDropItem*)pWnd)->GetData();
		UIStaticText.SetText(pInvItem->NameComplex());
		
		m_pCurrentItem = pInvItem;
		m_pCurrentDragDropItem = (CUIDragDropItem*)pWnd;
		
		ActivatePropertiesBox();
	}
	//сообщение от меню вызываемого правой кнопкой
	else if(pWnd == &UIPropertiesBox &&
			msg == CUIPropertiesBox::PROPERTY_CLICKED)
	{
		
		if(UIPropertiesBox.GetClickedItem())
		{
			switch(UIPropertiesBox.GetClickedItem()->GetValue())
			{
			case TO_SLOT_ACTION:	
				ToSlot();
				break;
			case TO_BELT_ACTION:	
				ToBelt();
				break;
			case TO_BAG_ACTION:	
				ToBag();
				break;
			case DROP_ACTION:	//выкинуть объект
				DropItem();
				break;
			case EAT_ACTION:	//съесть объект
				EatItem();
				break;
			case ARTIFACT_MERGER_ACTIVATE:
				StartArtifactMerger();
				break;
			case ARTIFACT_MERGER_DEACTIVATE:
				StopArtifactMerger();
				break;
			case ATTACH_ADDON:
				AttachAddon();
				break;
			case DETACH_ADDON:
				DetachAddon();
				break;

			}
		}
	}
	//сообщения от ArtifactMerger
	else if(pWnd == &UIArtifactMergerWnd && msg == CUIArtifactMerger::PERFORM_BUTTON_CLICKED)
	{
	}
	else if(pWnd == &UIArtifactMergerWnd && msg == CUIArtifactMerger::CLOSE_BUTTON_CLICKED)
	{
		StopArtifactMerger();
	}

	//кнопки cheats
	else if(pWnd == &UIButton1 && msg == CUIButton::BUTTON_CLICKED)
	{
		CActor *pActor = dynamic_cast<CActor*>(Level().CurrentEntity());
		
		if(pActor)
		{
			pActor->Sleep(1.f/6.f);
		}
	}
	else if(pWnd == &UIButton2 && msg == CUIButton::BUTTON_CLICKED)
	{
		CActor *pActor = dynamic_cast<CActor*>(Level().CurrentEntity());
		
		if(pActor)
		{
			pActor->Sleep(1.f);
		}
	}
	else if(pWnd == &UIButton3 && msg == CUIButton::BUTTON_CLICKED)
	{
		CActor *pActor = dynamic_cast<CActor*>(Level().CurrentEntity());
		
		if(pActor)
		{
			pActor->Sleep(1.f/60.f);
		}
	}
	else if(pWnd == &UIButton4 && msg == CUIButton::BUTTON_CLICKED)
	{
		CActor *pActor = dynamic_cast<CActor*>(Level().CurrentEntity());
		
		if(pActor)
		{
			pActor->ChangeHealth(pActor->m_fMedkit);
			pActor->ChangeBleeding(pActor->m_fMedkitWound);
		}
	}
	else if(pWnd == &UIButton5 && msg == CUIButton::BUTTON_CLICKED)
	{
		CActor *pActor = dynamic_cast<CActor*>(Level().CurrentEntity());
		
		if(pActor)
		{
			pActor->ChangeRadiation(-pActor->m_fAntirad);
		}
	}
	else if(pWnd == &UIButton6 && msg == CUIButton::BUTTON_CLICKED)
	{
		CActor *pActor = dynamic_cast<CActor*>(Level().CurrentEntity());
		
		if(pActor)
		{
			pActor->ChangeSatiety(0.1f);
		}
	}


	CUIWindow::SendMessage(pWnd, msg, pData);
}


void CUIInventoryWnd::OnMouse(int x, int y, E_MOUSEACTION mouse_action)
{
	//вызов дополнительного меню по правой кнопке
	if(mouse_action == RBUTTON_DOWN)
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
	static float y_rotate_angle = 0;

	y_rotate_angle += -0.01f;
   	if(y_rotate_angle>2*PI) y_rotate_angle = 0;

	UI3dStatic.SetGameObject(m_pCurrentItem);
	UI3dStatic.SetRotate(0,y_rotate_angle,0);
	
	CUIWindow::Draw();
}


void CUIInventoryWnd::Update()
{
	//CActor *pActor = dynamic_cast<CActor*>(Level().CurrentEntity());
	CEntityAlive *pEntityAlive = dynamic_cast<CEntityAlive*>(Level().CurrentEntity());

	if(pEntityAlive) 
	{
		UIProgressBarHealth.SetProgressPos(s16(pEntityAlive->GetHealth()*1000));
		UIProgressBarSatiety.SetProgressPos(s16(pEntityAlive->GetSatiety()*1000));
		UIProgressBarPower.SetProgressPos(s16(pEntityAlive->GetPower()*1000));
		UIProgressBarRadiation.SetProgressPos(s16(pEntityAlive->GetRadiation()*1000));

		
		//убрать объект drag&drop для уже использованной вещи
		//for(int i = 0; i <m_iUsedItems; i++) 
		int i = 0;
		for(DRAG_DROP_VECTOR_it it = m_vDragDropItems.begin(); 
			it != m_vDragDropItems.end(); 
			it++, i++) 
		{
			CInventoryItem* pItem = (CInventoryItem*)m_vDragDropItems[i].GetData();
			if(pItem && !pItem->Useful())
			{
				m_vDragDropItems[i].GetParent()->DetachChild(&m_vDragDropItems[i]);
				m_vDragDropItems[i].SetData(NULL);
				m_vDragDropItems[i].SetCustomUpdate(NULL);
				m_vDragDropItems.erase(m_vDragDropItems.begin()+i);

				if(m_pCurrentItem == pItem)
				{	
					m_pCurrentItem = NULL;
					m_pCurrentDragDropItem = NULL;
				}

				i = 0;
				it = m_vDragDropItems.begin(); 
			}
		}
	}

	CUIWindow::Update();
}

void CUIInventoryWnd::DropItem()
{
	CActor *pActor = dynamic_cast<CActor*>(Level().CurrentEntity());
	if(!pActor) return;

	m_pCurrentItem->Drop();
	
	(dynamic_cast<CUIDragDropList*>(m_pCurrentDragDropItem->GetParent()))->
										DetachChild(m_pCurrentDragDropItem);

	m_pCurrentItem = NULL;
	m_pCurrentDragDropItem = NULL;
}

void CUIInventoryWnd::EatItem()
{
	CActor *pActor = dynamic_cast<CActor*>(Level().CurrentEntity());
	if(!pActor) return;

	pActor->m_inventory.Eat(m_pCurrentItem);
	
	if(!m_pCurrentItem->Useful())
	{
		(dynamic_cast<CUIDragDropList*>(m_pCurrentDragDropItem->GetParent()))->
												DetachChild(m_pCurrentDragDropItem);
		m_pCurrentItem = NULL;
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
	InitInventory();
	inherited::Show();
}


void CUIInventoryWnd::ActivatePropertiesBox()
{
	int x,y;
	RECT rect = GetAbsoluteRect();
	HUD().GetUI()->GetCursor()->GetPos(x,y);
		
	UIPropertiesBox.RemoveAll();
	
	CEatableItem* pEatableItem = dynamic_cast<CEatableItem*>(m_pCurrentItem);
	CCustomOutfit* pOutfit = dynamic_cast<CCustomOutfit*>(m_pCurrentItem);
	CArtifactMerger* pArtifactMerger = dynamic_cast<CArtifactMerger*>(m_pCurrentItem);
	
	CWeapon* pWeapon = dynamic_cast<CWeapon*>(m_pCurrentItem);
	CScope* pScope = dynamic_cast<CScope*>(m_pCurrentItem);
	CSilencer* pSilencer = dynamic_cast<CSilencer*>(m_pCurrentItem);
	CGrenadeLauncher* pGrenadeLauncher = dynamic_cast<CGrenadeLauncher*>(m_pCurrentItem);
	

	if(m_pCurrentItem->m_slot<SLOTS_NUM && m_pInv->CanPutInSlot(m_pCurrentItem))
	{
		UIPropertiesBox.AddItem("Move to slot",  NULL, TO_SLOT_ACTION);
	}
	if(m_pCurrentItem->m_belt && m_pInv->CanPutInBelt(m_pCurrentItem))
	{
		UIPropertiesBox.AddItem("Move on belt",  NULL, TO_BELT_ACTION);
	}
	if(m_pCurrentItem->m_ruck && m_pInv->CanPutInRuck(m_pCurrentItem))
	{
		if(!pOutfit)
			UIPropertiesBox.AddItem("Move to bag",  NULL, TO_BAG_ACTION);
		else
			UIPropertiesBox.AddItem("Undress outfit",  NULL, TO_BAG_ACTION);
	}
	if(pOutfit && m_pInv->CanPutInSlot(m_pCurrentItem))
	{
		UIPropertiesBox.AddItem("Dress in outfit",  NULL, TO_SLOT_ACTION);
	}
	
	if(pWeapon)
	{
		if(pWeapon->IsGreandeLauncherAttached())
		{
			UIPropertiesBox.AddItem("Detach grenade launcher",  NULL, DETACH_ADDON);
		}
		if(pWeapon->IsScopeAttached())
		{
			m_sAddonName = pWeapon->GetScopeName();
			UIPropertiesBox.AddItem("Detach scope",  NULL, DETACH_ADDON);
		}
		if(pWeapon->IsSilencerAttached())
		{
			UIPropertiesBox.AddItem("Detach silencer",  NULL, DETACH_ADDON);
		}
	}
	
	//присоединение аддонов к активному слоту (2 или 3)
	if(pScope)
	{
		if(m_pInv->m_slots[PISTOL_SLOT].m_pIItem != NULL &&
		   m_pInv->m_slots[PISTOL_SLOT].m_pIItem->CanAttach(pScope))
		 {
			 UIPropertiesBox.AddItem("Attach scope to pitol",  NULL, ATTACH_ADDON);
			 m_pItemToUpgrade = m_pInv->m_slots[PISTOL_SLOT].m_pIItem;
		 }
		 if(m_pInv->m_slots[RIFLE_SLOT].m_pIItem != NULL &&
			m_pInv->m_slots[RIFLE_SLOT].m_pIItem->CanAttach(pScope))
		 {
			 UIPropertiesBox.AddItem("Attach scope to rifle",  NULL, ATTACH_ADDON);
			 m_pItemToUpgrade = m_pInv->m_slots[RIFLE_SLOT].m_pIItem;
		 }
	}
	else if(pSilencer)
	{
	}
	else if(pGrenadeLauncher)
	{
	}
	
	
	if(pEatableItem)
	{
		UIPropertiesBox.AddItem("Eat",  NULL, EAT_ACTION);
	}

	if(pArtifactMerger)
	{
		if(!UIArtifactMergerWnd.IsShown())
		{
			UIPropertiesBox.AddItem("Activate Merger", NULL, 
									ARTIFACT_MERGER_ACTIVATE);
			UIPropertiesBox.AddItem("Drop", NULL, DROP_ACTION);
		}
		else
		{
			UIPropertiesBox.AddItem("Deactivate Merger", NULL, 
									ARTIFACT_MERGER_DEACTIVATE);
		}
	}
	else
	{
		UIPropertiesBox.AddItem("Drop", NULL, DROP_ACTION);
	}

	
		
	UIPropertiesBox.AutoUpdateHeight();
	UIPropertiesBox.BringAllToTop();
	UIPropertiesBox.Show(x-rect.left, y-rect.top);
}


bool CUIInventoryWnd::ToSlot()
{
	if(!GetInventory()->CanPutInSlot(m_pCurrentItem)) return false;

	//попытаться закинуть элемент в соответствующий слот
	bool result = GetInventory()->Slot(m_pCurrentItem);

	if(!result) return false;

	((CUIDragDropList*)m_pCurrentDragDropItem->GetParent())->DetachChild(m_pCurrentDragDropItem);

	if(m_pCurrentItem->m_slot == OUTFIT_SLOT)
		UIOutfitSlot.AttachChild(m_pCurrentDragDropItem);
	else
		UITopList[m_pCurrentItem->m_slot].AttachChild(m_pCurrentDragDropItem);
			
	m_pMouseCapturer = NULL;

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

	return true;
}

//запуск и остановка меню работы с артефактами
void CUIInventoryWnd::StartArtifactMerger()
{
	UIArtifactMergerWnd.InitArtifactMerger(dynamic_cast<CArtifactMerger*>(m_pCurrentItem));
	UIArtifactMergerWnd.Show();
}
void CUIInventoryWnd::StopArtifactMerger()
{
	UIArtifactMergerWnd.Hide();
	
	//скинуть все элементы из усторйства артефактов в рюкзак
	for(DRAG_DROP_LIST_it it = UIArtifactMergerWnd.UIArtifactList.GetDragDropItemsList().begin(); 
 						  it!= UIArtifactMergerWnd.UIArtifactList.GetDragDropItemsList().end();
						  it++)
	{
		CUIDragDropItem* pDragDropItem = *it;
		UIBagList.AttachChild(pDragDropItem);
	}

	//((CUIDragDropList*)pDragDropItem->GetParent())->DetachChild(pDragDropItem);
	UIArtifactMergerWnd.UIArtifactList.DropAll();
}

//для работы с сочетателем артефактом извне
void CUIInventoryWnd::AddArtifactToMerger(CArtifact* pArtifact)
{
	m_vDragDropItems.push_back(CUIDragDropItem());
	CUIDragDropItem& UIDragDropItem = m_vDragDropItems.back();		

	UIDragDropItem.CUIStatic::Init(0,0, 50,50);
	UIDragDropItem.SetShader(GetEquipmentIconsShader());
	UIDragDropItem.SetGridHeight(pArtifact->m_iGridHeight);
	UIDragDropItem.SetGridWidth(pArtifact->m_iGridWidth);
	UIDragDropItem.GetUIStaticItem().SetOriginalRect(
										pArtifact->m_iXPos*INV_GRID_WIDTH,
										pArtifact->m_iYPos*INV_GRID_HEIGHT,
										pArtifact->m_iGridWidth*INV_GRID_WIDTH,
										pArtifact->m_iGridHeight*INV_GRID_HEIGHT);
	UIDragDropItem.SetData(pArtifact);
	UIArtifactMergerWnd.UIArtifactList.AttachChild(&UIDragDropItem);
}

void CUIInventoryWnd::AddItemToBag(PIItem pItem)
{
	m_vDragDropItems.push_back(CUIDragDropItem());
	CUIDragDropItem& UIDragDropItem = m_vDragDropItems.back();		

	UIDragDropItem.CUIStatic::Init(0,0, 50,50);
	UIDragDropItem.SetShader(GetEquipmentIconsShader());
	UIDragDropItem.SetGridHeight(pItem->m_iGridHeight);
	UIDragDropItem.SetGridWidth(pItem->m_iGridWidth);
	UIDragDropItem.GetUIStaticItem().SetOriginalRect(
										pItem->m_iXPos*INV_GRID_WIDTH,
										pItem->m_iYPos*INV_GRID_HEIGHT,
										pItem->m_iGridWidth*INV_GRID_WIDTH,
										pItem->m_iGridHeight*INV_GRID_HEIGHT);
	UIDragDropItem.SetData(pItem);
	m_vDragDropItems.push_back(UIDragDropItem);
	UIBagList.AttachChild(&UIDragDropItem);
}

void CUIInventoryWnd::AttachAddon()
{
	R_ASSERT(m_pItemToUpgrade);
	m_pItemToUpgrade->Attach(m_pCurrentItem);

	(dynamic_cast<CUIDragDropList*>(m_pCurrentDragDropItem->GetParent()))->
									DetachChild(m_pCurrentDragDropItem);
	m_pCurrentItem = NULL;
	m_pCurrentDragDropItem = NULL;

	m_pItemToUpgrade = NULL;
}
void CUIInventoryWnd::DetachAddon()
{
	R_ASSERT(*m_sAddonName);
	m_pCurrentItem->Detach(*m_sAddonName);
	m_sAddonName = NULL;
}