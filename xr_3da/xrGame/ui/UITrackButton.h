#pragma once

#include "UI3tButton.h"

class CUITrackBar;

class CUITrackButton : public CUI3tButton{
public:
	CUITrackButton();
	virtual ~CUITrackButton();

	virtual bool OnMouse(float x, float y, EUIMessages mouse_action);
			void SetOwner(CUITrackBar* owner);


	CUITrackBar*	m_pOwner;
};