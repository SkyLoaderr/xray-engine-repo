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

	// CUIWindow
	bool OnKeyboard(int dik, EUIMessages keyboard_action);
	void SendMessage(CUIWindow* pWnd, s16 msg, void* pData = 0);

	// drag drop handlers
	void BindDragDropListEnents(CUIDragDropListEx* lst);
	bool		xr_stdcall		OnItemDrop			(CUICellItem* itm);
	bool		xr_stdcall		OnItemStartDrag		(CUICellItem* itm);
	bool		xr_stdcall		OnItemDbClick		(CUICellItem* itm);
	bool		xr_stdcall		OnItemSelected		(CUICellItem* itm);
	bool		xr_stdcall		OnItemRButtonClick	(CUICellItem* itm);

protected:
	void				SetCurrentItem				(CUICellItem* itm);
	CUICellItem*		CurrentItem					();
	CInventoryItem*		CurrentIItem				();
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


	// data
	shared_str			m_sectionName;
	shared_str			m_sectionPrice;
	CUICellItem*		m_pCurrentCellItem;

	// background textures
	CUIStatic	m_slotsBack;
	CUIStatic	m_back;

	// buttons
	CUI3tButton m_btnOk;
	CUI3tButton m_btnCancel;
	CUI3tButton m_btnClear;

	// controls
	CUIPropertiesBox	m_propertiesBox;
	CUIItemInfo			m_itemInfo;
	CUIBagWnd			m_bag;
	CUIBuyWeaponTab		m_tab;
	CUIDragDropListEx*	m_list[MP_SLOT_NUM];

};