// UITradeWnd.h:  диалог торговли
// 
//////////////////////////////////////////////////////////////////////

#pragma once

class CInventoryOwner;
class CEatableItem;

#include "UIDialogWnd.h"
#include "UIStatic.h"
#include "UIButton.h"
#include "UIWpnDragDropItem.h"
#include "UIDragDropList.h"
#include "UIProgressBar.h"
#include "UIPropertiesBox.h"

#include "UIEditBox.h"
#include "UIListWnd.h"
#include "UIMessageBox.h"
#include "UIMultiTextStatic.h"

#include "UICharacterInfo.h"
#include "UIItemInfo.h"


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

	void DropCurrentItem();
	void SwitchToTalk();

protected:
	CUIStatic			UIStaticTop;
	CUIStatic			UIStaticBottom;

	CUIStatic			UIOurBagWnd;
	CUIStatic			UIOthersBagWnd;
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
	CUIFrameWindow		UIDescWnd;
	CUIItemInfo			UIItemInfo;

	
	//функции, выполн€ющие согласование отображаемых окошек
	//с реальным инвентарем
	static bool OurBagProc(CUIDragDropItem* pItem, CUIDragDropList* pList);
	static bool OthersBagProc(CUIDragDropItem* pItem, CUIDragDropList* pList);
	static bool OurTradeProc(CUIDragDropItem* pItem, CUIDragDropList* pList);
	static bool OthersTradeProc(CUIDragDropItem* pItem, CUIDragDropList* pList);

	bool ToOurTrade();
	bool ToOthersTrade();
	bool ToOurBag();
	bool ToOthersBag();
	void SendEvent_ItemDrop(PIItem pItem);
	
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
	// сообщени€ о результатах сделки
//	CUIMessageBox	 UIMessageBox;
	CUIStatic		UIDealMsg;
	CUIButton		UIDealClose;
	bool			m_bDealControlsVisible;

	void SwitchDealControls(bool on);
	bool GetDealControlStatus() { return m_bDealControlsVisible; }

	//список элементов drag drop
	DD_ITEMS_VECTOR	m_vDragDropItems;

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

	//устанавливает текущий предмет
	void SetCurrentItem(CInventoryItem* pItem);
};