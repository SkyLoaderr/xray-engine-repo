#ifndef __XR_UIGAMECS_H__
#define __XR_UIGAMECS_H__
#pragma once

#include "UIGameCustom.h"
#include "UIBuyMenu.h"
#include "UICHTMenu.h"
#include "UICSFragList.h"
#include "UICSPlayerList.h"
#include "UIIcon.h"

// refs
class CUICustomMenuItem;

class CUIGameCS: public CUIGameCustom{
	CUIBuyMenu			BuyMenu;
	CUIChangeTeamMenu	CTMenu;
	CUICSFragList		FragList;
	CUICSPlayerList		PlayerList;
	CUIIcon				BuyZone;
	CUIIcon				Artifact;
	CUIIcon				OwnBase;
	CUIIcon				EnemyBase;
	Ivector2			vMoneyPlace;
	Ivector2			vTimePlace;
	BOOL				CanBuy				();
	static void 		BuyItem				(CUICustomMenuItem* sender);
public:
						CUIGameCS			(CUI* parent);
	virtual 			~CUIGameCS			();
	virtual void		Render				();
	virtual void		OnFrame				();

	virtual bool		OnKeyboardPress		(int dik);
	virtual bool		OnKeyboardRelease	(int dik);
};
#endif // __XR_UIGAMECS_H__
