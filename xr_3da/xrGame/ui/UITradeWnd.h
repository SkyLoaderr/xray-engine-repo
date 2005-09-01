// UITradeWnd.h:  ������ ��������
// 
//////////////////////////////////////////////////////////////////////

#pragma once
#include "UIWindow.h"
#include "../inventory_space.h"

class CInventoryOwner;
class CEatableItem;

//#include "UIDialogWnd.h"
//#include "UIStatic.h"
//#include "UIButton.h"
//#include "UIWpnDragDropItem.h"
//#include "UIDragDropList.h"
//#include "UIProgressBar.h"
//#include "UIPropertiesBox.h"

//#include "UIEditBox.h"
//#include "UIListWnd.h"
//#include "UIMessageBox.h"
//#include "UIMultiTextStatic.h"

//#include "UICharacterInfo.h"
//#include "UIItemInfo.h"

class CTrade;
struct CUITradeInternal;
class CUIDragDropList;
class CUIWpnDragDropItem;

class CUITradeWnd: public CUIWindow
{
private:
	typedef CUIWindow inherited;
public:
	CUITradeWnd();
	virtual ~CUITradeWnd();

	virtual void Init();

//	typedef enum{TRADE_WND_CLOSED} E_MESSAGE;
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
	void StartTrade();
	void StopTrade();
protected:

	CUITradeInternal*	m_uidata;

	bool				bStarted;
	//�������, ����������� ������������ ������������ ������
	//� �������� ����������
	static bool OurBagProc(CUIDragDropItem* pItem, CUIDragDropList* pList);
	static bool OthersBagProc(CUIDragDropItem* pItem, CUIDragDropList* pList);
	static bool OurTradeProc(CUIDragDropItem* pItem, CUIDragDropList* pList);
	static bool OthersTradeProc(CUIDragDropItem* pItem, CUIDragDropList* pList);

	bool ToOurTrade();
	bool ToOthersTrade();
	bool ToOurBag();
	bool ToOthersBag();
	void SendEvent_ItemDrop(PIItem pItem);
	
	//������ ���� ����� � ������
	u32		CalcItemsPrice(CUIDragDropList* pList, CTrade* pTrade);
	float	CalcItemsWeight(CUIDragDropList* pList);	
	//������� ����� �� ������ � ��������� �� � ������ ������
	void SellItems(CUIDragDropList* pSellList,
				   CUIDragDropList* pBuyList,
				   CTrade* pTrade);


	//���������� ��������
	void PerformTrade();
	void UpdatePrices();
	void UpdateLists();

	void FillList	(TIItemContainer& cont, CUIDragDropList& list, bool do_colorize);
	bool			m_bDealControlsVisible;

	void SwitchDealControls(bool on);
	bool GetDealControlStatus() { return m_bDealControlsVisible; }
	bool CanMoveToOther(CUIDragDropItem* pItem);
	//������ ��������� drag drop
	xr_vector<CUIWpnDragDropItem*>	m_vDragDropItems;

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
	TIItemContainer ruck_list;

	//������������� ������� �������
	void SetCurrentItem(CInventoryItem* pItem);
};