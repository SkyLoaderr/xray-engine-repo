// UICarBodyWnd.cpp:  ������ ����������� ����� �� ��������� � � ����� 
//					  ���� � ���������
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
#include "../gameobject.h"

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
	uiXml.Init("$game_data$","carbody.xml");
	
	CUIXmlInit xml_init;

	CUIWindow::Init(0,0, Device.dwWidth, Device.dwHeight);

	//����������� �������� ����������
	AttachChild(&UIStaticTop);
	UIStaticTop.Init("ui\\ui_top_background", 0,0,1024,128);
	AttachChild(&UIStaticBottom);
	UIStaticBottom.Init("ui\\ui_bottom_background", 0,Device.dwHeight-32,1024,32);

	//������ � ����������� ��� � �������� �� ��������
	AttachChild(&UIOurIcon);
	xml_init.InitStatic(uiXml, "static_icon", 0, &UIOurIcon);
	AttachChild(&UIOthersIcon);
	xml_init.InitStatic(uiXml, "static_icon", 1, &UIOthersIcon);
	UIOurIcon.AttachChild(&UICharacterInfoLeft);
	UICharacterInfoLeft.Init(0,0, UIOurIcon.GetWidth(), UIOurIcon.GetHeight(), "trade_character.xml");
	UIOthersIcon.AttachChild(&UICharacterInfoRight);
	UICharacterInfoRight.Init(0,0, UIOthersIcon.GetWidth(), UIOthersIcon.GetHeight(), "trade_character.xml");


	//������ ��������
	AttachChild(&UIOurBagWnd);
	xml_init.InitFrameWindow(uiXml, "frame_window", 0, &UIOurBagWnd);
	AttachChild(&UIOthersBagWnd);
	xml_init.InitFrameWindow(uiXml, "frame_window", 1, &UIOthersBagWnd);

	//������ Drag&Drop
	UIOurBagWnd.AttachChild(&UIOurBagList);	
	xml_init.InitDragDropList(uiXml, "dragdrop_list", 0, &UIOurBagList);

	UIOthersBagWnd.AttachChild(&UIOthersBagList);	
	xml_init.InitDragDropList(uiXml, "dragdrop_list", 1, &UIOthersBagList);


	//���������� � ��������
	AttachChild(&UIDescWnd);
	xml_init.InitFrameWindow(uiXml, "frame_window", 2, &UIDescWnd);
	UIDescWnd.AttachChild(&UIStaticDesc);
	UIStaticDesc.Init("ui\\ui_inv_info_over_b", 5, UIDescWnd.GetHeight() - 310 ,260,310);
	UIStaticDesc.AttachChild(&UIItemInfo);
	UIItemInfo.Init(0,0, UIStaticDesc.GetWidth(), UIStaticDesc.GetHeight(), "inventory_item.xml");


	//�������� ��������������� ����������
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
}

void CUICarBodyWnd::InitCarBody(CInventory* pOurInv,    CGameObject* pOurObject,
								CInventory* pOthersInv, CGameObject* pOthersObject)
{
	
    m_pOurObject = pOurObject;
	m_pOthersObject = pOthersObject;

	m_pInv = pOurInv;
	m_pOthersInv = pOthersInv;

	
	CInventoryOwner* pOurInvOwner = dynamic_cast<CInventoryOwner*>(pOurObject);
	UICharacterInfoLeft.InitCharacter(pOurInvOwner);
	CInventoryOwner* pOthersInvOwner = dynamic_cast<CInventoryOwner*>(pOthersObject);
	if(pOthersInvOwner)	UICharacterInfoRight.InitCharacter(pOthersInvOwner);
	//CCar* pOthersCar = dynamic_cast<CCar*>(pOthersObject);	
	

	m_pMouseCapturer = NULL;
	UIPropertiesBox.Hide();
	UIMessageBox.Hide();
	EnableAll();

	UpdateLists();
}  

void CUICarBodyWnd::UpdateLists()
{

	//�������� ����� ����������� �������
	UIOurBagList.DropAll();
	UIOthersBagList.DropAll();


	for(u32 i = 0; i <MAX_ITEMS; ++i) 
	{
		m_vDragDropItems[i].SetData(NULL);
		m_vDragDropItems[i].SetWndRect(0,0,0,0);
		m_vDragDropItems[i].SetCustomUpdate(NULL);
	}


	m_iUsedItems = 0;

	ruck_list = m_pInv->m_ruck;
	ruck_list.sort(GreaterRoomInRuck);

	//��� ������
	PPIItem it;
	for(it =  ruck_list.begin(); ruck_list.end() != it; ++it) 
	{
		if((*it)) 
		{
			CUIDragDropItem& UIDragDropItem = m_vDragDropItems[m_iUsedItems];		

			UIDragDropItem.CUIStatic::Init(0,0, 50,50);
			UIDragDropItem.SetShader(GetEquipmentIconsShader());

			UIDragDropItem.SetGridHeight((*it)->GetGridHeight());
			UIDragDropItem.SetGridWidth((*it)->GetGridWidth());

			UIDragDropItem.GetUIStaticItem().SetOriginalRect(
									(*it)->GetXPos()*INV_GRID_WIDTH,
									(*it)->GetYPos()*INV_GRID_HEIGHT,
									(*it)->GetGridWidth()*INV_GRID_WIDTH,
									(*it)->GetGridHeight()*INV_GRID_HEIGHT);

			UIDragDropItem.SetData((*it));

			CWeaponAmmo* pWeaponAmmo  = dynamic_cast<CWeaponAmmo*>((*it));
			if(pWeaponAmmo)	UIDragDropItem.SetCustomUpdate(AmmoUpdateProc);

			CEatableItem* pEatableItem = dynamic_cast<CEatableItem*>((*it));
			if(pEatableItem) UIDragDropItem.SetCustomUpdate(FoodUpdateProc);


			//���������� ����������� ���������������
			UIDragDropItem.SetTextureScale(TRADE_ICONS_SCALE);
				
			UIOurBagList.AttachChild(&UIDragDropItem);
			++m_iUsedItems;
		}
	}


	ruck_list.clear();
	ruck_list.insert(ruck_list.begin(),
				m_pOthersInv->m_all.begin(),
				m_pOthersInv->m_all.end());

	ruck_list.sort(GreaterRoomInRuck);


	//����� ������
	for(it =  ruck_list.begin(); ruck_list.end() != it; ++it) 
	{
		if((*it)) 
		{
			CUIDragDropItem& UIDragDropItem = m_vDragDropItems[m_iUsedItems];		
				
			UIDragDropItem.CUIStatic::Init(0,0, 50,50);
			UIDragDropItem.SetShader(GetEquipmentIconsShader());

			UIDragDropItem.SetGridHeight((*it)->GetGridHeight());
			UIDragDropItem.SetGridWidth((*it)->GetGridWidth());

			UIDragDropItem.GetUIStaticItem().SetOriginalRect(
									(*it)->GetXPos()*INV_GRID_WIDTH,
									(*it)->GetYPos()*INV_GRID_HEIGHT,
									(*it)->GetGridWidth()*INV_GRID_WIDTH,
									(*it)->GetGridHeight()*INV_GRID_HEIGHT);

			UIDragDropItem.SetData((*it));

			CWeaponAmmo* pWeaponAmmo  = dynamic_cast<CWeaponAmmo*>((*it));
			if(pWeaponAmmo)	UIDragDropItem.SetCustomUpdate(AmmoUpdateProc);

			CEatableItem* pEatableItem = dynamic_cast<CEatableItem*>((*it));
			if(pEatableItem) UIDragDropItem.SetCustomUpdate(FoodUpdateProc);

			//���������� ����������� ���������������
			UIDragDropItem.SetTextureScale(TRADE_ICONS_SCALE);
			UIOthersBagList.AttachChild(&UIDragDropItem);
			++m_iUsedItems;
		}
	}
}




//------------------------------------------------
//��� ������ ������� �������, ������� ��� �������
void CUICarBodyWnd::SendMessage(CUIWindow *pWnd, s16 msg, void *pData)
{

	if(msg == CUIDragDropItem::ITEM_DRAG)
	{
		PIItem pInvItem = (PIItem)((CUIDragDropItem*)pWnd)->GetData();
		m_pCurrentDragDropItem = (CUIDragDropItem*)pWnd;

		SetCurrentItem(pInvItem);
	}
	else if(msg == CUIDragDropItem::ITEM_DB_CLICK)
	{
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


	CUIWindow::SendMessage(pWnd, msg, pData);
}

void CUICarBodyWnd::Draw()
{
	inherited::Draw();
}

void CUICarBodyWnd::Update()
{
	//������ ������ drag&drop ��� ��� �������������� ����
	for(int i = 0; i <m_iUsedItems; ++i) 
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



//��� ������ �������� ���������� ������� 
//�������� ����, ���� ����� ����� ������ ��
//��������� �������� CUIInventoryWnd � 
//�������� �������� GetInventory()
bool CUICarBodyWnd::OurBagProc(CUIDragDropItem* pItem, CUIDragDropList* pList)
{
	CUICarBodyWnd* this_car_body_wnd =  dynamic_cast<CUICarBodyWnd*>(pList->GetParent()->GetParent());
	R_ASSERT2(this_car_body_wnd, "wrong parent addressed as trade wnd");

	//return this_car_body_wnd->IsEnoughtOurRoom(pItem);
	PIItem pIItem = (PIItem)(pItem->GetData());

	bool result = this_car_body_wnd->IsEnoughtOurRoom(pItem);
	if(!result) return false;
	
	//���������� ��������� ���� ��� � ������
	//result = this_car_body_wnd->m_pInv->Take(pIItem);

//	if(result)
	{
		//������� ��������� - ������ ����
		NET_Packet						P;
		this_car_body_wnd->m_pOthersObject->u_EventGen(P,GE_OWNERSHIP_REJECT,
											this_car_body_wnd->m_pOthersObject->ID());
		P.w_u16							   (u16(pIItem->ID()));
		this_car_body_wnd->m_pOthersObject->u_EventSend	(P);

		//��� - ����� ���� 
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

	//���������� ��������� ���� ������� ��������� � ������
	//result = this_car_body_wnd->m_pOthersInv->Take(pIItem);

//	if(result)
	{

		//��� - ������ ����
		NET_Packet P;
		this_car_body_wnd->m_pOurObject->u_EventGen(P,GE_OWNERSHIP_REJECT,
										 this_car_body_wnd->m_pOurObject->ID());
		P.w_u16							 (u16(pIItem->ID()));
		this_car_body_wnd->m_pOurObject->u_EventSend			(P);

		//������� ��������� - ����� ���� 
		this_car_body_wnd->m_pOthersObject->u_EventGen(P,GE_OWNERSHIP_TAKE,
											this_car_body_wnd->m_pOthersObject->ID());
		P.w_u16								(u16(pIItem->ID()));
		this_car_body_wnd->m_pOthersObject->u_EventSend	(P);

		return true;
	}
//	else
//		return false;
}

//��������� ������ �� ���� �� ���� �������� � ������ ����� �������
bool CUICarBodyWnd::IsEnoughtOurRoom(CUIDragDropItem* pItem)
{
	ruck_list.clear();
	ruck_list.insert(ruck_list.begin(), 
					 m_pInv->m_ruck.begin(),
					 m_pInv->m_ruck.end());

	//�� ����, ��� ������������
	ruck_list.push_back((PIItem)pItem->GetData());
	
	return FreeRoom(ruck_list,RUCK_WIDTH,RUCK_HEIGHT);
}

bool CUICarBodyWnd::IsEnoughtOthersRoom(CUIDragDropItem* pItem)
{
	ruck_list.clear();
	ruck_list.insert(ruck_list.begin(), 
					 m_pOthersInv->m_ruck.begin(),
					 m_pOthersInv->m_ruck.end());

	//�� ����, ��� ������������
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

void CUICarBodyWnd::SetCurrentItem(CInventoryItem* pItem)
{
	m_pCurrentItem = pItem;
	UIItemInfo.InitItem(m_pCurrentItem);
}