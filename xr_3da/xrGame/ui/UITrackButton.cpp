#include "stdafx.h"
#include "UITrackButton.h"
#include "UITrackBar.h"
#include "../../xr_input.h"

CUITrackButton::CUITrackButton(){
}

CUITrackButton::~CUITrackButton(){
}

bool CUITrackButton::OnMouse(float x, float y, EUIMessages mouse_action){
	CUI3tButton::OnMouse(x,y,mouse_action);

	if (m_bCursorOverWindow)
	{
		if (pInput->iGetAsyncBtnState(0))
			m_pOwner->UpdatePosRelativeToMouse();
	}

	return false;
}

void CUITrackButton::SetOwner(CUITrackBar* owner){
	m_pOwner = owner;
}