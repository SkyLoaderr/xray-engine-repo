// CUITradeWnd.h:  ������ ��������
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


class CUITradeWnd: public CUIDialogWnd
{
private:
	typedef CUIDialogWnd inherited;
public:
	CUITradeWnd();
	virtual ~CUITradeWnd();

	virtual void Init();
	virtual void SendMessage(CUIWindow *pWnd, s16 msg, void *pData);

	void InitTrade();
	
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

	CUIFrameWindow		UIOurBagWnd;
	CUIFrameWindow		UIOthersBagWnd;
	CUIDragDropList		UIOurBagList;
	CUIDragDropList		UIOthersBagList;

	CUIFrameWindow		UIOurTradeWnd;
	CUIFrameWindow		UIOthersTradeWnd;
	CUIDragDropList		UIOurTradeList;
	CUIDragDropList		UIOthersTradeList;

	//������
	CUIButton			UIPerformTradeButton;

	
	//�������, ����������� ������������ ������������ ������
	//� �������� ����������
	static bool OurBagProc(CUIDragDropItem* pItem, CUIDragDropList* pList);
	static bool OthersBagProc(CUIDragDropItem* pItem, CUIDragDropList* pList);
	static bool OurTradeProc(CUIDragDropItem* pItem, CUIDragDropList* pList);
	static bool OthersTradeProc(CUIDragDropItem* pItem, CUIDragDropList* pList);

	//��������� ������ �� ���� �� ���� �������� � ������ ����� �������
	bool IsEnoughtOurRoom(CUIDragDropItem* pItem);
	bool IsEnoughtOthersRoom(CUIDragDropItem* pItem);

	bool ToOurTrade();
	bool ToOthersTrade();
	bool ToOurBag();
	bool ToOthersBag();
	
	//������ ���� ����� � ������
	u32 CalcItemsPrice(CUIDragDropList* pList, CTrade* pTrade);
	//������� ����� �� ������ � ��������� �� � ������ ������
	void SellItems(CUIDragDropList* pSellList,
				   CUIDragDropList* pBuyList,
				   CTrade* pTrade);


	//���������� ��������
	void PerformTrade();
	void UpdatePrices();
	void UpdateLists();

	//pop-up ���� ���������� �� ������� ������ ������
	CUIPropertiesBox UIPropertiesBox;
	//���� � ����������
	CUIMessageBox	 UIMessageBox;


	static const int MAX_ITEMS = 140;
	CUIDragDropItem m_vDragDropItems[MAX_ITEMS];
	int m_iUsedItems;

	//��������� ������ � ���� � ��� �������
	CInventory* m_pInv;
	CInventory* m_pOthersInv;
	CInventoryOwner* m_pInvOwner;
	CInventoryOwner* m_pOthersInvOwner;
	CTrade* m_pTrade;
	CTrade* m_pOthersTrade;

	u32 m_iOurTradePrice;
	u32 m_iOthersTradePrice;


	//������� � ������� �������� � ������� ������
	PIItem m_pCurrentItem;
	CUIDragDropItem* m_pCurrentDragDropItem;

	//��� ���������� �����
	TIItemList ruck_list;
};