#ifndef __XR_UIGAMEDM_H__
#define __XR_UIGAMEDM_H__
#pragma once

#include "UIGameCustom.h"


#include "ui/UIDialogWnd.h"
#include "ui/UIInventoryWnd.h"
#include "ui/UIMapWnd.h"

#include "ui/UIBuyWeaponWnd.h"
#include "ui\UISkinSelector.h"


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

	FRAGS_LISTS			m_aFragsLists;
	PLAYERS_LISTS		m_aPlayersLists;
	TEAMS_LIST			m_aTeamSections;

	CUIBuyWeaponWnd*		pBuyMenuTeam0;
	CUIBuyWeaponWnd*		pCurBuyMenu;

	CUISkinSelectorWnd*		pSkinMenuTeam0;
	CUISkinSelectorWnd*		pCurSkinMenu;

	virtual	void		ClearLists ();
	
	virtual CUIBuyWeaponWnd*		InitBuyMenu			(s16 Team = -1);
	virtual void					FillDefItems		(const char* caSection, CUIBuyWeaponWnd* pMenu);

	virtual CUISkinSelectorWnd*		InitSkinMenu		(s16 Team = -1);

	virtual s16			ModifyTeam			(s16 Team)	{return Team;};

public:
						CUIGameDM			(CUI* parent);
	virtual 			~CUIGameDM			();

	virtual	void		Init				();

	virtual	void		SetCurrentBuyMenu	()	{pCurBuyMenu = pBuyMenuTeam0; };
	virtual	void		SetCurrentSkinMenu	()	{pCurSkinMenu = pSkinMenuTeam0; };

	virtual void		Render				();
	virtual void		OnFrame				();

	virtual bool		IR_OnKeyboardPress		(int dik);
	virtual bool		IR_OnKeyboardRelease	(int dik);

	virtual void		OnBuyMenu_Ok			();

	virtual CUIDialogWnd*	GetBuyWnd			()	{ return (CUIDialogWnd*) pCurBuyMenu; };
	virtual CUIDialogWnd*	GetSkinWnd			()	{ return (CUIDialogWnd*) pCurSkinMenu; };

	virtual bool		CanBeReady				();

	/////////
	CUIInventoryWnd		InventoryMenu;
//	CUIBuyWeaponWnd		UIBuyWeaponWnd;
//	CUIStatsWnd*		pUIStatsWnd;
};
#endif // __XR_UIGAMEDM_H__
