#ifndef __XR_UIGAMEDM_H__
#define __XR_UIGAMEDM_H__
#pragma once

#include "UIGameCustom.h"


#include "ui/UIDialogWnd.h"
#include "ui/UIInventoryWnd.h"
#include "ui/UIMapWnd.h"

#include "ui/UIBuyWeaponWnd.h"
#include "ui/UISkinSelector.h"
#include "ui/UITextBanner.h"


// refs 
class CUIDMFragList;
class CUIDMPlayerList;
class CUIBuyWeaponWnd;
class CUISkinSelectorWnd;

class CUIGameDM: public CUIGameCustom
{
private:
	typedef CUIGameCustom inherited;
protected:
	DEF_VECTOR			(FRAGS_LISTS, CUIDMFragList*);
	DEF_VECTOR			(PLAYERS_LISTS, CUIDMPlayerList*);
	DEF_VECTOR			(TEAMS_LIST, std::string);
	DEF_VECTOR			(PRESET_ITEMS, s16);

	FRAGS_LISTS				m_aFragsLists;
	PLAYERS_LISTS			m_aPlayersLists;
	TEAMS_LIST				m_aTeamSections;

	CUIBuyWeaponWnd*		pBuyMenuTeam0;
	CUIBuyWeaponWnd*		pCurBuyMenu;

	PRESET_ITEMS			PresetItemsTeam0;
	PRESET_ITEMS*			pCurPresetItems;

	BOOL					m_bBuyEnabled;

	CUISkinSelectorWnd*		pSkinMenuTeam0;
	CUISkinSelectorWnd*		pCurSkinMenu;
	
	BOOL					m_bSkinSelected;

	virtual	void					ClearLists				();
	
	virtual CUIBuyWeaponWnd*		InitBuyMenu				(LPCSTR BasePriceSection, s16 Team);
	virtual void					FillDefItems			(const char* caSection, CUIBuyWeaponWnd* pMenu);

	virtual CUISkinSelectorWnd*		InitSkinMenu			(s16 Team = -1);

	virtual s16						ModifyTeam				(s16 Team)	{return Team;};
	//-----------------------------------------------------------------------------
	s16								m_iCurrentPlayersMoney;

	virtual	void					CheckItem				(PIItem pItem, PRESET_ITEMS* pPresetItems);

public:
									CUIGameDM				();
	virtual 						~CUIGameDM				();

	virtual	void					Init					();

	virtual	void					SetCurrentBuyMenu		()	;//{pCurBuyMenu = pBuyMenuTeam0; };
	virtual	void					SetCurrentSkinMenu		()	{pCurSkinMenu = pSkinMenuTeam0; };

	virtual void					Render					();
	virtual void					OnFrame					();

	virtual bool					IR_OnKeyboardPress		(int dik);
	virtual bool					IR_OnKeyboardRelease	(int dik);

	virtual void					OnBuyMenu_Ok			();
	virtual void					OnSkinMenu_Ok			();

	virtual CUIDialogWnd*			GetBuyWnd				()	{ return (CUIDialogWnd*) pCurBuyMenu; };
	virtual CUIDialogWnd*			GetSkinWnd				()	{ return (CUIDialogWnd*) pCurSkinMenu; };

	virtual bool					CanBeReady				();
	virtual BOOL					CanCallBuyMenu			();

	virtual	void					SetBuyMenuItems			();
	virtual void					ClearBuyMenu			();

	/////////
	CUIInventoryWnd					InventoryMenu;
	CUITextBanner					TimeMsgStatic, TimeMsgDyn;
//	CUIBuyWeaponWnd		UIBuyWeaponWnd;
//	CUIStatsWnd*		pUIStatsWnd;
};
#endif // __XR_UIGAMEDM_H__
