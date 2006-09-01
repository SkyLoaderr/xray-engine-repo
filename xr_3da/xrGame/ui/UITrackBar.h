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
	virtual void SetCurrentValue();
	virtual void SaveValue		();
	virtual bool IsChanged		();
	virtual void SeveBackUpValue();
	virtual void Undo			();
	virtual	void OnMessage(const char* message);
	// CUIWindow
	virtual void Init		(float x, float y, float width, float height);
	virtual void Enable		(bool status);
    virtual void Update		();				

protected:
			void UpdatePos();
			void UpdatePosRelativeToMouse();

    CUITrackButton*		m_pSlider;
	CUIFrameLineWnd*	m_pFrameLine;
	CUIFrameLineWnd*	m_pFrameLine_d;
	float				m_val;
	float				m_max;
	float				m_min;
	float				m_bakc_up;
};