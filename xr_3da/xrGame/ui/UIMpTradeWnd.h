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

struct SBuyItemInfo
{
	enum EItmState{e_undefined,e_bought,e_sold,e_own,e_shop};

						~SBuyItemInfo		();
						SBuyItemInfo		();
	shared_str			m_name_sect;
	CUICellItem*		m_cell_item;
	
	const EItmState&	GetState			() const				{return m_item_state;}
	void				SetState			(const EItmState& s);
	LPCSTR				GetStateAsText		() const;
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
		e_first					= 0,
		e_pistol				= e_first,
		e_pistol_ammo,
		e_rifle,
		e_rifle_ammo,
		e_outfit,
		e_medkit,
		e_granade,
		e_others,
		e_player_bag,
		e_player_total,
		e_shop					= e_player_total,
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

	virtual void				ItemToBelt					(const shared_str& sectionName);
	virtual void				ItemToRuck					(const shared_str& sectionName, u32 addons);
	virtual void				ItemToSlot					(const shared_str& sectionName, u32 addons);
	virtual void				SetupPlayerItemsBegin		();
	virtual void				SetupPlayerItemsEnd			();

	virtual const preset_items&	GetPreset					(ETradePreset idx);

private:
	//data
	shared_str			m_sectionName;
	shared_str			m_sectionPrice;
	u32					m_money;
	CStoreHierarchy*	m_store_hierarchy;
	CUICellItem*		m_pCurrentCellItem;
	ITEMS_vec			m_all_items;
	CItemMgr*			m_item_mngr;
	preset_items		m_preset_storage[_preset_count];
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

	CUI3tButton* 		m_btn_pistol_ammo;
	CUI3tButton*		m_btn_pistol_silencer;
	CUI3tButton* 		m_btn_rifle_ammo;
	CUI3tButton* 		m_btn_rifle_silencer;
	CUI3tButton* 		m_btn_rifle_scope;
	CUI3tButton* 		m_btn_rifle_glauncher;
	CUI3tButton* 		m_btn_rifle_ammo2;

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


	void	xr_stdcall	OnBtnPistolAmmoClicked		(CUIWindow* w, void* d);
	void	xr_stdcall	OnBtnPistolSilencerClicked	(CUIWindow* w, void* d);
	void	xr_stdcall	OnBtnRifleAmmoClicked		(CUIWindow* w, void* d);
	void	xr_stdcall	OnBtnRifleSilencerClicked	(CUIWindow* w, void* d);
	void	xr_stdcall	OnBtnRifleScopeClicked		(CUIWindow* w, void* d);
	void	xr_stdcall	OnBtnRifleGLClicked			(CUIWindow* w, void* d);
	void	xr_stdcall	OnBtnRifleAmmo2Clicked		(CUIWindow* w, void* d);

	// drag drop handlers
	bool	xr_stdcall	OnItemDrop					(CUICellItem* itm);
	bool	xr_stdcall	OnItemStartDrag				(CUICellItem* itm);
	bool	xr_stdcall	OnItemDbClick				(CUICellItem* itm);
	bool	xr_stdcall	OnItemSelected				(CUICellItem* itm);
	bool	xr_stdcall	OnItemRButtonClick			(CUICellItem* itm);

	void				FillUpSubLevelButtons		();
	void				FillUpSubLevelItems			();

	bool				TryToBuyItem				(SBuyItemInfo* itm, bool own_item);
	bool				TryToSellItem				(SBuyItemInfo* itm);
	bool				TryToAttachItemAsAddon		(SBuyItemInfo* buy_itm);
	void				SellItemAddons				(SBuyItemInfo* sell_itm, u8 addon_id);
	bool				CheckBuyPossibility			(const shared_str& sect_name);

	void				SetCurrentItem				(CUICellItem* itm);
	CInventoryItem*		CurrentIItem				();
	CUICellItem*		CurrentItem					();
	int					GetItemPrice				(CInventoryItem* itm);

	CInventoryItem*		CreateItem_internal			(const shared_str& name_sect);
	SBuyItemInfo*		CreateItem					(const shared_str& name_sect, SBuyItemInfo::EItmState state, bool find_if_exist);
	SBuyItemInfo*		FindItem					(CUICellItem* item);
	SBuyItemInfo*		FindItem					(const shared_str& name_sect, SBuyItemInfo::EItmState state);
	SBuyItemInfo*		FindItem					(SBuyItemInfo::EItmState state);
	void				DestroyItem					(SBuyItemInfo* item);

	void				RenewShopItem				(const shared_str& sect_name, bool b_just_bought);
	u32					GetItemCount				(SBuyItemInfo::EItmState state) const;
	u32					GetItemCount				(const shared_str& name_sect, SBuyItemInfo::EItmState state) const;
	u32					GetGroupCount				(const shared_str& name_group, SBuyItemInfo::EItmState state)const;

	void				ResetToOrigin				();
	void				ApplyPreset					(ETradePreset idx);
	void				StorePreset					(ETradePreset idx);
	void				DumpPreset					(ETradePreset idx);
	void				DumpAllItems				(LPCSTR reason);
	dd_list_type		GetListType					(CUIDragDropListEx* l);
	CUIDragDropListEx*	GetMatchedListForItem		(const shared_str& sect_name);
	const u32			GetRank						() const;
	void				SetInfoString				(LPCSTR str);
};

#include "../associative_vector.h"
class CItemMgr
{
	struct _i{
		u8			slot_idx;
		u32			cost[_RANK_COUNT];
	};
	typedef associative_vector<shared_str, _i>					COST_MAP;
	typedef COST_MAP::iterator									COST_MAP_IT;
	typedef COST_MAP::const_iterator							COST_MAP_CIT;
	COST_MAP				m_items;
public:
	void					Load			(const shared_str& sect);
	const u32				GetItemCost		(const shared_str& sect_name, u32 rank)	const;
	const u8				GetItemSlotIdx	(const shared_str& sect_name) const;
	const u32				GetItemIdx		(const shared_str& sect_name) const;
	void					Dump			() const;
	const u32				GetItemsCount	() const;
	const shared_str&		GetItemName		(u32 Idx) const;
};