#pragma once

#include "UIWindow.h"
#include "UIWndCallback.h"

class CUIScrollBar;

class CUIScrollView :public CUIWindow, public CUIWndCallback
{
typedef CUIWindow	inherited;
friend class CUIXmlInit; //for init

protected:
	CUIScrollBar*	m_VScrollBar;
	CUIWindow*		m_pad;
	float			m_rightIdent;
	void			RecalcSize			();
	void			UpdateScroll		();
	void			Init				();// need parent to be initialized
	void			OnScrollV			();
	void			SetRightIndention	(float val);
public:
					CUIScrollView		();
	virtual			~CUIScrollView		();
	virtual void	SendMessage			(CUIWindow* pWnd, s16 msg, void* pData = NULL);
	virtual void	OnMouse				(float x, float y, EUIMessages mouse_action);
	virtual void	Draw				();
			void	AddWindow			(CUIWindow* pWnd);
			void	RemoveWindow		(CUIWindow* pWnd);
			void	Clear				();
			void	ScrollToBegin		();
};
