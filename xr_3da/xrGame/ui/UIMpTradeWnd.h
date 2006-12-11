#pragma once

#include "UIBuyWndBase.h"
#include "UIWndCallback.h"



class CUIDragDropListEx;
class CUI3tButton;
class CUIStatic;
class CUIMpItemsStoreWnd;
class CStoreHierarchy;
class CUITabControl;
class CUICellItem;
class CInventoryItem;
class CItemCostMgr;

struct SBuyItemInfo
{
	enum EItmState{e_bought,e_sold,e_own,e_shop};
	~SBuyItemInfo		();
	EItmState			m_item_state;
	shared_str			m_name_sect;
	CUICellItem*		m_cell_item;
};

DEF_VECTOR(ITEMS_vec,SBuyItemInfo*);
typedef ITEMS_vec::const_iterator ITEMS_vec_cit;

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
		e_shop,
		e_player_bag,
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

	virtual const u8			GetItemIndex				(u32 slotNum, u32 idx, u8 &sectionNum);
	virtual const u8			GetBeltSize					();
	virtual const u8			GetWeaponIndexInBelt		(u32 indexInBelt, u8 &sectionId, u8 &itemId, u8 &count);
	virtual void				GetWeaponIndexByName		(const shared_str& sectionName, u8 &grpNum, u8 &idx);
	virtual u32					GetMoneyAmount				() const;
	virtual void				IgnoreMoney					(bool ignore);
	virtual void				SectionToSlot				(const u8 grpNum, u8 uIndexInSlot, bool bRealRepresentationSet);
	virtual void 				SetMoneyAmount				(u32 money);
	virtual bool 				CheckBuyAvailabilityInSlots	();
	virtual void				AddonToSlot					(int add_on, int slot, bool bRealRepresentationSet);
	virtual const shared_str&	GetWeaponNameByIndex		(u8 grpNum, u8 idx);
	virtual void 				SetSkin						(u8 SkinID);
	virtual void				IgnoreMoneyAndRank			(bool ignore);
	virtual void				ClearSlots					();
	virtual void				ClearRealRepresentationFlags();
	virtual const u8			GetWeaponIndex				(u32 slotNum);//
	virtual bool 				CanBuyAllItems				();
	virtual void 				ResetItems					();
	virtual void				SetRank						(u32 rank);

private:
	//data
	shared_str			m_sectionName;
	shared_str			m_sectionPrice;
	u32					m_money;
	CStoreHierarchy*	m_store_hierarchy;
	CUICellItem*		m_pCurrentCellItem;
	ITEMS_vec			m_all_items;
	CItemCostMgr*		m_cost_mngr;

//controls
	CUIWindow*			m_shop_wnd;
	CUIStatic*			m_static_money;
	CUIStatic*			m_static_current_item;
	CUIStatic*			m_static_rank;
	CUI3tButton* 		m_btn_shop_back;
	CUI3tButton* 		m_btn_ok;
	CUI3tButton* 		m_btn_cancel;
	CUITabControl*		m_root_tab_control;
	CUIDragDropListEx*	m_list[e_total_lists];
	void				UpdateMomeyIndicator		();
	void				UpdateShop					();

//handlers
	void	xr_stdcall	OnBtnOkClicked				(CUIWindow* w, void* d);
	void	xr_stdcall	OnBtnCancelClicked			(CUIWindow* w, void* d);
	void	xr_stdcall	OnBtnShopBackClicked		(CUIWindow* w, void* d);
	void	xr_stdcall	OnRootTabChanged			(CUIWindow* w, void* d);
	void	xr_stdcall	OnSubLevelBtnClicked		(CUIWindow* w, void* d);

	// drag drop handlers
	bool	xr_stdcall	OnItemDrop					(CUICellItem* itm);
	bool	xr_stdcall	OnItemStartDrag				(CUICellItem* itm);
	bool	xr_stdcall	OnItemDbClick				(CUICellItem* itm);
	bool	xr_stdcall	OnItemSelected				(CUICellItem* itm);
	bool	xr_stdcall	OnItemRButtonClick			(CUICellItem* itm);

	void				FillUpSubLevelButtons		();
	void				FillUpSubLevelItems			();

	void				SetCurrentItem				(CUICellItem* itm);
	CInventoryItem*		CurrentIItem				();
	CUICellItem*		CurrentItem					();
	int					GetItemPrice				(CInventoryItem* itm);
	CInventoryItem*		CreateItem_internal			(const shared_str& name_sect);
	SBuyItemInfo*		CreateItem					(const shared_str& name_sect, SBuyItemInfo::EItmState type);
};
