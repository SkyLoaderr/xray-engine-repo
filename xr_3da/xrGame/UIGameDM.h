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

	CUIWindow*				m_pFragLists;
	CUIWindow*				m_pPlayerLists;

	shared_str					m_time_caption;		
	shared_str					m_spectrmode_caption;		
	
	shared_str					m_spectator_caption;
	shared_str					m_pressjump_caption;
	shared_str					m_pressbuy_caption;

	virtual	void					ClearLists				();

public:
									CUIGameDM				();
	virtual 						~CUIGameDM				();

	virtual void					SetClGame				(game_cl_GameState* g);
	virtual	void					Init					();


			void					SetTimeMsgCaption		(LPCSTR str);
			void					SetSpectrModeMsgCaption		(LPCSTR str);

			void					SetSpectatorMsgCaption			(LPCSTR str);
			void					SetPressJumpMsgCaption			(LPCSTR str);
			void					SetPressBuyMsgCaption			(LPCSTR str);


			void					ShowFragList			(bool bShow);
			void					ShowPlayersList			(bool bShow);
};
#endif // __XR_UIGAMEDM_H__
