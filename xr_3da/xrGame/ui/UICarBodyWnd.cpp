// UICarBodyWnd.cpp:  диалог переложения вещей из багажника и с трупа 
//					  себе в инвентарь
//////////////////////////////////////////////////////////////////////



#include "stdafx.h"
#include "UITradeWnd.h"
#include "xrXMLParser.h"
#include "UIXmlInit.h"
#include "../Entity.h"
#include "../HUDManager.h"
#include "../WeaponAmmo.h"
#include "../Actor.h"
#include "../Car.h"
#include "../Trade.h"
#include "../UIGameSP.h"
#include "../eatable_item.h"
#include "../inventory.h"
#include "UIInventoryUtilities.h"
#include "../level.h"

//////////////////////////////////////////////////////////////////////////

using namespace InventoryUtilities;

const char * const CAR_BODY_XML		= "carbody_new.xml";
const char * const CARBODY_ITEM_XML	= "carbody_item.xml";

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUICarBodyWnd::CUICarBodyWnd()
{
	Init();
	Hide();

	SetFont(HUD().pFontMedium);
}

//////////////////////////////////////////////////////////////////////////

CUICarBodyWnd::~CUICarBodyWnd()
{
	ClearDragDrop(m_vDragDropItems);
}

//////////////////////////////////////////////////////////////////////////

void CUICarBodyWnd::Init()
{
	CUIXml uiXml;
	uiXml.Init("$game_data$", CAR_BODY_XML);
	
	CUIXmlInit xml_init;

	CUIWindow::Init(CUIXmlInit::ApplyAlignX(0, alCenter),
					CUIXmlInit::ApplyAlignY(0, alCenter),
					UI_BASE_WIDTH, UI_BASE_HEIGHT);

	//статические элементы интерфейса
	AttachChild(&UIStaticTop);
	UIStaticTop.Init("ui\\ui_top_background", 0,0,UI_BASE_WIDTH,128);
	AttachChild(&UIStaticBottom);
	UIStaticBottom.Init("ui\\ui_bottom_background", 0,UI_BASE_HEIGHT-32,UI_BASE_WIDTH,32);

	//иконки с изображение нас и партнера по торговле
	AttachChild(&UIOurIcon);
	xml_init.InitStatic(uiXml, "static_icon", 0, &UIOurIcon);
	AttachChild(&UIOthersIcon);
	xml_init.InitStatic(uiXml, "static_icon", 1, &UIOthersIcon);
	UIOurIcon.AttachChild(&UICharacterInfoLeft);
	UICharacterInfoLeft.Init(0,0, UIOurIcon.GetWidth(), UIOurIcon.GetHeight(), "trade_character.xml");
	UIOthersIcon.AttachChild(&UICharacterInfoRight);
	UICharacterInfoRight.Init(0,0, UIOthersIcon.GetWidth(), UIOthersIcon.GetHeight(), "trade_character.xml");

	//Списки торговли
	AttachChild(&UIOurBagWnd);
	xml_init.InitStatic(uiXml, "our_bag_static", 0, &UIOurBagWnd);
	AttachChild(&UIOthersBagWnd);
	xml_init.InitStatic(uiXml, "others_bag_static", 0, &UIOthersBagWnd);

	//Списки Drag&Drop
	UIOurBagWnd.AttachChild(&UIOurBagList);	
	xml_init.InitDragDropList(uiXml, "dragdrop_list", 0, &UIOurBagList);
	UIOurBagList.SetItemsScale(TRADE_ICONS_SCALE);

	UIOthersBagWnd.AttachChild(&UIOthersBagList);	
	xml_init.InitDragDropList(uiXml, "dragdrop_list", 1, &UIOthersBagList);
	UIOthersBagList.SetItemsScale(TRADE_ICONS_SCALE);


	//информация о предмете
	AttachChild(&UIDescWnd);
	xml_init.InitFrameWindow(uiXml, "frame_window", 0, &UIDescWnd);
	UIDescWnd.AttachChild(&UIStaticDesc);
	xml_init.InitStatic(uiXml, "descr_static", 0, &UIStaticDesc);
	UIDescWnd.AttachChild(&UIItemInfo);
	UIItemInfo.Init(0,0, UIDescWnd.GetWidth(), UIDescWnd.GetHeight(), CARBODY_ITEM_XML);


	//Элементы автоматического добавления
	xml_init.InitAutoStatic(uiXml, "auto_static", this);

	AttachChild(&UIPropertiesBox);
	UIPropertiesBox.Init("ui\\ui_frame",0,0,300,300);
	UIPropertiesBox.Hide();

	AttachChild(&UIMessageBox);
	UIMessageBox.Init("ui\\ui_frame", 0, 0, 300, 300);
	UIMessageBox.AutoCenter();
	UIMessageBox.Hide();
	
	//////
	UIOurBagList.SetCheckProc(OurBagProc);
	UIOthersBagList.SetCheckProc(OthersBagProc);

	SetCurrentItem(NULL);
	m_pCurrentDragDropItem = NULL;
	UIStaticDesc.SetText(NULL);

	AttachChild(&UITakeAll);
	xml_init.InitButton(uiXml, "take_all_btn", 0, &UITakeAll);
}

//////////////////////////////////////////////////////////////////////////

void CUICarBodyWnd::InitCarBody(CInventory* pOurInv,    CGameObject* pOurObject,
								CInventory* pOthersInv, CGameObject* pOthersObject)
{
	
    m_pOurObject = pOurObject;
	m_pOthersObject = pOthersObject;

	m_pInv = pOurInv;
	m_pOthersInv = pOthersInv;

	
	CInventoryOwner* pOurInvOwner = smart_cast<CInventoryOwner*>(pOurObject);
	UICharacterInfoLeft.InitCharacter(pOurInvOwner);
	CInventoryOwner* pOthersInvOwner = smart_cast<CInventoryOwner*>(pOthersObject);
	if(pOthersInvOwner)	UICharacterInfoRight.InitCharacter(pOthersInvOwner);
	//CCar* pOthersCar = smart_cast<CCar*>(pOthersObject);	
	

	m_pMouseCapturer = NULL;
	UIPropertiesBox.Hide();
	UIMessageBox.Hide();
	EnableAll();

	UpdateLists();
}  

//////////////////////////////////////////////////////////////////////////

void CUICarBodyWnd::UpdateLists()
{

	//очистить после предыдущего запуска
	UIOurBagList.DropAll();
	UIOthersBagList.DropAll();

	ClearDragDrop(m_vDragDropItems);

	ruck_list.clear();
	m_pInv->AddAvailableItems(ruck_list);
	ruck_list.sort(GreaterRoomInRuck);

	//Наш рюкзак
	PPIItem it;
	for(it =  ruck_list.begin(); ruck_list.end() != it; ++it) 
	{
		if((*it)) 
		{
	//		CUIDragDropItem& UIDragDropItem = m_vDragDropItems[m_iUsedItems];		
			m_vDragDropItems.push_back(xr_new<CUIWpnDragDropItem>());
			CUIDragDropItem& UIDragDropItem = *m_vDragDropItems.back();


			UIDragDropItem.CUIStatic::Init(0, 0, INV_GRID_WIDTH, INV_GRID_HEIGHT);
			UIDragDropItem.SetShader(GetEquipmentIconsShader());

			UIDragDropItem.SetGridHeight((*it)->GetGridHeight());
			UIDragDropItem.SetGridWidth((*it)->GetGridWidth());

			UIDragDropItem.GetUIStaticItem().SetOriginalRect(
									(*it)->GetXPos()*INV_GRID_WIDTH,
									(*it)->GetYPos()*INV_GRID_HEIGHT,
									(*it)->GetGridWidth()*INV_GRID_WIDTH,
									(*it)->GetGridHeight()*INV_GRID_HEIGHT);

			UIDragDropItem.SetData((*it));

			UIDragDropItem.SetFont(HUD().pFontLetterica16Russian);

			CWeaponAmmo* pWeaponAmmo  = smart_cast<CWeaponAmmo*>((*it));
			if(pWeaponAmmo)	UIDragDropItem.SetCustomUpdate(AmmoUpdateProc);

			CEatableItem* pEatableItem = smart_cast<CEatableItem*>((*it));
			if(pEatableItem) UIDragDropItem.SetCustomUpdate(FoodUpdateProc);


			//установить коэффициент масштабирования
			UIDragDropItem.SetTextureScale(TRADE_ICONS_SCALE);
				
			UIOurBagList.AttachChild(&UIDragDropItem);
	//		++m_iUsedItems;
		}
	}


	ruck_list.clear();
	m_pOthersInv->AddAvailableItems(ruck_list);
	ruck_list.sort(GreaterRoomInRuck);

	//Чужой рюкзак
	for(it =  ruck_list.begin(); ruck_list.end() != it; ++it) 
	{
		if((*it)) 
		{
			//CUIDragDropItem& UIDragDropItem = m_vDragDropItems[m_iUsedItems];		
			m_vDragDropItems.push_back(xr_new<CUIWpnDragDropItem>());
			CUIDragDropItem& UIDragDropItem = *m_vDragDropItems.back();

				
			UIDragDropItem.CUIStatic::Init(0, 0, INV_GRID_WIDTH, INV_GRID_HEIGHT);
			UIDragDropItem.SetShader(GetEquipmentIconsShader());

			UIDragDropItem.SetGridHeight((*it)->GetGridHeight());
			UIDragDropItem.SetGridWidth((*it)->GetGridWidth());

			UIDragDropItem.GetUIStaticItem().SetOriginalRect(
									(*it)->GetXPos()*INV_GRID_WIDTH,
									(*it)->GetYPos()*INV_GRID_HEIGHT,
									(*it)->GetGridWidth()*INV_GRID_WIDTH,
									(*it)->GetGridHeight()*INV_GRID_HEIGHT);

			UIDragDropItem.SetData((*it));

			UIDragDropItem.SetFont(HUD().pFontLetterica16Russian);

			CWeaponAmmo* pWeaponAmmo  = smart_cast<CWeaponAmmo*>((*it));
			if(pWeaponAmmo)	UIDragDropItem.SetCustomUpdate(AmmoUpdateProc);

			CEatableItem* pEatableItem = smart_cast<CEatableItem*>((*it));
			if(pEatableItem) UIDragDropItem.SetCustomUpdate(FoodUpdateProc);

			//установить коэффициент масштабирования
			UIDragDropItem.SetTextureScale(TRADE_ICONS_SCALE);
			UIOthersBagList.AttachChild(&UIDragDropItem);
//			++m_iUsedItems;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
//как только подняли элемент, сделать его текущим
//////////////////////////////////////////////////////////////////////////

void CUICarBodyWnd::SendMessage(CUIWindow *pWnd, s16 msg, void *pData)
{

	if(msg == DRAG_DROP_ITEM_DRAG)
	{
		if (m_pCurrentDragDropItem) m_pCurrentDragDropItem->Highlight(false);
		PIItem pInvItem = (PIItem)((CUIDragDropItem*)pWnd)->GetData();
		m_pCurrentDragDropItem = (CUIDragDropItem*)pWnd;
		SetCurrentItem(pInvItem);
		m_pCurrentDragDropItem->Rescale(1.0f);
	}
	else if(msg == DRAG_DROP_ITEM_DB_CLICK)
	{
		if (m_pCurrentDragDropItem) m_pCurrentDragDropItem->Highlight(false);
		PIItem pInvItem = (PIItem)((CUIDragDropItem*)pWnd)->GetData();
		m_pCurrentDragDropItem = (CUIDragDropItem*)pWnd;
		
		SetCurrentItem(pInvItem);

		if(m_pCurrentDragDropItem->GetParent() == &UIOurBagList)
			ToOthersBag();
		else if(m_pCurrentDragDropItem->GetParent() == &UIOthersBagList)
			ToOurBag();
		else
			R_ASSERT2(false, "wrong parent for car_body wnd");
	}
	else if (BUTTON_CLICKED == msg && &UITakeAll == pWnd)
	{
		TakeAll();
	}

	CUIWindow::SendMessage(pWnd, msg, pData);
}

//////////////////////////////////////////////////////////////////////////

void CUICarBodyWnd::Draw()
{
	inherited::Draw();
}

//////////////////////////////////////////////////////////////////////////

void CUICarBodyWnd::Update()
{
	//убрать объект drag&drop для уже использованной вещи
	for(u32 i = 0; i <m_vDragDropItems.size(); ++i) 
	{
		CInventoryItem* pItem = (CInventoryItem*)m_vDragDropItems[i]->GetData();
		if(pItem && !pItem->Useful())
		{
			m_vDragDropItems[i]->GetParent()->DetachChild(m_vDragDropItems[i]);
			m_vDragDropItems[i]->SetData(NULL);
			m_vDragDropItems[i]->SetCustomUpdate(NULL);

			xr_delete(m_vDragDropItems[i]);
			m_vDragDropItems.erase(m_vDragDropItems.begin()+i);
		}
	}

	inherited::Update();
}

//////////////////////////////////////////////////////////////////////////

void CUICarBodyWnd::Show() 
{ 
	inherited::Show();
	m_pCurrentDragDropItem = NULL;
}

//////////////////////////////////////////////////////////////////////////

//при вызове проверки необходимо помнить 
//иерархию окон, чтоб знать какой именно из
//родителей является CUIInventoryWnd и 
//содержит свойство GetInventory()
bool CUICarBodyWnd::OurBagProc(CUIDragDropItem* pItem, CUIDragDropList* pList)
{
	CUICarBodyWnd* this_car_body_wnd =  smart_cast<CUICarBodyWnd*>(pList->GetParent()->GetParent());
	R_ASSERT2(this_car_body_wnd, "wrong parent addressed as trade wnd");

	PIItem pIItem = (PIItem)(pItem->GetData());
	
	//другому инвентарю - отдать вещь
	NET_Packet						P;
	this_car_body_wnd->m_pOthersObject->u_EventGen(P,GE_OWNERSHIP_REJECT,
										this_car_body_wnd->m_pOthersObject->ID());
	P.w_u16							   (u16(pIItem->ID()));
	this_car_body_wnd->m_pOthersObject->u_EventSend	(P);

	//нам - взять вещь 
	this_car_body_wnd->m_pOurObject->u_EventGen(P,GE_OWNERSHIP_TAKE,
										this_car_body_wnd->m_pOurObject->ID());
	P.w_u16							(u16(pIItem->ID()));
	this_car_body_wnd->m_pOurObject->u_EventSend	(P);

	return true;
}

//////////////////////////////////////////////////////////////////////////

bool CUICarBodyWnd::OthersBagProc(CUIDragDropItem* pItem, CUIDragDropList* pList)
{
	CUICarBodyWnd* this_car_body_wnd =  smart_cast<CUICarBodyWnd*>(pList->GetParent()->GetParent());
	R_ASSERT2(this_car_body_wnd, "wrong parent addressed as trade wnd");



	PIItem pIItem = (PIItem)(pItem->GetData());

	//нам - отдать вещь
	NET_Packet P;
	this_car_body_wnd->m_pOurObject->u_EventGen(P,GE_OWNERSHIP_REJECT,
									 this_car_body_wnd->m_pOurObject->ID());
	P.w_u16							 (u16(pIItem->ID()));
	this_car_body_wnd->m_pOurObject->u_EventSend			(P);

	//другому инвентарю - взять вещь 
	this_car_body_wnd->m_pOthersObject->u_EventGen(P,GE_OWNERSHIP_TAKE,
										this_car_body_wnd->m_pOthersObject->ID());
	P.w_u16								(u16(pIItem->ID()));
	this_car_body_wnd->m_pOthersObject->u_EventSend	(P);

	return true;
}

//////////////////////////////////////////////////////////////////////////

bool CUICarBodyWnd::ToOurBag()
{
	if(!OurBagProc(m_pCurrentDragDropItem, 
				   &UIOurBagList)) return false;

	((CUIDragDropList*)m_pCurrentDragDropItem->GetParent())->
											   DetachChild(m_pCurrentDragDropItem);
	UIOurBagList.AttachChild(m_pCurrentDragDropItem);

	m_pMouseCapturer = NULL;

	return true;
}

//////////////////////////////////////////////////////////////////////////

bool CUICarBodyWnd::ToOthersBag()
{
	if(!OthersBagProc(m_pCurrentDragDropItem, 
				      &UIOthersBagList)) return false;

	((CUIDragDropList*)m_pCurrentDragDropItem->GetParent())->DetachChild(m_pCurrentDragDropItem);
	UIOthersBagList.AttachChild(m_pCurrentDragDropItem);

	m_pMouseCapturer = NULL;

	return true;
}

//////////////////////////////////////////////////////////////////////////

void CUICarBodyWnd::DisableAll()
{
	UIOurBagWnd.Enable(false);
	UIOthersBagWnd.Enable(false);
}

//////////////////////////////////////////////////////////////////////////

void CUICarBodyWnd::EnableAll()
{
	UIOurBagWnd.Enable(true);
	UIOthersBagWnd.Enable(true);
}

//////////////////////////////////////////////////////////////////////////

void CUICarBodyWnd::SetCurrentItem(CInventoryItem* pItem)
{
	m_pCurrentItem = pItem;
	const int offset = -10;

	UIItemInfo.InitItem(m_pCurrentItem);
	UIItemInfo.AlignRight(UIItemInfo.UIWeight, offset);
	UIItemInfo.AlignRight(UIItemInfo.UICost, offset);
	UIItemInfo.AlignRight(UIItemInfo.UICondition, offset);
}

//////////////////////////////////////////////////////////////////////////

void CUICarBodyWnd::TakeAll()
{
	for (DRAG_DROP_LIST_it it = UIOthersBagList.GetDragDropItemsList().begin();
		 it != UIOthersBagList.GetDragDropItemsList().end();)
	{
		SendMessage(*it++, DRAG_DROP_ITEM_DB_CLICK, NULL);
	}
}