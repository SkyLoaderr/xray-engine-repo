// UICarBodyWnd.cpp:  диалог переложения вещей из багажника и с трупа 
//					  себе в инвентарь
//////////////////////////////////////////////////////////////////////



#include "stdafx.h"
#include "UITradeWnd.h"

#include "xrXMLParser.h"
#include "UIXmlInit.h"



#include "..\\Entity.h"
#include "..\\HUDManager.h"
#include "..\\WeaponAmmo.h"
#include "..\\Actor.h"
#include "..\\Trade.h"
#include "..\\UIGameSP.h"
#include "..\\gameobject.h"

#include "UIInventoryUtilities.h"
using namespace InventoryUtilities;

#define TRADE_ICONS_SCALE (4.f/5.f)


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUICarBodyWnd::CUICarBodyWnd()
{
	Init();
	Hide();

	SetFont(HUD().pFontMedium);
}

CUICarBodyWnd::~CUICarBodyWnd()
{
}

void CUICarBodyWnd::Init()
{
	CUIXml uiXml;
	uiXml.Init("$game_data$","trade.xml");
	
	CUIXmlInit xml_init;

	CUIWindow::Init(0,0, Device.dwWidth, Device.dwHeight);

	//статические элементы интерфейса
	//AttachChild(&UIStaticTop);
	//UIStaticTop.Init("ui\\ui_inv_quick_slots", 0,0,1024,128);
	AttachChild(&UIStaticBottom);
	UIStaticBottom.Init("ui\\ui_bottom_background", 0,Device.dwHeight-32,1024,32);

	//Списки торговли
	AttachChild(&UIOurBagWnd);
	xml_init.InitFrameWindow(uiXml, "frame_window", 0, &UIOurBagWnd);
	AttachChild(&UIOthersBagWnd);
	xml_init.InitFrameWindow(uiXml, "frame_window", 1, &UIOthersBagWnd);

	//Списки Drag&Drop
	UIOurBagWnd.AttachChild(&UIOurBagList);	
	xml_init.InitDragDropList(uiXml, "dragdrop_list", 0, &UIOurBagList);

	UIOthersBagWnd.AttachChild(&UIOthersBagList);	
	xml_init.InitDragDropList(uiXml, "dragdrop_list", 1, &UIOthersBagList);



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

	//Кнопки
//	AttachChild(&UIPerformTradeButton);
//	xml_init.InitButton(uiXml, "button", 0, &UIPerformTradeButton);

//	AttachChild(&UIToTalkButton);
//	xml_init.InitButton(uiXml, "button", 1, &UIToTalkButton);

	m_pCurrentItem = NULL;
	m_pCurrentDragDropItem = NULL;
	UIStaticDesc.SetText(NULL);
}

void CUICarBodyWnd::InitCarBody(CInventory* pOurInv,    CGameObject* pOurObject,
								CInventory* pOthersInv, CGameObject* pOthersObject)
{
	
    m_pOurObject = pOurObject;
	m_pOthersObject = pOthersObject;

	m_pInv = pOurInv;
	m_pOthersInv = pOthersInv;
	

	m_pMouseCapturer = NULL;
	UIPropertiesBox.Hide();

	UIMessageBox.Hide();
	EnableAll();

	UpdateLists();
}  

void CUICarBodyWnd::UpdateLists()
{
	//очистить после предыдущего запуска
	UIOurBagList.DropAll();
	UIOthersBagList.DropAll();

	for(u32 i = 0; i <MAX_ITEMS; i++) 
	{
		m_vDragDropItems[i].SetData(NULL);
		m_vDragDropItems[i].SetWndRect(0,0,0,0);
		m_vDragDropItems[i].SetCustomUpdate(NULL);
	}

	
	m_iUsedItems = 0;

	ruck_list = m_pInv->m_ruck;
	ruck_list.sort(GreaterRoomInRuck);

	//Наш рюкзак
	PPIItem it;
	for(it =  ruck_list.begin(); it !=  ruck_list.end(); it++) 
	{
			if((*it)) 
			{
				CUIDragDropItem& UIDragDropItem = m_vDragDropItems[m_iUsedItems];		
//				UIDragDropItem.Init((*it)->m_sIconTexture, 0,0, 50,50);

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


				//установить коэффициент масштабирования
				UIDragDropItem.SetTextureScale(TRADE_ICONS_SCALE);
				
				UIOurBagList.AttachChild(&UIDragDropItem);
				m_iUsedItems++;
			}
	}


	//ruck_list = m_pOthersInv->m_ruck;
	ruck_list.clear();
	ruck_list.insert(ruck_list.begin(),
					m_pOthersInv->m_all.begin(),
					 m_pOthersInv->m_all.end());
/*	ruck_list.insert(ruck_list.begin(),
					 m_pOthersInv->m_ruck.begin(),
					 m_pOthersInv->m_ruck.end());
	ruck_list.insert(ruck_list.end(),
					 m_pOthersInv->m_belt.begin(),
					 m_pOthersInv->m_belt.end());*/

					 /*m_pOthersInv->m_ruck.begin(),
					 m_pOthersInv->m_ruck.end());*/

	ruck_list.sort(GreaterRoomInRuck);



	//Чужой рюкзак
	for(it =  ruck_list.begin(); it !=  ruck_list.end(); it++) 
	{
			if((*it)) 
			{
				CUIDragDropItem& UIDragDropItem = m_vDragDropItems[m_iUsedItems];		
				
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

				//установить коэффициент масштабирования
				UIDragDropItem.SetTextureScale(TRADE_ICONS_SCALE);
				UIOthersBagList.AttachChild(&UIDragDropItem);
				m_iUsedItems++;
			}
	}
}




//------------------------------------------------
//как только подняли элемент, сделать его текущим
void CUICarBodyWnd::SendMessage(CUIWindow *pWnd, s16 msg, void *pData)
{

/*	if(pWnd == &UIToTalkButton && msg == CUIButton::BUTTON_CLICKED)
	{
		GetParent()->SendMessage(this, TRADE_WND_CLOSED);
	}
	else if(pWnd == &UIMessageBox && msg == CUIMessageBox::OK_CLICKED)
	{
		GetTop()->SetCapture(&UIMessageBox, false);
		UIMessageBox.Hide();
		EnableAll();
	}
	else if(pWnd == &UIPerformTradeButton && msg == CUIButton::BUTTON_CLICKED)
	{
		PerformTrade();
	}
	else*/ if(msg == CUIDragDropItem::ITEM_DRAG)
	{
		PIItem pInvItem = (PIItem)((CUIDragDropItem*)pWnd)->GetData();
		m_pCurrentDragDropItem = (CUIDragDropItem*)pWnd;
		m_pCurrentItem = pInvItem;
		UIStaticDesc.SetText(pInvItem->NameComplex());
	}
	else if(msg == CUIDragDropItem::ITEM_DB_CLICK)
	{
		PIItem pInvItem = (PIItem)((CUIDragDropItem*)pWnd)->GetData();
		m_pCurrentDragDropItem = (CUIDragDropItem*)pWnd;
		m_pCurrentItem = pInvItem;
		UIStaticDesc.SetText(pInvItem->NameComplex());
		

		if(m_pCurrentDragDropItem->GetParent() == &UIOurBagList)
			ToOthersBag();
		else if(m_pCurrentDragDropItem->GetParent() == &UIOthersBagList)
			ToOurBag();
		else
			R_ASSERT2(false, "wrong parent for car_body wnd");
	}


	CUIWindow::SendMessage(pWnd, msg, pData);
}

void CUICarBodyWnd::Draw()
{
	static float y_rotate_angle = 0;
	y_rotate_angle += -0.01f;
   	if(y_rotate_angle>2*PI) y_rotate_angle = 0;
	UI3dStatic.SetGameObject(m_pCurrentItem);
	UI3dStatic.SetRotate(0,y_rotate_angle,0);


	inherited::Draw();
}

void CUICarBodyWnd::Update()
{
	//убрать объект drag&drop для уже использованной вещи
	for(int i = 0; i <m_iUsedItems; i++) 
	{
		CInventoryItem* pItem = (CInventoryItem*)m_vDragDropItems[i].GetData();
		if(pItem && !pItem->Useful())
		{
			m_vDragDropItems[i].GetParent()->DetachChild(&m_vDragDropItems[i]);
			m_vDragDropItems[i].SetData(NULL);
			m_vDragDropItems[i].SetCustomUpdate(NULL);
		}
	}

	inherited::Update();
}

void CUICarBodyWnd::Show() 
{ 
	inherited::Show();
}



//при вызове проверки необходимо помнить 
//иерархию окон, чтоб знать какой именно из
//родителей является CUIInventoryWnd и 
//содержит свойство GetInventory()
bool CUICarBodyWnd::OurBagProc(CUIDragDropItem* pItem, CUIDragDropList* pList)
{
	CUICarBodyWnd* this_car_body_wnd =  dynamic_cast<CUICarBodyWnd*>(pList->GetParent()->GetParent());
	R_ASSERT2(this_car_body_wnd, "wrong parent addressed as trade wnd");

	//return this_car_body_wnd->IsEnoughtOurRoom(pItem);
	PIItem pIItem = (PIItem)(pItem->GetData());

	bool result = this_car_body_wnd->IsEnoughtOurRoom(pItem);
	if(!result) return false;
	
	//попытаться поместить вещь нам в рюкзак
	//result = this_car_body_wnd->m_pInv->Take(pIItem);

//	if(result)
	{
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
//	else
//		return false;
}

bool CUICarBodyWnd::OthersBagProc(CUIDragDropItem* pItem, CUIDragDropList* pList)
{
	CUICarBodyWnd* this_car_body_wnd =  dynamic_cast<CUICarBodyWnd*>(pList->GetParent()->GetParent());
	R_ASSERT2(this_car_body_wnd, "wrong parent addressed as trade wnd");



	PIItem pIItem = (PIItem)(pItem->GetData());

	bool result = this_car_body_wnd->IsEnoughtOthersRoom(pItem);
	if(!result) return false;

	//попытаться поместить вещь другому инвентарю в рюкзак
	//result = this_car_body_wnd->m_pOthersInv->Take(pIItem);

//	if(result)
	{

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
//	else
//		return false;
}

//проверяет влезут ли вещи из окна торговли в рюкзак после покупки
bool CUICarBodyWnd::IsEnoughtOurRoom(CUIDragDropItem* pItem)
{
	ruck_list.clear();
	ruck_list.insert(ruck_list.begin(), 
					 m_pInv->m_ruck.begin(),
					 m_pInv->m_ruck.end());

	//та вещь, что переноситься
	ruck_list.push_back((PIItem)pItem->GetData());
	
	return FreeRoom(ruck_list,RUCK_WIDTH,RUCK_HEIGHT);
}

bool CUICarBodyWnd::IsEnoughtOthersRoom(CUIDragDropItem* pItem)
{
	ruck_list.clear();
	ruck_list.insert(ruck_list.begin(), 
					 m_pOthersInv->m_ruck.begin(),
					 m_pOthersInv->m_ruck.end());

	//та вещь, что переноситься
	ruck_list.push_back((PIItem)pItem->GetData());
	
	return FreeRoom(ruck_list,RUCK_WIDTH,RUCK_HEIGHT);
}


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
bool CUICarBodyWnd::ToOthersBag()
{
	if(!OthersBagProc(m_pCurrentDragDropItem, 
				      &UIOthersBagList)) return false;

	((CUIDragDropList*)m_pCurrentDragDropItem->GetParent())->DetachChild(m_pCurrentDragDropItem);
	UIOthersBagList.AttachChild(m_pCurrentDragDropItem);

	m_pMouseCapturer = NULL;

	return true;
}


void CUICarBodyWnd::DisableAll()
{
	UIOurBagWnd.Enable(false);
	UIOthersBagWnd.Enable(false);
}
void CUICarBodyWnd::EnableAll()
{
	UIOurBagWnd.Enable(true);
	UIOthersBagWnd.Enable(true);
}