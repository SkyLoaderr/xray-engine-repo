#pragma once
#include "UIWindow.h"
#include "UIWndCallback.h"

class CUITabControl;
class CUIStatic;
class CUIXml;

class CUIVideoPlayerWnd :public CUIWindow, public CUIWndCallback
{
	typedef CUIWindow inherited;
	CUITabControl*	m_tabControl;
	CUIStatic*		m_surface;
	Flags8			m_flags;
	enum			{eAutoPlay =(1<<0),ePlaying=(1<<1),};
	ref_texture		m_texture;
	ref_sound		m_sound;
private:
	void			OnBtnPlayClicked		();
	void			OnBtnPauseClicked		();
	void			OnTabChanged			(CUIWindow* pWnd, void* pData);
public:
	void			Init					(CUIXml* doc, LPCSTR start_from);
	virtual void	SendMessage				(CUIWindow* pWnd, s16 msg, void* pData);
	void			SetFile					(LPCSTR fn);

	virtual void	Draw					();
	virtual void	Update					();
			void	Play					();
			void	Stop					();
};
