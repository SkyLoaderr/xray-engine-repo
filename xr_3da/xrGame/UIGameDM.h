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


	DEF_VECTOR			(FRAGS_LISTS, CUIDMFragList*);
	DEF_VECTOR			(PLAYERS_LISTS, CUIDMPlayerList*);

//  moved to game_cl_deathmatch
//	DEF_VECTOR			(TEAMS_LIST, std::string);
//	DEF_VECTOR			(PRESET_ITEMS, s16);

	CUIWindow*				m_pFragLists;
	CUIWindow*				m_pPlayerLists;
//	FRAGS_LISTS				m_aFragsLists;
//	PLAYERS_LISTS			m_aPlayersLists;

//  moved to game_cl_deathmatch
//	TEAMS_LIST				m_aTeamSections;
//	CUIBuyWeaponWnd*		pBuyMenuTeam0;
//	CUIBuyWeaponWnd*		pCurBuyMenu;

//	PRESET_ITEMS			PresetItemsTeam0;
//	PRESET_ITEMS*			pCurPresetItems;

//	CUISkinSelectorWnd*		pSkinMenuTeam0;
//	CUISkinSelectorWnd*		pCurSkinMenu;
	
//	BOOL					m_bSkinSelected;

	shared_str					m_time_caption;		
	shared_str					m_spectrmode_caption;		
	
	shared_str					m_spectator_caption;
	shared_str					m_pressjump_caption;
	shared_str					m_pressbuy_caption;

	virtual	void					ClearLists				();
	

	//-----------------------------------------------------------------------------
//	s16								m_iCurrentPlayersMoney;

//  moved to game_cl_deathmatch
//	virtual CUIBuyWeaponWnd*		InitBuyMenu				(LPCSTR BasePriceSection, s16 Team);
//	virtual CUISkinSelectorWnd*		InitSkinMenu			(s16 Team = -1);
//	virtual	void					CheckItem				(PIItem pItem, PRESET_ITEMS* pPresetItems);
//	virtual s16						ModifyTeam				(s16 Team)	{return Team;};

public:
//	BOOL							m_bBuyEnabled;

									CUIGameDM				();
	virtual 						~CUIGameDM				();

	virtual void					SetClGame				(game_cl_GameState* g);
	virtual	void					Init					();

//  moved to game_cl_deathmatch
//	virtual	void					SetCurrentBuyMenu		()	;//{pCurBuyMenu = pBuyMenuTeam0; };
//	virtual	void					SetCurrentSkinMenu		()	{pCurSkinMenu = pSkinMenuTeam0; };
//	virtual void					OnBuyMenu_Ok			();
//	virtual void					OnSkinMenu_Ok			();
//	virtual CUIDialogWnd*			GetBuyWnd				()	{ return (CUIDialogWnd*) pCurBuyMenu; };
//	virtual CUIDialogWnd*			GetSkinWnd				()	{ return (CUIDialogWnd*) pCurSkinMenu; };
//	virtual bool					CanBeReady				();
//	virtual BOOL					CanCallBuyMenu			();
//	virtual	void					SetBuyMenuItems			();

//	virtual void					Render					();
//	virtual void					OnFrame					();

//	virtual bool					IR_OnKeyboardPress		(int dik);
//	virtual bool					IR_OnKeyboardRelease	(int dik);




//	virtual void					ClearBuyMenu			();

			void					SetTimeMsgCaption		(LPCSTR str);
			void					SetSpectrModeMsgCaption		(LPCSTR str);

			void					SetSpectatorMsgCaption			(LPCSTR str);
			void					SetPressJumpMsgCaption			(LPCSTR str);
			void					SetPressBuyMsgCaption			(LPCSTR str);

//	CUITextBanner					TimeMsgStatic, TimeMsgDyn;
			void					ShowFragList			(bool bShow);
			void					ShowPlayersList			(bool bShow);

// moved to game_cl_deathmatch
//	CUIInventoryWnd					InventoryMenu;

};
#endif // __XR_UIGAMEDM_H__
