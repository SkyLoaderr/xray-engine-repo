#ifndef __XR_UIGAMETDM_H__
#define __XR_UIGAMETDM_H__
#pragma once

#include "UIGameCustom.h"
#include "UIGameDM.h"

#include "ui/UIDialogWnd.h"
#include "ui/UIInventoryWnd.h"
#include "ui/UIMapWnd.h"
#include "ui/UISpawnWnd.h"

#include "ui/UIBuyWeaponWnd.h"

// refs 

class CUITDMFragList;
class CUITDMPlayerList;
class CUIBuyWeaponWnd;
class CUISkinSelectorWnd;
class game_cl_TeamDeathmatch;

class CUIGameTDM: public CUIGameDM
{
private:
	game_cl_TeamDeathmatch * m_game;
	typedef CUIGameDM inherited;

protected:
//moved to game_cl_teamdeathmatch
//	CUISpawnWnd*		pUITeamSelectWnd;
//
//	CUIBuyWeaponWnd*	pBuyMenuTeam1;
//	CUIBuyWeaponWnd*	pBuyMenuTeam2;
//
//	CUISkinSelectorWnd*	pSkinMenuTeam1;
//	CUISkinSelectorWnd*	pSkinMenuTeam2;
//
//	PRESET_ITEMS		PresetItemsTeam1;
//	PRESET_ITEMS		PresetItemsTeam2;
//
//	BOOL				m_bTeamSelected;
//
//	virtual s16			ModifyTeam			(s16 Team)	{return Team-1;};
public:
	CUIGameTDM			();
	virtual 			~CUIGameTDM			();
	virtual void		SetClGame			(game_cl_GameState* g);
	virtual	void		Init				();

	virtual bool		IR_OnKeyboardPress	(int dik);

//moved to game_cl_teamdeathmatch
//	virtual void		OnTeamSelect		(int Result);
//	virtual bool		CanBeReady			();
//	virtual	void		SetCurrentBuyMenu	();
//	virtual	void		SetCurrentSkinMenu	();
};
#endif // __XR_UIGAMETDM_H__
