// UITradeWnd.cpp:  диалог торговли
// 
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

#include "UIInventoryUtilities.h"
using namespace InventoryUtilities;

#define TRADE_ICONS_SCALE (4.f/5.f)


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUITradeWnd::CUITradeWnd()
{
	Init();
	Hide();

	SetFont(HUD().pFontMedium);
}

CUITradeWnd::~CUITradeWnd()
{
}

void CUITradeWnd::Init()
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


	AttachChild(&UIOurMoneySign);
	UIOurMoneySign.Init(40,265,150,30);
	UIOurMoneySign.SetText("our money");
	AttachChild(&UIOthersMoneySign);
	UIOthersMoneySign.Init(730,265,150,30);
	UIOthersMoneySign.SetText("others money");
	AttachChild(&UIOurItemsPriceSign);
	UIOurItemsPriceSign.Init(360,260,150,30);
	UIOurItemsPriceSign.SetText("our items price");
	AttachChild(&UIOthersItemsPriceSign);
	UIOthersItemsPriceSign.Init(360,485,150,30);
	UIOthersItemsPriceSign.SetText("others items price");

	AttachChild(&UIOurMoney);
	UIOurMoney.Init(40,295,150,30);
	AttachChild(&UIOthersMoney);
	UIOthersMoney.Init(730,295,150,30);
	AttachChild(&UIOurItemsPrice);
	UIOurItemsPrice.Init(360,290,150,30);
	AttachChild(&UIOthersItemsPrice);
	UIOthersItemsPrice.Init(360,515,150,30);




	//Списки торговли
	AttachChild(&UIOurBagWnd);
	xml_init.InitFrameWindow(uiXml, "frame_window", 0, &UIOurBagWnd);
	AttachChild(&UIOthersBagWnd);
	xml_init.InitFrameWindow(uiXml, "frame_window", 1, &UIOthersBagWnd);

	AttachChild(&UIOurTradeWnd);
	xml_init.InitFrameWindow(uiXml, "frame_window", 2, &UIOurTradeWnd);
	AttachChild(&UIOthersTradeWnd);
	xml_init.InitFrameWindow(uiXml, "frame_window", 3, &UIOthersTradeWnd);


	//Списки Drag&Drop
	UIOurBagWnd.AttachChild(&UIOurBagList);	
	xml_init.InitDragDropList(uiXml, "dragdrop_list", 0, &UIOurBagList);

	UIOthersBagWnd.AttachChild(&UIOthersBagList);	
	xml_init.InitDragDropList(uiXml, "dragdrop_list", 1, &UIOthersBagList);

	UIOurTradeWnd.AttachChild(&UIOurTradeList);	
	xml_init.InitDragDropList(uiXml, "dragdrop_list", 2, &UIOurTradeList);

	UIOthersTradeWnd.AttachChild(&UIOthersTradeList);	
	xml_init.InitDragDropList(uiXml, "dragdrop_list", 3, &UIOthersTradeList);


	AttachChild(&UIPropertiesBox);
	UIPropertiesBox.Init("ui\\ui_frame",0,0,300,300);
	UIPropertiesBox.Hide();

	AttachChild(&UIMessageBox);
	UIMessageBox.Init("ui\\ui_frame", 0, 0, 300, 300);
	UIMessageBox.AutoCenter();
	UIMessageBox.Hide();
	
	
	//////
	UIOurBagList.SetCheckProc(OurBagProc);
	UIOurTradeList.SetCheckProc(OurTradeProc);
	UIOthersBagList.SetCheckProc(OthersBagProc);
	UIOthersTradeList.SetCheckProc(OthersTradeProc);

	//Кнопки
	AttachChild(&UIPerformTradeButton);
	xml_init.InitButton(uiXml, "button", 0, &UIPerformTradeButton);


}

void CUITradeWnd::InitTrade()
{
	CActor *pActor = dynamic_cast<CActor *>(Level().CurrentEntity());
	if (!pActor || !pActor->GetTrade()->IsInTradeState()) return;

	m_pInvOwner = dynamic_cast<CInventoryOwner*>(Level().CurrentEntity());
	if(!m_pInvOwner) return;

	m_pOthersInvOwner = pActor->GetTrade()->GetPartner();
	
	m_pInv = &m_pInvOwner->m_inventory;
	m_pOthersInv = pActor->GetTrade()->GetPartnerInventory();
	
	m_pTrade = pActor->GetTrade();
	m_pOthersTrade = pActor->GetTrade()->GetPartnerTrade();
	
	m_pMouseCapturer = NULL;
	UIPropertiesBox.Hide();

	UIMessageBox.Hide();
	EnableAll();

	UpdateLists();
}  




//------------------------------------------------
//как только подняли элемент, сделать его текущим
void CUITradeWnd::SendMessage(CUIWindow *pWnd, s16 msg, void *pData)
{
	if(pWnd == &UIMessageBox && msg == CUIMessageBox::OK_CLICKED)
	{
		GetTop()->SetCapture(&UIMessageBox, false);
		UIMessageBox.Hide();
		EnableAll();
	}
	else if(pWnd == &UIPerformTradeButton && msg == CUIButton::BUTTON_CLICKED)
	{
		PerformTrade();
	}
	else if(msg == CUIDragDropItem::ITEM_DRAG)
	{
		PIItem pInvItem = (PIItem)((CUIDragDropItem*)pWnd)->GetData();
		m_pCurrentDragDropItem = (CUIDragDropItem*)pWnd;
		m_pCurrentItem = pInvItem;
//		UIStaticText.SetText(pInvItem->NameComplex());
	}
	else if(msg == CUIDragDropItem::ITEM_DB_CLICK)
	{
		PIItem pInvItem = (PIItem)((CUIDragDropItem*)pWnd)->GetData();
		m_pCurrentDragDropItem = (CUIDragDropItem*)pWnd;
		m_pCurrentItem = pInvItem;
		//UIStaticText.SetText(pInvItem->NameComplex());

		if(m_pCurrentDragDropItem->GetParent() == &UIOurBagList)
			ToOurTrade();
		else if(m_pCurrentDragDropItem->GetParent() == &UIOurTradeList)
			ToOurBag();
		else if(m_pCurrentDragDropItem->GetParent() == &UIOthersBagList)
			ToOthersTrade();
		else if(m_pCurrentDragDropItem->GetParent() == &UIOthersTradeList)
			ToOthersBag();
		else
			R_ASSERT2(false, "wrong parent for trade wnd");
	}


	CUIWindow::SendMessage(pWnd, msg, pData);


	//Обрабатывем сообщение уже после того как вещь
	//была перемещена куда-нибудь
	if(dynamic_cast<CUIDragDropItem*>(pWnd))
	{
		UpdatePrices();
	}

}

void CUITradeWnd::Draw()
{
	inherited::Draw();
}

void CUITradeWnd::Update()
{
	//остановить торговлю, если нужно
	CActor *pActor = dynamic_cast<CActor *>(Level().CurrentEntity());

	if (pActor && !pActor->GetTrade()->IsInTradeState())
	{
		CUIGameSP* pGameSP = dynamic_cast<CUIGameSP*>(HUD().GetUI()->UIGame());
		if(pGameSP)pGameSP->StartStopMenu(this);
	}

	inherited::Update();
}

void CUITradeWnd::Show()
{
	InitTrade();
	inherited::Show();
}

void CUITradeWnd::Hide()
{
	inherited::Hide();

	CActor *pActor = dynamic_cast<CActor *>(Level().CurrentEntity());
	if(!pActor) return;

	if (pActor->GetTrade()->IsInTradeState())
	{
		pActor->GetTrade()->StopTrade();
	}
}



//при вызове проверки необходимо помнить 
//иерархию окон, чтоб знать какой именно из
//родителей является CUIInventoryWnd и 
//содержит свойство GetInventory()
bool CUITradeWnd::OurBagProc(CUIDragDropItem* pItem, CUIDragDropList* pList)
{
	CUITradeWnd* this_trade_wnd =  dynamic_cast<CUITradeWnd*>(pList->GetParent()->GetParent());
	R_ASSERT2(this_trade_wnd, "wrong parent addressed as trade wnd");

	if(pItem->GetParent() != &this_trade_wnd->UIOurTradeList) return false;

	return true;
}
bool CUITradeWnd::OthersBagProc(CUIDragDropItem* pItem, CUIDragDropList* pList)
{
	CUITradeWnd* this_trade_wnd =  dynamic_cast<CUITradeWnd*>(pList->GetParent()->GetParent());
	R_ASSERT2(this_trade_wnd, "wrong parent addressed as trade wnd");

	if(pItem->GetParent() != &this_trade_wnd->UIOthersTradeList) return false;

	return true;
}
bool CUITradeWnd::OurTradeProc(CUIDragDropItem* pItem, CUIDragDropList* pList)
{
	CUITradeWnd* this_trade_wnd =  dynamic_cast<CUITradeWnd*>(pList->GetParent()->GetParent());
	R_ASSERT2(this_trade_wnd, "wrong parent addressed as trade wnd");

	if(pItem->GetParent() != &this_trade_wnd->UIOurBagList) return false;

	return this_trade_wnd->IsEnoughtOthersRoom(pItem);
}
bool CUITradeWnd::OthersTradeProc(CUIDragDropItem* pItem, CUIDragDropList* pList)
{
	CUITradeWnd* this_trade_wnd =  dynamic_cast<CUITradeWnd*>(pList->GetParent()->GetParent());
	R_ASSERT2(this_trade_wnd, "wrong parent addressed as trade wnd");

	if(pItem->GetParent() != &this_trade_wnd->UIOthersBagList) return false;

	return this_trade_wnd->IsEnoughtOurRoom(pItem);
}

//проверяет влезут ли вещи из окна торговли в рюкзак после покупки
bool CUITradeWnd::IsEnoughtOurRoom(CUIDragDropItem* pItem)
{
	ruck_list.clear();

	//та вещь, что переноситься
	ruck_list.push_back((PIItem)pItem->GetData());
	
	for(WINDOW_LIST_it it = UIOurBagWnd.GetChildWndList().begin(); 
 	  				   it!= UIOurBagWnd.GetChildWndList().end(); it++)
	{
		CUIDragDropItem* pDragDropItem = dynamic_cast<CUIDragDropItem*>(*it);
		if(pDragDropItem)
		{
			ruck_list.push_back((PIItem)pDragDropItem->GetData());
		}
	}


	for(WINDOW_LIST_it it = UIOthersTradeWnd.GetChildWndList().begin(); 
 	  				   it!= UIOthersTradeWnd.GetChildWndList().end(); it++)
	{
		CUIDragDropItem* pDragDropItem = dynamic_cast<CUIDragDropItem*>(*it);
		if(pDragDropItem)
		{
			ruck_list.push_back((PIItem)pDragDropItem->GetData());
		}
	}

	return FreeRoom(ruck_list,RUCK_WIDTH,RUCK_HEIGHT);
//	return true;
}

bool CUITradeWnd::IsEnoughtOthersRoom(CUIDragDropItem* pItem)
{
	ruck_list.clear();

	//та вещь, что переноситься
	ruck_list.push_back((PIItem)pItem->GetData());
	
	//вещи из рюкзака и окна торговли
	for(WINDOW_LIST_it it = UIOthersBagList.GetChildWndList().begin(); 
 	  				   it!= UIOthersBagList.GetChildWndList().end(); it++)
	{
		CUIDragDropItem* pDragDropItem = dynamic_cast<CUIDragDropItem*>(*it);
		if(pDragDropItem)
		{
			ruck_list.push_back((PIItem)pDragDropItem->GetData());
		}
	}

	for(WINDOW_LIST_it it = UIOurTradeList.GetChildWndList().begin(); 
 	  				   it!= UIOurTradeList.GetChildWndList().end(); it++)
	{
		CUIDragDropItem* pDragDropItem = dynamic_cast<CUIDragDropItem*>(*it);
		if(pDragDropItem)
		{
			ruck_list.push_back((PIItem)pDragDropItem->GetData());
		}
	}

	return FreeRoom(ruck_list, RUCK_WIDTH, RUCK_HEIGHT);
}


bool CUITradeWnd::ToOurTrade()
{
	((CUIDragDropList*)m_pCurrentDragDropItem->GetParent())->DetachChild(m_pCurrentDragDropItem);
	UIOurTradeList.AttachChild(m_pCurrentDragDropItem);
		
	UpdatePrices();

	m_pMouseCapturer = NULL;

	return true;
}
bool CUITradeWnd::ToOthersTrade()
{	
	((CUIDragDropList*)m_pCurrentDragDropItem->GetParent())->DetachChild(m_pCurrentDragDropItem);
	UIOthersTradeList.AttachChild(m_pCurrentDragDropItem);
	
	UpdatePrices();

	m_pMouseCapturer = NULL;

	return true;
}
bool CUITradeWnd::ToOurBag()
{
	((CUIDragDropList*)m_pCurrentDragDropItem->GetParent())->DetachChild(m_pCurrentDragDropItem);
	UIOurBagList.AttachChild(m_pCurrentDragDropItem);
	
	UpdatePrices();
	
	m_pMouseCapturer = NULL;

	return true;
}
bool CUITradeWnd::ToOthersBag()
{
	((CUIDragDropList*)m_pCurrentDragDropItem->GetParent())->DetachChild(m_pCurrentDragDropItem);
	UIOthersBagList.AttachChild(m_pCurrentDragDropItem);

	UpdatePrices();

	m_pMouseCapturer = NULL;

	return true;
}

u32 CUITradeWnd::CalcItemsPrice(CUIDragDropList* pList, CTrade* pTrade)
{
	u32 iPrice = 0;
	
	for(WINDOW_LIST_it it = pList->GetChildWndList().begin(); 
 	  				   it!= pList->GetChildWndList().end(); it++)
	{
		CUIDragDropItem* pDragDropItem = dynamic_cast<CUIDragDropItem*>(*it);
		if(pDragDropItem)
		{
			iPrice += pTrade->GetItemPrice((PIItem)pDragDropItem->GetData());
		}
	}

	return iPrice;
}

void CUITradeWnd::PerformTrade()
{
	if(UIMessageBox.IsShown()) return;

	
	int our_money = (int)m_pInvOwner->m_dwMoney;
	int others_money = (int)m_pOthersInvOwner->m_dwMoney;

	int delta_price = int(m_iOurTradePrice-m_iOthersTradePrice);

	our_money+=delta_price;
	others_money-=delta_price;
		
	if(our_money<0) 
	{
		UIMessageBox.SetText("You don't have enought money!");
	} 
	else if(others_money<0) 
	{
		UIMessageBox.SetText("Your opponent doesn't have enought money!");
	}
	//денег хватает, продать вещи
	else if(m_iOurTradePrice>0 || m_iOthersTradePrice>0)
	{
		SellItems(&UIOurTradeList, &UIOthersBagList, m_pTrade);
		SellItems(&UIOthersTradeList, &UIOurBagList, m_pOthersTrade);
		UpdatePrices();

		UIMessageBox.SetText("The deal is done!");
	}

	UIMessageBox.Show();
	DisableAll();
}
void CUITradeWnd::DisableAll()
{
	UIOurBagWnd.Enable(false);
	UIOthersBagWnd.Enable(false);
	UIOurTradeWnd.Enable(false);
	UIOthersTradeWnd.Enable(false);
}
void CUITradeWnd::EnableAll()
{
	UIOurBagWnd.Enable(true);
	UIOthersBagWnd.Enable(true);
	UIOurTradeWnd.Enable(true);
	UIOthersTradeWnd.Enable(true);
}

void CUITradeWnd::UpdatePrices()
{
	m_iOurTradePrice = CalcItemsPrice(&UIOurTradeList, m_pOthersTrade);
	m_iOthersTradePrice = CalcItemsPrice(&UIOthersTradeList, m_pOthersTrade);

	char buf[255];
	sprintf(buf, "%d$", m_iOurTradePrice);
	UIOurItemsPrice.SetText(buf);
	sprintf(buf, "%d$", m_iOthersTradePrice);
	UIOthersItemsPrice.SetText(buf);

	sprintf(buf, "%d$", m_pInvOwner->m_dwMoney);
	UIOurMoney.SetText(buf);
	sprintf(buf, "%d$", m_pOthersInvOwner->m_dwMoney);
	UIOthersMoney.SetText(buf);
}

void CUITradeWnd::SellItems(CUIDragDropList* pSellList,
							CUIDragDropList* pBuyList,
							CTrade* pTrade)
{
	for(WINDOW_LIST_it it = pSellList->GetChildWndList().begin(); 
  				   it!=  pSellList->GetChildWndList().end(); it++)
	{	
		CUIDragDropItem* pDragDropItem = dynamic_cast<CUIDragDropItem*>(*it);
		if(pDragDropItem)
		{
			pTrade->SellItem((PIItem)pDragDropItem->GetData());
			
			//заносим в списко того, кто покупает товар
			//если это простой сталкер, то 
			//вещь обязана появиться у него,
			//а с  торговцем надо решать.
			pBuyList->AttachChild(pDragDropItem);
		}
	}

	pSellList->DropAll();
}

void CUITradeWnd::UpdateLists()
{
	//обновить надписи
	char buf[255];
	sprintf(buf, "%d$", m_pInvOwner->m_dwMoney);
	UIOurMoney.SetText(buf);
	sprintf(buf, "%d$", m_pOthersInvOwner->m_dwMoney);
	UIOthersMoney.SetText(buf);
	sprintf(buf, "0$");
	UIOurItemsPrice.SetText(buf);
	UIOthersItemsPrice.SetText(buf);
	
	//очистить после предыдущего запуска
	UIOurBagList.DropAll();
	UIOthersBagList.DropAll();
	UIOurTradeList.DropAll();
	UIOthersTradeList.DropAll();


	for(u32 i = 0; i <MAX_ITEMS; i++) 
	{
		m_vDragDropItems[i].SetData(NULL);
		m_vDragDropItems[i].SetWndRect(0,0,0,0);
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
				UIDragDropItem.Init((*it)->m_sIconTexture, 0,0, 50,50);
				UIDragDropItem.SetGridHeight((*it)->m_iGridHeight);
				UIDragDropItem.SetGridWidth((*it)->m_iGridWidth);

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


	ruck_list = m_pOthersInv->m_ruck;
	ruck_list.sort(GreaterRoomInRuck);


	//Чужой рюкзак
	for(it =  ruck_list.begin(); it !=  ruck_list.end(); it++) 
	{
			if((*it)) 
			{
				CUIDragDropItem& UIDragDropItem = m_vDragDropItems[m_iUsedItems];		
				UIDragDropItem.Init((*it)->m_sIconTexture, 0,0, 50,50);
				UIDragDropItem.SetGridHeight((*it)->m_iGridHeight);
				UIDragDropItem.SetGridWidth((*it)->m_iGridWidth);

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