#ifndef __XR_UIGAMECUSTOM_H__
#define __XR_UIGAMECUSTOM_H__
#pragma once

// refs
class CUI;

class CUIGameCustom{
protected:
	u32					uFlags;
	CUI*				m_Parent;
public:
	enum{
		flShowFragList	= (1<<1),

		fl_force_dword	= u32(-1)
	};
	void				SetFlag				(u32 mask, BOOL flag){if (flag) uFlags|=mask; else uFlags&=~mask; }
	void				InvertFlag			(u32 mask){if (uFlags&mask) uFlags&=~mask; else uFlags|=mask; }
	BOOL				GetFlag				(u32 mask){return uFlags&mask;}
public:
						CUIGameCustom		(CUI* parent){uFlags=0;m_Parent=parent;}
	virtual				~CUIGameCustom		(){}
	virtual void		Render				()=0;
	virtual void		OnFrame				()=0;

	virtual bool		IR_OnKeyboardPress		(int dik)=0;
	virtual bool		IR_OnKeyboardRelease	(int dik)=0;

	//by Dandy 4.07.03
	virtual bool		IR_OnMouseMove			(int dx, int dy) {return false;}
};

//by Dandy
#include "ui.h"

#endif // __XR_UIGAMECUSTOM_H__
