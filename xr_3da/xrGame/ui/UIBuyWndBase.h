#pragma once

#include "UIDialogWnd.h"

class CUIDragDropListEx;

class IBuyWnd:			public CUIDialogWnd
{
public:
	virtual						~IBuyWnd					()	{};
	virtual void 				Init						(const shared_str& sectionName, const shared_str& sectionPrice)			=0;
	virtual void				BindDragDropListEvents		(CUIDragDropListEx* lst, bool bDrag)							=0;


	virtual const u8			GetItemIndex				(u32 slotNum, u32 idx, u8 &sectionNum)							=0;
	virtual const u8			GetBeltSize					()																=0;
	virtual const u8			GetWeaponIndexInBelt		(u32 indexInBelt, u8 &sectionId, u8 &itemId, u8 &count)			=0;
	virtual void				GetWeaponIndexByName		(const shared_str& sectionName, u8 &grpNum, u8 &idx)			=0;
	virtual int					GetMoneyAmount				() const														=0;
	virtual void				IgnoreMoney					(bool ignore)													=0;
	virtual void				SectionToSlot				(const u8 grpNum, u8 uIndexInSlot, bool bRealRepresentationSet)	=0;
	virtual void 				SetMoneyAmount				(int money)														=0;
	virtual bool 				CheckBuyAvailabilityInSlots	()															=0;
	virtual void				AddonToSlot					(int add_on, int slot, bool bRealRepresentationSet)				=0;
	virtual const shared_str&	GetWeaponNameByIndex		(u8 grpNum, u8 idx)												=0;
	virtual void 				SetSkin						(u8 SkinID)														=0;
	virtual void				IgnoreMoneyAndRank			(bool ignore)												=0;
	virtual void				ClearSlots					()															=0;
	virtual void				ClearRealRepresentationFlags()															=0;
	virtual const u8			GetWeaponIndex				(u32 slotNum)												=0;
	virtual bool 				CanBuyAllItems				()															=0;
	virtual void 				ResetItems					()															=0;
	virtual void				SetRank						(int rank)													=0;

};
