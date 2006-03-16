}// UITradeWnd.cpp:  диалог торговли
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
#include "UIMultiTextStatic.h"

#include "../InventoryOwner.h"
#include "../character_info.h"

#include "UICharacterInfo.h"
#include "UIDragDropList.h"
using namespace InventoryUtilities;

//////////////////////////////////////////////////////////////////////////

#define				TRADE_XML			"trade.xml"
#define				TRADE_CHARACTER_XML	"trade_character.xml"
#define				TRADE_ITEM_XML		"trade_item.xml"


struct CUITradeInternal{
	CUIStatic			UIStaticTop;
	CUIStatic			UIStaticBottom;

	CUIStatic			UIOurBagWnd;
	CUIStatic			UIOurMoneyStatic;
	CUIStatic			UIOthersBagWnd;
	CUIStatic			UIOtherMoneyStatic;
	CUIDragDropList		UIOurBagList;
	CUIDragDropList		UIOthersBagList;

	CUIStatic			UIOurTradeWnd;
	CUIStatic			UIOthersTradeWnd;
	CUIMultiTextStatic	UIOurPriceCaption;
	CUIMultiTextStatic	UIOthersPriceCaption;
	CUIDragDropList		UIOurTradeList;
	CUIDragDropList		UIOthersTradeList;

	//кнопки
	CUIButton			UIPerformTradeButton;
	CUIButton			UIToTalkButton;

	//информаци€ о персонажах 
	CUIStatic			UIOurIcon;
	CUIStatic			UIOthersIcon;
	CUICharacterInfo	UICharacterInfoLeft;
	CUICharacterInfo	UICharacterInfoRight;

	//информаци€ о перетаскиваемом предмете
	CUIStatic			UIDescWnd;
	CUIItemInfo			UIItemInfo;

	//pop-up меню вызываемое по нажатию правой кнопки
	CUIPropertiesBox UIPropertiesBox;

	SDrawStaticStruct*	UIDealMsg;
};

CUITradeWnd::CUITradeWnd()
	:	m_pCurrentDragDropItem	(NULL),
		m_bDealControlsVisible	(false),
		m_pTrade(NULL),
		m_pOthersTrade(NULL),
		bStarted(false)
{
	m_uidata = xr_new<CUITradeInternal>();
	Init();
	Hide();

	SetFont(HUD().Font().pFontMedium);
}

//////////////////////////////////////////////////////////////////////////

CUITradeWnd::~CUITradeWnd()
{
	ClearDragDrop	(m_vDragDropItems);
	xr_delete		(m_uidata);
}

//////////////////////////////////////////////////////////////////////////

void CUITradeWnd::Init()
{
	CUIXml uiXml;
	bool xml_result = uiXml.Init(CONFIG_PATH, UI_PATH, TRADE_XML);
	R_ASSERT3(xml_result, "xml file not found", TRADE_XML);
	CUIXmlInit xml_init;
	
	CUIWindow::Init(0, 0, UI_BASE_WIDTH, UI_BASE_HEIGHT);

	//статические элементы интерфейса
	AttachChild(&m_uidata->UIStaticTop);
	m_uidata->UIStaticTop.Init("ui\\ui_top_background", 0,0,UI_BASE_WIDTH,128);
	AttachChild(&m_uidata->UIStaticBottom);
	m_uidata->UIStaticBottom.Init("ui\\ui_bottom_background", 0,UI_BASE_HEIGHT-32,UI_BASE_WIDTH,32);

	//иконки с изображение нас и партнера по торговле
	AttachChild(&m_uidata->UIOurIcon);
	xml_init.InitStatic(uiXml, "static_icon", 0, &m_uidata->UIOurIcon);
	AttachChild(&m_uidata->UIOthersIcon);
	xml_init.InitStatic(uiXml, "static_icon", 1, &m_uidata->UIOthersIcon);
	m_uidata->UIOurIcon.AttachChild(&m_uidata->UICharacterInfoLeft);
	m_uidata->UICharacterInfoLeft.Init(0,0, m_uidata->UIOurIcon.GetWidth(), m_uidata->UIOurIcon.GetHeight(), TRADE_CHARACTER_XML);
	m_uidata->UIOthersIcon.AttachChild(&m_uidata->UICharacterInfoRight);
	m_uidata->UICharacterInfoRight.Init(0,0, m_uidata->UIOthersIcon.GetWidth(), m_uidata->UIOthersIcon.GetHeight(), TRADE_CHARACTER_XML);


	//—писки торговли
	AttachChild(&m_uidata->UIOurBagWnd);
	xml_init.InitStatic(uiXml, "our_bag_static", 0, &m_uidata->UIOurBagWnd);
	AttachChild(&m_uidata->UIOthersBagWnd);
	xml_init.InitStatic(uiXml, "others_bag_static", 0, &m_uidata->UIOthersBagWnd);

	m_uidata->UIOurBagWnd.AttachChild(&m_uidata->UIOurMoneyStatic);
	xml_init.InitStatic(uiXml, "our_money_static", 0, &m_uidata->UIOurMoneyStatic);

	m_uidata->UIOthersBagWnd.AttachChild(&m_uidata->UIOtherMoneyStatic);
	xml_init.InitStatic(uiXml, "other_money_static", 0, &m_uidata->UIOtherMoneyStatic);

	AttachChild(&m_uidata->UIOurTradeWnd);
	xml_init.InitStatic(uiXml, "static", 0, &m_uidata->UIOurTradeWnd);
	AttachChild(&m_uidata->UIOthersTradeWnd);
	xml_init.InitStatic(uiXml, "static", 1, &m_uidata->UIOthersTradeWnd);

	m_uidata->UIOurTradeWnd.AttachChild(&m_uidata->UIOurPriceCaption);
	xml_init.InitMultiTextStatic(uiXml, "price_mt_static", 0, &m_uidata->UIOurPriceCaption);
	m_uidata->UIOurPriceCaption.GetPhraseByIndex(0)->effect.SetNewRenderMethod(false);

	m_uidata->UIOthersTradeWnd.AttachChild(&m_uidata->UIOthersPriceCaption);
	xml_init.InitMultiTextStatic(uiXml, "price_mt_static", 0, &m_uidata->UIOthersPriceCaption);

	//—писки Drag&Drop
	m_uidata->UIOurBagWnd.AttachChild(&m_uidata->UIOurBagList);	
	xml_init.InitDragDropList(uiXml, "dragdrop_list", 0, &m_uidata->UIOurBagList);
	m_uidata->UIOurBagList.SetItemsScaleXY(TRADE_ICONS_SCALE,TRADE_ICONS_SCALE);

	m_uidata->UIOthersBagWnd.AttachChild(&m_uidata->UIOthersBagList);	
	xml_init.InitDragDropList(uiXml, "dragdrop_list", 1, &m_uidata->UIOthersBagList);
	m_uidata->UIOthersBagList.SetItemsScaleXY(TRADE_ICONS_SCALE,TRADE_ICONS_SCALE);

	m_uidata->UIOurTradeWnd.AttachChild(&m_uidata->UIOurTradeList);	
	xml_init.InitDragDropList(uiXml, "dragdrop_list", 2, &m_uidata->UIOurTradeList);
	m_uidata->UIOurTradeList.SetItemsScaleXY(TRADE_ICONS_SCALE,TRADE_ICONS_SCALE);

	m_uidata->UIOthersTradeWnd.AttachChild(&m_uidata->UIOthersTradeList);	
	xml_init.InitDragDropList(uiXml, "dragdrop_list", 3, &m_uidata->UIOthersTradeList);
	m_uidata->UIOthersTradeList.SetItemsScaleXY(TRADE_ICONS_SCALE,TRADE_ICONS_SCALE);

	
	//информаци€ о предмете
	AttachChild(&m_uidata->UIDescWnd);
	xml_init.InitStatic(uiXml, "desc_static", 0, &m_uidata->UIDescWnd);
	m_uidata->UIDescWnd.AttachChild(&m_uidata->UIItemInfo);
	m_uidata->UIItemInfo.Init(0,0, m_uidata->UIDescWnd.GetWidth(), m_uidata->UIDescWnd.GetHeight(), TRADE_ITEM_XML);


	AttachChild(&m_uidata->UIPropertiesBox);
	m_uidata->UIPropertiesBox.Init("ui\\ui_frame",0,0,300,300);
	m_uidata->UIPropertiesBox.Hide();

	m_uidata->UIOurBagList.SetCheckProc(OurBagProc);
	m_uidata->UIOurTradeList.SetCheckProc(OurTradeProc);
	m_uidata->UIOthersBagList.SetCheckProc(OthersBagProc);
	m_uidata->UIOthersTradeList.SetCheckProc(OthersTradeProc);


	//Ёлементы автоматического добавлени€
	xml_init.InitAutoStatic(uiXml, "auto_static", this);


	// нопки
	AttachChild(&m_uidata->UIPerformTradeButton);
	xml_init.InitButton(uiXml, "button", 0, &m_uidata->UIPerformTradeButton);

	AttachChild(&m_uidata->UIToTalkButton);
	xml_init.InitButton(uiXml, "button", 1, &m_uidata->UIToTalkButton);

	m_uidata->UIDealMsg = NULL;
}

//////////////////////////////////////////////////////////////////////////

void CUITradeWnd::InitTrade(CInventoryOwner* pOur, CInventoryOwner* pOthers)
{
	VERIFY(pOur);
	VERIFY(pOthers);

	m_uidata->UIOthersBagList.HighlightAllCells(false);
	m_uidata->UIOthersTradeList.HighlightAllCells(false);
	m_uidata->UIOurTradeList.HighlightAllCells(false);
	m_uidata->UIOurBagList.HighlightAllCells(false);

	m_pInvOwner = pOur;
	m_pOthersInvOwner = pOthers;
	m_uidata->UIOthersPriceCaption.GetPhraseByIndex(0)->SetText(*CStringTable().translate("Opponent Items"));

	m_uidata->UICharacterInfoLeft.InitCharacter(m_pInvOwner->object_id());
	m_uidata->UICharacterInfoRight.InitCharacter(m_pOthersInvOwner->object_id());

	m_pInv = &m_pInvOwner->inventory();
	m_pOthersInv = pOur->GetTrade()->GetPartnerInventory();
		
	m_pTrade = pOur->GetTrade();
	m_pOthersTrade = pOur->GetTrade()->GetPartnerTrade();
    	
	m_pMouseCapturer = NULL;
	m_uidata->UIPropertiesBox.Hide();

//	UIMessageBox.Hide();
	EnableAll();

	ClearDragDrop(m_vDragDropItems);
	UpdateLists(eBoth);
}  

//------------------------------------------------
//как только подн€ли элемент, сделать его текущим
void CUITradeWnd::SendMessage(CUIWindow *pWnd, s16 msg, void *pData)
{
	if(pWnd == &m_uidata->UIToTalkButton && msg == BUTTON_CLICKED)
	{
		SwitchToTalk();
	}
/*	else if(pWnd == &UIDealClose && msg == BUTTON_CLICKED)
	{
		SwitchDealControls(false);
		EnableAll();
	}*/
	else if(pWnd == &m_uidata->UIPerformTradeButton && msg == BUTTON_CLICKED)
	{
		PerformTrade();
	}
	else if(msg == DRAG_DROP_ITEM_DRAG)
	{
		if (m_pCurrentDragDropItem) 
			m_pCurrentDragDropItem->Highlight(false);

		m_pCurrentDragDropItem			= (CUIDragDropItem*)pWnd;
		PIItem pInvItem					= (PIItem)m_pCurrentDragDropItem->GetData();
		SetCurrentItem					(pInvItem);
		m_pCurrentDragDropItem->Rescale	(1.0f,1.0f);
	}
	else if(msg == DRAG_DROP_ITEM_DB_CLICK)
	{
		if (m_pCurrentDragDropItem) 
			m_pCurrentDragDropItem->Highlight(false);

		m_pCurrentDragDropItem			= (CUIDragDropItem*)pWnd;
		PIItem pInvItem					= (PIItem)m_pCurrentDragDropItem->GetData();
		
		SetCurrentItem(pInvItem);
		
		if(m_pCurrentDragDropItem->GetParent() == &m_uidata->UIOurBagList)
			ToOurTrade();
		else if(m_pCurrentDragDropItem->GetParent() == &m_uidata->UIOurTradeList)
			ToOurBag();
		else if(m_pCurrentDragDropItem->GetParent() == &m_uidata->UIOthersBagList)
			ToOthersTrade();
		else if(m_pCurrentDragDropItem->GetParent() == &m_uidata->UIOthersTradeList)
			ToOthersBag();
		else
			R_ASSERT2(false, "wrong parent for trade wnd");
	}

	CUIWindow::SendMessage(pWnd, msg, pData);


	//ќбрабатывем сообщение уже после того как вещь
	//была перемещена куда-нибудь
	if(smart_cast<CUIDragDropItem*>(pWnd))
	{
		UpdatePrices();
	}

}

//////////////////////////////////////////////////////////////////////////

void CUITradeWnd::Draw()
{
	inherited::Draw();
	if(m_uidata->UIDealMsg)		m_uidata->UIDealMsg->Draw();

}

//////////////////////////////////////////////////////////////////////////
extern void UpdateCameraDirection(CGameObject* pTo);

void CUITradeWnd::Update()
{
	EListType et = eNone;
	if(m_pInv->ModifyFrame()==Device.dwFrame && m_pOthersInv->ModifyFrame()==Device.dwFrame){
		et = eBoth;
	}else
	if(m_pInv->ModifyFrame()==Device.dwFrame){
		et = e1st;
	}else
	if(m_pOthersInv->ModifyFrame()==Device.dwFrame){
		et = e2nd;
	}
	if(et!=eNone)
		UpdateLists(et);

	//убрать объект drag&drop дл€ уже использованной вещи
	for(u32 i = 0; i <m_vDragDropItems.size(); ++i) 
	{
		CInventoryItem* pItem = (CInventoryItem*)m_vDragDropItems[i]->GetData();
		if(pItem && !pItem->Useful())
		{
			m_vDragDropItems[i]->GetParent()->DetachChild	(m_vDragDropItems[i]);
			m_vDragDropItems[i]->SetData					(NULL);
			m_vDragDropItems[i]->SetCustomDraw				(NULL);

			UpdatePrices									();

			xr_delete										(m_vDragDropItems[i]);
			m_vDragDropItems.erase							(m_vDragDropItems.begin()+i);
		}
	}

	inherited::Update			();
	UpdateCameraDirection		(smart_cast<CGameObject*>(m_pOthersInvOwner));
	if(m_uidata->UIDealMsg){
		m_uidata->UIDealMsg->Update();
		if( !m_uidata->UIDealMsg->IsActual()){
			HUD().GetUI()->UIGame()->RemoveCustomStatic("not_enough_money");
			m_uidata->UIDealMsg = NULL;
		}

	}

}

void CUITradeWnd::Show()
{
	inherited::Show		(true);
	inherited::Enable	(true);

	SetCurrentItem		(NULL);
	ResetAll			();
	m_uidata->UIDealMsg = NULL;
}

void CUITradeWnd::Hide()
{
	inherited::Show(false);
	inherited::Enable(false);
	if(bStarted)
		StopTrade();
	
	m_uidata->UIDealMsg = NULL;
	if(HUD().GetUI()->UIGame())
		HUD().GetUI()->UIGame()->RemoveCustomStatic("not_enough_money");
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

//при вызове проверки необходимо помнить 
//иерархию окон, чтоб знать какой именно из
//родителей €вл€етс€ CUIInventoryWnd и 
//содержит свойство GetInventory()
bool CUITradeWnd::OurBagProc(CUIDragDropItem* pItem, CUIDragDropList* pList)
{
	CUITradeWnd* this_trade_wnd =  smart_cast<CUITradeWnd*>(pList->GetParent()->GetParent());
	R_ASSERT2(this_trade_wnd, "wrong parent addressed as trade wnd");

	if(&this_trade_wnd->m_uidata->UIOurTradeList != pItem->GetParent()) return false;

	return true;
}

//////////////////////////////////////////////////////////////////////////

bool CUITradeWnd::OthersBagProc(CUIDragDropItem* pItem, CUIDragDropList* pList)
{
	CUITradeWnd* this_trade_wnd =  smart_cast<CUITradeWnd*>(pList->GetParent()->GetParent());
	R_ASSERT2(this_trade_wnd, "wrong parent addressed as trade wnd");

	if(&this_trade_wnd->m_uidata->UIOthersTradeList != pItem->GetParent()) return false;

	return true;
}

//////////////////////////////////////////////////////////////////////////

bool CUITradeWnd::OurTradeProc(CUIDragDropItem* pItem, CUIDragDropList* pList)
{
	CUITradeWnd* this_trade_wnd =  smart_cast<CUITradeWnd*>(pList->GetParent()->GetParent());
	R_ASSERT2(this_trade_wnd, "wrong parent addressed as trade wnd");

	if(&this_trade_wnd->m_uidata->UIOurBagList != pItem->GetParent()) return false;

	return this_trade_wnd->CanMoveToOther(pItem);
}

//////////////////////////////////////////////////////////////////////////
#include "../trade_parameters.h"
bool CUITradeWnd::CanMoveToOther(CUIDragDropItem* itm)
{
	PIItem pItem = (PIItem)itm->GetData();

	float r1 = CalcItemsWeight(&m_uidata->UIOurTradeList);	// our
	float r2 = CalcItemsWeight(&m_uidata->UIOthersTradeList);	// other

	float itmWeight			= pItem->Weight();
	float otherInvWeight	= m_pOthersInv->CalcTotalWeight();
	float otherMaxWeight	= m_pOthersInv->GetMaxWeight();

	if (!m_pOthersInvOwner->trade_parameters().enabled(
			CTradeParameters::action_buy(0),
			pItem->object().cNameSect()
		))
		return				(false);

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

	if(&this_trade_wnd->m_uidata->UIOthersBagList != pItem->GetParent()) return false;

	return true;
}

//////////////////////////////////////////////////////////////////////////

bool CUITradeWnd::ToOurTrade()
{
	if (!CanMoveToOther(m_pCurrentDragDropItem))return false;


	((CUIDragDropList*)m_pCurrentDragDropItem->GetParent())->DetachChild(m_pCurrentDragDropItem);
	m_uidata->UIOurTradeList.AttachChild(m_pCurrentDragDropItem);
		
	UpdatePrices();

	m_pMouseCapturer = NULL;

	return true;
}

//////////////////////////////////////////////////////////////////////////

bool CUITradeWnd::ToOthersTrade()
{	
	((CUIDragDropList*)m_pCurrentDragDropItem->GetParent())->DetachChild(m_pCurrentDragDropItem);
	m_uidata->UIOthersTradeList.AttachChild(m_pCurrentDragDropItem);
	
	UpdatePrices();

	m_pMouseCapturer = NULL;

	return true;
}

//////////////////////////////////////////////////////////////////////////

bool CUITradeWnd::ToOurBag()
{
	((CUIDragDropList*)m_pCurrentDragDropItem->GetParent())->DetachChild(m_pCurrentDragDropItem);
	m_uidata->UIOurBagList.AttachChild(m_pCurrentDragDropItem);
	
	UpdatePrices();
	
	m_pMouseCapturer = NULL;

	return true;
}

//////////////////////////////////////////////////////////////////////////

bool CUITradeWnd::ToOthersBag()
{

	((CUIDragDropList*)m_pCurrentDragDropItem->GetParent())->DetachChild(m_pCurrentDragDropItem);
	m_uidata->UIOthersBagList.AttachChild(m_pCurrentDragDropItem);

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

void CUITradeWnd::PerformTrade()
{

	if (m_uidata->UIOurTradeList.GetDragDropItemsList().empty() && m_uidata->UIOthersTradeList.GetDragDropItemsList().empty()) return;

	int our_money = (int)m_pInvOwner->m_dwMoney;
	int others_money = (int)m_pOthersInvOwner->m_dwMoney;

	int delta_price = int(m_iOurTradePrice-m_iOthersTradePrice);

	our_money+=delta_price;
	others_money-=delta_price;

	//денег хватает, продать вещи
	if(our_money>0 && others_money>0 && (m_iOurTradePrice>0 || m_iOthersTradePrice>0))
	{
		m_pOthersTrade->OnPerformTrade(m_iOthersTradePrice, m_iOurTradePrice);
		
		if (m_pCurrentDragDropItem) m_pCurrentDragDropItem->Highlight(false);
		SellItems(&m_uidata->UIOurTradeList, &m_uidata->UIOthersBagList, m_pTrade);
		SellItems(&m_uidata->UIOthersTradeList, &m_uidata->UIOurBagList, m_pOthersTrade);
	}else
	{
		m_uidata->UIDealMsg				= HUD().GetUI()->UIGame()->AddCustomStatic("not_enough_money", true);
		m_uidata->UIDealMsg->m_endTime	= Device.fTimeGlobal+2.0f;// sec
	}
	SetCurrentItem(NULL);
}

void CUITradeWnd::DisableAll()
{
	m_uidata->UIOurBagWnd.Enable(false);
	m_uidata->UIOthersBagWnd.Enable(false);
	m_uidata->UIOurTradeWnd.Enable(false);
	m_uidata->UIOthersTradeWnd.Enable(false);
}

//////////////////////////////////////////////////////////////////////////

void CUITradeWnd::EnableAll()
{
	m_uidata->UIOurBagWnd.Enable(true);
	m_uidata->UIOthersBagWnd.Enable(true);
	m_uidata->UIOurTradeWnd.Enable(true);
	m_uidata->UIOthersTradeWnd.Enable(true);
}

//////////////////////////////////////////////////////////////////////////

void CUITradeWnd::UpdatePrices()
{
	m_iOurTradePrice	= CalcItemsPrice(&m_uidata->UIOurTradeList,		 m_pTrade);
	m_iOthersTradePrice = CalcItemsPrice(&m_uidata->UIOthersTradeList, m_pOthersTrade);


	char buf[255];
	sprintf(buf, "%d$", m_iOurTradePrice);
	m_uidata->UIOurPriceCaption.GetPhraseByIndex(2)->str = buf;
	sprintf(buf, "%d$", m_iOthersTradePrice);
	m_uidata->UIOthersPriceCaption.GetPhraseByIndex(2)->str = buf;

	sprintf(buf, "%d RU", m_pInvOwner->m_dwMoney);
	m_uidata->UIOurMoneyStatic.SetText(buf);

	sprintf(buf, "%d RU", m_pOthersInvOwner->m_dwMoney);
	m_uidata->UIOtherMoneyStatic.SetText(buf);
}

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
		pDragDropItem->SetColor(0xffffffff);//un-colorize
		pTrade->SellItem((PIItem)pDragDropItem->GetData());
			
		if(pDragDropItem->GetParent())
			pDragDropItem->GetParent()->DetachChild(pDragDropItem);

		pBuyList->AttachChild(pDragDropItem);
	}
	pSellList->DropAll();
}

void CUITradeWnd::UpdateLists(EListType mode)
{
	if(mode==eBoth||mode==e1st){
		m_uidata->UIOurBagList.DropAll();
		m_uidata->UIOurTradeList.DropAll();
	}

	if(mode==eBoth||mode==e2nd){
		m_uidata->UIOthersBagList.DropAll();
		m_uidata->UIOthersTradeList.DropAll();
	}

	//обновить надписи
	UpdatePrices();

	ClearDragDrop(m_vDragDropItems);


	if(mode==eBoth||mode==e1st){
		ruck_list.clear				();
   		m_pInv->AddAvailableItems	(ruck_list, true);
		std::sort					(ruck_list.begin(),ruck_list.end(),GreaterRoomInRuck);
		FillList(ruck_list, m_uidata->UIOurBagList, true);
	}

	if(mode==eBoth||mode==e2nd){
		ruck_list.clear					();
		m_pOthersInv->AddAvailableItems	(ruck_list, true);
		std::sort						(ruck_list.begin(),ruck_list.end(),GreaterRoomInRuck);
		FillList(ruck_list, m_uidata->UIOthersBagList, false);
	}
}

void CUITradeWnd::FillList	(TIItemContainer& cont, CUIDragDropList& dragDropList, bool do_colorize)
{
	TIItemContainer::iterator it;
	for(it =  cont.begin(); cont.end() != it; ++it) 
	{
		if((*it)) 
		{
			m_vDragDropItems.push_back(xr_new<CUIWpnDragDropItem>());
			CUIDragDropItem& UIDragDropItem = *m_vDragDropItems.back();

			UIDragDropItem.CUIStatic::Init(0,0, 50,50);
			UIDragDropItem.SetShader(GetEquipmentIconsShader());
			UIDragDropItem.SetGridWidth((*it)->GetGridWidth());
			UIDragDropItem.SetGridHeight((*it)->GetGridHeight());

			UIDragDropItem.SetFont(HUD().Font().pFontLetterica16Russian);

			UIDragDropItem.GetUIStaticItem().SetOriginalRect(
									float((*it)->GetXPos()*INV_GRID_WIDTH),
									float((*it)->GetYPos()*INV_GRID_HEIGHT),
									float((*it)->GetGridWidth()*INV_GRID_WIDTH),
									float((*it)->GetGridHeight()*INV_GRID_HEIGHT));

			UIDragDropItem.SetData((*it));

			CWeaponAmmo* pWeaponAmmo  = smart_cast<CWeaponAmmo*>((*it));
			if(pWeaponAmmo)	UIDragDropItem.SetCustomDraw(AmmoDrawProc);

			CEatableItem* pEatableItem = smart_cast<CEatableItem*>((*it));
			if(pEatableItem) UIDragDropItem.SetCustomDraw(FoodDrawProc);

			UIDragDropItem.SetStretchTexture(true);

			if(do_colorize){
				switch((*it)->m_eItemPlace){
				case eItemPlaceSlot:
					UIDragDropItem.SetColor(0xff7c0000);
					break;
				case eItemPlaceBelt:
					UIDragDropItem.SetColor(0xff007c00);
					break;
				};
			}
			dragDropList.AttachChild(&UIDragDropItem);
		}
	}

}

void CUITradeWnd::SetCurrentItem(CInventoryItem* pItem)
{
	m_pCurrentItem = pItem;

	m_uidata->UIItemInfo.InitItem(m_pCurrentItem);
}

void CUITradeWnd::SwitchToTalk()
{
	GetMessageTarget()->SendMessage(this, TRADE_WND_CLOSED);
}
