#ifndef __XR_UIGAMEDM_H__
#define __XR_UIGAMEDM_H__
#pragma once

#include "UIGameCustom.h"

// refs 
class CUIDMFragList;
class CUIDMPlayerList;

class CUIGameDM: public CUIGameCustom{
	CUIDMFragList*		pFragList;
	CUIDMPlayerList*	pPlayerList;
public:
						CUIGameDM			();
	virtual 			~CUIGameDM			();
	virtual void		Render				();
	virtual void		OnFrame				();

	virtual bool		OnKeyboardPress		(int dik);
	virtual bool		OnKeyboardRelease	(int dik);
};
#endif // __XR_UIGAMEDM_H__
