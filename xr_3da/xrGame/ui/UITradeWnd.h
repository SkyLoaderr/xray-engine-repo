#pragma once
#include "UIWindow.h"
#include "../inventory_space.h"

class CInventoryOwner;
class CEatableItem;
class CTrade;
struct CUITradeInternal;

class CUIDragDropListEx;
class CUICellItem;

class CUITradeWnd: public CUIWindow
{
private:
	typedef CUIWindow inherited;
public:
						CUITradeWnd					();
	virtual				~CUITradeWnd				();

	virtual void		Init						();

	virtual void		SendMessage					(CUIWindow *pWnd, s16 msg, void *pData);

	void				InitTrade					(CInventoryOwner* pOur, CInventoryOwner* pOthers);
	
	virtual void 		Draw						();
	virtual void 		Update						();
	virtual void 		Show						();
	virtual void 		Hide						();

	void 				DisableAll					();
	void 				EnableAll					();

	void 				SwitchToTalk				();
	void 				StartTrade					();
	void 				StopTrade					();
protected:

	CUITradeInternal*	m_uidata;

	bool				bStarted;
/*
	static bool			urBagProc(CUIDragDropItem* pItem, CUIDragDropList* pList);
	static bool			OthersBagProc(CUIDragDropItem* pItem, CUIDragDropList* pList);
	static bool			OurTradeProc(CUIDragDropItem* pItem, CUIDragDropList* pList);
	static bool			OthersTradeProc(CUIDragDropItem* pItem, CUIDragDropList* pList);
*/
	bool 				ToOurTrade					();
	bool 				ToOthersTrade				();
	bool 				ToOurBag					();
	bool 				ToOthersBag					();
	void 				SendEvent_ItemDrop			(PIItem pItem);
	
	u32					CalcItemsPrice				(CUIDragDropListEx* pList, CTrade* pTrade);
	float				CalcItemsWeight				(CUIDragDropListEx* pList);

	void				SellItems					(CUIDragDropListEx* pSellList, CUIDragDropListEx* pBuyList, CTrade* pTrade);

	void				PerformTrade				();
	void				UpdatePrices				();

	enum EListType{eNone,e1st,e2nd,eBoth};
	void				UpdateLists					(EListType);

	void				FillList					(TIItemContainer& cont, CUIDragDropListEx& list, bool do_colorize);

	bool				m_bDealControlsVisible;

	bool				CanMoveToOther				(PIItem pItem);

	//��������� ������ � ���� � ��� �������
	CInventory*			m_pInv;
	CInventory*			m_pOthersInv;
	CInventoryOwner*	m_pInvOwner;
	CInventoryOwner*	m_pOthersInvOwner;
	CTrade*				m_pTrade;
	CTrade*				m_pOthersTrade;

	u32					m_iOurTradePrice;
	u32					m_iOthersTradePrice;


	CUICellItem*		m_pCurrentCellItem;
	TIItemContainer		ruck_list;


	void				SetCurrentItem				(CUICellItem* itm);
	CUICellItem*		CurrentItem					();
	PIItem				CurrentIItem				();

	bool		xr_stdcall		OnItemDrop			(CUICellItem* itm);
	bool		xr_stdcall		OnItemStartDrag		(CUICellItem* itm);
	bool		xr_stdcall		OnItemDbClick		(CUICellItem* itm);
	bool		xr_stdcall		OnItemSelected		(CUICellItem* itm);
	bool		xr_stdcall		OnItemRButtonClick	(CUICellItem* itm);

	void				BindDragDropListEnents		(CUIDragDropListEx* lst);

};