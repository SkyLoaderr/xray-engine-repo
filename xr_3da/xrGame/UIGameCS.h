#ifndef __XR_UIGAMECS_H__
#define __XR_UIGAMECS_H__
#pragma once

#include "UIGameCustom.h"

// refs 
class CUIBuyMenu;
class CUICSFragList;
class CUICSPlayerList;

class CUIGameCS: public CUIGameCustom{
	CUIBuyMenu*			pBuyMenu;
	CUICSFragList*		pFragList;
	CUICSPlayerList*	pPlayerList;
public:
						CUIGameCS			();
	virtual 			~CUIGameCS			();
	virtual void		Render				();
	virtual void		OnFrame				();

	virtual bool		OnKeyboardPress		(int dik);
	virtual bool		OnKeyboardRelease	(int dik);
};
#endif // __XR_UIGAMECS_H__
