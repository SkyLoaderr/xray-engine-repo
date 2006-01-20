#pragma once

#include "UIWindow.h"
#include "UIWndCallback.h"

class CUIScrollBar;

class CUIScrollView :public CUIWindow, public CUIWndCallback
{
typedef CUIWindow	inherited;
friend class CUIXmlInit; //for init
protected:
enum {eVertFlip=(1<<0),eNeedRecalc=(1<<1),eFixedScrollBar=(1<<2),eItemsSelectabe=(1<<3),eInverseDir=(1<<4)};
	CUIScrollBar*	m_VScrollBar;
	CUIWindow*		m_pad;
	float			m_rightIdent;
	float			m_leftIdent;
	float			m_vertInterval;
	Flags16			m_flags;

virtual void		RecalcSize			();
		void		UpdateScroll		();	
		void		OnScrollV			();
		void		SetRightIndention	(float val);
		void		SetLeftIndention	(float val);
		void		SetVertFlip			(bool val)							{m_flags.set(eVertFlip, val);}
public:
	using CUIWindow::Init;
			
					CUIScrollView		();
	virtual			~CUIScrollView		();
			void	Init				();// need parent to be initialized
	virtual void	SendMessage			(CUIWindow* pWnd, s16 msg, void* pData = NULL);
	virtual bool	OnMouse				(float x, float y, EUIMessages mouse_action);
	virtual void	Draw				();
	virtual void	Update				();
			void	AddWindow			(CUIWindow* pWnd, bool auto_delete);
			void	RemoveWindow		(CUIWindow* pWnd);
			void	Clear				();
			void	ScrollToBegin		();
			void	ScrollToEnd			();
			bool	GetVertFlip			()									{return !!m_flags.test(eVertFlip);}
			u32		GetSize				();
	CUIWindow*		GetItem				(u32 idx);
			void	SetFixedScrollBar	(bool b);
			float	GetDesiredChildWidth();
			void	SetSelected			(CUIWindow*);
};

#define ADD_TEXT_TO_VIEW3(txt,st,view)		st = xr_new<CUIStatic>();						\
											st->SetText(txt);								\
											st->SetWidth(view->GetDesiredChildWidth());		\
											st->AdjustHeightToText();						\
											view->AddWindow(st, true)

#define ADD_TEXT_TO_VIEW2(txt,view)			CUIStatic*	pSt;								\
											ADD_TEXT_TO_VIEW3(txt,pSt,view)