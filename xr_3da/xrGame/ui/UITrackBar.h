//
//

#pragma once

#include "UIWindow.h"
#include "UIOptionsItem.h"

class CUI3tButton;
class CUIFrameLineWnd;

class CUITrackBar : public CUIWindow, public CUIOptionsItem {
public:
	CUITrackBar();
	~CUITrackBar();
	// CUIOptionsItem
	virtual void	SetCurrentValue();
	virtual void	SaveValue();
	// CUIWindow
	virtual void Init(float x, float y, float width, float height);
	virtual void OnMouse(float x, float y, EUIMessages mouse_action);
	virtual void Enable(bool status);
	virtual void OnMouseDown(bool left_button = true);
	

			

protected:
			void UpdatePos();

    CUI3tButton*		m_pSlider;
	CUIFrameLineWnd*	m_pFrameLine;
	CUIFrameLineWnd*	m_pFrameLine_d;
	float				m_val;
	float				m_max;
	float				m_min;
};