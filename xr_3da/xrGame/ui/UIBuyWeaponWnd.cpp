// UIBuyWeaponWnd.cpp:	окошко, для покупки оружия в режиме CS
//						
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <dinput.h>
#include "UIBuyWeaponWnd.h"

#include "xrXMLParser.h"
#include "UIXmlInit.h"
#include "../hudmanager.h"
#include "UISkinSelector.h"
#include "../uigamesp.h"
#include "../level.h"
#include "UIInventoryUtilities.h"
#include "../InfoPortion.h"
#include "../game_cl_Deathmatch.h"
#include "../string_table.h"



//////////////////////////////////////////////////////////////////////////

using namespace InventoryUtilities;

//////////////////////////////////////////////////////////////////////////


int			g_iOkAccelerator, g_iCancelAccelerator;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUIBuyWeaponWnd::CUIBuyWeaponWnd(LPCSTR strSectionName, LPCSTR strPricesSection)
		: UIBagWnd(BagProc)
{
	m_StrSectionName		= NULL;
	m_StrPricesSection		= strPricesSection;
	m_iCurrentActiveSlot	= NO_ACTIVE_SLOT;
	Hide();

	m_pCurrentDragDropItem = NULL;
	m_iMoneyAmount = 10000;

//	m_iUsedItems	= 0;
	m_iIconTextureY	= 0;
	m_iIconTextureX	= 0;

	SetFont(HUD().Font().pFontMedium);

//	m_mlCurrLevel	= mlRoot;
	m_bIgnoreMoney	= false;

	// Инициализируем вещи
	Init(strSectionName, strPricesSection);

	Hide();
}

CUIBuyWeaponWnd::~CUIBuyWeaponWnd()
{
}

void CUIBuyWeaponWnd::Init(LPCSTR strSectionName, LPCSTR strPricesSection)
{
	m_StrSectionName = strSectionName;

	CUIXml uiXml;
	bool xml_result = uiXml.Init(CONFIG_PATH, UI_PATH, BUY_WND_XML_NAME);
	R_ASSERT2(xml_result, "xml file not found");

	inherited::DetachAll();

	CUIXmlInit xml_init;

	CUIWindow::Init(CUIXmlInit::ApplyAlignX(0, alCenter),
					CUIXmlInit::ApplyAlignY(0, alCenter),
					UI_BASE_WIDTH, UI_BASE_WIDTH);

	AttachChild(&UIStaticTop);
	xml_init.InitStatic(uiXml, "slots_static", 0, &UIStaticTop);

	AttachChild(&UIStaticBelt);
	xml_init.InitStatic(uiXml, "static", 0, &UIStaticBelt);

	AttachChild(&UIBagWnd);
	UIBagWnd.Init(uiXml,"bag_background_static", strSectionName, strPricesSection);
//	xml_init.InitStatic(uiXml, "bag_background_static", 0, &UIBagWnd);

	////////////////////////////////////////
	// окно с описанием активной вещи

	AttachChild(&UIDescWnd);
	xml_init.InitStatic(uiXml, "desc_static", 0, &UIDescWnd);

	////////////////////////////////////
	//Окно с информации о персонаже
	AttachChild(&UIPersonalWnd);
	xml_init.InitStatic(uiXml, "personal_static", 0, &UIPersonalWnd);

	// Tabcontrol для оружия
	AttachChild(&UITabControl);
	UITabControl.Init(uiXml, "tab");

	BringToTop(&UITabControl);	

	// Кнопки OK и Cancel для выходи из диалога покупки оружия
	AttachChild(&UIBtnOK);
	xml_init.Init3tButton(uiXml, "ok_button", 0, &UIBtnOK);
	UIBtnOK.HighlightText(false);
	g_iOkAccelerator = uiXml.ReadAttribInt("ok_button", 0, "accel");

	AttachChild(&UIBtnCancel);
	xml_init.Init3tButton(uiXml, "cancel_button", 0, &UIBtnCancel);
	UIBtnCancel.HighlightText(false);
	g_iCancelAccelerator = uiXml.ReadAttribInt("cancel_button", 0, "accel");

	// Последний лист является списком ящиков оружия (тяжелое, снайперское, штурмовое, и т.д.)
	//for (int i = 0; i < 20; ++i)
	//{
	//	CUIDragDropList *pNewDDList = xr_new<CUIDragDropList>();
	//	R_ASSERT(pNewDDList);

	//	pNewDDList->SetCheckProc(BagProc);
	//	// Так только одно подокно всегда аттачено, а, бывает, у нас работа обычно ведется со 
	//	// всеми подокнами, то запоминаем адрес this, как парента в MessageTarget'e
	//	pNewDDList->SetMessageTarget(this);
	//	m_WeaponSubBags.push_back(pNewDDList);
	//	// Устанавливаем скейл для элементов этого листа
	//	pNewDDList->SetItemsScaleXY(SECTION_ICON_SCALE, SECTION_ICON_SCALE);

	//	xml_init.InitDragDropList(uiXml, "dragdrop_list", 1, pNewDDList);
	//}
	//// Заполняем массив со списком оружия
	UIBagWnd.UpdateBuyPossibility();	//ReInitItems(strSectionName);
	// Инициализируем ящички с оружием
//	InitWeaponBoxes();

	//Списки Drag&Drop
	AttachChild(&UITopList[BELT_SLOT]);
	xml_init.InitDragDropList(uiXml, "dragdrop_list", 0, &UITopList[BELT_SLOT]);

	AttachChild(&UITopList[OUTFIT_SLOT]);
	xml_init.InitDragDropList(uiXml, "dragdrop_list", 7, &UITopList[OUTFIT_SLOT]);

	AttachChild(&UITopList[0]);
	xml_init.InitDragDropList(uiXml, "dragdrop_list", 2, &UITopList[0]);
	//-----------------------------------------------------
	//чтобы нельзя было продать нож
	UITopList[0].BlockCustomPlacement();
	UITopList[0].EnableDoubleClick(false);
	UITopList[0].Enable(false);
	//-----------------------------------------------------

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

	UITopList[KNIFE_SLOT].SetCheckProc(SlotProc0);
	UITopList[PISTOL_SLOT].SetCheckProc(SlotProc1);
	UITopList[RIFLE_SLOT].SetCheckProc(SlotProc2);
	UITopList[GRENADE_SLOT].SetCheckProc(SlotProc3);
	UITopList[APPARATUS_SLOT].SetCheckProc(SlotProc4);
	UITopList[OUTFIT_SLOT].SetCheckProc(OutfitSlotProc);
	UITopList[BELT_SLOT].SetCheckProc(BeltProc);

	//pop-up menu
	AttachChild(&UIPropertiesBox);
	UIPropertiesBox.Init("ui\\ui_pop_up",0,0,300,300);
	UIPropertiesBox.Hide();

	// Статики для вывода текста
	AttachChild(&UITotalMoneyHeader);
	xml_init.InitStatic(uiXml, "total_money_header_static", 0, &UITotalMoneyHeader);

	AttachChild(&UITotalMoney);
	xml_init.InitStatic(uiXml, "total_money_static", 0, &UITotalMoney);

	AttachChild(&UIItemCostHeader);
	xml_init.InitStatic(uiXml, "item_cost_header_static", 0, &UIItemCostHeader);

	AttachChild(&UIItemCost);
	xml_init.InitStatic(uiXml, "item_cost_static", 0, &UIItemCost);

	// Иконка изображения персонажа в костюме
	AttachChild(&UIOutfitIcon);
	xml_init.InitStatic(uiXml, "outfit_static", 0, &UIOutfitIcon);
	UIOutfitIcon.SetShader(GetMPCharIconsShader());
//	UIOutfitIcon.SetTextureScaleXY(0.68f,0.68f);
	UIOutfitIcon.SetStretchTexture(true);
	UIOutfitIcon.ClipperOn();

	UIDescWnd.AttachChild(&UIItemInfo);
	UIItemInfo.Init(0, 0, UIDescWnd.GetWidth(), UIDescWnd.GetHeight(), BUY_MP_ITEM_XML);
}

void CUIBuyWeaponWnd::OnMouseScroll(int iDirection){

	//int iActiveIndex = UITabControl.GetActiveIndex();
	//
	//switch (iDirection)
	//{
	//case WINDOW_MOUSE_WHEEL_DOWN:
	//	if (iActiveIndex > 0)
	//		UITabControl.SetNewActiveTab(iActiveIndex - 1);
	//	break;
	//case WINDOW_MOUSE_WHEEL_UP:
	//	if (iActiveIndex < 5)
	//		UITabControl.SetNewActiveTab(iActiveIndex + 1);
	//	break;
	//default:
	//	NODEFAULT;
	//}
}

//void CUIBuyWeaponWnd::ReInitItems	(LPCSTR strSectionName)
//{
//	// Заполняем массив со списком оружия
//	m_StrSectionName = strSectionName;
//
//	ClearSlots();
//
//	// очищаем секции
//	for (u32 i=0; i<wpnSectStorage.size(); i++)
//	{
//		WPN_SECT_NAMES WpnSectName = wpnSectStorage[i];
//		for (u32 j=0; j<WpnSectName.size(); j++)
//		{
//			xr_string WpnName = WpnSectName[j];
//			WpnName.clear();
//		};
//		WpnSectName.clear();
//	}
//	wpnSectStorage.clear();
//
//	InitWpnSectStorage();
//	ClearWpnSubBags();
//	FillWpnSubBags();
//
//	UIBagWnd.AttachChild(m_WeaponSubBags[UITabControl.GetActiveIndex() + 1]);
//
//	// Проверяем возможность покупки для каждой вещи
//	CheckBuyAvailabilityInShop();
//};

void CUIBuyWeaponWnd::InitInventory() 
{
	m_pMouseCapturer = NULL;
//	MenuLevelDown();
//	MenuLevelDown();
//	ClearWpnSubBag(RIFLE_SLOT);
//	FillWpnSubBag(RIFLE_SLOT);
}  

//проверка на помещение инвентаря в слоты
// KNIFE SLOT
bool CUIBuyWeaponWnd::SlotProc0(CUIDragDropItem* pItem, CUIDragDropList* pList)
{
	CUIBuyWeaponWnd* this_inventory = smart_cast<CUIBuyWeaponWnd*>(pList->GetParent());
	R_ASSERT2(this_inventory, "wrong parent addressed as inventory wnd");

	CUIDragDropItemMP *pDDItemMP = smart_cast<CUIDragDropItemMP*>(pItem);
	R_ASSERT(pDDItemMP);

	if(!this_inventory->CanPutInSlot(pDDItemMP, KNIFE_SLOT)) 
		return false;

	this_inventory->SlotToSection(KNIFE_SLOT);

	if (pDDItemMP->m_bIsInfinite)
        this_inventory->UIBagWnd.CreateCopy(pDDItemMP);

	// И отнимаем от денег стоимость вещи.
	if (!pDDItemMP->m_bAlreadyPaid)
	{
		this_inventory->SetMoneyAmount(this_inventory->GetMoneyAmount() - 
			static_cast<int>(pDDItemMP->GetCost() * (pDDItemMP->m_bHasRealRepresentation ? fRealItemSellMultiplier : 1)));
		pDDItemMP->m_bAlreadyPaid = true;
	}
	return true;
}

// PISTOL SLOT
bool CUIBuyWeaponWnd::SlotProc1(CUIDragDropItem* pItem, CUIDragDropList* pList)
{
	CUIBuyWeaponWnd* this_inventory = smart_cast<CUIBuyWeaponWnd*>(pList->GetParent());
	R_ASSERT2(this_inventory, "wrong parent addressed as inventory wnd");

	CUIDragDropItemMP *pDDItemMP = smart_cast<CUIDragDropItemMP*>(pItem);
	R_ASSERT(pDDItemMP);

	if(!this_inventory->CanPutInSlot(pDDItemMP, PISTOL_SLOT)) return false;

	this_inventory->SlotToSection(PISTOL_SLOT);

	if (pDDItemMP->m_bIsInfinite)
        this_inventory->UIBagWnd.CreateCopy(pDDItemMP);


	// И отнимаем от денег стоимость вещи.
	if (!pDDItemMP->m_bAlreadyPaid)
	{
		this_inventory->SetMoneyAmount(this_inventory->GetMoneyAmount() - 
			static_cast<int>(pDDItemMP->GetCost() * (pDDItemMP->m_bHasRealRepresentation ? fRealItemSellMultiplier : 1)));
		pDDItemMP->m_bAlreadyPaid = true;
	}
	return true;
}

// RIFLE_SLOT
bool CUIBuyWeaponWnd::SlotProc2(CUIDragDropItem* pItem, CUIDragDropList* pList)
{
	CUIBuyWeaponWnd* this_inventory = smart_cast<CUIBuyWeaponWnd*>(pList->GetParent());
	R_ASSERT2(this_inventory, "wrong parent addressed as inventory wnd");

	CUIDragDropItemMP::AddonIDs	addonID;
	CUIDragDropItemMP *pDDItemMP	= smart_cast<CUIDragDropItemMP*>(pItem), 
					  *pAddonOwner	= this_inventory->IsItemAnAddon(pDDItemMP, addonID);

	R_ASSERT(pDDItemMP);

	// Если аддон
	if (pAddonOwner)	
	{
		pAddonOwner->AttachDetachAddon(pDDItemMP, !pAddonOwner->IsAddonAttached(addonID), pDDItemMP->m_bHasRealRepresentation);
		return false;
	}
	// Не аддон
	if(!this_inventory->CanPutInSlot(pDDItemMP, RIFLE_SLOT)) return false;

	this_inventory->SlotToSection(RIFLE_SLOT);

	if (pDDItemMP->m_bIsInfinite)
        this_inventory->UIBagWnd.CreateCopy(pDDItemMP);

	// И отнимаем от денег стоимость вещи.
	if (!pDDItemMP->m_bAlreadyPaid)
	{
		this_inventory->SetMoneyAmount(this_inventory->GetMoneyAmount() -
			static_cast<int>(pDDItemMP->GetCost() * (pDDItemMP->m_bHasRealRepresentation ? fRealItemSellMultiplier : 1)));
		pDDItemMP->m_bAlreadyPaid = true;
	}

	return true;
}


// GRENADE_SLOT
bool CUIBuyWeaponWnd::SlotProc3(CUIDragDropItem* pItem, CUIDragDropList* pList)
{
	CUIBuyWeaponWnd* this_inventory = smart_cast<CUIBuyWeaponWnd*>(pList->GetParent());
	R_ASSERT2(this_inventory, "wrong parent addressed as inventory wnd");

	CUIDragDropItemMP *pDDItemMP = smart_cast<CUIDragDropItemMP*>(pItem);
	R_ASSERT(pDDItemMP);

	// Если вещь не граната
	// и если в слоте уже есть 1 граната
	if (!this_inventory->CanPutInSlot(pDDItemMP, GRENADE_SLOT) ||
		!pList->GetDragDropItemsList().empty()) 
		return false;

	if (pDDItemMP->m_bIsInfinite)
        this_inventory->UIBagWnd.CreateCopy(pDDItemMP);

	// И отнимаем от денег стоимость вещи.
	if (!pDDItemMP->m_bAlreadyPaid)
	{
		this_inventory->SetMoneyAmount(this_inventory->GetMoneyAmount() -
			static_cast<int>(pDDItemMP->GetCost() * (pDDItemMP->m_bHasRealRepresentation ? fRealItemSellMultiplier : 1)));
		pDDItemMP->m_bAlreadyPaid = true;
	}

	return true;
}

//APPARATUS_SLOT
bool CUIBuyWeaponWnd::SlotProc4(CUIDragDropItem* pItem, CUIDragDropList* pList)
{
	CUIBuyWeaponWnd* this_inventory = smart_cast<CUIBuyWeaponWnd*>(pList->GetParent());
	R_ASSERT2(this_inventory, "wrong parent addressed as inventory wnd");

	CUIDragDropItemMP *pDDItemMP = smart_cast<CUIDragDropItemMP*>(pItem);
	R_ASSERT(pDDItemMP);

	if(!this_inventory->CanPutInSlot(pDDItemMP, APPARATUS_SLOT)) return false;

	this_inventory->SlotToSection(APPARATUS_SLOT);

	if (pDDItemMP->m_bIsInfinite)
        this_inventory->UIBagWnd.CreateCopy(pDDItemMP);

	// И отнимаем от денег стоимость вещи.
	if (!pDDItemMP->m_bAlreadyPaid)
	{
		this_inventory->SetMoneyAmount(this_inventory->GetMoneyAmount() -
			static_cast<int>(pDDItemMP->GetCost() * (pDDItemMP->m_bHasRealRepresentation ? fRealItemSellMultiplier : 1)));
		pDDItemMP->m_bAlreadyPaid = true;
	}

	return true;
}

//одеть костюм
bool CUIBuyWeaponWnd::OutfitSlotProc(CUIDragDropItem* pItem, CUIDragDropList* pList)
{
	CUIBuyWeaponWnd* this_inventory = smart_cast<CUIBuyWeaponWnd*>(pList->GetParent());
	R_ASSERT2(this_inventory, "wrong parent addressed as inventory wnd");

	CUIDragDropItemMP *pDDItemMP = smart_cast<CUIDragDropItemMP*>(pItem);
	R_ASSERT(pDDItemMP);
	
	if (OUTFIT_SLOT == pDDItemMP->GetSlot())
	{
		this_inventory->SlotToSection(OUTFIT_SLOT);
		pDDItemMP->Show(false);

		// Teперь отображаем соответствующую иконку персонажа, в зависимости от цвета команды, 
		// и купленного костюма

		// Сначала проверим проинициализирован ли правильно костюм
		if (pDDItemMP->m_fAdditionalInfo.size() < 2)
			R_ASSERT(!"Unknown suit");

		xr_vector<float>::iterator it = pDDItemMP->m_fAdditionalInfo.begin();
		this_inventory->UIOutfitIcon.GetUIStaticItem().SetOriginalRect(
			static_cast<int>(*it), 
			static_cast<int>(*(it+1)),
			SKIN_TEX_WIDTH, SKIN_TEX_HEIGHT - 15);
		this_inventory->UIOutfitIcon.Show(true);
		this_inventory->UIOutfitIcon.SetColor(pDDItemMP->GetColor());

		if (pDDItemMP->m_bIsInfinite)
			this_inventory->UIBagWnd.CreateCopy(pDDItemMP);

		// И отнимаем от денег стоимость вещи.
		if (!pDDItemMP->m_bAlreadyPaid)
		{
			this_inventory->SetMoneyAmount(this_inventory->GetMoneyAmount() -
				static_cast<int>(pDDItemMP->GetCost() * (pDDItemMP->m_bHasRealRepresentation ? fRealItemSellMultiplier : 1)));
			pDDItemMP->m_bAlreadyPaid = true;
		}
		return true;
	}	
	return false;
}

//на пояс
bool CUIBuyWeaponWnd::BeltProc(CUIDragDropItem* pItem, CUIDragDropList* pList)
{
	CUIBuyWeaponWnd* this_inventory = smart_cast<CUIBuyWeaponWnd*>(pList->GetParent());
	R_ASSERT2(this_inventory, "wrong parent addressed as inventory wnd");

	CUIDragDropItemMP *pDDItemMP = smart_cast<CUIDragDropItemMP*>(pItem);
	// У нас не может быть обычная вещь в этом диалоге.
	R_ASSERT(pDDItemMP);

	CUIDragDropItemMP::AddonIDs tmp;

	// Если вещь - аддон, то никуда ее не суем
	if (!this_inventory->CanPutInBelt(pDDItemMP)		||
		this_inventory->IsItemAnAddon(pDDItemMP, tmp)) 
		return false;

	if (pDDItemMP->m_bIsInfinite)
        this_inventory->UIBagWnd.CreateCopy(pDDItemMP);
	
	// И отнимаем от денег стоимость вещи.
	if (!pDDItemMP->m_bAlreadyPaid)
	{
		this_inventory->SetMoneyAmount(this_inventory->GetMoneyAmount() -
			static_cast<int>(pDDItemMP->GetCost() * (pDDItemMP->m_bHasRealRepresentation ? fRealItemSellMultiplier : 1)));
		pDDItemMP->m_bAlreadyPaid = true;
	}

	return true;
}

//------------------------------------------------
//как только подняли элемент, сделать его текущим
void CUIBuyWeaponWnd::SendMessage(CUIWindow *pWnd, s16 msg, void *pData)
{
	TABS_VECTOR_it	it;

	switch (msg)
	{
	case TAB_CHANGED:
        OnTabChange();	break;

	case XR_MENU_LEVEL_CHANGED:
		OnMenuLevelChange(); break;

	case DRAG_DROP_ITEM_DB_CLICK:
		SetCurrentDDItem(pWnd);
		OnDDItemDbClick(); break;

	case BUTTON_CLICKED:
		OnButtonClicked(pWnd); break;

	case DRAG_DROP_ITEM_DRAG:
		SetCurrentDDItem(pWnd);
		OnDDItemDrag(); break;

	case DRAG_DROP_REFRESH_ACTIVE_ITEM:
		if (m_pCurrentDragDropItem) 
			m_pCurrentDragDropItem->Highlight(true); break;
	}

//	if(msg == DRAG_DROP_ITEM_DRAG)
//	{
////		PIItem pInvItem = (PIItem)((CUIDragDropItemMP*)pWnd)->GetData();
////		SetCurrentItem(pInvItem);
//		if (m_pCurrentDragDropItem) m_pCurrentDragDropItem->Highlight(false);
//		m_pCurrentDragDropItem = (CUIDragDropItemMP*)pWnd;
//
//		// Cкейлим и увеличиваем текстуру если разрещено перетаскивание
//		if (m_pCurrentDragDropItem->IsDragDropEnabled())
//		{
//			m_pCurrentDragDropItem->Rescale(1.0f,1.0f);
//		}
//
//		// Disable highliht in all DD lists
//		for (int i = 0; i < PDA_SLOT; ++i)
//		{
//			UITopList[i].HighlightAllCells(false);
//		}
//	}
//	else 
	if(msg == DRAG_DROP_ITEM_DB_CLICK)
	{

//		CUIDragDropItemMP	*pAddonOwner;

		//// Проверяем, а не находимся ли мы на уровне ящиков?
		//if (m_pCurrentDragDropItem->GetParent() == m_WeaponSubBags.back())
		//{
		//	ApplyFilter(RIFLE_SLOT, weaponFilterName, m_pCurrentDragDropItem->GetSectionName());
		//	if (mlRoot == m_mlCurrLevel) MenuLevelUp();
		//	OnKeyboard(m_pCurrentDragDropItem->GetPosInSectionsGroup() + 2, WINDOW_KEY_PRESSED);
		//}

	}
	////по нажатию правой кнопки
	//else if(msg == DRAG_DROP_ITEM_RBUTTON_CLICK)
	//{
	//	m_pCurrentDragDropItem = (CUIDragDropItemMP*)pWnd;

	//	ActivatePropertiesBox();
	//}
	//сообщение от меню вызываемого правой кнопкой
	//else 
		if(pWnd == &UIPropertiesBox &&
		msg == PROPERTY_CLICKED)
	{
		if(UIPropertiesBox.GetClickedItem())
		{
			switch(UIPropertiesBox.GetClickedItem()->GetValue())
			{
			case BUY_ITEM_ACTION:
				SendMessage(m_pCurrentDragDropItem, DRAG_DROP_ITEM_DB_CLICK, NULL);
				break;
			case CANCEL_BUYING_ACTION:
				m_pCurrentDragDropItem->MoveOnNextDrop();
				UIBagWnd.SendMessage(m_pCurrentDragDropItem, DRAG_DROP_ITEM_DROP, NULL);
				break;
			case ATTACH_SCOPE_ADDON:
				m_pCurrentDragDropItem->AttachDetachAddon(CUIDragDropItemMP::ID_SCOPE, true, m_pCurrentDragDropItem->m_bHasRealRepresentation);
				break;
			case ATTACH_SILENCER_ADDON:
				m_pCurrentDragDropItem->AttachDetachAddon(CUIDragDropItemMP::ID_SILENCER, true, m_pCurrentDragDropItem->m_bHasRealRepresentation);
				break;
			case ATTACH_GRENADE_LAUNCHER_ADDON:
				m_pCurrentDragDropItem->AttachDetachAddon(CUIDragDropItemMP::ID_GRENADE_LAUNCHER, true, m_pCurrentDragDropItem->m_bHasRealRepresentation);
				break;
			case DETACH_SCOPE_ADDON:
				m_pCurrentDragDropItem->AttachDetachAddon(CUIDragDropItemMP::ID_SCOPE, false, m_pCurrentDragDropItem->m_bHasRealRepresentation);
				break;
			case DETACH_SILENCER_ADDON:
				m_pCurrentDragDropItem->AttachDetachAddon(CUIDragDropItemMP::ID_SILENCER, false, m_pCurrentDragDropItem->m_bHasRealRepresentation);
				break;
			case DETACH_GRENADE_LAUNCHER_ADDON:
				m_pCurrentDragDropItem->AttachDetachAddon(CUIDragDropItemMP::ID_GRENADE_LAUNCHER, false, m_pCurrentDragDropItem->m_bHasRealRepresentation);
				break;
			}
		}
	}


	// Кнопки ОК и Отмена
	else if (&UIBtnOK == pWnd && BUTTON_CLICKED == msg && CanBuyAllItems())
	{

	}
	else if (&UIBtnCancel == pWnd && BUTTON_CLICKED == msg)
	{
		
	}
	//else if (STATIC_FOCUS_RECEIVED == msg && &UIBtnOK == pWnd)
	//{
	//	UIBtnOK.SetTextColor(0xFFF0D9B6);
	//}
	//else if (STATIC_FOCUS_RECEIVED == msg && &UIBtnCancel == pWnd)
	//{
	//	UIBtnCancel.SetTextColor(0xFFF0D9B6);
	//}
	//else if (STATIC_FOCUS_LOST == msg && &UIBtnOK == pWnd)
	//{
	//	UIBtnOK.SetTextColor(0xFFEE9B17);
	//}
	//else if (STATIC_FOCUS_LOST == msg && &UIBtnCancel == pWnd)
	//{
	//	UIBtnCancel.SetTextColor(0xFFEE9B17);
	//}
	// Если костюмчик вернулся в слот, то спрятать его
	else if (OUTFIT_RETURNED_BACK == msg && pWnd->GetParent() == &UITopList[OUTFIT_SLOT])
	{
		CUIDragDropItemMP *pDDItemMP = smart_cast<CUIDragDropItemMP*>(pWnd);
		if (pDDItemMP && OUTFIT_SLOT == pDDItemMP->GetSlot())
		{
			pDDItemMP->Show(false);
		}
	}
	//else if (DRAG_DROP_REFRESH_ACTIVE_ITEM == msg)
	//{
	//	if (m_pCurrentDragDropItem) m_pCurrentDragDropItem->Highlight(true);
	//}
	//else if (WINDOW_LBUTTON_DOWN == msg && &UIOutfitIcon == pWnd)
	//{
	//	if (UITopList[OUTFIT_SLOT].GetChildWndList().back())
	//	{
	//		UITopList[OUTFIT_SLOT].GetChildWndList().back()->Show(true);
	//	}
	//}
	//else if (WINDOW_RBUTTON_DOWN == msg && &UIOutfitIcon == pWnd)
	//{
	//	if (UITopList[OUTFIT_SLOT].GetChildWndList().back())
	//	{
	//		UITopList[OUTFIT_SLOT].GetChildWndList().back()->Show(true);
	//		SendMessage(UITopList[OUTFIT_SLOT].GetChildWndList().back(), DRAG_DROP_ITEM_RBUTTON_CLICK, NULL);
	//	}
	//}

	CUIWindow::SendMessage(pWnd, msg, pData);
}

void CUIBuyWeaponWnd::OnTabChange(){
	UIBagWnd.ShowSection(UITabControl.GetActiveIndex());
}

void CUIBuyWeaponWnd::OnButtonClicked(CUIWindow* pWnd){
	if (pWnd == &UIBtnOK)
	{
		if (!CanBuyAllItems())
            return;

		Game().StartStopMenu(this,true);
		game_cl_Deathmatch * dm = smart_cast<game_cl_Deathmatch *>(&(Game()));
		dm->OnBuyMenu_Ok();
	}
	else if (pWnd == &UIBtnCancel)
	{
		Game().StartStopMenu(this,true);
	}

}

void CUIBuyWeaponWnd::OnDDItemDrag(){
	if (m_pCurrentDragDropItem->IsDragDropEnabled())
		m_pCurrentDragDropItem->Rescale(1.0f,1.0f);

	// Disable highliht in all DD lists
	for (int i = 0; i < PDA_SLOT; ++i)
		UITopList[i].HighlightAllCells(false);
}

void CUIBuyWeaponWnd::SetCurrentDDItem(CUIWindow* pWnd){
	if (m_pCurrentDragDropItem) 
		m_pCurrentDragDropItem->Highlight(false);

	m_pCurrentDragDropItem = smart_cast<CUIDragDropItemMP*>(pWnd);
	R_ASSERT2(m_pCurrentDragDropItem, "CUIBuyWeaponWnd::SetCurrentDDItem - invalid item");
}

void CUIBuyWeaponWnd::OnDDItemDbClick(){
	if (m_pCurrentDragDropItem->IsDragDropEnabled())
	{

		// "Поднять" вещь для освобождения занимаемого места
		SendMessage(m_pCurrentDragDropItem, DRAG_DROP_ITEM_DRAG, NULL);

		//попытаться закинуть элемент в слот, рюкзак или на пояс
		if(!ToSlot())
			if(!ToBelt())
				if(!ToBag())
				//если нельзя, то просто упорядочить элемент в своем списке
				{
					CUIWindow* pWindowParent = m_pCurrentDragDropItem->GetParent();
					pWindowParent->DetachChild(m_pCurrentDragDropItem);
					pWindowParent->AttachChild(m_pCurrentDragDropItem);
					m_pCurrentDragDropItem->Rescale(	((CUIDragDropList*)m_pCurrentDragDropItem->GetParent())->GetItemsScaleX(),
														((CUIDragDropList*)m_pCurrentDragDropItem->GetParent())->GetItemsScaleY()	);
				}
	}
}

void CUIBuyWeaponWnd::OnMenuLevelChange(){
	if (UIBagWnd.GetMenuLevel() == mlRoot)
		UITabControl.SetActiveState();
}

void CUIBuyWeaponWnd::OnMouse(int x, int y, EUIMessages mouse_action)
{
	if (WINDOW_LBUTTON_DOWN == mouse_action || WINDOW_RBUTTON_DOWN == mouse_action)
	{
		Irect		r = UIOutfitIcon.GetAbsoluteRect();
		Ivector2	p;
		p.x = x;
		p.y = y;

		if ( r.in(p))
		{
			SendMessage(&UIOutfitIcon, static_cast<s16>(mouse_action), NULL);
		}
	}

	//вызов дополнительного меню по правой кнопке
	if(mouse_action == WINDOW_RBUTTON_DOWN)
	{
		if(UIPropertiesBox.IsShown())
		{
			UIPropertiesBox.Hide();
		}
	}

	inherited::OnMouse(x, y, mouse_action);
}

void CUIBuyWeaponWnd::Draw()
{
	inherited::Draw();
}


void CUIBuyWeaponWnd::Update()
{
	// Update money amount for Bag
	UIBagWnd.UpdateBuyPossibility();
	UIBagWnd.UpdateMoney(m_iMoneyAmount);

	// Если активная вещь изменилась, то обновляем информацию о ее стоимости
	static CUIDragDropItemMP *pOldCurrentDragDropItem = reinterpret_cast<CUIDragDropItemMP*>(1);
	static string16		str;
	//m_pCurrentDragDropItem = UIBagWnd.GetCurrentItem();

	if (pOldCurrentDragDropItem != m_pCurrentDragDropItem && m_pCurrentDragDropItem)
	{
		pOldCurrentDragDropItem	= m_pCurrentDragDropItem;
		pOldCurrentDragDropItem->Highlight(false);

		UIItemCost.SetText("");
		FillItemInfo(NULL);

		if (m_pCurrentDragDropItem)
		{
			if (m_pCurrentDragDropItem->GetSlot() != WEAPON_BOXES_SLOT)
			{
				int cost = m_pCurrentDragDropItem->GetCost();
				if (m_pCurrentDragDropItem->m_bHasRealRepresentation)
					cost = static_cast<int>(cost * fRealItemSellMultiplier);
				sprintf(str, "%i", cost);
				UIItemCost.SetText(str);
				FillItemInfo(m_pCurrentDragDropItem);
			}
			m_pCurrentDragDropItem->Highlight(true);
		}
	}

	// Если деньги иземниличь то обновить и их
	static int oldMoneyAmount	= 0xffffffff;

	if (oldMoneyAmount != m_iMoneyAmount)
	{
		oldMoneyAmount = m_iMoneyAmount;
		sprintf(str, "%i", m_iMoneyAmount);
		UITotalMoney.SetText(str);
	}

	// Ecли в слоте с костюмом армор показывается, то спрятать его.
	bool flag = true;
	if (UITopList[OUTFIT_SLOT].GetDragDropItemsList().empty() && flag)
	{
		UIOutfitIcon.GetUIStaticItem().SetOriginalRect(m_iIconTextureX, m_iIconTextureY, SKIN_TEX_WIDTH, SKIN_TEX_HEIGHT);
		flag = false;
	}

	if (!UITopList[OUTFIT_SLOT].GetDragDropItemsList().empty())
		flag = true;

	CUIWindow::Update();
}

void CUIBuyWeaponWnd::DropItem()
{
	CActor *pActor = smart_cast<CActor*>(Level().CurrentEntity());
	if(!pActor) return;

	m_pCurrentItem->Drop();

	(smart_cast<CUIDragDropList*>(m_pCurrentDragDropItem->GetParent()))->
		DetachChild(m_pCurrentDragDropItem);
	m_pCurrentDragDropItem = NULL;
}

void CUIBuyWeaponWnd::Show() 
{ 
	m_pMouseCapturer = NULL;
	inherited::Show();

	if (GameID() != GAME_SINGLE)
	{
		CActor *pActor = smart_cast<CActor*>(Level().CurrentEntity());
		if(!pActor) return;
		pActor->HideCurrentWeapon(GEG_PLAYER_BUYMENU_OPEN);//, false);
	}
}

void CUIBuyWeaponWnd::Hide()
{
	inherited::Hide();

	if (GameID() != GAME_SINGLE)
	{
		CActor *pActor = smart_cast<CActor*>(Level().CurrentEntity());
		if(!pActor) return;

		pActor->RestoreHidedWeapon(GEG_PLAYER_BUYMENU_CLOSE);
	}
}


// Контекстное меню
void CUIBuyWeaponWnd::ActivatePropertiesBox()
{
	int x,y;
	Irect rect = GetAbsoluteRect();
	GetUICursor()->GetPos(x,y);

	UIPropertiesBox.RemoveAll();

	// Если обычная вещь
	if((m_pCurrentDragDropItem->GetSlot() < MP_SLOTS_NUM				||
		static_cast<u32>(-1) == m_pCurrentDragDropItem->GetSlot())		&& 
		&UIBagWnd == m_pCurrentDragDropItem->GetParent()->GetParent())
	{
		// Если вещь не аддон, то действие одно, а если аддон, то другое
		CUIDragDropItemMP::AddonIDs		ID;
		CUIDragDropItemMP				*pDDItemMP = IsItemAnAddon(m_pCurrentDragDropItem, ID);

		if (m_pCurrentDragDropItem->GetCost() > GetMoneyAmount())
		{
			return;
		}
		
		if (pDDItemMP)
		{
			m_pCurrentDragDropItem = pDDItemMP;
			UIPropertiesBox.AddItem("Attach Addon", 
									NULL,
									pDDItemMP->IsAddonAttached(ID) ? DETACH_SILENCER_ADDON + static_cast<int>(ID) : ATTACH_SILENCER_ADDON + static_cast<int>(ID));
		}
		else
		{
			UIPropertiesBox.AddItem("Buy Item",  NULL, BUY_ITEM_ACTION);
		}
	}
	else
	{
		UIPropertiesBox.AddItem("Cancel", NULL, CANCEL_BUYING_ACTION);
		// Так как оружие еще в сумке, то просматриваем его список аддонов и модифицируем меню
		if (m_pCurrentDragDropItem->bAddonsAvailable)
		{
			xr_string strMenuItem;
			for (int i = 0; i < 3; ++i)
			{
				switch (m_pCurrentDragDropItem->m_AddonInfo[i].iAttachStatus)
				{
					// If addon detached
				case 0:
					// Если денег на аддон хватает
					if (GetAddonByID(m_pCurrentDragDropItem, static_cast<CUIDragDropItemMP::AddonIDs>(i))->GetCost() <= GetMoneyAmount())
					{
						strMenuItem = xr_string("Attach ") + m_pCurrentDragDropItem->m_strAddonTypeNames[i];
						UIPropertiesBox.AddItem(strMenuItem.c_str(), NULL, ATTACH_SILENCER_ADDON + i);
					}
					break;
				case 1:
					strMenuItem = xr_string("Detach ") + m_pCurrentDragDropItem->m_strAddonTypeNames[i];
					UIPropertiesBox.AddItem(strMenuItem.c_str(), NULL, DETACH_SILENCER_ADDON + i);
					break;
				default:
					continue;
				}
			}
		}
	}

	UIPropertiesBox.AutoUpdateSize();
	UIPropertiesBox.BringAllToTop();
	UIPropertiesBox.Show(x-rect.left, y-rect.top);
}


bool CUIBuyWeaponWnd::ToSlot()
{
	if ( !UIBagWnd.IsItemInBag(m_pCurrentDragDropItem) )
		return false;


	// Специальное поведение для гранат
	if (GRENADE_SLOT == m_pCurrentDragDropItem->GetSlot() && 
		!UITopList[GRENADE_SLOT].GetDragDropItemsList().empty()) return false;

	// Если вещь аддон
	CUIDragDropItemMP::AddonIDs ID;
	CUIDragDropItemMP * pDDItemMP = IsItemAnAddon(m_pCurrentDragDropItem, ID);
	if (pDDItemMP && !UIBagWnd.IsItemInBag(pDDItemMP))
        pDDItemMP->AttachDetachAddon(ID, true, m_pCurrentDragDropItem->m_bHasRealRepresentation);
	

	// Если вещь без номера слота, то ее поместить никуда нельзя (например патроны)
	int itemSlot = m_pCurrentDragDropItem->GetSlot();
	if (itemSlot < MP_SLOTS_NUM && itemSlot >= 0)
	{
		SlotToSection(itemSlot);

		m_pCurrentDragDropItem->MoveOnNextDrop();
		UITopList[itemSlot].SendMessage(m_pCurrentDragDropItem, DRAG_DROP_ITEM_DROP, NULL);

		if (m_pCurrentDragDropItem->m_bIsInfinite)
		{
		}

		BuyReaction();

		m_pMouseCapturer = NULL;
		return true;
	}
	return false;
}

void CUIBuyWeaponWnd::BuyReaction(){
	int iActiveIndex = UITabControl.GetActiveIndex();

	if (2 == iActiveIndex || 4 == iActiveIndex)
	{
		; // do nothing
	}
	else
		UITabControl.SetActiveState();
}

bool CUIBuyWeaponWnd::ToBag()
{
	// if in bag
	if (UIBagWnd.IsItemInBag(m_pCurrentDragDropItem))
		return false;

	// if in belt
	if (BeltToSection(m_pCurrentDragDropItem)) 
		return true;

	// if in slot
	if (SlotToSection(m_pCurrentDragDropItem->GetSlot())) 
		return true;

	return false;
}

bool CUIBuyWeaponWnd::ToBelt()
{
	// cheking
	if (!CanPutInBelt(m_pCurrentDragDropItem)							||
		&UITopList[BELT_SLOT] == m_pCurrentDragDropItem->GetParent()	||
		!UITopList[BELT_SLOT].CanPlaceItem(m_pCurrentDragDropItem)		||
		IsItemAnAddonSimple(m_pCurrentDragDropItem)						||
		!UIBagWnd.IsItemInBag(m_pCurrentDragDropItem)
		)
		return false;

// 	CUIDragDropItemMP *pDDItemMP = smart_cast<CUIDragDropItemMP*>(*UITopList[SlotNum].GetDragDropItemsList().begin());
	m_pCurrentDragDropItem->MoveOnNextDrop();
	m_pCurrentDragDropItem->AttachDetachAllAddons(false);
	UITopList[BELT_SLOT].SendMessage(m_pCurrentDragDropItem, DRAG_DROP_ITEM_DROP, NULL);

	//m_pCurrentDragDropItem->GetParent()->DetachChild(m_pCurrentDragDropItem);
	//UITopList[BELT_SLOT].AttachChild(m_pCurrentDragDropItem);

	//if (!m_pCurrentDragDropItem->m_bAlreadyPaid)
	//{
	//	SetMoneyAmount(GetMoneyAmount() -
	//		static_cast<int>(m_pCurrentDragDropItem->GetCost() * (m_pCurrentDragDropItem->m_bHasRealRepresentation ? fRealItemSellMultiplier : 1)));
	//	m_pCurrentDragDropItem->m_bAlreadyPaid = true;
	//}

	m_pMouseCapturer = NULL;

	return true;
}

bool CUIBuyWeaponWnd::SlotToSection(const u32 SlotNum)
{
	if (SlotNum >= MP_SLOTS_NUM) return false;
	// Выкидываем вещь из слота если есть.
	if (!UITopList[SlotNum].GetDragDropItemsList().empty())
	{
		CUIDragDropItemMP *pDDItemMP = smart_cast<CUIDragDropItemMP*>(*UITopList[SlotNum].GetDragDropItemsList().begin());
		pDDItemMP->MoveOnNextDrop();
		pDDItemMP->AttachDetachAllAddons(false);
		UIBagWnd.SendMessage(pDDItemMP, DRAG_DROP_ITEM_DROP, NULL);
	}

	return true;
}

bool CUIBuyWeaponWnd::CanPutInSlot(CUIDragDropItemMP *pDDItemMP, const u32 slotNum)
{
	return slotNum == pDDItemMP->GetSlot();
}

//////////////////////////////////////////////////////////////////////////
//
//const char * CUIBuyWeaponWnd::GetWeaponName(u32 slotNum)
//{
//	CUIDragDropItemMP *pDDItemMP = GetWeapon(slotNum, 0);
//	if (pDDItemMP)
//		return pDDItemMP->GetSectionName();
//	return NULL;
//}

//////////////////////////////////////////////////////////////////////////
//
//const char * CUIBuyWeaponWnd::GetWeaponNameInBelt(u32 indexInBelt)
//{
//	CUIDragDropItemMP *pDDItemMP = GetWeapon(BELT_SLOT, indexInBelt);
//	if (pDDItemMP)
//		return pDDItemMP->GetSectionName();
//	return NULL;
//}

//////////////////////////////////////////////////////////////////////////

const u8 CUIBuyWeaponWnd::GetItemIndex(u32 slotNum, u32 idx, u8 &sectionNum)
{
	CUIDragDropItemMP *pDDItemMP = GetWeapon(slotNum, idx);

	return UIBagWnd.GetItemIndex(pDDItemMP, sectionNum);	
}

//////////////////////////////////////////////////////////////////////////

const u8 CUIBuyWeaponWnd::GetWeaponIndex(u32 slotNum)
{
	u8 tmp;
	return GetItemIndex(slotNum, 0, tmp);
}

//////////////////////////////////////////////////////////////////////////

const u8 CUIBuyWeaponWnd::GetWeaponIndexInBelt(u32 indexInBelt, u8 &sectionId, u8 &itemId)
{
	itemId = GetItemIndex(BELT_SLOT, indexInBelt, sectionId);
	return itemId;
}

//////////////////////////////////////////////////////////////////////////
//
//const char * CUIBuyWeaponWnd::GetWeaponNameByIndex(u32 grpNum, u8 idx)
//{
//	// Удаляем информацию о аддонах
//	idx &= 0x1f;
//
////	if (wpnSectStorage.size() <= grpNum || idx > wpnSectStorage[grpNum].size()) return NULL;
////	return wpnSectStorage[grpNum][idx].c_str();
//	return NULL;
//}

void CUIBuyWeaponWnd::GetWeaponIndexByName(const xr_string sectionName, u8 &grpNum, u8 &idx)
{
	UIBagWnd.GetWeaponIndexByName(sectionName, grpNum, idx);
}


//////////////////////////////////////////////////////////////////////////

CUIDragDropItemMP * CUIBuyWeaponWnd::GetWeapon(u32 slotNum, u32 idx)
{
	R_ASSERT(slotNum < MP_SLOTS_NUM);

	if (UITopList[slotNum].GetDragDropItemsList().empty()) 
		return NULL;

	R_ASSERT(idx < UITopList[slotNum].GetDragDropItemsList().size());

	DRAG_DROP_LIST_it it = UITopList[slotNum].GetDragDropItemsList().begin(); 
	std::advance(it, idx);
	return smart_cast<CUIDragDropItemMP*>(*it);
}

//////////////////////////////////////////////////////////////////////////

const u8 CUIBuyWeaponWnd::GetWeaponAddonInfoByIndex(u8 idx)
{
	return (idx & 0xe0) >> 5;
}

//////////////////////////////////////////////////////////////////////////

bool CUIBuyWeaponWnd::CanPutInBelt(CUIDragDropItemMP *pDDItemMP)
{
	return pDDItemMP->GetSlot() == GRENADE_SLOT || pDDItemMP->GetSlot() > 7;
}

//////////////////////////////////////////////////////////////////////////

bool CUIBuyWeaponWnd::BeltToSection(CUIDragDropItemMP *pDDItemMP)
{
	R_ASSERT(pDDItemMP);

	if (&UITopList[BELT_SLOT] != pDDItemMP->GetParent()) return false;
	// Берем вещь
	pDDItemMP->MoveOnNextDrop();
	// ...и посылаем ему сообщение переместиться в сумку
//	pDDItemMP->GetParent()->DetachChild(pDDItemMP);
//	UIBagWnd.PutItem(pDDItemMP);
	UIBagWnd.SendMessage(pDDItemMP, DRAG_DROP_ITEM_DROP, NULL);
//	m_WeaponSubBags[pDDItemMP->GetSectionGroupID()]->SendMessage(pDDItemMP, DRAG_DROP_ITEM_DROP, NULL);

	return true;
}

//////////////////////////////////////////////////////////////////////////
//
//int CUIBuyWeaponWnd::GetFirstFreeIndex()
//{
//	R_ASSERT(m_iUsedItems < MAX_ITEMS);
//
//	if (!m_iEmptyIndexes.empty())
//	{
//		int result = *m_iEmptyIndexes.begin();
//		m_iEmptyIndexes.erase(result);
//		return result;
//	}
//	return m_iUsedItems++;
//}

//////////////////////////////////////////////////////////////////////////

const u8 CUIBuyWeaponWnd::GetBeltSize()
{
	return static_cast<u8>(UITopList[BELT_SLOT].GetDragDropItemsList().size());
}

bool CUIBuyWeaponWnd::OnKeyboard(int dik, EUIMessages keyboard_action)
{
	if (WINDOW_KEY_RELEASED == keyboard_action) 
		return false;

	switch (UIBagWnd.GetMenuLevel())
	{
	// Первый уровень - выбор типа оружия (по табконтролу)
	case mlRoot:
		if (UITabControl.OnKeyboard(dik, keyboard_action))
			return true;
//		{
//			if (mlRoot == m_mlCurrLevel)
//			{
//				MenuLevelUp();
//				return true;
//			};
//		}
		break;
	default:
		if (UIBagWnd.OnKeyboard(dik, keyboard_action))
			return true;

	//case mlBoxes:

	//	if (dik < DIK_0 && dik > DIK_ESCAPE)
	//	{
	//		for (DRAG_DROP_LIST_it it = m_WeaponSubBags.back()->GetDragDropItemsList().begin(); it != m_WeaponSubBags.back()->GetDragDropItemsList().end(); ++it)
	//		{
	//			CUIDragDropItemMP *pDDItemMP = smart_cast<CUIDragDropItemMP*>(*it);
	//			R_ASSERT(pDDItemMP);
	//			if (static_cast<u32>(dik - 2) == pDDItemMP->GetPosInSectionsGroup())
	//			{
	//				ApplyFilter(RIFLE_SLOT, weaponFilterName, pDDItemMP->GetSectionName());
	//				MenuLevelUp();
	//				return true;
	//				break;
	//			}
	//		}
	//	}
	//	break;
	//// Третий уровень - выбор конкретного оружия в данной группе
	//case mlWpnSubType:

	//	// Быстрая проверка на возможность дальнейшего плодотворного сотрудничетва
	//	if (dik < DIK_0 && dik > DIK_ESCAPE)
	//	{
	//		// Глубокий (более медленный и точный) поиск
	//		if (m_WeaponSubBags[UITabControl.GetActiveIndex() + 1]->GetDragDropItemsList().size() >= static_cast<u32>(dik - 2))
	//		{
	//			DRAG_DROP_LIST_it it = m_WeaponSubBags[UITabControl.GetActiveIndex() + 1]->GetDragDropItemsList().begin();
	//			// Пробегаемся по всем вещам и ищем, нет ли вещи с нужным номером
	//			for (; it != m_WeaponSubBags[UITabControl.GetActiveIndex() + 1]->GetDragDropItemsList().end(); ++it)
	//			{
	//				CUIDragDropItemMP *pDDItemMP = static_cast<CUIDragDropItemMP*>(*it);
	//				// Опа! Нашли.
	//				if (pDDItemMP->GetPosInSectionsGroup() == static_cast<u32>(dik - 2))
	//				{
	//					// Муваем ее в слот
	//					SendMessage(pDDItemMP, DRAG_DROP_ITEM_DB_CLICK, NULL);
	//					MenuLevelDown();
	//					MenuLevelDown();
	//					return true;
	//				}
	//			}
	//		}
	//	}
	//	break;
	//default:
	//	NODEFAULT;
	}

	if (DIK_ESCAPE == dik)
	{
		GetHolder()->StartStopMenu(this, true);		return true;
	}

	if (dik == g_iOkAccelerator)
	{
		SendMessage(&UIBtnOK, BUTTON_CLICKED, NULL);
		return true;
	}

//	if (dik == g_iCancelAccelerator)
//	{
//		if (mlRoot == m_mlCurrLevel)
//		{
//			//--------------------- for E3 ---------------------
/////			SendMessage(&UIBtnOK, CUIButton::BUTTON_CLICKED, NULL);
//			//--------------------------------------------------
//			SendMessage(&UIBtnCancel, BUTTON_CLICKED, NULL);
//			return true;
//		}
//	}

	return true;
}

//const u8 CUIBuyWeaponWnd::GetCurrentSuit()
//{
//	if (!UITopList[OUTFIT_SLOT].GetDragDropItemsList().empty())
//	{
//		CUIDragDropItemMP *pDDItemMP = smart_cast<CUIDragDropItemMP*>(UITopList[OUTFIT_SLOT].GetDragDropItemsList().front());
//		if (0 == xr_strcmp(pDDItemMP->GetSectionName(), "a"))
//		{
//
//		}
//	}
//	return static_cast<u8>(-1);
//}

//////////////////////////////////////////////////////////////////////////
//
//void CUIBuyWeaponWnd::CheckBuyAvailabilityInShop()
//{
//	// Пробегаемся по всем вещам и проверяем иx на возможность покупки
//	//for (WEAPON_TYPES_it it = m_WeaponSubBags.begin(); it != m_WeaponSubBags.end() - 1; ++it)	
//	//{
//	//	for (DRAG_DROP_LIST_it it2 = (*it)->GetDragDropItemsList().begin(); it2 != (*it)->GetDragDropItemsList().end(); ++it2)
//	//	{
//	//		// В данном диалоге вещь не UIDragDropItemMP быть не может
//	//		CUIDragDropItemMP *pDDItemMP = smart_cast<CUIDragDropItemMP*>(*it2);
//	//		R_ASSERT(pDDItemMP);
//
//	//		if (pDDItemMP->GetCost() <= GetMoneyAmount())
//	//		{
//	//			// Если эта вещь была при актере во время вызова меню покупки, то ее цвет - полупрозрачный
//	//			if (pDDItemMP->m_bHasRealRepresentation)
//	//				pDDItemMP->SetColor(cAbleToBuyOwned);
//	//			else
//	//				pDDItemMP->SetColor(cAbleToBuy);
//	//			pDDItemMP->EnableDragDrop(true);
//	//		}
//	//		else
//	//		{
//	//			if (m_pCurrentDragDropItem != pDDItemMP)
//	//			{
//	//				if (!pDDItemMP->m_bHasRealRepresentation)
//	//				{
//	//					pDDItemMP->SetColor(cUnableToBuy);
//	//					pDDItemMP->EnableDragDrop(false);
//	//				}
//	//			}
//	//		}
//	//	}
//	//}
//}

//////////////////////////////////////////////////////////////////////////

bool CUIBuyWeaponWnd::CheckBuyAvailabilityInSlots()
{
	// массив приоритетов слотов при проверке
	const int arrSize = 5;
	int priorityArr[arrSize] =
	{
		RIFLE_SLOT,
		PISTOL_SLOT,
		OUTFIT_SLOT,
		GRENADE_SLOT,
		KNIFE_SLOT
	};

	bool status = true;

	// Пробегаемся по вещам в слотах, и смотрим есть ли
	// денежки на эту вещь. Если есть, то отнимаем цену вещи
	// от суммы денег, и продолжанм провнрку
	for (int j = 0; j < arrSize; ++j)
	{
		// Если вещь есть
		if (!UITopList[priorityArr[j]].GetDragDropItemsList().empty())
		{
			DRAG_DROP_LIST &pList = UITopList[priorityArr[j]].GetDragDropItemsList();

			for (DRAG_DROP_LIST_it it = pList.begin(); it != pList.end(); ++it)
			{
				CUIDragDropItemMP *pDDItemMP = smart_cast<CUIDragDropItemMP*>(*it);
				R_ASSERT(pDDItemMP);

				// И если на нее хватает денег, то отнимаем ее цену от количества денег
				// Если эта вещь принесена игроком, то деньги не отнимаем
				if (!pDDItemMP->m_bHasRealRepresentation)
				{
					if (pDDItemMP->GetCost() <= GetMoneyAmount())
					{
						SetMoneyAmount(GetMoneyAmount() - pDDItemMP->GetCost());
						pDDItemMP->SetColor(cAbleToBuy);
					}
					else
					{
						pDDItemMP->SetColor(cUnableToBuy);
						status = false;

						// Для армора закрашиваем дополнительно и иконку с изображением персонажа
						if (OUTFIT_SLOT == pDDItemMP->GetSlot())
							UIOutfitIcon.SetColor(cUnableToBuy);
						pDDItemMP->m_bAlreadyPaid = false;
					}
				}
				else
				{
					pDDItemMP->SetColor(cAbleToBuyOwned);

					// Для армора закрациваем дополнительно и иконку с изображением персонажа
					if (OUTFIT_SLOT == pDDItemMP->GetSlot())
						UIOutfitIcon.SetColor(cAbleToBuyOwned);
				}
			}
		}
	}

	// У пояса наименьший приоритет
	for (DRAG_DROP_LIST_it it = UITopList[BELT_SLOT].GetDragDropItemsList().begin();
		 it != UITopList[BELT_SLOT].GetDragDropItemsList().end();
		 ++it)
	{
		CUIDragDropItemMP *pDDItemMP = smart_cast<CUIDragDropItemMP*>(*it);
		R_ASSERT(pDDItemMP);

		if (!pDDItemMP->m_bHasRealRepresentation)
		{
			if (pDDItemMP->GetCost() <= GetMoneyAmount())
			{
				SetMoneyAmount(GetMoneyAmount() - pDDItemMP->GetCost());
				pDDItemMP->SetColor(cAbleToBuy);
			}
			else
			{
				pDDItemMP->SetColor(cUnableToBuy);
				pDDItemMP->m_bAlreadyPaid = false;
				status = false;
			}
		}
		else
			pDDItemMP->SetColor(cAbleToBuyOwned);
	}

	return status;
}

//////////////////////////////////////////////////////////////////////////

void CUIBuyWeaponWnd::SetSkin(u8 SkinID)
{
	const shared_str		skinsParamName = "skins";
	shared_str				tmpStr;

	// Читаем список скинов
	tmpStr = pSettings->r_string(m_StrSectionName, *skinsParamName);

	R_ASSERT(_GetItemCount(*tmpStr) > SkinID);

	// Получаем имя скина, и координаты соответствующей иконки
	string32 a;
	tmpStr = _GetItem(*tmpStr, SkinID, a);
	sscanf(pSettings->r_string("multiplayer_skins", a), "%i,%i", &m_iIconTextureX, &m_iIconTextureY);
}

//////////////////////////////////////////////////////////////////////////

void CUIBuyWeaponWnd::ClearSlots()
{
	IgnoreMoney(true);
	SlotToSection(KNIFE_SLOT);
	SlotToSection(PISTOL_SLOT);
	SlotToSection(RIFLE_SLOT);
	SlotToSection(GRENADE_SLOT);
	SlotToSection(APPARATUS_SLOT);
	SlotToSection(OUTFIT_SLOT);

	DRAG_DROP_LIST list = UITopList[BELT_SLOT].GetDragDropItemsList();
	DRAG_DROP_LIST_it it;

	for (it = list.begin(); it != list.end(); ++it)
		BeltToSection(static_cast<CUIDragDropItemMP*>(*it));

	//for (int i = 0; i < m_iUsedItems; ++i)
	//	BeltToSection(&m_vDragDropItems[i]);

//	UIBagWnd.ReturnAllChildren();

	IgnoreMoney(false);
}

//////////////////////////////////////////////////////////////////////////

void CUIBuyWeaponWnd::SectionToSlot(const char *sectionName, bool bRealRepresentationSet)
{
	CUIDragDropItemMP* pDDItem = UIBagWnd.GetItemBySectoin(sectionName);	
	int iSlot = pDDItem->GetSlot();

	if (UIBagWnd.IsItemInBag(pDDItem))
		if (UITopList[iSlot].GetDragDropItemsList().empty() || GRENADE_SLOT == iSlot)
		{
			pDDItem->m_bHasRealRepresentation = bRealRepresentationSet;
			SendMessage(pDDItem, DRAG_DROP_ITEM_DB_CLICK, NULL);
		}

	//for (int i = 0; i < m_iUsedItems; ++i)
	//{
	//	if (0 == xr_strcmp(m_vDragDropItems[i].GetSectionName(), sectionName))
	//	{
	//		if (IsItemInShop(i))
	//		{
	//			CUIDragDropItemMP &DDItemMP = m_vDragDropItems[i];
	//			// Если слот не пуст то не перемещаем
	//			if (UITopList[DDItemMP.GetSlot()].GetDragDropItemsList().empty() || GRENADE_SLOT == DDItemMP.GetSlot())
	//			{
	//				DDItemMP.m_bHasRealRepresentation = bRealRepresentationSet;
	//				SendMessage(&DDItemMP, DRAG_DROP_ITEM_DB_CLICK, NULL);
	//			}
	//			break;
	//		}
	//	}
	//}
}

//////////////////////////////////////////////////////////////////////////

void CUIBuyWeaponWnd::SectionToSlot(const u8 grpNum, u8 uIndexInSlot, bool bRealRepresentationSet)
{
	// get item
	CUIDragDropItemMP* pDDItem = UIBagWnd.GetItemBySectoin(grpNum, uIndexInSlot);

	// "Выкусываем" флаги аддонов
	u8 uAddonFlags = GetWeaponAddonInfoByIndex(uIndexInSlot);
	uIndexInSlot &= 0x1f; // 0x1f = 00011111;

	if (UIBagWnd.IsItemInBag(pDDItem))
		if (UITopList[pDDItem->GetSlot()].GetDragDropItemsList().empty() || GRENADE_SLOT == pDDItem->GetSlot() || NO_ACTIVE_SLOT == pDDItem->GetSlot())
		{
			pDDItem->m_bHasRealRepresentation = bRealRepresentationSet;
			SendMessage(pDDItem, DRAG_DROP_ITEM_DB_CLICK, NULL);
			// Проверяем индекс на наличие флагов аддонов, и если они есть, то 
			// аттачим аддоны к мувнутому оружию
			if (uAddonFlags != 0)
			{
				if ((uAddonFlags & 1) != 0)
					pDDItem->AttachDetachAddon(CUIDragDropItemMP::ID_SCOPE, true, bRealRepresentationSet);
				if ((uAddonFlags & 2) != 0)
					pDDItem->AttachDetachAddon(CUIDragDropItemMP::ID_GRENADE_LAUNCHER, true, bRealRepresentationSet);
				if ((uAddonFlags & 4) != 0)
					pDDItem->AttachDetachAddon(CUIDragDropItemMP::ID_SILENCER, true, bRealRepresentationSet);
			}
		}
		
	//for (int i = 0; i < m_iUsedItems; ++i)
	//{
	//	if (grpNum == m_vDragDropItems[i].GetSectionGroupID() &&
	//		uIndexInSlot == m_vDragDropItems[i].GetPosInSectionsGroup())
	//	{
	//		if (IsItemInShop(i))
	//		{
	//			CUIDragDropItemMP &DDItemMP = m_vDragDropItems[i];
	//			// Если слот не пуст то не перемещаем
	//			if (UITopList[DDItemMP.GetSlot()].GetDragDropItemsList().empty() || GRENADE_SLOT == DDItemMP.GetSlot() || NO_ACTIVE_SLOT == DDItemMP.GetSlot())
	//			{
	//				DDItemMP.m_bHasRealRepresentation = bRealRepresentationSet;
	//				SendMessage(&DDItemMP, DRAG_DROP_ITEM_DB_CLICK, NULL);
	//				// Проверяем индекс на наличие флагов аддонов, и если они есть, то 
	//				// аттачим аддоны к мувнутому оружию
	//				if (uAddonFlags != 0)
	//				{
	//					if ((uAddonFlags & 1) != 0)
	//						DDItemMP.AttachDetachAddon(CUIDragDropItemMP::ID_SCOPE, true, bRealRepresentationSet);
	//					if ((uAddonFlags & 2) != 0)
	//						DDItemMP.AttachDetachAddon(CUIDragDropItemMP::ID_GRENADE_LAUNCHER, true, bRealRepresentationSet);
	//					if ((uAddonFlags & 4) != 0)
	//						DDItemMP.AttachDetachAddon(CUIDragDropItemMP::ID_SILENCER, true, bRealRepresentationSet);
	//				}
	//			}
	//			break;
	//		}
	//	}
	//}
}

//////////////////////////////////////////////////////////////////////////
//
//bool CUIBuyWeaponWnd::IsItemInShop(int idx)
//{
//	CUIDragDropItemMP &DDItemMP = m_vDragDropItems[idx];
//	if (std::find(m_WeaponSubBags[DDItemMP.GetSectionGroupID()]->GetDragDropItemsList().begin(),
//		m_WeaponSubBags[DDItemMP.GetSectionGroupID()]->GetDragDropItemsList().end(),
//		&DDItemMP) != m_WeaponSubBags[DDItemMP.GetSectionGroupID()]->GetDragDropItemsList().end())
//	{
//		return true;
//	}
//
//	return false;
//}


//////////////////////////////////////////////////////////////////////////

void CUIBuyWeaponWnd::ClearRealRepresentationFlags()
{
	UIBagWnd.ClearRealRepresentationFlags();
}

//////////////////////////////////////////////////////////////////////////

void CUIBuyWeaponWnd::SetMoneyAmount(int moneyAmount)
{
	m_iMoneyAmount = moneyAmount;
	if (!m_bIgnoreMoney)
		UIBagWnd.UpdateBuyPossibility();
}

//////////////////////////////////////////////////////////////////////////

bool CUIBuyWeaponWnd::CanBuyAllItems()
{
	for (int i = 0; i < MP_SLOTS_NUM; ++i)
	{
		for (DRAG_DROP_LIST_it it = UITopList[i].GetDragDropItemsList().begin();
			 it != UITopList[i].GetDragDropItemsList().end(); ++it)
		{
			if (cUnableToBuy == (*it)->GetColor())
				return false;
		}
	}

	return true;
}


//////////////////////////////////////////////////////////////////////////

int CUIBuyWeaponWnd::GetMoneyAmount() const
{
	if (!m_bIgnoreMoney)
	{
		return m_iMoneyAmount;
	}
	// Retuin some really big value
	return 10000000;
}

//////////////////////////////////////////////////////////////////////////

CUIDragDropItemMP * CUIBuyWeaponWnd::GetAddonByID(CUIDragDropItemMP *pAddonOwner, CUIDragDropItemMP::AddonIDs ID)
{
	return UIBagWnd.GetAddonByID(pAddonOwner, ID);
}

//////////////////////////////////////////////////////////////////////////

CUIDragDropItemMP * CUIBuyWeaponWnd::IsItemAnAddon(CUIDragDropItemMP *pPossibleAddon, CUIDragDropItemMP::AddonIDs &ID)
{
	return UIBagWnd.IsItemAnAddon(pPossibleAddon, ID);
	//R_ASSERT(pPossibleAddon);

	//// проверяем не аддон ли pPossibleAddon вещи в слот(е/ах)
	//if (!UITopList[RIFLE_SLOT].GetDragDropItemsList().empty())
	//{
	//	CUIDragDropItemMP * pDDItemMP = smart_cast<CUIDragDropItemMP*>(UITopList[RIFLE_SLOT].GetDragDropItemsList().front());

	//	if (pDDItemMP && pDDItemMP->bAddonsAvailable)
	//	{
	//		for (u8 j = 0; j < CUIDragDropItemMP::NUM_OF_ADDONS; ++j)
	//		{
	//			// Если один из типов аддонов
	//			if (pPossibleAddon->GetSectionName() == pDDItemMP->m_AddonInfo[j].strAddonName)
	//			{
	//				ID = static_cast<CUIDragDropItemMP::AddonIDs>(j);
	//				return pDDItemMP;
	//			}
	//		}
	//	}
	//}

	//return NULL;
}

//////////////////////////////////////////////////////////////////////////

bool CUIBuyWeaponWnd::IsItemAnAddonSimple(CUIDragDropItemMP *pPossibleAddon) const
{
	return UIBagWnd.IsItemAnAddonSimple(pPossibleAddon);
	//R_ASSERT(pPossibleAddon);
	//if (!pPossibleAddon) return false;

	//xr_string str = pPossibleAddon->GetSectionName();
	//return str.find("addon") != xr_string::npos;
}

//////////////////////////////////////////////////////////////////////////

void CUIBuyWeaponWnd::ApplyFilter(int slotNum, const shared_str &name, const shared_str &value)
{
	//CUIDragDropList *shop = m_WeaponSubBags[slotNum];

	//// Сначала восстанавливаем полный список оружия
	////ClearWpnSubBag(slotNum);
	////FillWpnSubBag(slotNum);

	//// Если параметры name и value пусты, то ничего не делаем. Это эквивалентно отмене фильтра
	//if (!name || !value) return;

	//int idx = 0;
	//// Пробегаемся по всему списку и перекидываем в последний список оружие затребованного типа
	//for (DRAG_DROP_LIST_it it = shop->GetDragDropItemsList().begin(); it != shop->GetDragDropItemsList().end();)
	//{
	//	CUIDragDropItemMP *pDDItemMP = smart_cast<CUIDragDropItemMP*>(*it++);
	//	R_ASSERT(pDDItemMP);

	//	if (0 != xr_strcmp(value, pSettings->r_string(pDDItemMP->GetSectionName(), *name)))
	//	{
	//		// Нашли оружие не соответствующее фильтру
	//		RemoveWeapon(shop, pDDItemMP);
	//	}
	//	else
	//	{
	//		if (!UITopList[RIFLE_SLOT].GetDragDropItemsList().empty() &&
	//			0 == xr_strcmp(pDDItemMP->GetSectionName(),	smart_cast<CUIDragDropItemMP*>(UITopList[RIFLE_SLOT].GetDragDropItemsList().front())->GetSectionName()))
	//		{
	//			RemoveWeapon(shop, pDDItemMP);
	//			++idx;
	//		}
	//		else
	//		{
	//			pDDItemMP->SetPosInSectionsGroup(idx++);
	//		}
	//	}
	//}

	//shop->RearrangeItems();
}

//////////////////////////////////////////////////////////////////////////
//
//void CUIBuyWeaponWnd::RemoveWeapon(CUIDragDropList *shop, CUIDragDropItem *item)
//{
//	shop->DetachChild(item);
//
//	for (int i = 0; i < m_iUsedItems; ++i)
//	{
//		if (&m_vDragDropItems[i] == item)
//		{
//			m_iEmptyIndexes.insert(i);
//			break;
//		}
//	}
//}

//////////////////////////////////////////////////////////////////////////

void CUIBuyWeaponWnd::FillItemInfo(CUIDragDropItemMP *pDDItemMP)
{
	{
		UIItemInfo.UIName.SetText			("");
		UIItemInfo.UIWeight.SetText			("");
		UIItemInfo.UIItemImage.TextureOff	();
		UIItemInfo.UIDesc.RemoveAll			();
		if (pDDItemMP)
		{
			CStringTable stbl;
			if (pSettings->line_exist(pDDItemMP->GetSectionName(), "inv_name"))
				UIItemInfo.UIName.SetText(*stbl(pSettings->r_string(pDDItemMP->GetSectionName(), "inv_name")));

			UIItemInfo.UIItemImage.SetShader(pDDItemMP->GetUIStaticItem().GetShader());
			Irect o_r = pDDItemMP->GetUIStaticItem().GetOriginalRect();
			Irect v_r = pDDItemMP->GetUIStaticItem().GetRect();
			UIItemInfo.UIItemImage.GetUIStaticItem().SetOriginalRect(o_r.x1, o_r.y1, o_r.width(), o_r.height());
			UIItemInfo.UIItemImage.GetUIStaticItem().SetRect(v_r);
			UIItemInfo.UIItemImage.SetStretchTexture(true);
			UIItemInfo.UIItemImage.SetWidth	(_min(v_r.width(),UIItemInfo.UIItemImageSize.x));
			UIItemInfo.UIItemImage.SetHeight(_min(v_r.height(),UIItemInfo.UIItemImageSize.y));
			UIItemInfo.UIItemImage.TextureOn();

			if (pSettings->line_exist(pDDItemMP->GetSectionName(), WEAPON_DESCRIPTION_FIELD))
			{
				CUIString str;
				str.SetText(*CStringTable()(pSettings->r_string(pDDItemMP->GetSectionName(), WEAPON_DESCRIPTION_FIELD)));
				UIItemInfo.UIDesc.AddParsedItem<CUIListItem>(str, 0, UIItemInfo.UIDesc.GetTextColor());
			}
		}
	}
}

bool CUIBuyWeaponWnd::BagProc(CUIDragDropItem* pItem, CUIDragDropList* pList)
{
	CUIBuyWeaponWnd* this_inventory = smart_cast<CUIBuyWeaponWnd*>(pList->GetTop());
	R_ASSERT2(this_inventory, "wrong parent addressed");

	CUIDragDropItemMP* pDDItemMP = smart_cast<CUIDragDropItemMP*>(pItem);
	R_ASSERT(pDDItemMP);
	
	pDDItemMP->AttachDetachAllAddons(false);

	// Need to delete copy before adding
	if (pDDItemMP->m_bIsInfinite)
        this_inventory->UIBagWnd.DeleteCopy(pDDItemMP);

	pDDItemMP->GetParent()->DetachChild(pDDItemMP);
	pDDItemMP->GetOwner()->AttachChild(pDDItemMP);
	pDDItemMP->Rescale(pDDItemMP->GetOwner()->GetItemsScaleX(), pDDItemMP->GetOwner()->GetItemsScaleY());

	if (OUTFIT_SLOT == pDDItemMP->GetSlot())
		this_inventory->UIOutfitIcon.SetColor(cAbleToBuy);

	if (pDDItemMP->m_bAlreadyPaid)
	{
		this_inventory->SetMoneyAmount(this_inventory->GetMoneyAmount() +
			static_cast<int>(pDDItemMP->GetCost() * (pDDItemMP->m_bHasRealRepresentation ? fRealItemSellMultiplier : 1)));

		// Если у вещи есть аддоны, то прибавляем и также и их половинную стоимость
		pDDItemMP->m_bAlreadyPaid = false;
	}

	if (pDDItemMP->GetCost() > this_inventory->GetMoneyAmount() && !pDDItemMP->m_bHasRealRepresentation)
		pDDItemMP->EnableDragDrop(false);

	// Если это армор, то убедимся, что он стал видимым
	if (OUTFIT_SLOT == pDDItemMP->GetSlot())
		pDDItemMP->Show(true);

	return false;
}
