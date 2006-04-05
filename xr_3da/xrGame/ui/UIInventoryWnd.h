#pragma once

class CInventory;

#include "UIDialogWnd.h"
#include "UIStatic.h"
#include "UI3tButton.h"

#include "UIDragDropItem.h"
#include "UIWpnDragDropItem.h"

//.#include "UIDragDropList.h"
#include "UIProgressBar.h"

#include "UIPropertiesBox.h"
#include "UIOutfitSlot.h"

#include "UIOutfitInfo.h"
#include "UIItemInfo.h"
#include "UITimeWnd.h"


class CArtefact;
class CUISleepWnd;
class CUIDragDropListEx;
class CUICellItem;

class CUIInventoryWnd: public CUIDialogWnd
{
private:
	typedef CUIDialogWnd	inherited;
	bool					m_b_need_reinit;
public:
							CUIInventoryWnd				();
	virtual					~CUIInventoryWnd			();

	virtual void			Init						();

	void					InitInventory				();
	void					InitInventory_delayed		();
	virtual bool			StopAnyMove					()					{return false;}

	virtual void			SendMessage					(CUIWindow *pWnd, s16 msg, void *pData);
	virtual bool			OnMouse						(float x, float y, EUIMessages mouse_action);
	virtual bool			OnKeyboard					(int dik, EUIMessages keyboard_action);


	IC CInventory*			GetInventory				()					{return m_pInv;}

	virtual void			Update						();
	virtual void			Draw						();

	virtual void			Show						();
	virtual void			Hide						();

	void					AddItemToBag				(PIItem pItem);

protected:
	enum eInventorySndAction{	eInvSndOpen	=0,
								eInvSndClose,
								eInvItemToSlot,
								eInvItemToBelt,
								eInvItemToRuck,
								eInvProperties,
								eInvDropItem,
								eInvAttachAddon,
								eInvDetachAddon,
								eInvSndMax};

	ref_sound				sounds					[eInvSndMax];
	void					PlaySnd					(eInventorySndAction a);
	friend class			CUITradeWnd;

    CUIStatic			UIBeltSlots;
	CUIStatic			UIBack;
	CUIStatic*			UIRankFrame;
	CUIStatic*			UIRank;

	CUIStatic			UIBagWnd;
	CUIStatic			UIMoneyWnd;
	CUIStatic			UIDescrWnd;
	CUIFrameWindow		UIPersonalWnd;

	CUISleepWnd*		UISleepWnd;
	CUI3tButton*		UISellAll;
	
	CUI3tButton			UIDropButton;
	CUI3tButton			UIExitButton;

//	CUIStatic			UIStaticTop;
//	CUIStatic			UIStaticBelt;
	CUIStatic			UIStaticBottom;
	CUITimeWnd			UITimeWnd;

	CUIStatic			UIStaticPersonal;
		
	#define SLOTS_NUM 5

	CUIDragDropListEx*			m_pUIBagList;
	CUIDragDropListEx*			m_pUIBeltList;
	CUIDragDropListEx*			m_pUITopList				[SLOTS_NUM]; 
	CUIOutfitDragDropList*		m_pUIOutfitList;
	void						ClearAllLists				();
	void						BindDragDropListEnents		(CUIDragDropListEx* lst);
	enum EListType{
		iwSlot,
		iwBag,
		iwBelt
	};
	EListType					GetType				(CUIDragDropListEx* l);
	CUIDragDropListEx*			GetSlotList			(u32 slot_idx);

	bool		xr_stdcall		OnItemDrop			(CUICellItem* itm);
	bool		xr_stdcall		OnItemStartDrag		(CUICellItem* itm);
	bool		xr_stdcall		OnItemDbClick		(CUICellItem* itm);
	bool		xr_stdcall		OnItemSelected		(CUICellItem* itm);
	bool		xr_stdcall		OnItemRButtonClick	(CUICellItem* itm);
	


	CUIProgressBar		UIProgressBarHealth;
	CUIProgressBar		UIProgressBarSatiety;
	CUIProgressBar		UIProgressBarPsyHealth;
	CUIProgressBar		UIProgressBarRadiation;

	//pop-up меню вызываемое по нажатию правой кнопки
	CUIPropertiesBox	UIPropertiesBox;
	
	//информация о персонаже
	CUIOutfitInfo		UIOutfitInfo;
	CUIItemInfo			UIItemInfo;

	CInventory*				m_pInv;

	CUICellItem*			m_pCurrentCellItem;
	//---------------------------------------------------------------------
	void				SendEvent_Item2Slot			(PIItem	pItem);
	void				SendEvent_Item2Belt			(PIItem	pItem);
	void				SendEvent_Item2Ruck			(PIItem	pItem);
	void				SendEvent_Item_Drop			(PIItem	pItem);
	void				SendEvent_Item_Eat			(PIItem	pItem);
	void 				SendEvent_Item_Sell			(PIItem	pItem);
	void 				SendEvent_ActivateArtefact	(PIItem	pItem);
	void				SendEvent_ActivateSlot		(PIItem	pItem);

	//---------------------------------------------------------------------

	void					ProcessPropertiesBoxClicked	();
	void					ActivatePropertiesBox		();
	void					Activate_Artefact			();

	void					DropCurrentItem				();
	void					EatCurrentItem				();
	
	bool					ToSlot						(CUICellItem* itm, bool force_place);
	bool					ToBag						(CUICellItem* itm, bool b_use_cursor_pos);
	bool					ToBelt						(CUICellItem* itm, bool b_use_cursor_pos);
	void					SellItem					();


	void					AttachAddon					(PIItem item_to_upgrade);
	void					DetachAddon					(const char* addon_name);

	void					SetCurrentItem				(CUICellItem* itm);
	CUICellItem*			CurrentItem					();
	PIItem					CurrentIItem				();

	TIItemContainer			ruck_list;
	u32						m_iCurrentActiveSlot;
};