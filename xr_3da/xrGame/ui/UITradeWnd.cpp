}// UITradeWnd.cpp:  ������ ��������
// 
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "UITradeWnd.h"
#include "xrXMLParser.h"
#include "UIXmlInit.h"
#include "../Entity.h"
#include "../HUDManager.h"
#include "../WeaponAmmo.h"
#include "../Actor.h"
#include "../Trade.h"
#include "../UIGameSP.h"
#include "UIInventoryUtilities.h"
#include "../inventoryowner.h"
#include "../eatable_item.h"
#include "../inventory.h"
#include "../level.h"
#include "UIInventoryWnd.h"

#include "../InventoryOwner.h"
#include "../character_info.h"

using namespace InventoryUtilities;

//////////////////////////////////////////////////////////////////////////

const char * const	TRADE_XML	= "trade.xml";

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUITradeWnd::CUITradeWnd()
	:	m_pCurrentDragDropItem	(NULL)

{
	Init();
	Hide();

	SetFont(HUD().pFontMedium);
}

CUITradeWnd::~CUITradeWnd()
{
	ClearDragDrop(m_vDragDropItems);
}

void CUITradeWnd::Init()
{
	CUIXml uiXml;
	bool xml_result = uiXml.Init("$game_data$", TRADE_XML);
	R_ASSERT3(xml_result, "xml file not found", TRADE_XML);
	CUIXmlInit xml_init;
	
	CUIWindow::Init(0,0, UI_BASE_WIDTH, UI_BASE_HEIGHT);

	//����������� �������� ����������
	AttachChild(&UIStaticTop);
	UIStaticTop.Init("ui\\ui_top_background", 0,0,UI_BASE_WIDTH,128);
	AttachChild(&UIStaticBottom);
	UIStaticBottom.Init("ui\\ui_bottom_background", 0,UI_BASE_HEIGHT-32,UI_BASE_WIDTH,32);

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
	xml_init.InitStatic(uiXml, "our_bag_static", 0, &UIOurBagWnd);
	AttachChild(&UIOthersBagWnd);
	xml_init.InitStatic(uiXml, "others_bag_static", 0, &UIOthersBagWnd);

	AttachChild(&UIOurTradeWnd);
	xml_init.InitStatic(uiXml, "static", 0, &UIOurTradeWnd);
	AttachChild(&UIOthersTradeWnd);
	xml_init.InitStatic(uiXml, "static", 1, &UIOthersTradeWnd);

	UIOurTradeWnd.AttachChild(&UIOurPriceCaption);
	xml_init.InitMultiTextStatic(uiXml, "price_mt_static", 0, &UIOurPriceCaption);
	UIOurPriceCaption.GetPhraseByIndex(0)->effect.SetNewRenderMethod(false);

	UIOthersTradeWnd.AttachChild(&UIOthersPriceCaption);
	xml_init.InitMultiTextStatic(uiXml, "price_mt_static", 0, &UIOthersPriceCaption);

	//������ Drag&Drop
	UIOurBagWnd.AttachChild(&UIOurBagList);	
	xml_init.InitDragDropList(uiXml, "dragdrop_list", 0, &UIOurBagList);
	UIOurBagList.SetItemsScale(TRADE_ICONS_SCALE);

	UIOthersBagWnd.AttachChild(&UIOthersBagList);	
	xml_init.InitDragDropList(uiXml, "dragdrop_list", 1, &UIOthersBagList);
	UIOthersBagList.SetItemsScale(TRADE_ICONS_SCALE);

	UIOurTradeWnd.AttachChild(&UIOurTradeList);	
	xml_init.InitDragDropList(uiXml, "dragdrop_list", 2, &UIOurTradeList);
	UIOurTradeList.SetItemsScale(TRADE_ICONS_SCALE);

	UIOthersTradeWnd.AttachChild(&UIOthersTradeList);	
	xml_init.InitDragDropList(uiXml, "dragdrop_list", 3, &UIOthersTradeList);
	UIOthersTradeList.SetItemsScale(TRADE_ICONS_SCALE);

	
	//���������� � ��������
	AttachChild(&UIDescWnd);
	xml_init.InitFrameWindow(uiXml, "desc_frame_window", 0, &UIDescWnd);
	UIDescWnd.AttachChild(&UIItemInfo);
	UIItemInfo.Init(0,0, UIDescWnd.GetWidth(), UIDescWnd.GetHeight(), "trade_item.xml");


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


	//�������� ��������������� ����������
	xml_init.InitAutoStatic(uiXml, "auto_static", this);


	//������
	AttachChild(&UIPerformTradeButton);
	xml_init.InitButton(uiXml, "button", 0, &UIPerformTradeButton);

	AttachChild(&UIToTalkButton);
	xml_init.InitButton(uiXml, "button", 1, &UIToTalkButton);
}

void CUITradeWnd::InitTrade(CInventoryOwner* pOur, CInventoryOwner* pOthers)
{
	VERIFY(pOur);
	VERIFY(pOthers);

	UIOthersBagList.HighlightAllCells(false);
	UIOthersTradeList.HighlightAllCells(false);
	UIOurTradeList.HighlightAllCells(false);
	UIOurBagList.HighlightAllCells(false);

	m_pInvOwner = pOur;
	m_pOthersInvOwner = pOthers;
	string256	buf;
	strconcat(buf, m_pOthersInvOwner->CharacterInfo().Name(), "'s items");
	UIOthersPriceCaption.GetPhraseByIndex(0)->str = buf;

	UICharacterInfoLeft.InitCharacter(m_pInvOwner);
	UICharacterInfoRight.InitCharacter(m_pOthersInvOwner);

	m_pInv = &m_pInvOwner->inventory();
	m_pOthersInv = pOur->GetTrade()->GetPartnerInventory();
		
	m_pTrade = pOur->GetTrade();
	m_pOthersTrade = pOur->GetTrade()->GetPartnerTrade();
    	
	m_pMouseCapturer = NULL;
	UIPropertiesBox.Hide();

	UIMessageBox.Hide();
	EnableAll();

	ClearDragDrop(m_vDragDropItems);
	UpdateLists();
}  




//------------------------------------------------
//��� ������ ������� �������, ������� ��� �������
void CUITradeWnd::SendMessage(CUIWindow *pWnd, s16 msg, void *pData)
{
	if(pWnd == &UIToTalkButton && msg == CUIButton::BUTTON_CLICKED)
	{
		GetMessageTarget()->SendMessage(this, TRADE_WND_CLOSED);
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
	else if(msg == CUIDragDropItem::ITEM_DRAG)
	{
		if (m_pCurrentDragDropItem) m_pCurrentDragDropItem->Highlight(false);
		PIItem pInvItem = (PIItem)((CUIDragDropItem*)pWnd)->GetData();
		m_pCurrentDragDropItem = (CUIDragDropItem*)pWnd;
		SetCurrentItem(pInvItem);
	}
	else if(msg == CUIDragDropItem::ITEM_DB_CLICK)
	{
		if (m_pCurrentDragDropItem) m_pCurrentDragDropItem->Highlight(false);
		PIItem pInvItem = (PIItem)((CUIDragDropItem*)pWnd)->GetData();
		m_pCurrentDragDropItem = (CUIDragDropItem*)pWnd;
		
		SetCurrentItem(pInvItem);
		
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


	//����������� ��������� ��� ����� ���� ��� ����
	//���� ���������� ����-������
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
	//������ ������ drag&drop ��� ��� �������������� ����
	for(u32 i = 0; i <m_vDragDropItems.size(); ++i) 
	{
		CInventoryItem* pItem = (CInventoryItem*)m_vDragDropItems[i]->GetData();
		if(pItem && !pItem->Useful())
		{
			m_vDragDropItems[i]->GetParent()->DetachChild(m_vDragDropItems[i]);
			m_vDragDropItems[i]->SetData(NULL);
			m_vDragDropItems[i]->SetCustomUpdate(NULL);

			UpdatePrices();

			xr_delete(m_vDragDropItems[i]);
			m_vDragDropItems.erase(m_vDragDropItems.begin()+i);
		}
	}

	inherited::Update();
}

void CUITradeWnd::Show()
{
	inherited::Show(true);
	inherited::Enable(true);

	ResetAll();
}

void CUITradeWnd::Hide()
{
	inherited::Show(false);
	inherited::Enable(false);

}



//��� ������ �������� ���������� ������� 
//�������� ����, ���� ����� ����� ������ ��
//��������� �������� CUIInventoryWnd � 
//�������� �������� GetInventory()
bool CUITradeWnd::OurBagProc(CUIDragDropItem* pItem, CUIDragDropList* pList)
{
	CUITradeWnd* this_trade_wnd =  dynamic_cast<CUITradeWnd*>(pList->GetParent()->GetParent());
	R_ASSERT2(this_trade_wnd, "wrong parent addressed as trade wnd");

	if(&this_trade_wnd->UIOurTradeList != pItem->GetParent()) return false;

	return true;
}
bool CUITradeWnd::OthersBagProc(CUIDragDropItem* pItem, CUIDragDropList* pList)
{
	CUITradeWnd* this_trade_wnd =  dynamic_cast<CUITradeWnd*>(pList->GetParent()->GetParent());
	R_ASSERT2(this_trade_wnd, "wrong parent addressed as trade wnd");

	if(&this_trade_wnd->UIOthersTradeList != pItem->GetParent()) return false;

	return true;
}
bool CUITradeWnd::OurTradeProc(CUIDragDropItem* pItem, CUIDragDropList* pList)
{
	CUITradeWnd* this_trade_wnd =  dynamic_cast<CUITradeWnd*>(pList->GetParent()->GetParent());
	R_ASSERT2(this_trade_wnd, "wrong parent addressed as trade wnd");

	if(&this_trade_wnd->UIOurBagList != pItem->GetParent()) return false;

	return true;
}
bool CUITradeWnd::OthersTradeProc(CUIDragDropItem* pItem, CUIDragDropList* pList)
{
	CUITradeWnd* this_trade_wnd =  dynamic_cast<CUITradeWnd*>(pList->GetParent()->GetParent());
	R_ASSERT2(this_trade_wnd, "wrong parent addressed as trade wnd");

	if(&this_trade_wnd->UIOthersBagList != pItem->GetParent()) return false;

	return true;
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
	
	for(DRAG_DROP_LIST_it it = pList->GetDragDropItemsList().begin(); 
 			  			  pList->GetDragDropItemsList().end() != it; 
						  ++it)
	{
		CUIDragDropItem* pDragDropItem = *it;
		iPrice += pTrade->GetItemPrice((PIItem)pDragDropItem->GetData());
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
	//����� �������, ������� ����
	else if(m_iOurTradePrice>0 || m_iOthersTradePrice>0)
	{
		if (m_pCurrentDragDropItem) m_pCurrentDragDropItem->Highlight(false);
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
	m_iOurTradePrice = CalcItemsPrice(&UIOurTradeList,		 m_pTrade);
	m_iOthersTradePrice = CalcItemsPrice(&UIOthersTradeList, m_pOthersTrade);


	char buf[255];
	sprintf(buf, "%d$", m_iOurTradePrice);
	UIOurPriceCaption.GetPhraseByIndex(2)->str = buf;
	sprintf(buf, "%d$", m_iOthersTradePrice);
	UIOthersPriceCaption.GetPhraseByIndex(2)->str = buf;

	sprintf(buf, "%d$", m_pInvOwner->m_dwMoney);
	UIOurBagWnd.SetText(buf);
	sprintf(buf, "%d$", m_pOthersInvOwner->m_dwMoney);
	UIOthersBagWnd.SetText(buf);
}

//////////////////////////////////////////////////////////////////////////

void CUITradeWnd::SellItems(CUIDragDropList* pSellList,
							CUIDragDropList* pBuyList,
							CTrade* pTrade)
{

	for(DRAG_DROP_LIST_it it = pSellList->GetDragDropItemsList().begin(); 
 			  			  pSellList->GetDragDropItemsList().end() != it; 
						  ++it)
	{	
		CUIDragDropItem* pDragDropItem = *it;
		pTrade->SellItem((PIItem)pDragDropItem->GetData());
			
		//������� � ������ ����, ��� �������� �����
		//���� ��� ������� �������, �� 
		//���� ������� ��������� � ����,
		//� �  ��������� ���� ������.
		pBuyList->AttachChild(pDragDropItem);
	}
	pSellList->DropAll();
}

void CUITradeWnd::UpdateLists()
{
	//�������� ����� ����������� �������
	UIOurBagList.DropAll();
	UIOthersBagList.DropAll();
	UIOurTradeList.DropAll();
	UIOthersTradeList.DropAll();

	//�������� �������
	UpdatePrices();

	ClearDragDrop(m_vDragDropItems);

	ruck_list.clear();
   	m_pInv->AddAvailableItems(ruck_list);
	ruck_list.sort(GreaterRoomInRuck);

	//��� ������
	PPIItem it;
	for(it =  ruck_list.begin(); ruck_list.end() != it; ++it) 
	{
		if((*it)) 
		{
			//CUIDragDropItem& UIDragDropItem = m_vDragDropItems[m_iUsedItems];		
			m_vDragDropItems.push_back(xr_new<CUIWpnDragDropItem>());
			CUIDragDropItem& UIDragDropItem = *m_vDragDropItems.back();

			UIDragDropItem.CUIStatic::Init(0,0, 50,50);
			UIDragDropItem.SetShader(GetEquipmentIconsShader());
			UIDragDropItem.SetGridWidth((*it)->GetGridWidth());
			UIDragDropItem.SetGridHeight((*it)->GetGridHeight());

			UIDragDropItem.SetFont(HUD().pFontLetterica16Russian);

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
//			++m_iUsedItems;
		}
	}

	ruck_list.clear();
	m_pOthersInv->AddAvailableItems(ruck_list);
	ruck_list.sort(GreaterRoomInRuck);


	//����� ������
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

			UIDragDropItem.SetFont(HUD().pFontLetterica16Russian);

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
//			++m_iUsedItems;
		}
	}
}

void CUITradeWnd::SetCurrentItem(CInventoryItem* pItem)
{
	m_pCurrentItem = pItem;
	UIItemInfo.InitItem(m_pCurrentItem);
}

//////////////////////////////////////////////////////////////////////////

void CUITradeWnd::DropCurrentItem()
{
	CActor *pActor = dynamic_cast<CActor*>(Level().CurrentEntity());
	if(!pActor) return;

	m_pCurrentItem->Drop();
	m_pCurrentDragDropItem->Highlight(false);

	(dynamic_cast<CUIDragDropList*>(m_pCurrentDragDropItem->GetParent()))->
		DetachChild(m_pCurrentDragDropItem);

	//-----------------------------------------------------------------------
	CUIInventoryWnd::SendEvent_ItemDrop(m_pCurrentItem);
	//-----------------------------------------------------------------------
	SetCurrentItem(NULL);
	m_pCurrentDragDropItem = NULL;
}