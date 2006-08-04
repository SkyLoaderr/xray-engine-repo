#pragma once

#include "UIDialogWnd.h"
#include "UIStatic.h"
#include "UIDragDropListEx.h"
#include "UIBuyWeaponTab.h"
#include "UIBagWnd.h"
#include "UI3tButton.h"
#include "UIPropertiesBox.h"
#include "UIItemInfo.h"

typedef enum {
	MP_SLOT_PISTOL,
	MP_SLOT_RIFLE,
	MP_SLOT_BELT,
	MP_SLOT_OUTFIT,

	MP_SLOT_NUM
} MP_BUY_SLOT;


class CUIBuyWnd : public CUIDialogWnd{
public:
	CUIBuyWnd();
	~CUIBuyWnd();

	// own
	void Init(LPCSTR sectionName, LPCSTR sectionPrice);
	void OnTabChange();
	void OnMenuLevelChange();
	void SetSkin(u8 SkinID);
	bool CanBuyAllItems();
	void SetMoneyAmount(int money);
	bool CheckBuyAvailabilityInSlots();

	// handlers
	void OnBtnOk();
	void OnBtnCancel();
	void OnBtnClear();
	void OnMoneyChange();
	void OnBtnBulletBuy(int slot);
	void OnBtnRifleGrenade();
	void AfterBuy();
	void HightlightCurrAmmo();

	// CUIWindow
	bool OnKeyboard(int dik, EUIMessages keyboard_action);
	void SendMessage(CUIWindow* pWnd, s16 msg, void* pData = 0);

	// drag drop handlers
	void		BindDragDropListEvents(CUIDragDropListEx* lst);
	bool		xr_stdcall		OnItemDrop			(CUICellItem* itm);
	bool		xr_stdcall		OnItemStartDrag		(CUICellItem* itm);
	bool		xr_stdcall		OnItemDbClick		(CUICellItem* itm);
	bool		xr_stdcall		OnItemSelected		(CUICellItem* itm);
	bool		xr_stdcall		OnItemRButtonClick	(CUICellItem* itm);

	///////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////
	const u8	GetWeaponIndex(u32 slotNum);//
	const u8	GetWeaponIndexInBelt(u32 indexInBelt, u8 &sectionId, u8 &itemId);//
	const u8	GetItemIndex(u32 slotNum, u32 idx, u8 &sectionNum);//
	
	void		GetWeaponIndexByName(const xr_string sectionName, u8 &grpNum, u8 &idx);//
	char*		GetWeaponNameByIndex(u8 grpNum, u8 idx);//

	// Получить данные о аддонах к оружию. Младшие 3 бита, если установлены в 1 означают:
	// 2 - Silencer, 1 - Grenade Launcher, 0 - Scope
	const u8	GetWeaponAddonInfoByIndex(u8 idx);//
	const u8	GetBeltSize();//
	void		SetRank(int rank);//
//	void		ReInitItems	(LPCSTR strSectionName);

	void		SectionToSlot(const u8 grpNum, u8 uIndexInSlot, bool bRealRepresentationSet);
	void		SectionToSlot(LPCSTR name, const u8 addon_info, bool bRealRepresentationSet);
	void		SectionToSlot(const char *sectionName, bool bRealRepresentationSet);
	void		ClearSlots();
	void		ClearRealRepresentationFlags();

	void		ReloadItemsPrices	();
	void		IgnoreMoney(bool ignore);
	void		IgnoreMoneyAndRank	(bool ignore);
	int			GetMoneyAmount() const;

protected:
	void				SetCurrentItem				(CUICellItem* itm);
	CUICellItem*		CurrentItem					();
	CInventoryItem*		CurrentIItem				();
	CWeapon*			GetPistol					();
	CWeapon*			GetRifle					();
	void				ActivatePropertiesBox		();
	EListType			GetType						(CUIDragDropListEx* l);
	CUIDragDropListEx*	GetSlotList					(u32 slot_idx);
	MP_BUY_SLOT			GetLocalSlot				(u32 slot);
	bool				ToSlot						(CUICellItem* itm, bool force_place);
	bool				ToBag						(CUICellItem* itm, bool b_use_cursor_pos);
	bool				ToBelt						(CUICellItem* itm, bool b_use_cursor_pos);
	bool				CanPutInSlot				(CInventoryItem* iitm);
	bool				CanPutInBag					(CInventoryItem* iitm);
	bool				CanPutInBelt				(CInventoryItem* iitm);
	bool				ClearTooExpensiveItems		();
	bool				ClearSlot_ifTooExpensive	(int slot);
	bool				SlotToSection				(int slot);
	void				AttachAddon					(CInventoryItem* item_to_upgrade);
	void				DetachAddon					(const char* addon_name);
	void				ProcessPropertiesBoxClicked	();
	void				UpdateOutfit				();
	void				Highlight					(int slot);


	// data
	shared_str			m_sectionName;
	shared_str			m_sectionPrice;
	CUICellItem*		m_pCurrentCellItem;

//	bool		m_bIgnoreMoney;
	bool		m_bIgnoreMoneyAndRank;

	// background textures
	CUIStatic	m_slotsBack;
	CUIStatic	m_back;

	// buttons
	CUI3tButton m_btnOk;
	CUI3tButton m_btnCancel;
	CUI3tButton m_btnClear;

	CUI3tButton m_btnPistolBullet;
	CUI3tButton m_btnPistolSilencer;
	CUI3tButton m_btnRifleBullet;
	CUI3tButton m_btnRifleSilencer;
	CUI3tButton m_btnRifleScope;
	CUI3tButton m_btnRifleGrenadelauncer;
	CUI3tButton m_btnRifleGrenade;

	// text
	CUIStatic	m_moneyInfo;
	
	// controls
	CUIPropertiesBox	m_propertiesBox;
	CUIItemInfo			m_itemInfo;
	CUIStatic			m_rankInfo;
	CUIBagWnd			m_bag;
	CUIBuyWeaponTab		m_tab;
	CUIDragDropListEx*	m_list[MP_SLOT_NUM];

};