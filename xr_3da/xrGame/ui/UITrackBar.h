//
//

#pragma once

#include "UIWindow.h"
#include "UIOptionsItem.h"

class CUI3tButton;
class CUIFrameLineWnd;
class CUITrackButton;

class CUITrackBar : public CUIWindow, public CUIOptionsItem {
	friend class CUITrackButton;
public:
	CUITrackBar();
	~CUITrackBar();
	// CUIOptionsItem
	virtual void	SetCurrentValue();
	virtual void	SaveValue();
	// CUIWindow
	virtual void Init(float x, float y, float width, float height);
//	virtual bool OnMouse(float x, float y, EUIMessages mouse_action);
	virtual void Enable(bool status);
	//virtual void OnMouseDown(bool left_button = true);
    virtual void Update();			

protected:
			void UpdatePos();
			void UpdatePosRelativeToMouse();

    CUITrackButton*		m_pSlider;
	CUIFrameLineWnd*	m_pFrameLine;
	CUIFrameLineWnd*	m_pFrameLine_d;
	float				m_val;
	float				m_max;
	float				m_min;
};