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
		flShowBuyMenu	= (1<<0),
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

	virtual bool		OnKeyboardPress		(int dik)=0;
	virtual bool		OnKeyboardRelease	(int dik)=0;
};
#endif // __XR_UIGAMECUSTOM_H__
