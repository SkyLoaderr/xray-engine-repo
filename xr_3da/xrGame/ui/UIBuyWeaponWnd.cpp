// UIBuyWeaponWnd.cpp:	окошко, для покупки оружия в режиме CS
//						
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UIBuyWeaponWnd.h"

#include "xrXMLParser.h"
#include "UIXmlInit.h"

//#include "../actor.h"
#include "../uigamesp.h"
#include "../hudmanager.h"

#include "../WeaponAmmo.h"
#include "../CustomOutfit.h"
#include "../ArtifactMerger.h"

//#include "../weapon.h"
//#include "../silencer.h"
//#include "../scope.h"
//#include "../grenadelauncher.h"
//
//#include "../weapon.h"
//#include "../silencer.h"
//#include "../scope.h"
//#include "../grenadelauncher.h"

#include "../script_space.h"
#include "../ai_script_processor.h"

//#include "../eatable_item.h"
//#include "../inventory.h"

#include "UIInventoryUtilities.h"
using namespace InventoryUtilities;

#include "../InfoPortion.h"

#define MAX_ITEMS	70

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUIBuyWeaponWnd::CUIBuyWeaponWnd(char *strSectionName)
{
	m_iCurrentActiveSlot = NO_ACTIVE_SLOT;
	Hide();

	SetCurrentItem(NULL);

	m_pCurrentDragDropItem = NULL;
	m_pItemToUpgrade = NULL;

	m_iUsedItems	= 0;

	Init(strSectionName);

	SetFont(HUD().pFontMedium);
}

CUIBuyWeaponWnd::~CUIBuyWeaponWnd()
{
}

void CUIBuyWeaponWnd::Init(char *strSectionName)
{
	CUIXml uiXml;
	bool xml_result = uiXml.Init("$game_data$","inventoryMP.xml");
	R_ASSERT2(xml_result, "xml file not found");

	inherited::DetachAll();

	CUIXmlInit xml_init;

	CUIWindow::Init(0,0, Device.dwWidth, Device.dwHeight);

	AttachChild(&UIStaticTop);
	UIStaticTop.Init("ui\\ui_inv_quick_slots", 0,0,1024,128);
	AttachChild(&UIStaticBottom);
	UIStaticBottom.Init("ui\\ui_bottom_background", 0,Device.dwHeight-32,1024,32);


	AttachChild(&UIStaticBelt);
	xml_init.InitStatic(uiXml, "static", 0, &UIStaticBelt);

	AttachChild(&UIBagWnd);
	xml_init.InitFrameWindow(uiXml, "frame_window", 0, &UIBagWnd);

	////////////////////////////////////////
	//окно с описанием активной вещи

	//для работы с артефактами
	AttachChild(&UIArtifactMergerWnd);
	xml_init.InitWindow(uiXml, "frame_window", 1, &UIArtifactMergerWnd);
	UIArtifactMergerWnd.Hide();

	AttachChild(&UIDescWnd);
	xml_init.InitFrameWindow(uiXml, "frame_window", 2, &UIDescWnd);
	UIDescWnd.AttachChild(&UIStaticDesc);
	UIStaticDesc.Init("ui\\ui_inv_info_over_b", 5, UIDescWnd.GetHeight() - 310 ,260,310);

	//информация о предмете
	UIStaticDesc.AttachChild(&UIItemInfo);
	UIItemInfo.Init(0,0, UIStaticDesc.GetWidth(), UIStaticDesc.GetHeight(), "inventory_item.xml");

	////////////////////////////////////
	//Окно с информации о персонаже
	AttachChild(&UIPersonalWnd);
	xml_init.InitFrameWindow(uiXml, "frame_window", 3, &UIPersonalWnd);

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

	//информация о персонаже
	UIStaticPersonal.AttachChild(&UICharacterInfo);
	UICharacterInfo.Init(0,0, UIStaticPersonal.GetWidth(), UIStaticPersonal.GetHeight(), "inventory_character.xml");

	// Tabcontrol для оружия
	AttachChild(&UIWeaponsTabControl);
	xml_init.InitTabControl(uiXml, "tab", 0, &UIWeaponsTabControl);

	//Элементы автоматического добавления
	xml_init.InitAutoStatic(uiXml, "auto_static", this);


	//кнопки внизу
	AttachChild(&UISleepButton);
	xml_init.InitButton(uiXml, "sleep_button", 0, &UISleepButton);


	//окошко для диалога параметров сна
	AttachChild(&UISleepWnd);
	xml_init.InitWindow(uiXml, "sleep_window", 0, &UISleepWnd);
	UISleepWnd.Hide();

	/*	AttachChild(&UIButton1);
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
	xml_init.InitButton(uiXml, "button", 5, &UIButton6);*/

	// Кнопки OK и Cancel для выходи из диалога покупки оружия
	AttachChild(&UIBtnOK);
	xml_init.InitButton(uiXml, "ok_button", 0, &UIBtnOK);

	AttachChild(&UIBtnCancel);
	xml_init.InitButton(uiXml, "cancel_button", 0, &UIBtnCancel);

	//Списки Drag&Drop
	AttachChild(&UITopList[BELT_SLOT]);
	xml_init.InitDragDropList(uiXml, "dragdrop_list", 0, &UITopList[BELT_SLOT]);

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

	UITopList[BELT_SLOT].SetCheckProc(BeltProc);

	//pop-up menu
	AttachChild(&UIPropertiesBox);
	UIPropertiesBox.Init("ui\\ui_pop_up",0,0,300,300);
	UIPropertiesBox.Hide();

	for (int i = 0; i < UIWeaponsTabControl.GetTabsCount(); ++i)
	{
		CUIDragDropList *pNewDDList = xr_new<CUIDragDropList>();
		R_ASSERT(pNewDDList);

		xml_init.InitDragDropList(uiXml, "dragdrop_list", 1, pNewDDList);
		if (i == UIWeaponsTabControl.GetActiveIndex())
		{
			pNewDDList->Show(true);
			UIBagWnd.AttachChild(pNewDDList);
		}
		else 
			pNewDDList->Show(false);

		pNewDDList->SetCheckProc(BagProc);
		m_WeaponSubBags.push_back(pNewDDList);
	}
	// Заполняем массив со списком оружия
	ReInitItems(strSectionName);
}

void CUIBuyWeaponWnd::ReInitItems	(char *strSectionName)
{
	// Заполняем массив со списком оружия
	std::strcpy(m_SectionName, strSectionName);
	// очищаем слоты
	SlotToSection(KNIFE_SLOT		);
	SlotToSection(PISTOL_SLOT		);
	SlotToSection(RIFLE_SLOT		);
	SlotToSection(GRENADE_SLOT		);
	SlotToSection(APPARATUS_SLOT	);
	SlotToSection(OUTFIT_SLOT		);

	// очищаем секции
	for (u32 i=0; i<wpnSectStorage.size(); i++)
	{
		WPN_SECT_NAMES WpnSectName = wpnSectStorage[i];
		for (u32 j=0; j<WpnSectName.size(); j++)
		{
			std::string WpnName = WpnSectName[j];
			WpnName.clear();
		};
		WpnSectName.clear();
	}
	wpnSectStorage.clear();

	InitWpnSectStorage();
	ClearWpnSubBags();
	FillWpnSubBags();

};

void CUIBuyWeaponWnd::InitInventory() 
{
	m_pMouseCapturer = NULL;
}  


//при вызове проверки необходимо помнить 
//иерархию окон, чтоб знать какой именно из
//родителей является CUIBuyWeaponWnd и 
//содержит свойство GetInventory()

//проверка на помещение инвентаря в слоты
bool CUIBuyWeaponWnd::SlotProc0(CUIDragDropItem* pItem, CUIDragDropList* pList)
{
	CUIBuyWeaponWnd* this_inventory = dynamic_cast<CUIBuyWeaponWnd*>(pList->GetParent());
	R_ASSERT2(this_inventory, "wrong parent addressed as inventory wnd");

	CUIDragDropItemMP *pDDItemMP = dynamic_cast<CUIDragDropItemMP*>(pItem);
	R_ASSERT(pDDItemMP);

	if(!this_inventory->CanPutInSlot(pDDItemMP, KNIFE_SLOT)) return false;

	this_inventory->SlotToSection(KNIFE_SLOT);
	return true;
}

bool CUIBuyWeaponWnd::SlotProc1(CUIDragDropItem* pItem, CUIDragDropList* pList)
{
	CUIBuyWeaponWnd* this_inventory = dynamic_cast<CUIBuyWeaponWnd*>(pList->GetParent());
	R_ASSERT2(this_inventory, "wrong parent addressed as inventory wnd");

	CUIDragDropItemMP *pDDItemMP = dynamic_cast<CUIDragDropItemMP*>(pItem);
	R_ASSERT(pDDItemMP);

	if(!this_inventory->CanPutInSlot(pDDItemMP, PISTOL_SLOT)) return false;

	this_inventory->SlotToSection(PISTOL_SLOT);
	return true;
}
bool CUIBuyWeaponWnd::SlotProc2(CUIDragDropItem* pItem, CUIDragDropList* pList)
{
	CUIBuyWeaponWnd* this_inventory = dynamic_cast<CUIBuyWeaponWnd*>(pList->GetParent());
	R_ASSERT2(this_inventory, "wrong parent addressed as inventory wnd");

	CUIDragDropItemMP *pDDItemMP = dynamic_cast<CUIDragDropItemMP*>(pItem);
	R_ASSERT(pDDItemMP);

	if(!this_inventory->CanPutInSlot(pDDItemMP, RIFLE_SLOT)) return false;

	this_inventory->SlotToSection(RIFLE_SLOT);
	return true;
}
bool CUIBuyWeaponWnd::SlotProc3(CUIDragDropItem* pItem, CUIDragDropList* pList)
{
	CUIBuyWeaponWnd* this_inventory = dynamic_cast<CUIBuyWeaponWnd*>(pList->GetParent());
	R_ASSERT2(this_inventory, "wrong parent addressed as inventory wnd");

	CUIDragDropItemMP *pDDItemMP = dynamic_cast<CUIDragDropItemMP*>(pItem);
	R_ASSERT(pDDItemMP);

	// Если вещь не граната
	// и если в слоте уже есть 1 граната
	if (!this_inventory->CanPutInSlot(pDDItemMP, GRENADE_SLOT) ||
		!pList->GetDragDropItemsList().empty()) 
		return false;

	return true;
}
bool CUIBuyWeaponWnd::SlotProc4(CUIDragDropItem* pItem, CUIDragDropList* pList)
{
	CUIBuyWeaponWnd* this_inventory = dynamic_cast<CUIBuyWeaponWnd*>(pList->GetParent());
	R_ASSERT2(this_inventory, "wrong parent addressed as inventory wnd");

	CUIDragDropItemMP *pDDItemMP = dynamic_cast<CUIDragDropItemMP*>(pItem);
	R_ASSERT(pDDItemMP);

	if(!this_inventory->CanPutInSlot(pDDItemMP, APPARATUS_SLOT)) return false;

	this_inventory->SlotToSection(APPARATUS_SLOT);
	return true;
}

//одеть костюм
bool CUIBuyWeaponWnd::OutfitSlotProc(CUIDragDropItem* pItem, CUIDragDropList* pList)
{
//	CUIBuyWeaponWnd* this_inventory = dynamic_cast<CUIBuyWeaponWnd*>(pList->GetParent());
//	R_ASSERT2(this_inventory, "wrong parent addressed as inventory wnd");
//
//	PIItem pInvItem = (PIItem)pItem->GetData();
//
//	if(!this_inventory->GetInventory()->CanPutInSlot(pInvItem)) return false;
//
//	if(pInvItem->GetSlot() == OUTFIT_SLOT)
//		return this_inventory->GetInventory()->Slot(pInvItem);
//	else
	return false;
}

//в рюкзак
bool CUIBuyWeaponWnd::BagProc(CUIDragDropItem* pItem, CUIDragDropList* pList)
{
	CUIDragDropItemMP *pDDItemMP = dynamic_cast<CUIDragDropItemMP*>(pItem);

	// У нас не может быть обычная вещь в этом диалоге.
	R_ASSERT(pDDItemMP);
	
	static_cast<CUIDragDropList*>(pDDItemMP->GetParent())->
		DetachChild(pDDItemMP);
	pDDItemMP->GetOwner()->AttachChild(pDDItemMP);

	return false;
}

//на пояс
bool CUIBuyWeaponWnd::BeltProc(CUIDragDropItem* pItem, CUIDragDropList* pList)
{
	CUIBuyWeaponWnd* this_inventory = dynamic_cast<CUIBuyWeaponWnd*>(pList->GetParent());
	R_ASSERT2(this_inventory, "wrong parent addressed as inventory wnd");

	CUIDragDropItemMP *pDDItemMP = dynamic_cast<CUIDragDropItemMP*>(pItem);
	// У нас не может быть обычная вещь в этом диалоге.
	R_ASSERT(pDDItemMP);

	if (!this_inventory->CanPutInBelt(pDDItemMP)) return false;
	
	return true;
}

//------------------------------------------------
//как только подняли элемент, сделать его текущим
void CUIBuyWeaponWnd::SendMessage(CUIWindow *pWnd, s16 msg, void *pData)
{
	if(msg == CUIDragDropItem::ITEM_DRAG)
	{
		PIItem pInvItem = (PIItem)((CUIDragDropItemMP*)pWnd)->GetData();

		SetCurrentItem(pInvItem);
		m_pCurrentDragDropItem = (CUIDragDropItemMP*)pWnd;
	}
	else if(msg == CUIDragDropItem::ITEM_DB_CLICK)
	{
		m_pCurrentDragDropItem = (CUIDragDropItemMP*)pWnd;

		// "Поднять" вещь для освобождения занимаемого места
		SendMessage(m_pCurrentDragDropItem, CUIDragDropItem::ITEM_DRAG, NULL);

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
		m_pCurrentDragDropItem = (CUIDragDropItemMP*)pWnd;

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
			case BUY_ITEM_ACTION:
				SendMessage(m_pCurrentDragDropItem, CUIDragDropItem::ITEM_DB_CLICK, NULL);
				break;
			case CANCEL_BUYING_ACTION:
				m_pCurrentDragDropItem->MoveOnNextDrop();
				UIBagWnd.SendMessage(m_pCurrentDragDropItem, CUIDragDropItem::ITEM_DROP, NULL);
				break;
			case ATTACH_SCOPE_ADDON:
				m_pCurrentDragDropItem->AttachDetachAddon(CUIDragDropItemMP::ID_SCOPE);
				break;
			case ATTACH_SILENCER_ADDON:
				m_pCurrentDragDropItem->AttachDetachAddon(CUIDragDropItemMP::ID_SILENCER);
				break;
			case ATTACH_GRENADE_LAUNCHER_ADDON:
				m_pCurrentDragDropItem->AttachDetachAddon(CUIDragDropItemMP::ID_GRENADE_LAUNCHER);
				break;
			case DETACH_SCOPE_ADDON:
				m_pCurrentDragDropItem->AttachDetachAddon(CUIDragDropItemMP::ID_SCOPE, false);
				break;
			case DETACH_SILENCER_ADDON:
				m_pCurrentDragDropItem->AttachDetachAddon(CUIDragDropItemMP::ID_SILENCER, false);
				break;
			case DETACH_GRENADE_LAUNCHER_ADDON:
				m_pCurrentDragDropItem->AttachDetachAddon(CUIDragDropItemMP::ID_GRENADE_LAUNCHER, false);
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
	else if(pWnd == &UISleepWnd && msg == CUISleepWnd::PERFORM_BUTTON_CLICKED)
	{
		CActor *pActor = dynamic_cast<CActor*>(Level().CurrentEntity());
		if(!pActor) return;

		CUIGameSP* pGameSP = dynamic_cast<CUIGameSP*>(HUD().GetUI()->UIGame());
		if(!pGameSP) return;

		pActor->GoSleep(1000*3600*5);
		StopSleepWnd();
		pGameSP->StartStopMenu(this);

		return;
	}
	else if(pWnd == &UISleepWnd && msg == CUISleepWnd::CLOSE_BUTTON_CLICKED)
	{
		StopSleepWnd();
	}

	//кнопки cheats
	else if(pWnd == &UISleepButton && msg == CUIButton::BUTTON_CLICKED)
	{
		if(UISleepWnd.IsShown())
			StopSleepWnd();
		else
			StartSleepWnd();
		/*

		if(!pActor->IsSleeping())
		{
		CUIGameSP* pGameSP = dynamic_cast<CUIGameSP*>(HUD().GetUI()->UIGame());
		if(!pGameSP) return;

		pActor->GoSleep();

		pGameSP->StartStopMenu(this);

		return;
		}
		else
		pActor->Awoke();
		*/
	}
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
	else if (&UIWeaponsTabControl == pWnd && CUITabControl::TAB_CHANGED == msg)
	{
		m_WeaponSubBags[*static_cast<int*>(pData)]->Show(false);
		m_WeaponSubBags[UIWeaponsTabControl.GetActiveIndex()]->Show(true);
		UIBagWnd.DetachChild(m_WeaponSubBags[*static_cast<int*>(pData)]);
		UIBagWnd.AttachChild(m_WeaponSubBags[UIWeaponsTabControl.GetActiveIndex()]);
	}
	// Кнопки ОК и Отмена
	else if (&UIBtnOK == pWnd && CUIButton::BUTTON_CLICKED == msg)
	{
		HUD().GetUI()->UIGame()->StartStopMenu(this);
		HUD().GetUI()->UIGame()->OnBuyMenu_Ok();
	}
	else if (&UIBtnCancel == pWnd && CUIButton::BUTTON_CLICKED == msg)
	{
		HUD().GetUI()->UIGame()->StartStopMenu(this);
		HUD().GetUI()->UIGame()->OnBuyMenu_Cancel();
	}

	CUIWindow::SendMessage(pWnd, msg, pData);
}


void CUIBuyWeaponWnd::OnMouse(int x, int y, E_MOUSEACTION mouse_action)
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

void CUIBuyWeaponWnd::Draw()
{
	CUIWindow::Draw();
}


void CUIBuyWeaponWnd::Update()
{
//	//CActor *pActor = dynamic_cast<CActor*>(Level().CurrentEntity());
//	CEntityAlive *pEntityAlive = dynamic_cast<CEntityAlive*>(Level().CurrentEntity());
//
//	if(pEntityAlive) 
//	{
//		UIProgressBarHealth.SetProgressPos(s16(pEntityAlive->GetHealth()*1000));
//		UIProgressBarSatiety.SetProgressPos(s16(pEntityAlive->GetSatiety()*1000));
//		UIProgressBarPower.SetProgressPos(s16(pEntityAlive->GetPower()*1000));
//		UIProgressBarRadiation.SetProgressPos(s16(pEntityAlive->GetRadiation()*1000));
//
//		//убрать объект drag&drop для уже использованной вещи
//		for(int i = 0; i <m_iUsedItems; i++) 
//		{
//			CInventoryItem* pItem = (CInventoryItem*)m_vDragDropItems[i].GetData();
//			if(pItem && !pItem->Useful())
//			{
//				m_vDragDropItems[i].GetParent()->DetachChild(&m_vDragDropItems[i]);
//				m_vDragDropItems[i].SetData(NULL);
//				m_vDragDropItems[i].SetCustomUpdate(NULL);
//
//				if(m_pCurrentItem == pItem)
//				{	
//					SetCurrentItem(NULL);
//					m_pCurrentDragDropItem = NULL;
//				}
//			}
//		}
//	}

	if (wpnSectStorage[GRENADE_SLOT].size() != m_WeaponSubBags[GRENADE_SLOT]->GetDragDropItemsList().size())
	{
		ClearWpnSubBag(GRENADE_SLOT);
		FillWpnSubBag(GRENADE_SLOT);
	}
	CUIWindow::Update();
}

void CUIBuyWeaponWnd::DropItem()
{
	CActor *pActor = dynamic_cast<CActor*>(Level().CurrentEntity());
	if(!pActor) return;

	m_pCurrentItem->Drop();

	(dynamic_cast<CUIDragDropList*>(m_pCurrentDragDropItem->GetParent()))->
		DetachChild(m_pCurrentDragDropItem);

	SetCurrentItem(NULL);
	m_pCurrentDragDropItem = NULL;
}

/*
void CUIBuyWeaponWnd::EatItem()
{
	CActor *pActor = dynamic_cast<CActor*>(Level().CurrentEntity());
	if(!pActor) return;

	pActor->inventory().Eat(m_pCurrentItem);

	if(!m_pCurrentItem->Useful())
	{
		(dynamic_cast<CUIDragDropList*>(m_pCurrentDragDropItem->GetParent()))->
			DetachChild(m_pCurrentDragDropItem);
		SetCurrentItem(NULL);
		m_pCurrentDragDropItem = NULL;
	}

	/ *m_pCurrentItem->Drop();

	NET_Packet P;
	pActor->u_EventGen(P,GE_DESTROY,m_pCurrentItem->ID());
	P.w_u16(u16(m_pCurrentItem->ID()));
	pActor->u_EventSend(P);* /
}
*/


void CUIBuyWeaponWnd::Show() 
{ 
	/*	//спрятать вещь из активного слота в инвентарь на время вызова менюшки
	CActor *pActor = dynamic_cast<CActor*>(Level().CurrentEntity());
	if(pActor)
	{
	m_iCurrentActiveSlot = pActor->inventory().GetActiveSlot();

	if(pActor->inventory().ActiveItem())
	{
	CWeapon* pWeapon = dynamic_cast<CWeapon*>(pActor->inventory().ActiveItem());
	if(pWeapon) pWeapon->ResetPending();
	}
	pActor->inventory().Activate(NO_ACTIVE_SLOT);
	}


	*/
	InitInventory();
	inherited::Show();
}

void CUIBuyWeaponWnd::Hide()
{
	//достать вещь в активный слот
//	CActor *pActor = dynamic_cast<CActor*>(Level().CurrentEntity());
//	if(pActor && m_iCurrentActiveSlot != NO_ACTIVE_SLOT && 
//		pActor->inventory().m_slots[m_iCurrentActiveSlot].m_pIItem)
//	{
//		pActor->inventory().Activate(m_iCurrentActiveSlot);
//		m_iCurrentActiveSlot = NO_ACTIVE_SLOT;
//	}

	// For test purposes
//	const u8 a = GetWeaponIndexInBelt(0);
//	const u8 e = GetWeaponIndexInBelt(1);
//	const char *b = GetWeaponNameInBelt(2);
//	const char *c = GetWeaponNameInBelt(3);
//	const char *d = GetWeaponNameInBelt(GetBeltSize() - 1);

	inherited::Hide();
}


// Контекстное меню
void CUIBuyWeaponWnd::ActivatePropertiesBox()
{
	int x,y;
	RECT rect = GetAbsoluteRect();
	HUD().GetUI()->GetCursor()->GetPos(x,y);

	UIPropertiesBox.RemoveAll();

	if(m_pCurrentDragDropItem->GetSlot() < MP_SLOTS_NUM && &UIBagWnd == m_pCurrentDragDropItem->GetParent()->GetParent())
	{
		UIPropertiesBox.AddItem("Buy Item",  NULL, BUY_ITEM_ACTION);
		// Так как оружие еще в сумке, то просматриваем его список аддонов и модифицируем меню
		if (m_pCurrentDragDropItem->bAddonsAvailable)
		{
			std::string strMenuItem;
			for (int i = 0; i < 3; ++i)
			{
				switch (m_pCurrentDragDropItem->m_AddonInfo[i].iAttachStatus)
				{
				// If addon detached
				case 0:
					strMenuItem = std::string("Attach ") + m_pCurrentDragDropItem->m_strAddonTypeNames[i];
					UIPropertiesBox.AddItem(strMenuItem.c_str(), NULL, ATTACH_SCOPE_ADDON + i);
					break;
				case 1:
					strMenuItem = std::string("Detach ") + m_pCurrentDragDropItem->m_strAddonTypeNames[i];
					UIPropertiesBox.AddItem(strMenuItem.c_str(), NULL, DETACH_SCOPE_ADDON + i);
					break;
				default:
					continue;
				}
			}
		}
	}
	else
	{
		UIPropertiesBox.AddItem("Cancel", NULL, CANCEL_BUYING_ACTION);
	}

	UIPropertiesBox.AutoUpdateSize();
	UIPropertiesBox.BringAllToTop();
	UIPropertiesBox.Show(x-rect.left, y-rect.top);
}


bool CUIBuyWeaponWnd::ToSlot()
{
	// Проверить, что вещь лежит в сумке, а нев слоте
	if (m_pCurrentDragDropItem->GetParent()->GetParent() != &UIBagWnd) return false;

	// Специальное поведение для гранат, так как они у нас не кончаются
	if (GRENADE_SLOT == m_pCurrentDragDropItem->GetSlot() && 
		!UITopList[GRENADE_SLOT].GetDragDropItemsList().empty()) return false;

	SlotToSection(m_pCurrentDragDropItem->GetSlot());

	m_pCurrentDragDropItem->MoveOnNextDrop();
	UITopList[m_pCurrentDragDropItem->GetSlot()].SendMessage(m_pCurrentDragDropItem, CUIDragDropItem::ITEM_DROP, NULL);

	m_pMouseCapturer = NULL;

	return true;
}

bool CUIBuyWeaponWnd::ToBag()
{
	bool retStat = true;

	// Проверить, что вещь лежит в сумке, а не в слоте
	if (m_pCurrentDragDropItem->GetParent()->GetParent() == &UIBagWnd) return false;

	// Если вещь на поясе, то выкидываем ее в инвентарь
	retStat = BeltToSection(m_pCurrentDragDropItem);
	if (retStat) return retStat;

	// Если вещь в слоте
	retStat = SlotToSection(m_pCurrentDragDropItem->GetSlot());
	if (retStat) return retStat;
	return false;
}

bool CUIBuyWeaponWnd::ToBelt()
{
	// Ecли вещь не впоясобросаемая
	// Если вещь уже на поясе
	// Если места больше нет
	if (!CanPutInBelt(m_pCurrentDragDropItem) ||
		&UITopList[BELT_SLOT] == m_pCurrentDragDropItem->GetParent() ||
		!UITopList[BELT_SLOT].CanPlaceItem(m_pCurrentDragDropItem)) 
		return false;

	static_cast<CUIDragDropList*>(m_pCurrentDragDropItem->GetParent())->DetachChild(m_pCurrentDragDropItem);
	UITopList[BELT_SLOT].AttachChild(m_pCurrentDragDropItem);

	m_pMouseCapturer = NULL;

	return true;
}

//запуск и остановка меню работы с артефактами
void CUIBuyWeaponWnd::StartArtifactMerger()
{
	UIArtifactMergerWnd.InitArtifactMerger(dynamic_cast<CArtifactMerger*>(m_pCurrentItem));
	UIArtifactMergerWnd.Show();
}
void CUIBuyWeaponWnd::StopArtifactMerger()
{
	UIArtifactMergerWnd.Hide();

	//скинуть все элементы из усторйства артефактов в рюкзак
	for(DRAG_DROP_LIST_it it = UIArtifactMergerWnd.UIArtifactList.GetDragDropItemsList().begin(); 
		UIArtifactMergerWnd.UIArtifactList.GetDragDropItemsList().end() != it;
		++it)
	{
		CUIDragDropItem* pDragDropItem = *it;
//		UIBagList.AttachChild(pDragDropItem);
		m_WeaponSubBags[UIWeaponsTabControl.GetActiveIndex()]->AttachChild(pDragDropItem);
	}

	//((CUIDragDropList*)pDragDropItem->GetParent())->DetachChild(pDragDropItem);
	UIArtifactMergerWnd.UIArtifactList.DropAll();
}

//для работы с сочетателем артефактом извне
/*
void CUIBuyWeaponWnd::AddArtifactToMerger(CArtifact* pArtifact)
{
	CUIDragDropItem& UIDragDropItem = m_vDragDropItems[m_iUsedItems];
	++m_iUsedItems; R_ASSERT(m_iUsedItems<MAX_ITEMS);

	UIDragDropItem.CUIStatic::Init(0,0, 50,50);
	UIDragDropItem.SetShader(GetEquipmentIconsShader());
	UIDragDropItem.SetGridHeight(pArtifact->GetGridHeight());
	UIDragDropItem.SetGridWidth(pArtifact->GetGridWidth());
	UIDragDropItem.GetUIStaticItem().SetOriginalRect(
		pArtifact->GetXPos()*INV_GRID_WIDTH,
		pArtifact->GetYPos()*INV_GRID_HEIGHT,
		pArtifact->GetGridWidth()*INV_GRID_WIDTH,
		pArtifact->GetGridHeight()*INV_GRID_HEIGHT);
	UIDragDropItem.SetData(pArtifact);
	UIArtifactMergerWnd.UIArtifactList.AttachChild(&UIDragDropItem);
}
*/

//void CUIBuyWeaponWnd::AddItemToBag(PIItem pItem)
//{
//	CUIDragDropItem& UIDragDropItem = m_vDragDropItems[m_iUsedItems];
//	m_iUsedItems++; R_ASSERT(m_iUsedItems<MAX_ITEMS);
//
//	UIDragDropItem.CUIStatic::Init(0,0, 50,50);
//	UIDragDropItem.SetShader(GetEquipmentIconsShader());
//	UIDragDropItem.SetGridHeight(pItem->GetGridHeight());
//	UIDragDropItem.SetGridWidth(pItem->GetGridWidth());
//	UIDragDropItem.GetUIStaticItem().SetOriginalRect(
//		pItem->GetXPos()*INV_GRID_WIDTH,
//		pItem->GetYPos()*INV_GRID_HEIGHT,
//		pItem->GetGridWidth()*INV_GRID_WIDTH,
//		pItem->GetGridHeight()*INV_GRID_HEIGHT);
//	if(m_pInv->Get(pItem->ID(), false))
//		UIBeltList.AttachChild(&UIDragDropItem);
//	else if(m_pInv->Get(pItem->ID(), true))
//		m_WeaponSubBags[UIWeaponsTabControl.GetActiveIndex()]->AttachChild(&UIDragDropItem);
////		UIBagList.AttachChild(&UIDragDropItem);
//
//
//	UIDragDropItem.SetData(pItem);
//}

/*
void CUIBuyWeaponWnd::AttachAddon()
{
	R_ASSERT(m_pItemToUpgrade);
	m_pItemToUpgrade->Attach(m_pCurrentItem);


	//спрятать вещь из активного слота в инвентарь на время вызова менюшки
	CActor *pActor = dynamic_cast<CActor*>(Level().CurrentEntity());
	if(pActor)
	{
		if(m_pItemToUpgrade == pActor->inventory().ActiveItem())
		{
			m_iCurrentActiveSlot = pActor->inventory().GetActiveSlot();
			pActor->inventory().Activate(NO_ACTIVE_SLOT);
		}
	}




	(dynamic_cast<CUIDragDropList*>(m_pCurrentDragDropItem->GetParent()))->
		DetachChild(m_pCurrentDragDropItem);
	SetCurrentItem(NULL);
	m_pCurrentDragDropItem = NULL;

	m_pItemToUpgrade = NULL;
}
void CUIBuyWeaponWnd::DetachAddon(const char* addon_name)
{
	m_pCurrentItem->Detach(addon_name);


	//спрятать вещь из активного слота в инвентарь на время вызова менюшки
	CActor *pActor = dynamic_cast<CActor*>(Level().CurrentEntity());
	if(pActor)
	{
		if(m_pCurrentItem == pActor->inventory().ActiveItem())
		{
			m_iCurrentActiveSlot = pActor->inventory().GetActiveSlot();
			pActor->inventory().Activate(NO_ACTIVE_SLOT);
		}
	}
}
*/

void CUIBuyWeaponWnd::SetCurrentItem(CInventoryItem* pItem)
{
	m_pCurrentItem = pItem;
	UIItemInfo.InitItem(m_pCurrentItem);
}
/////////////////////////////////////////////////
//запуск и остановка диалога параметров сна
void  CUIBuyWeaponWnd::StartSleepWnd()
{
	UISleepWnd.InitSleepWnd();
	UISleepWnd.Show();
	UISleepButton.Enable(false);
	UISleepButton.Show(false);

}
void  CUIBuyWeaponWnd::StopSleepWnd()
{
	UISleepWnd.Hide();

	UISleepButton.Reset();
	UISleepButton.Enable(true);
	UISleepButton.Show(true);
}

//-----------------------------------------------------------------------------/
//  Buy weapon stuff
//-----------------------------------------------------------------------------/
void CUIBuyWeaponWnd::InitWpnSectStorage()
{
	WPN_SECT_NAMES		wpnOneType;
	string16			wpnSection;	
	string256			wpnNames, wpnSingleName;

	// Поле strSectionName должно содержать имя секции
	R_ASSERT(xr_strcmp(m_SectionName,""));

	for (int i = 1; i < 20; ++i)
	{
		// Очищаем буфер
		wpnOneType.clear();

		// Имя поля
		sprintf(wpnSection, "slot%i", i);
		if (!pSettings->line_exist(m_SectionName, wpnSection)) 
		{
			wpnSectStorage.push_back(wpnOneType);
			continue;
		}

		// Читаем данные этого поля
		std::strcpy(wpnNames, pSettings->r_string(m_SectionName, wpnSection));
		u32 count	= _GetItemCount(wpnNames);
		// теперь для каждое имя оружия, разделенные запятыми, заносим в массив
		for (u32 i = 0; i < count; ++i)
		{
			_GetItem(wpnNames, i, wpnSingleName);
			wpnOneType.push_back(wpnSingleName);
		}

		if (!wpnOneType.empty())
			wpnSectStorage.push_back(wpnOneType);
	}
}

void CUIBuyWeaponWnd::FillWpnSubBags()
{
	for (WPN_LISTS::size_type i = 0; i < wpnSectStorage.size(); ++i)
	{
		FillWpnSubBag(static_cast<u32>(i));
	}
}

void CUIBuyWeaponWnd::FillWpnSubBag(const u32 slotNum)
{
	for (WPN_SECT_NAMES::size_type j = 0; j < wpnSectStorage[slotNum].size(); ++j)
	{
		CUIDragDropItemMP& UIDragDropItem = m_vDragDropItems[GetFirstFreeIndex()];

		UIDragDropItem.CUIStatic::Init(0,0, 50,50);
		UIDragDropItem.SetShader(GetEquipmentIconsShader());

		//properties used by inventory menu
		int m_iGridWidth	= pSettings->r_u32(wpnSectStorage[slotNum][j].c_str(), "inv_grid_width");
		int m_iGridHeight	= pSettings->r_u32(wpnSectStorage[slotNum][j].c_str(), "inv_grid_height");

		int m_iXPos			= pSettings->r_u32(wpnSectStorage[slotNum][j].c_str(), "inv_grid_x");
		int m_iYPos			= pSettings->r_u32(wpnSectStorage[slotNum][j].c_str(), "inv_grid_y");

		u32 m_slot;

		if(pSettings->line_exist(wpnSectStorage[slotNum][j].c_str(), "slot"))
			m_slot = pSettings->r_u32(wpnSectStorage[slotNum][j].c_str(), "slot");
		else
			m_slot = NO_ACTIVE_SLOT;
		
		InitAddonsInfo(UIDragDropItem, wpnSectStorage[slotNum][j]);

		UIDragDropItem.SetSlot(m_slot);
		UIDragDropItem.SetSection(slotNum);

		UIDragDropItem.SetGridHeight(m_iGridHeight);
		UIDragDropItem.SetGridWidth(m_iGridWidth);

		UIDragDropItem.GetUIStaticItem().SetOriginalRect(
			m_iXPos * INV_GRID_WIDTH,
			m_iYPos * INV_GRID_HEIGHT,
			m_iGridWidth * INV_GRID_WIDTH,
			m_iGridHeight * INV_GRID_HEIGHT);

		// Количество доступных секций должно быть не больше затребованных
		R_ASSERT(slotNum < m_WeaponSubBags.size());
		m_WeaponSubBags[slotNum]->AttachChild(&UIDragDropItem);
		UIDragDropItem.SetAutoDelete(false);

		// Сохраняем указатель на лист - "хозяин" веши
		UIDragDropItem.SetOwner(m_WeaponSubBags[slotNum]);

		UIDragDropItem.SetSectionName(wpnSectStorage[slotNum][j].c_str());
	}
}

void CUIBuyWeaponWnd::InitAddonsInfo(CUIDragDropItemMP &DDItemMP, const std::string &sectioName)
{
	// Для каждого аддона проверяем его запись в ltx файле, и заполняем структуру
	// -- SCOPE --
	if (pSettings->line_exist(sectioName.c_str(), "scope_status") && 
		pSettings->r_u32(sectioName.c_str(), "scope_status") == 2)
	{
		DDItemMP.bAddonsAvailable = true;
		DDItemMP.m_AddonInfo[CUIDragDropItemMP::ID_SCOPE].iAttachStatus = 0;
		DDItemMP.m_AddonInfo[CUIDragDropItemMP::ID_SCOPE].strAddonName = pSettings->r_string(sectioName.c_str(), "scope_name");
		DDItemMP.m_AddonInfo[CUIDragDropItemMP::ID_SCOPE].x = pSettings->r_u32(sectioName.c_str(), "scope_x");
		DDItemMP.m_AddonInfo[CUIDragDropItemMP::ID_SCOPE].y = pSettings->r_u32(sectioName.c_str(), "scope_y");
		DDItemMP.m_UIStaticSilencer.SetShader(InventoryUtilities::GetEquipmentIconsShader());

		// Теперь инициализируем изображение

		DDItemMP.m_UIStaticScope.SetShader(InventoryUtilities::GetEquipmentIconsShader());

		const char* section  = DDItemMP.m_AddonInfo[CUIDragDropItemMP::ID_SCOPE].strAddonName.c_str();
		u32 iGridWidth = pSettings->r_u32(section, "inv_grid_width");
		u32 iGridHeight = pSettings->r_u32(section, "inv_grid_height");
		u32 iXPos = pSettings->r_u32(section, "inv_grid_x");
		u32 iYPos = pSettings->r_u32(section, "inv_grid_y");

		DDItemMP.m_UIStaticScope.SetOriginalRect(
			iXPos*INV_GRID_WIDTH, iYPos*INV_GRID_HEIGHT,
			iGridWidth*INV_GRID_WIDTH, iGridHeight*INV_GRID_HEIGHT);
	}

	// -- SILENCER --
	if (pSettings->line_exist(sectioName.c_str(), "silencer_status") && 
		pSettings->r_u32(sectioName.c_str(), "silencer_status") == 2)
	{
		DDItemMP.bAddonsAvailable = true;
		DDItemMP.m_AddonInfo[CUIDragDropItemMP::ID_SILENCER].iAttachStatus = 0;
		DDItemMP.m_AddonInfo[CUIDragDropItemMP::ID_SILENCER].strAddonName = pSettings->r_string(sectioName.c_str(), "silencer_name");
		DDItemMP.m_AddonInfo[CUIDragDropItemMP::ID_SILENCER].x = pSettings->r_u32(sectioName.c_str(), "silencer_x");
		DDItemMP.m_AddonInfo[CUIDragDropItemMP::ID_SILENCER].y = pSettings->r_u32(sectioName.c_str(), "silencer_y");

		// Теперь инициализируем изображение

		DDItemMP.m_UIStaticSilencer.SetShader(InventoryUtilities::GetEquipmentIconsShader());

		const char* section  = DDItemMP.m_AddonInfo[CUIDragDropItemMP::ID_SILENCER].strAddonName.c_str();
		u32 iGridWidth = pSettings->r_u32(section, "inv_grid_width");
		u32 iGridHeight = pSettings->r_u32(section, "inv_grid_height");
		u32 iXPos = pSettings->r_u32(section, "inv_grid_x");
		u32 iYPos = pSettings->r_u32(section, "inv_grid_y");

		DDItemMP.m_UIStaticSilencer.SetOriginalRect(
			iXPos*INV_GRID_WIDTH, iYPos*INV_GRID_HEIGHT,
			iGridWidth*INV_GRID_WIDTH, iGridHeight*INV_GRID_HEIGHT);
	}

	// -- GL --
	if (pSettings->line_exist(sectioName.c_str(), "grenade_launcher_status") && 
		pSettings->r_u32(sectioName.c_str(), "grenade_launcher_status") == 2)
	{
		DDItemMP.bAddonsAvailable = true;
		DDItemMP.m_AddonInfo[CUIDragDropItemMP::ID_GRENADE_LAUNCHER].iAttachStatus = 0;
		DDItemMP.m_AddonInfo[CUIDragDropItemMP::ID_GRENADE_LAUNCHER].strAddonName = pSettings->r_string(sectioName.c_str(), "grenade_launcher_name");
		DDItemMP.m_AddonInfo[CUIDragDropItemMP::ID_GRENADE_LAUNCHER].x = pSettings->r_u32(sectioName.c_str(), "grenade_launcher_x");
		DDItemMP.m_AddonInfo[CUIDragDropItemMP::ID_GRENADE_LAUNCHER].y = pSettings->r_u32(sectioName.c_str(), "grenade_launcher_y");

		// Теперь инициализируем изображение

		DDItemMP.m_UIStaticGrenadeLauncher.SetShader(InventoryUtilities::GetEquipmentIconsShader());

		const char* section  = DDItemMP.m_AddonInfo[CUIDragDropItemMP::ID_GRENADE_LAUNCHER].strAddonName.c_str();
		u32 iGridWidth = pSettings->r_u32(section, "inv_grid_width");
		u32 iGridHeight = pSettings->r_u32(section, "inv_grid_height");
		u32 iXPos = pSettings->r_u32(section, "inv_grid_x");
		u32 iYPos = pSettings->r_u32(section, "inv_grid_y");

		DDItemMP.m_UIStaticGrenadeLauncher.SetOriginalRect(
			iXPos*INV_GRID_WIDTH, iYPos*INV_GRID_HEIGHT,
			iGridWidth*INV_GRID_WIDTH, iGridHeight*INV_GRID_HEIGHT);
	}
}

void CUIBuyWeaponWnd::ClearWpnSubBags()
{
	for (WEAPON_TYPES::size_type i = 0; i < m_WeaponSubBags.size(); ++i)
	{
		ClearWpnSubBag(static_cast<u32>(i));
	}
}

void CUIBuyWeaponWnd::ClearWpnSubBag(const u32 slotNum)
{
	for (DRAG_DROP_LIST_it it = m_WeaponSubBags[slotNum]->GetDragDropItemsList().begin(); it != m_WeaponSubBags[slotNum]->GetDragDropItemsList().end(); ++it)
	{
		for (int i = 0; i < m_iUsedItems; ++i)
		{
			if (&m_vDragDropItems[i] == (*it))
				m_iEmptyIndexes.insert(i);
		}
	}
	m_WeaponSubBags[slotNum]->DropAll();
}

bool CUIBuyWeaponWnd::SlotToSection(const u32 SlotNum)
{
	if (SlotNum >= MP_SLOTS_NUM) return false;
	// Выкидываем вещь из слота если есть.
	if (!UITopList[SlotNum].GetDragDropItemsList().empty())
	{
		CUIDragDropItemMP *pDDItemMP = dynamic_cast<CUIDragDropItemMP*>(*UITopList[SlotNum].GetDragDropItemsList().begin());
		if (pDDItemMP)
		// Берем текущее оружие в слоте...
		pDDItemMP->MoveOnNextDrop();
		// ...и посылаем ему сообщение переместиться в сумку
		m_WeaponSubBags[pDDItemMP->GetSection()]->SendMessage(pDDItemMP, 
									CUIDragDropItem::ITEM_DROP, NULL);
	}

	return true;
}

bool CUIBuyWeaponWnd::CanPutInSlot(CUIDragDropItemMP *pDDItemMP, const u32 slotNum)
{
	return slotNum == pDDItemMP->GetSlot();
}

const char * CUIBuyWeaponWnd::GetWeaponName(u32 slotNum)
{
	CUIDragDropItemMP *pDDItemMP = GetWeapon(slotNum, 0);
	if (pDDItemMP)
		return pDDItemMP->GetSectionName();
	return NULL;
}

const char * CUIBuyWeaponWnd::GetWeaponNameInBelt(u32 indexInBelt)
{
	CUIDragDropItemMP *pDDItemMP = GetWeapon(BELT_SLOT, indexInBelt);
	if (pDDItemMP)
		return pDDItemMP->GetSectionName();
	return NULL;
}

const u8 CUIBuyWeaponWnd::GetItemIndex(u32 slotNum, u32 idxInArr)
{
	u8 returnID = static_cast<u8>(-1);

	CUIDragDropItemMP *pDDItemMP = GetWeapon(slotNum, idxInArr);
	if (!pDDItemMP) return returnID;

	for (WPN_LISTS::const_iterator it = wpnSectStorage.begin(); it != wpnSectStorage.end(); ++it)
	{
		WPN_SECT_NAMES::difference_type diff = std::distance((*it).begin(),
			std::find((*it).begin(), (*it).end(), pDDItemMP->GetSectionName()));
		if (diff < static_cast<WPN_SECT_NAMES::difference_type>((*it).size()))
		{
			returnID = static_cast<u8>(diff);

			// Проверяем на наличие приаттаченых аддонов к оружию
			if (pDDItemMP->bAddonsAvailable)
			{
				u8	flags = 0;
				for (int i = 0; i < 3; ++i)
				{
					if (1 == pDDItemMP->m_AddonInfo[i].iAttachStatus)
						flags |= 1;

					flags = flags << 1;
				}
				flags = flags << 4;

				// В результате старшие 3 бита являются флагами признаков аддонов:
				// FF - Scope, FE - Silencer, FD - Grenade Launcher
				returnID |= flags;
			}
			return returnID;
		}
	}
	return returnID;
}

const u8 CUIBuyWeaponWnd::GetWeaponIndex(u32 slotNum)
{
	return GetItemIndex(slotNum, 0);
}

const u8 CUIBuyWeaponWnd::GetWeaponIndexInBelt(u32 indexInBelt)
{
	return GetItemIndex(BELT_SLOT, indexInBelt);
}

const char * CUIBuyWeaponWnd::GetWeaponNameByIndex(u32 slotNum, u8 idx)
{
	// Удаляем информацию о аддонах
	idx &= 0x1f;

	if (wpnSectStorage.size() <= slotNum || idx > wpnSectStorage[slotNum].size()) return NULL;
	return wpnSectStorage[slotNum][idx].c_str();
}

CUIBuyWeaponWnd::CUIDragDropItemMP * CUIBuyWeaponWnd::GetWeapon(u32 slotNum, u32 idx)
{
	R_ASSERT(slotNum < MP_SLOTS_NUM);
	if (UITopList[slotNum].GetDragDropItemsList().empty()) return NULL;
	R_ASSERT(idx < UITopList[slotNum].GetDragDropItemsList().size());


	DRAG_DROP_LIST_it it = UITopList[slotNum].GetDragDropItemsList().begin(); 
	std::advance(it, idx);
	return dynamic_cast<CUIDragDropItemMP*>(*it);
}

const u8 GetWeaponAddonInfoByIndex(u8 idx)
{
	return (idx & 0xe0) >> 5;
}

bool CUIBuyWeaponWnd::CanPutInBelt(CUIDragDropItemMP *pDDItemMP)
{
	return pDDItemMP->GetSlot() == GRENADE_SLOT;
}

bool CUIBuyWeaponWnd::BeltToSection(CUIDragDropItemMP *pDDItemMP)
{
	R_ASSERT(pDDItemMP);

	if (&UITopList[BELT_SLOT] != pDDItemMP->GetParent()) return false;
	// Берем вещь
	pDDItemMP->MoveOnNextDrop();
	// ...и посылаем ему сообщение переместиться в сумку
	m_WeaponSubBags[pDDItemMP->GetSection()]->SendMessage(pDDItemMP, CUIDragDropItem::ITEM_DROP, NULL);
	return true;
}

int CUIBuyWeaponWnd::GetFirstFreeIndex()
{
	R_ASSERT(m_iUsedItems < MAX_ITEMS);

	if (!m_iEmptyIndexes.empty())
	{
		int result = *m_iEmptyIndexes.begin();
		m_iEmptyIndexes.erase(result);
		return result;
	}
	return m_iUsedItems++;
}

const u8 CUIBuyWeaponWnd::GetBeltSize()
{
	return static_cast<u8>(UITopList[BELT_SLOT].GetDragDropItemsList().size());
}

//-----------------------------------------------------------------------------/
//  CUIDragDropItemMP class
//-----------------------------------------------------------------------------/

void CUIBuyWeaponWnd::CUIDragDropItemMP::AttachDetachAddon(int iAddonIndex, bool bAttach)
{
	R_ASSERT(iAddonIndex >= 0 && iAddonIndex < 4);
	if (m_AddonInfo[iAddonIndex].iAttachStatus != -1)
		m_AddonInfo[iAddonIndex].iAttachStatus = bAttach ? 1 : 0;
}

void CUIBuyWeaponWnd::CUIDragDropItemMP::ClipperOff()
{
	inherited::ClipperOff();

	if(1 == m_AddonInfo[ID_SCOPE].iAttachStatus) inherited::ClipperOff(m_UIStaticScope);
	if(1 == m_AddonInfo[ID_SILENCER].iAttachStatus)  inherited::ClipperOff(m_UIStaticSilencer);
	if(1 == m_AddonInfo[ID_GRENADE_LAUNCHER].iAttachStatus)  inherited::ClipperOff(m_UIStaticGrenadeLauncher);
}

void CUIBuyWeaponWnd::CUIDragDropItemMP::ClipperOn()
{
	inherited::ClipperOn();

	if(1 == m_AddonInfo[ID_SCOPE].iAttachStatus) this->TextureClipper(0,0,NULL, m_UIStaticScope);
	if(1 == m_AddonInfo[ID_SILENCER].iAttachStatus)  this->TextureClipper(0,0,NULL, m_UIStaticSilencer);
	if(1 == m_AddonInfo[ID_GRENADE_LAUNCHER].iAttachStatus)  this->TextureClipper(0,0,NULL, m_UIStaticGrenadeLauncher);
}

void CUIBuyWeaponWnd::CUIDragDropItemMP::Draw()
{
	inherited::Draw();

	RECT rect = GetAbsoluteRect();

	//отцентрировать родительскую иконку по центру ее окна
	int right_offset = (GetWidth()-m_UIStaticItem.GetOriginalRectScaled().width())/2;
	int down_offset = (GetHeight()-m_UIStaticItem.GetOriginalRectScaled().height())/2;

	if(1 == m_AddonInfo[ID_SCOPE].iAttachStatus)
	{
		m_UIStaticScope.SetPos(rect.left + right_offset + iFloor(0.5f+(float)m_AddonInfo[ID_SCOPE].x * GetTextureScale()),
			rect.top + down_offset + iFloor(0.5f+(float)m_AddonInfo[ID_SCOPE].y * GetTextureScale()));
		if(m_bClipper) TextureClipper(right_offset + iFloor(0.5f+(float)m_AddonInfo[ID_SCOPE].x * GetTextureScale()),
			down_offset + iFloor(0.5f+(float)m_AddonInfo[ID_SCOPE].y * GetTextureScale()),
			NULL, m_UIStaticScope);
		m_UIStaticScope.Render();
	}
	if(1 == m_AddonInfo[ID_SILENCER].iAttachStatus)
	{
		m_UIStaticSilencer.SetPos(rect.left + right_offset + iFloor(0.5f+(float)m_AddonInfo[ID_SILENCER].x * GetTextureScale()),
			rect.top + down_offset + iFloor(0.5f+(float)m_AddonInfo[ID_SILENCER].y * GetTextureScale()));

		if(m_bClipper) TextureClipper(right_offset + iFloor(0.5f+(float)m_AddonInfo[ID_SILENCER].x * GetTextureScale()),
			down_offset +  iFloor(0.5f+(float)m_AddonInfo[ID_SILENCER].y * GetTextureScale()), 
			NULL, m_UIStaticSilencer);

		m_UIStaticSilencer.Render();
	}
	if(1 == m_AddonInfo[ID_GRENADE_LAUNCHER].iAttachStatus)
	{
		m_UIStaticGrenadeLauncher.SetPos(rect.left + right_offset + iFloor(0.5f+(float)m_AddonInfo[ID_GRENADE_LAUNCHER].x * GetTextureScale()),
			rect.top + down_offset + iFloor(0.5f+(float)m_AddonInfo[ID_GRENADE_LAUNCHER].y * GetTextureScale()));
		if(m_bClipper) TextureClipper(right_offset + iFloor(0.5f+(float)m_AddonInfo[ID_GRENADE_LAUNCHER].x * GetTextureScale()),
			down_offset +  iFloor(0.5f+(float)m_AddonInfo[ID_GRENADE_LAUNCHER].y * GetTextureScale()), 
			NULL, m_UIStaticGrenadeLauncher);
		m_UIStaticGrenadeLauncher.Render();

	}
}