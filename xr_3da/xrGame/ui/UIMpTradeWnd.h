#pragma once

#include "UIBuyWndShared.h"
#include "UIBuyWndBase.h"
#include "UIWndCallback.h"
#include "restrictions.h"


class CUIDragDropListEx;
class CUI3tButton;
class CUIStatic;
class CUIMpItemsStoreWnd;
class CStoreHierarchy;
class CUITabControl;
class CUICellItem;
class CInventoryItem;
class CItemMgr;
class CPresetMgr;

struct SBuyItemInfo
{
	enum EItmState{e_undefined,e_bought,e_sold,e_own,e_shop};
	~SBuyItemInfo		();
	SBuyItemInfo		();
	shared_str			m_name_sect;
	CUICellItem*		m_cell_item;
	
	const EItmState&	GetState	() const				{return m_item_state;}
	void				SetState	(const EItmState& s);
	LPCSTR				GetStateAsText() const;
private:
	EItmState			m_item_state;

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
	enum dd_list_type{
		dd_shop					=0,
		dd_own_bag				=1,
		dd_own_slot				=2,
	};
public:
								CUIMpTradeWnd				();
	virtual						~CUIMpTradeWnd				();
	virtual void				SendMessage					(CUIWindow* pWnd, s16 msg, void* pData = NULL);
	
	//
	virtual void				Update						();				
	virtual bool 				OnKeyboard					(int dik, EUIMessages keyboard_action);

public:
	virtual void 				Init						(const shared_str& sectionName, const shared_str& sectionPrice);
	virtual void				BindDragDropListEvents		(CUIDragDropListEx* lst, bool bDrag);

	virtual const u8			GetItemIndex				(u32 slotNum, u32 idx, u8 &sectionNum);
	virtual const u8			GetBeltSize					();
	virtual const u8			GetWeaponIndexInBelt		(u32 indexInBelt, u8 &sectionId, u8 &itemId, u8 &count);
	virtual void				GetWeaponIndexByName		(const shared_str& sectionName, u8 &grpNum, u8 &idx);
	virtual u32					GetMoneyAmount				() const;
	virtual void 				SetMoneyAmount				(u32 money);
	virtual void				IgnoreMoney					(bool ignore);
	virtual void				SectionToSlot				(const u8 grpNum, u8 uIndexInSlot, bool bRealRepresentationSet);
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

	void						SetInfoString				(LPCSTR str);

private:
	//data
	shared_str			m_sectionName;
	shared_str			m_sectionPrice;
	u32					m_money;
	CStoreHierarchy*	m_store_hierarchy;
	CUICellItem*		m_pCurrentCellItem;
	ITEMS_vec			m_all_items;
	CItemMgr*			m_item_mngr;
	CPresetMgr*			m_preset_mngr;
//controls
	CUIWindow*			m_shop_wnd;
	CUIStatic*			m_static_money;
	CUIStatic*			m_static_current_item;
	CUIStatic*			m_static_rank;
	CUIStatic*			m_static_information;
	CUI3tButton* 		m_btn_shop_back;
	CUI3tButton* 		m_btn_ok;
	CUI3tButton* 		m_btn_cancel;

	CUI3tButton* 		m_btn_preset_1;
	CUI3tButton* 		m_btn_preset_2;
	CUI3tButton* 		m_btn_preset_3;
	CUI3tButton* 		m_btn_last_set;
	CUI3tButton* 		m_btn_save_preset;
	CUI3tButton* 		m_btn_reset;
	CUI3tButton* 		m_btn_sell;

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

	void	xr_stdcall	OnBtnPreset1Clicked			(CUIWindow* w, void* d);
	void	xr_stdcall	OnBtnPreset2Clicked			(CUIWindow* w, void* d);
	void	xr_stdcall	OnBtnPreset3Clicked			(CUIWindow* w, void* d);
	void	xr_stdcall	OnBtnLastSetClicked			(CUIWindow* w, void* d);
	void	xr_stdcall	OnBtnSavePresetClicked		(CUIWindow* w, void* d);
	void	xr_stdcall	OnBtnResetClicked			(CUIWindow* w, void* d);
	void	xr_stdcall	OnBtnSellClicked			(CUIWindow* w, void* d);

	// drag drop handlers
	bool	xr_stdcall	OnItemDrop					(CUICellItem* itm);
	bool	xr_stdcall	OnItemStartDrag				(CUICellItem* itm);
	bool	xr_stdcall	OnItemDbClick				(CUICellItem* itm);
	bool	xr_stdcall	OnItemSelected				(CUICellItem* itm);
	bool	xr_stdcall	OnItemRButtonClick			(CUICellItem* itm);

	void				FillUpSubLevelButtons		();
	void				FillUpSubLevelItems			();

	bool				TryToBuyItem				(CUICellItem* itm);
	bool				TryToSellItem				(CUICellItem* itm);

	bool				CheckBuyPossibility			(const shared_str& sect_name);

	void				SetCurrentItem				(CUICellItem* itm);
	CInventoryItem*		CurrentIItem				();
	CUICellItem*		CurrentItem					();
	int					GetItemPrice				(CInventoryItem* itm);

	CInventoryItem*		CreateItem_internal			(const shared_str& name_sect);
	SBuyItemInfo*		CreateItem					(const shared_str& name_sect, SBuyItemInfo::EItmState state);
	SBuyItemInfo*		FindItem					(CUICellItem* item);
	SBuyItemInfo*		FindItem					(const shared_str& name_sect, SBuyItemInfo::EItmState type);
	void				DestroyItem					(SBuyItemInfo* item);

	void				RenewShopItem				(const shared_str& sect_name, bool b_just_bought);
	u32					GetItemCount				(const shared_str& name_sect, SBuyItemInfo::EItmState state) const;
	u32					GetGroupCount				(const shared_str& name_group, SBuyItemInfo::EItmState state)const;


	dd_list_type		GetListType					(CUIDragDropListEx* l);
	CUIDragDropListEx*	GetMatchedListForItem		(const shared_str& sect_name);
	const u32			GetRank						() const;
};

#include "../associative_vector.h"
class CItemMgr
{
	struct _i{
		u8			slot_idx;
		u32			cost[_RANK_COUNT];
	};
	typedef associative_vector<shared_str, _i>	COST_MAP;
	typedef COST_MAP::iterator									COST_MAP_IT;
	typedef COST_MAP::const_iterator							COST_MAP_CIT;
	COST_MAP				m_items;
public:
	void					Load			(const shared_str& sect);
	const u32				GetItemCost		(const shared_str& sect_name, u32 rank)	const;
	const u8				GetItemSlotIdx	(const shared_str& sect_name) const;
	void					Dump			() const;
};

class CPresetMgr
{
public:
	struct _item
	{
		shared_str			sect_name;
		u32					count;
		shared_str			addons_sect[3];
	};
private:
	DEF_VECTOR				(items,_item);
	items					m_storage[4]; // 0 -last set 1-2-3 user preset
public:
		CPresetMgr			();
	const items&			GetPreset			(u32 idx);
	void					StorePreset			(u32 idx, CUIMpTradeWnd* source);
};