#ifndef __XR_UIGAMECS_H__
#define __XR_UIGAMECS_H__
#pragma once

#include "UIGameCustom.h"
#include "UIBuyMenu.h"
#include "UICSFragList.h"
#include "UICSPlayerList.h"
#include "UIIcon.h"

// refs
class CCustomMenuItem;

class CUIGameCS: public CUIGameCustom{
	CUIBuyMenu			BuyMenu;
	CUICSFragList		FragList;
	CUICSPlayerList		PlayerList;
	CUIIcon				BuyZone;
	CUIIcon				Artifact;
	CUIIcon				OwnBase;
	CUIIcon				EnemyBase;
	Ivector2			vMoneyPlace;
	BOOL				CanBuy				();
	static void 		BuyItem				(CCustomMenuItem* sender);
public:
						CUIGameCS			(CUI* parent);
	virtual 			~CUIGameCS			();
	virtual void		Render				();
	virtual void		OnFrame				();

	virtual bool		OnKeyboardPress		(int dik);
	virtual bool		OnKeyboardRelease	(int dik);
};
#endif // __XR_UIGAMECS_H__
