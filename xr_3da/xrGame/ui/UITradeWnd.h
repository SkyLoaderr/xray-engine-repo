// CUITradeWnd.h:  диалог торговли
// 
//////////////////////////////////////////////////////////////////////

#pragma once

#include "..\inventory.h"


#include "UIDialogWnd.h"
#include "UIStatic.h"
#include "UIButton.h"
#include "UIDragDropItem.h"
#include "UIDragDropList.h"
#include "UIProgressBar.h"
#include "UIPropertiesBox.h"

#include "UIEditBox.h"
#include "UIListWnd.h"
#include "UIMessageBox.h"


class CUITradeWnd: public CUIWindow
{
private:
	typedef CUIWindow inherited;
public:
	CUITradeWnd();
	virtual ~CUITradeWnd();

	virtual void Init();

	typedef enum{TRADE_WND_CLOSED} E_MESSAGE;
	virtual void SendMessage(CUIWindow *pWnd, s16 msg, void *pData);

	void InitTrade(CInventoryOwner* pOur, CInventoryOwner* pOthers);
	
	virtual void Draw();
	virtual void Update();
	virtual void Show();
	virtual void Hide();

	void DisableAll();
	void EnableAll();


protected:
	CUIStatic			UIStaticTop;
	CUIStatic			UIStaticBottom;
	
	CUIStatic			UIOurMoneySign;
	CUIStatic			UIOthersMoneySign;
	CUIStatic			UIOurItemsPriceSign;
	CUIStatic			UIOthersItemsPriceSign;

	CUIStatic			UIOurMoney;
	CUIStatic			UIOthersMoney;
	CUIStatic			UIOurItemsPrice;
	CUIStatic			UIOthersItemsPrice;
	
	CUIStatic			UIItemDescription;

	CUIFrameWindow		UIOurBagWnd;
	CUIFrameWindow		UIOthersBagWnd;
	CUIDragDropList		UIOurBagList;
	CUIDragDropList		UIOthersBagList;

	CUIFrameWindow		UIOurTradeWnd;
	CUIFrameWindow		UIOthersTradeWnd;
	CUIDragDropList		UIOurTradeList;
	CUIDragDropList		UIOthersTradeList;

	//кнопки
	CUIButton			UIPerformTradeButton;
	CUIButton			UIToTalkButton;

	
	//функции, выполн€ющие согласование отображаемых окошек
	//с реальным инвентарем
	static bool OurBagProc(CUIDragDropItem* pItem, CUIDragDropList* pList);
	static bool OthersBagProc(CUIDragDropItem* pItem, CUIDragDropList* pList);
	static bool OurTradeProc(CUIDragDropItem* pItem, CUIDragDropList* pList);
	static bool OthersTradeProc(CUIDragDropItem* pItem, CUIDragDropList* pList);

	//провер€ет влезут ли вещи из окна торговли в рюкзак после покупки
	bool IsEnoughtOurRoom(CUIDragDropItem* pItem);
	bool IsEnoughtOthersRoom(CUIDragDropItem* pItem);

	bool ToOurTrade();
	bool ToOthersTrade();
	bool ToOurBag();
	bool ToOthersBag();
	
	//посчет цены вещей в списке
	u32 CalcItemsPrice(CUIDragDropList* pList, CTrade* pTrade);
	//продажа вещей из списка с переносом их в другой список
	void SellItems(CUIDragDropList* pSellList,
				   CUIDragDropList* pBuyList,
				   CTrade* pTrade);


	//произвести торговлю
	void PerformTrade();
	void UpdatePrices();
	void UpdateLists();

	//pop-up меню вызываемое по нажатию правой кнопки
	CUIPropertiesBox UIPropertiesBox;
	//окно с сообщением
	CUIMessageBox	 UIMessageBox;


	static const int MAX_ITEMS = 140;
	CUIDragDropItem m_vDragDropItems[MAX_ITEMS];
	int m_iUsedItems;

	//указатели игрока и того с кем торгуем
	CInventory* m_pInv;
	CInventory* m_pOthersInv;
	CInventoryOwner* m_pInvOwner;
	CInventoryOwner* m_pOthersInvOwner;
	CTrade* m_pTrade;
	CTrade* m_pOthersTrade;

	u32 m_iOurTradePrice;
	u32 m_iOthersTradePrice;


	//элемент с которым работают в текущий момент
	PIItem m_pCurrentItem;
	CUIDragDropItem* m_pCurrentDragDropItem;

	//дл€ сортировки вещей
	TIItemList ruck_list;
};