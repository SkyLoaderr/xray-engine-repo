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
class game_cl_Deathmatch;

class CUIGameDM: public CUIGameCustom
{
private:
	game_cl_Deathmatch *	m_game;
	typedef CUIGameCustom inherited;
protected:
	enum{
		flShowFragList	= (1<<1),
		fl_force_dword	= u32(-1)	};

	void				SetFlag					(u32 mask, BOOL flag){if (flag) uFlags|=mask; else uFlags&=~mask; }
	void				InvertFlag				(u32 mask){if (uFlags&mask) uFlags&=~mask; else uFlags|=mask; }
	BOOL				GetFlag					(u32 mask){return uFlags&mask;}

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


	CUISkinSelectorWnd*		pSkinMenuTeam0;
	CUISkinSelectorWnd*		pCurSkinMenu;
	
	BOOL					m_bSkinSelected;

	ref_str					m_time_caption;		
	ref_str					m_spectrmode_caption;		
	
	ref_str					m_spectator_caption;
	ref_str					m_pressjump_caption;
	ref_str					m_pressbuy_caption;

	virtual	void					ClearLists				();
	
	virtual CUIBuyWeaponWnd*		InitBuyMenu				(LPCSTR BasePriceSection, s16 Team);

	virtual CUISkinSelectorWnd*		InitSkinMenu			(s16 Team = -1);

	virtual s16						ModifyTeam				(s16 Team)	{return Team;};
	//-----------------------------------------------------------------------------
	s16								m_iCurrentPlayersMoney;

	virtual	void					CheckItem				(PIItem pItem, PRESET_ITEMS* pPresetItems);

public:
	BOOL							m_bBuyEnabled;

									CUIGameDM				();
	virtual 						~CUIGameDM				();

	virtual void					SetClGame				(game_cl_GameState* g);
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

			void					SetTimeMsgCaption		(LPCSTR str);
			void					SetSpectrModeMsgCaption		(LPCSTR str);

			void					SetSpectatorMsgCaption			(LPCSTR str);
			void					SetPressJumpMsgCaption			(LPCSTR str);
			void					SetPressBuyMsgCaption			(LPCSTR str);
	/////////
	CUIInventoryWnd					InventoryMenu;
//	CUITextBanner					TimeMsgStatic, TimeMsgDyn;

//	CUIBuyWeaponWnd		UIBuyWeaponWnd;
//	CUIStatsWnd*		pUIStatsWnd;
};
#endif // __XR_UIGAMEDM_H__
