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
#include "../string_table.h"

#include "../InventoryOwner.h"
#include "../character_info.h"

using namespace InventoryUtilities;

//////////////////////////////////////////////////////////////////////////

const char * const TRADE_XML			= "trade.xml";
const char * const TRADE_CHARACTER_XML	= "trade_character.xml";
const char * const TRADE_ITEM_XML		= "trade_item.xml";

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUITradeWnd::CUITradeWnd()
	:	m_pCurrentDragDropItem	(NULL),
		m_bDealControlsVisible	(false),
		m_pTrade(NULL),
		m_pOthersTrade(NULL),
		bStarted(false)
{
	Init();
	Hide();

	SetFont(HUD().Font().pFontMedium);
}

//////////////////////////////////////////////////////////////////////////

CUITradeWnd::~CUITradeWnd()
{
	ClearDragDrop(m_vDragDropItems);
}

//////////////////////////////////////////////////////////////////////////

void CUITradeWnd::Init()
{
	CUIXml uiXml;
	bool xml_result = uiXml.Init(CONFIG_PATH, UI_PATH, TRADE_XML);
	R_ASSERT3(xml_result, "xml file not found", TRADE_XML);
	CUIXmlInit xml_init;
	
	CUIWindow::Init(0, 0, UI_BASE_WIDTH, UI_BASE_HEIGHT);

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
	UICharacterInfoLeft.Init(0,0, UIOurIcon.GetWidth(), UIOurIcon.GetHeight(), TRADE_CHARACTER_XML);
	UIOthersIcon.AttachChild(&UICharacterInfoRight);
	UICharacterInfoRight.Init(0,0, UIOthersIcon.GetWidth(), UIOthersIcon.GetHeight(), TRADE_CHARACTER_XML);


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
	UIOurBagList.SetItemsScaleXY(TRADE_ICONS_SCALE,TRADE_ICONS_SCALE);

	UIOthersBagWnd.AttachChild(&UIOthersBagList);	
	xml_init.InitDragDropList(uiXml, "dragdrop_list", 1, &UIOthersBagList);
	UIOthersBagList.SetItemsScaleXY(TRADE_ICONS_SCALE,TRADE_ICONS_SCALE);

	UIOurTradeWnd.AttachChild(&UIOurTradeList);	
	xml_init.InitDragDropList(uiXml, "dragdrop_list", 2, &UIOurTradeList);
	UIOurTradeList.SetItemsScaleXY(TRADE_ICONS_SCALE,TRADE_ICONS_SCALE);

	UIOthersTradeWnd.AttachChild(&UIOthersTradeList);	
	xml_init.InitDragDropList(uiXml, "dragdrop_list", 3, &UIOthersTradeList);
	UIOthersTradeList.SetItemsScaleXY(TRADE_ICONS_SCALE,TRADE_ICONS_SCALE);

	
	//���������� � ��������
	AttachChild(&UIDescWnd);
	xml_init.InitFrameWindow(uiXml, "desc_frame_window", 0, &UIDescWnd);
	UIDescWnd.AttachChild(&UIItemInfo);
	UIItemInfo.Init(0,0, UIDescWnd.GetWidth(), UIDescWnd.GetHeight(), TRADE_ITEM_XML);


	AttachChild(&UIPropertiesBox);
	UIPropertiesBox.Init("ui\\ui_frame",0,0,300,300);
	UIPropertiesBox.Hide();

//	AttachChild(&UIMessageBox);
//	UIMessageBox.Init("ui\\ui_frame", 0, 0, 300, 300);
//	UIMessageBox.AutoCenter();
//	UIMessageBox.Hide();
	
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

	UIDescWnd.AttachChild(&UIDealMsg);
	xml_init.InitStatic(uiXml, "deal_static", 0, &UIDealMsg);
	UIDealMsg.Show(false);

	UIDescWnd.AttachChild(&UIDealClose);
	xml_init.InitButton(uiXml, "deal_button", 0, &UIDealClose);
	UIDealClose.Show(false);
	UIDealClose.SetMessageTarget(this);
}

//////////////////////////////////////////////////////////////////////////

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
	UIOthersPriceCaption.GetPhraseByIndex(0)->SetText(*CStringTable()("Opponent Items"));

	UICharacterInfoLeft.InitCharacter(m_pInvOwner);
	UICharacterInfoRight.InitCharacter(m_pOthersInvOwner);

	m_pInv = &m_pInvOwner->inventory();
	m_pOthersInv = pOur->GetTrade()->GetPartnerInventory();
		
	m_pTrade = pOur->GetTrade();
	m_pOthersTrade = pOur->GetTrade()->GetPartnerTrade();
    	
	m_pMouseCapturer = NULL;
	UIPropertiesBox.Hide();

//	UIMessageBox.Hide();
	EnableAll();

	ClearDragDrop(m_vDragDropItems);
	UpdateLists();
}  

//------------------------------------------------
//��� ������ ������� �������, ������� ��� �������
void CUITradeWnd::SendMessage(CUIWindow *pWnd, s16 msg, void *pData)
{
	if(pWnd == &UIToTalkButton && msg == BUTTON_CLICKED)
	{
		SwitchToTalk();
	}
/*	else if(pWnd == &UIDealClose && msg == BUTTON_CLICKED)
	{
		SwitchDealControls(false);
		EnableAll();
	}*/
	else if(pWnd == &UIPerformTradeButton && msg == BUTTON_CLICKED)
	{
		PerformTrade();
	}
	else if(msg == DRAG_DROP_ITEM_DRAG)
	{
		if (m_pCurrentDragDropItem) m_pCurrentDragDropItem->Highlight(false);
		PIItem pInvItem = (PIItem)((CUIDragDropItem*)pWnd)->GetData();
		m_pCurrentDragDropItem = (CUIDragDropItem*)pWnd;
		SetCurrentItem(pInvItem);
		m_pCurrentDragDropItem->Rescale(1.0f,1.0f);
	}
	else if(msg == DRAG_DROP_ITEM_DB_CLICK)
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
	if(smart_cast<CUIDragDropItem*>(pWnd))
	{
		UpdatePrices();
	}

}

//////////////////////////////////////////////////////////////////////////

void CUITradeWnd::Draw()
{
	inherited::Draw();
}

//////////////////////////////////////////////////////////////////////////

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
			m_vDragDropItems[i]->SetCustomDraw(NULL);

			UpdatePrices();

			xr_delete(m_vDragDropItems[i]);
			m_vDragDropItems.erase(m_vDragDropItems.begin()+i);
		}
	}

	inherited::Update();
}

//////////////////////////////////////////////////////////////////////////
//#include "../ai/trader/ai_trader.h"
void CUITradeWnd::Show()
{
	inherited::Show(true);
	inherited::Enable(true);


	ResetAll();
}

//////////////////////////////////////////////////////////////////////////

void CUITradeWnd::Hide()
{
	inherited::Show(false);
	inherited::Enable(false);
	if(bStarted)
		StopTrade();
}
void CUITradeWnd::StartTrade()
{
	if (m_pTrade)m_pTrade->TradeCB(true);
	if (m_pOthersTrade)m_pOthersTrade->TradeCB(true);
	bStarted = true;
}

void CUITradeWnd::StopTrade()
{
	if (m_pTrade) m_pTrade->TradeCB(false);
	if (m_pOthersTrade) m_pOthersTrade->TradeCB(false);
	bStarted = false;
}

//��� ������ �������� ���������� ������� 
//�������� ����, ���� ����� ����� ������ ��
//��������� �������� CUIInventoryWnd � 
//�������� �������� GetInventory()
bool CUITradeWnd::OurBagProc(CUIDragDropItem* pItem, CUIDragDropList* pList)
{
	CUITradeWnd* this_trade_wnd =  smart_cast<CUITradeWnd*>(pList->GetParent()->GetParent());
	R_ASSERT2(this_trade_wnd, "wrong parent addressed as trade wnd");

	if(&this_trade_wnd->UIOurTradeList != pItem->GetParent()) return false;

	return true;
}

//////////////////////////////////////////////////////////////////////////

bool CUITradeWnd::OthersBagProc(CUIDragDropItem* pItem, CUIDragDropList* pList)
{
	CUITradeWnd* this_trade_wnd =  smart_cast<CUITradeWnd*>(pList->GetParent()->GetParent());
	R_ASSERT2(this_trade_wnd, "wrong parent addressed as trade wnd");

	if(&this_trade_wnd->UIOthersTradeList != pItem->GetParent()) return false;

	return true;
}

//////////////////////////////////////////////////////////////////////////

bool CUITradeWnd::OurTradeProc(CUIDragDropItem* pItem, CUIDragDropList* pList)
{
	CUITradeWnd* this_trade_wnd =  smart_cast<CUITradeWnd*>(pList->GetParent()->GetParent());
	R_ASSERT2(this_trade_wnd, "wrong parent addressed as trade wnd");

	if(&this_trade_wnd->UIOurBagList != pItem->GetParent()) return false;

	return this_trade_wnd->CanMoveToOther(pItem);
}

//////////////////////////////////////////////////////////////////////////

bool CUITradeWnd::CanMoveToOther(CUIDragDropItem* itm)
{
	PIItem pItem = (PIItem)itm->GetData();

	float r1 = CalcItemsWeight(&UIOurTradeList);	// our
	float r2 = CalcItemsWeight(&UIOthersTradeList);	// other

	float itmWeight			= pItem->Weight();
	float otherInvWeight	= m_pOthersInv->CalcTotalWeight();
	float otherMaxWeight	= m_pOthersInv->GetMaxWeight();

	if(otherInvWeight-r2+r1+itmWeight > otherMaxWeight){
		Msg("partner inventory is full");
		return false;
	}
	return true;
}

bool CUITradeWnd::OthersTradeProc(CUIDragDropItem* pItem, CUIDragDropList* pList)
{
	CUITradeWnd* this_trade_wnd =  smart_cast<CUITradeWnd*>(pList->GetParent()->GetParent());
	R_ASSERT2(this_trade_wnd, "wrong parent addressed as trade wnd");

	if(&this_trade_wnd->UIOthersBagList != pItem->GetParent()) return false;

	return true;
}

//////////////////////////////////////////////////////////////////////////

bool CUITradeWnd::ToOurTrade()
{
	if (!CanMoveToOther(m_pCurrentDragDropItem))return false;


	((CUIDragDropList*)m_pCurrentDragDropItem->GetParent())->DetachChild(m_pCurrentDragDropItem);
	UIOurTradeList.AttachChild(m_pCurrentDragDropItem);
		
	UpdatePrices();

	m_pMouseCapturer = NULL;

	return true;
}

//////////////////////////////////////////////////////////////////////////

bool CUITradeWnd::ToOthersTrade()
{	
	((CUIDragDropList*)m_pCurrentDragDropItem->GetParent())->DetachChild(m_pCurrentDragDropItem);
	UIOthersTradeList.AttachChild(m_pCurrentDragDropItem);
	
	UpdatePrices();

	m_pMouseCapturer = NULL;

	return true;
}

//////////////////////////////////////////////////////////////////////////

bool CUITradeWnd::ToOurBag()
{
	((CUIDragDropList*)m_pCurrentDragDropItem->GetParent())->DetachChild(m_pCurrentDragDropItem);
	UIOurBagList.AttachChild(m_pCurrentDragDropItem);
	
	UpdatePrices();
	
	m_pMouseCapturer = NULL;

	return true;
}

//////////////////////////////////////////////////////////////////////////

bool CUITradeWnd::ToOthersBag()
{

	((CUIDragDropList*)m_pCurrentDragDropItem->GetParent())->DetachChild(m_pCurrentDragDropItem);
	UIOthersBagList.AttachChild(m_pCurrentDragDropItem);

	UpdatePrices();

	m_pMouseCapturer = NULL;

	return true;
}

//////////////////////////////////////////////////////////////////////////
float CUITradeWnd::CalcItemsWeight(CUIDragDropList* pList)
{
	float res = 0.0f;
	for(DRAG_DROP_LIST_it it = pList->GetDragDropItemsList().begin(); 
 			  			  pList->GetDragDropItemsList().end() != it; 
						  ++it)
	{
		CUIDragDropItem* pDragDropItem = *it;
		res += ((PIItem)pDragDropItem->GetData())->Weight();
	}
	return res;
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

//////////////////////////////////////////////////////////////////////////

void CUITradeWnd::PerformTrade()
{
	if(GetDealControlStatus()) return;

	if (UIOurTradeList.GetDragDropItemsList().empty() && UIOthersTradeList.GetDragDropItemsList().empty()) return;

	int our_money = (int)m_pInvOwner->m_dwMoney;
	int others_money = (int)m_pOthersInvOwner->m_dwMoney;

	int delta_price = int(m_iOurTradePrice-m_iOthersTradePrice);

	our_money+=delta_price;
	others_money-=delta_price;

	CStringTable stbl;

	if(our_money<0) 
	{
		UIDealMsg.SetText(*stbl("You don't have enought money!"));
	} 
	else if(others_money<0) 
	{
		UIDealMsg.SetText(*stbl("Your opponent doesn't have enought money!"));
	}
	//����� �������, ������� ����
	else if(m_iOurTradePrice>0 || m_iOthersTradePrice>0)
	{
		m_pOthersTrade->OnPerformTrade(m_iOthersTradePrice, m_iOurTradePrice);
		
		if (m_pCurrentDragDropItem) m_pCurrentDragDropItem->Highlight(false);
		SellItems(&UIOurTradeList, &UIOthersBagList, m_pTrade);
		SellItems(&UIOthersTradeList, &UIOurBagList, m_pOthersTrade);
		UpdatePrices();
		
	//	UIDealMsg.SetText(*stbl("The deal is done!"));

	}
	SetCurrentItem(NULL);
//	SwitchDealControls(true);
//	DisableAll();
}

//////////////////////////////////////////////////////////////////////////

void CUITradeWnd::DisableAll()
{
	UIOurBagWnd.Enable(false);
	UIOthersBagWnd.Enable(false);
	UIOurTradeWnd.Enable(false);
	UIOthersTradeWnd.Enable(false);
}

//////////////////////////////////////////////////////////////////////////

void CUITradeWnd::EnableAll()
{
	UIOurBagWnd.Enable(true);
	UIOthersBagWnd.Enable(true);
	UIOurTradeWnd.Enable(true);
	UIOthersTradeWnd.Enable(true);
}

//////////////////////////////////////////////////////////////////////////

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

	DRAG_DROP_LIST list_to_sell = pSellList->GetDragDropItemsList();
	for(DRAG_DROP_LIST_it it = list_to_sell.begin(); 
 			  			  list_to_sell.end() != it; 
						  ++it)
	{	
		CUIDragDropItem* pDragDropItem = *it;
		pTrade->SellItem((PIItem)pDragDropItem->GetData());
			
		//������� � ������ ����, ��� �������� �����
		//���� ��� ������� �������, �� 
		//���� ������� ��������� � ����,
		//� �  ��������� ���� ������.
		if(pDragDropItem->GetParent())
			pDragDropItem->GetParent()->DetachChild(pDragDropItem);
		pBuyList->AttachChild(pDragDropItem);
	}
	pSellList->DropAll();
}

//////////////////////////////////////////////////////////////////////////

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

	ruck_list.clear				();
   	m_pInv->AddAvailableItems	(ruck_list, true);
	std::sort					(ruck_list.begin(),ruck_list.end(),GreaterRoomInRuck);

	//��� ������
	TIItemContainer::iterator it;
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

			UIDragDropItem.SetFont(HUD().Font().pFontLetterica16Russian);

			UIDragDropItem.GetUIStaticItem().SetOriginalRect(
									(*it)->GetXPos()*INV_GRID_WIDTH,
									(*it)->GetYPos()*INV_GRID_HEIGHT,
									(*it)->GetGridWidth()*INV_GRID_WIDTH,
									(*it)->GetGridHeight()*INV_GRID_HEIGHT);

			UIDragDropItem.SetData((*it));

			CWeaponAmmo* pWeaponAmmo  = smart_cast<CWeaponAmmo*>((*it));
			if(pWeaponAmmo)	UIDragDropItem.SetCustomDraw(AmmoDrawProc);

			CEatableItem* pEatableItem = smart_cast<CEatableItem*>((*it));
			if(pEatableItem) UIDragDropItem.SetCustomDraw(FoodDrawProc);

			//���������� ����������� ���������������
//.			UIDragDropItem.SetTextureScaleXY(TRADE_ICONS_SCALE, TRADE_ICONS_SCALE);
			UIDragDropItem.SetStretchTexture(true);
				
			UIOurBagList.AttachChild(&UIDragDropItem);
//			++m_iUsedItems;
		}
	}

	ruck_list.clear					();
	m_pOthersInv->AddAvailableItems	(ruck_list, true);
	std::sort						(ruck_list.begin(),ruck_list.end(),GreaterRoomInRuck);

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

			UIDragDropItem.SetFont(HUD().Font().pFontLetterica16Russian);

			UIDragDropItem.GetUIStaticItem().SetOriginalRect(
									(*it)->GetXPos()*INV_GRID_WIDTH,
									(*it)->GetYPos()*INV_GRID_HEIGHT,
									(*it)->GetGridWidth()*INV_GRID_WIDTH,
									(*it)->GetGridHeight()*INV_GRID_HEIGHT);

			UIDragDropItem.SetData((*it));

			CWeaponAmmo* pWeaponAmmo  = smart_cast<CWeaponAmmo*>((*it));
			if(pWeaponAmmo)	UIDragDropItem.SetCustomDraw(AmmoDrawProc);

			CEatableItem* pEatableItem = smart_cast<CEatableItem*>((*it));
			if(pEatableItem) UIDragDropItem.SetCustomDraw(FoodDrawProc);

			//���������� ����������� ���������������
//.			UIDragDropItem.SetTextureScaleXY(TRADE_ICONS_SCALE,TRADE_ICONS_SCALE);
			UIDragDropItem.SetStretchTexture(true);
			UIOthersBagList.AttachChild(&UIDragDropItem);
//			++m_iUsedItems;
		}
	}
}

//////////////////////////////////////////////////////////////////////////

void CUITradeWnd::SetCurrentItem(CInventoryItem* pItem)
{
	m_pCurrentItem = pItem;
	const int offset = -15;

	UIItemInfo.InitItem(m_pCurrentItem);
	UIItemInfo.AlignRight(UIItemInfo.UIWeight, offset);
	UIItemInfo.AlignRight(UIItemInfo.UICost, offset);
	UIItemInfo.AlignRight(UIItemInfo.UICondition, offset);

}

//////////////////////////////////////////////////////////////////////////

void CUITradeWnd::DropCurrentItem()
{
	CActor *pActor = smart_cast<CActor*>(Level().CurrentEntity());
	if(!pActor) return;

	m_pCurrentItem->Drop();
	m_pCurrentDragDropItem->Highlight(false);

	(smart_cast<CUIDragDropList*>(m_pCurrentDragDropItem->GetParent()))->
		DetachChild(m_pCurrentDragDropItem);

	//-----------------------------------------------------------------------
	CUIInventoryWnd::SendEvent_ItemDrop(m_pCurrentItem);
	//-----------------------------------------------------------------------
	SetCurrentItem(NULL);
	m_pCurrentDragDropItem = NULL;
}

//////////////////////////////////////////////////////////////////////////

void CUITradeWnd::SwitchToTalk()
{
	GetMessageTarget()->SendMessage(this, TRADE_WND_CLOSED);
}

//////////////////////////////////////////////////////////////////////////

void CUITradeWnd::SwitchDealControls(bool on)
{
	m_bDealControlsVisible = on;

	UIDealClose.Show(on);
//	UIDealClose.SetCapture(this, on);
	UIDealClose.Enable(on);
	UIDealClose.HighlightItem(false);

	UIDealMsg.Show(on);

	UIItemInfo.Show(!on);
}