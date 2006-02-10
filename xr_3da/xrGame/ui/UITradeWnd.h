// UITradeWnd.h:  ������ ��������
// 
//////////////////////////////////////////////////////////////////////

#pragma once
#include "UIWindow.h"
#include "../inventory_space.h"

class CInventoryOwner;
class CEatableItem;
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

	virtual void SendMessage(CUIWindow *pWnd, s16 msg, void *pData);

	void InitTrade(CInventoryOwner* pOur, CInventoryOwner* pOthers);
	
	virtual void Draw();
	virtual void Update();
	virtual void Show();
	virtual void Hide();

	void DisableAll();
	void EnableAll();

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

	enum EListType{eNone,e1st,e2nd,eBoth};
	void UpdateLists			(EListType);

	void FillList	(TIItemContainer& cont, CUIDragDropList& list, bool do_colorize);
	bool			m_bDealControlsVisible;

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