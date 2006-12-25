#pragma once

#include "UIWindow.h"
#include "UIOptionsItem.h"

class CUI3tButton;
class CUIFrameLineWnd;
class CUITrackButton;

class CUITrackBar : public CUIWindow, public CUIOptionsItem 
{
	friend class CUITrackButton;
public:
					CUITrackBar				();
	// CUIOptionsItem
	virtual void 	SetCurrentValue			();
	virtual void 	SaveValue				();
	virtual bool 	IsChanged				();
	virtual void 	SeveBackUpValue			();
	virtual void 	Undo					();
	virtual void	Draw					();
	virtual bool	OnMouse						(float x, float y, EUIMessages mouse_action);
	virtual	void 	OnMessage				(const char* message);
	// CUIWindow
	virtual void	Init					(float x, float y, float width, float height);
	virtual void	Enable					(bool status);
			void	SetInvert				(bool v){m_b_invert=v;}
			bool	GetInvert				() const	{return m_b_invert;};
			void	SetStep					(float step){m_step=step;}
protected:
			void 	UpdatePos				();
			void 	UpdatePosRelativeToMouse();

    CUI3tButton*		m_pSlider;
	CUIFrameLineWnd*	m_pFrameLine;
	CUIFrameLineWnd*	m_pFrameLine_d;
	float				m_val;
	float				m_max;
	float				m_min;
	float				m_step;
	float				m_back_up;
	bool				m_b_invert;
};