#pragma once

#include "UIWindow.h"
#include "UIWndCallback.h"

class CUIScrollBar;

class CUIScrollView :public CUIWindow, public CUIWndCallback
{
typedef CUIWindow	inherited;
friend class CUIXmlInit; //for init
enum {eVertFlip=(1<<0),eNeedRecalc=(1<<1),};
protected:
	CUIScrollBar*	m_VScrollBar;
	CUIWindow*		m_pad;
	float			m_rightIdent;
	Flags16			m_flags;
	void			RecalcSize			();
	void			UpdateScroll		();
	void			Init				();// need parent to be initialized
	void			OnScrollV			();
	void			SetRightIndention	(float val);
public:
					CUIScrollView		();
	virtual			~CUIScrollView		();
	virtual void	SendMessage			(CUIWindow* pWnd, s16 msg, void* pData = NULL);
	virtual bool	OnMouse				(float x, float y, EUIMessages mouse_action);
	virtual void	Draw				();
	virtual void	Update				();
			void	AddWindow			(CUIWindow* pWnd);
			void	RemoveWindow		(CUIWindow* pWnd);
			void	Clear				();
			void	ScrollToBegin		();
			void	SetVertFlip			(bool val)							{m_flags.set(eVertFlip, val);}
			bool	GetVertFlip			()									{return !!m_flags.test(eVertFlip);}
			u32		GetSize				();
	CUIWindow*		GetItem				(u32 idx);
};