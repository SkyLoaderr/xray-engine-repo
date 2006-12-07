#pragma once

#include "UIDialogWnd.h"
#include "UIWndCallback.h"



class CUIDragDropListEx;
class CUIBuyWeaponTab;
class CUI3tButton;

class CUIMpTradeWnd :public CUIDialogWnd, public CUIWndCallback
{
public:
						CUIMpTradeWnd			();
	virtual				~CUIMpTradeWnd			();

	void 				Init					(const shared_str& sectionName, const shared_str& sectionPrice);
	void				BindDragDropListEvents	(CUIDragDropListEx* lst, bool bDrag);

	const u8			GetItemIndex			(u32 slotNum, u32 idx, u8 &sectionNum);
	const u8			GetBeltSize				();
	const u8			GetWeaponIndexInBelt	(u32 indexInBelt, u8 &sectionId, u8 &itemId, u8 &count);
	void				GetWeaponIndexByName	(const shared_str& sectionName, u8 &grpNum, u8 &idx);
	int					GetMoneyAmount			() const;
	void				IgnoreMoney				(bool ignore);
	void				SectionToSlot			(const u8 grpNum, u8 uIndexInSlot, bool bRealRepresentationSet);
	void 				SetMoneyAmount			(int money);
	bool 				CheckBuyAvailabilityInSlots	();
	void				AddonToSlot				(int add_on, int slot, bool bRealRepresentationSet);
	const shared_str&	GetWeaponNameByIndex	(u8 grpNum, u8 idx);
	void 				SetSkin					(u8 SkinID);
	void				IgnoreMoneyAndRank			(bool ignore);
	void				ClearSlots					();
	void				ClearRealRepresentationFlags();
	const u8			GetWeaponIndex				(u32 slotNum);//
	bool 				CanBuyAllItems				();
	void 				ResetItems					();
	void				SetRank						(int rank);

private:
	//data
	shared_str			m_sectionName;
	shared_str			m_sectionPrice;
	int					m_rank;
	int					m_money;

	//controls
	CUI3tButton* 		m_btn_ok;
	CUI3tButton* 		m_btn_cancel;
	CUIBuyWeaponTab*	m_tab_control;

//handlers
	void __stdcall		OnBtnOkClicked				(CUIWindow* w, void* d);
	void __stdcall		OnBtnCancelClicked			(CUIWindow* w, void* d);

};
