#pragma once

#include "UIBuyWndBase.h"
#include "UIWndCallback.h"



class CUIDragDropListEx;
class CUI3tButton;
class CUIStatic;
class CUIMpItemsStoreWnd;
class CStoreHierarchy;
class CUITabControl;

class CUIMpTradeWnd :	public IBuyWnd, 
						public CUIWndCallback
{
		typedef CUIDialogWnd	inherited;
public:
	enum{
		e_first					=0,
		e_pistol				= e_first,
		e_pistol_ammo,
		e_rifle,
		e_rifle_ammo,
		e_outfit,
		e_medkit,
		e_granade,
		e_others,
		e_bag,
		e_total_lists,
	};
public:
						CUIMpTradeWnd			();
	virtual				~CUIMpTradeWnd			();
	virtual void		SendMessage				(CUIWindow* pWnd, s16 msg, void* pData = NULL);
	
	//
	virtual void		Update					();				
	virtual bool 		OnKeyboard				(int dik, EUIMessages keyboard_action);

public:
	virtual void 		Init					(const shared_str& sectionName, const shared_str& sectionPrice);
	virtual void		BindDragDropListEvents	(CUIDragDropListEx* lst, bool bDrag);

	virtual const u8			GetItemIndex			(u32 slotNum, u32 idx, u8 &sectionNum);
	virtual const u8			GetBeltSize				();
	virtual const u8			GetWeaponIndexInBelt	(u32 indexInBelt, u8 &sectionId, u8 &itemId, u8 &count);
	virtual void				GetWeaponIndexByName	(const shared_str& sectionName, u8 &grpNum, u8 &idx);
	virtual int					GetMoneyAmount			() const;
	virtual void				IgnoreMoney				(bool ignore);
	virtual void				SectionToSlot			(const u8 grpNum, u8 uIndexInSlot, bool bRealRepresentationSet);
	virtual void 				SetMoneyAmount			(int money);
	virtual bool 				CheckBuyAvailabilityInSlots	();
	virtual void				AddonToSlot				(int add_on, int slot, bool bRealRepresentationSet);
	virtual const shared_str&	GetWeaponNameByIndex	(u8 grpNum, u8 idx);
	virtual void 				SetSkin					(u8 SkinID);
	virtual void				IgnoreMoneyAndRank			(bool ignore);
	virtual void				ClearSlots					();
	virtual void				ClearRealRepresentationFlags();
	virtual const u8			GetWeaponIndex				(u32 slotNum);//
	virtual bool 				CanBuyAllItems				();
	virtual void 				ResetItems					();
	virtual void				SetRank						(int rank);

private:
	//data
	shared_str			m_sectionName;
	shared_str			m_sectionPrice;
	int					m_rank;
	int					m_money;
	CStoreHierarchy*	m_store_hierarchy;

//controls
//.	CUIMpItemsStoreWnd* m_items_storage;
	CUIWindow*			m_shop_wnd;
	CUIStatic*			m_static_money;
	CUI3tButton* 		m_btn_shop_back;
	CUI3tButton* 		m_btn_ok;
	CUI3tButton* 		m_btn_cancel;
	CUITabControl*		m_root_tab_control;
	CUIDragDropListEx*	m_list[e_total_lists];
	void				UpdateMomeyIndicator		();
	void				UpdateShop					();

//handlers
	void	__stdcall	OnBtnOkClicked				(CUIWindow* w, void* d);
	void	__stdcall	OnBtnCancelClicked			(CUIWindow* w, void* d);
	void	__stdcall	OnBtnShopBackClicked		(CUIWindow* w, void* d);
	void	__stdcall	OnRootTabChanged			(CUIWindow* w, void* d);
	void	__stdcall	OnSubLevelBtnClicked		(CUIWindow* w, void* d);

	void				FillUpSubLevelButtons		();
	void				FillUpSubLevelItems			();
};
