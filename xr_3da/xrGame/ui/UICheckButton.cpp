// UICheckButton.cpp: класс кнопки, имеющей 2 состояния:
// с галочкой и без
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include ".\uicheckbutton.h"
#include "../Level.h"
#include "../HUDManager.h"

CUICheckButton::CUICheckButton(void)
{
}

CUICheckButton::~CUICheckButton(void)
{
}

//реакция на мышь
void CUICheckButton::OnMouse(int x, int y, EUIMessages mouse_action)
{
	inherited::OnMouse(x, y, mouse_action);
	
	//если кнопка была только что нажата
	if(m_bButtonClicked)
		if(m_bIsChecked) 
		{
			m_bIsChecked = false;
			GetMessageTarget()->SendMessage(this, CHECK_BUTTON_RESET);
		}
		else
		{
			m_bIsChecked = true;
			GetMessageTarget()->SendMessage(this, CHECK_BUTTON_SET);
		}
}

//прорисовка окна
void CUICheckButton::Draw()
{
	inherited::Draw();

	RECT rect = GetAbsoluteRect();

	//нарисовать галочку
	if(m_bIsChecked)
	{
		GetFont()->SetColor(0xFF00FF00);
		float dy = ((float)rect.bottom-(float)rect.top)/2.0f;
		HUD().OutText(GetFont(), GetClipRect(), (float)rect.left, (float)rect.top+dy,	"X");
	}

	GetFont()->OnRender();
}


void CUICheckButton::Update()
{
	inherited::Update();
}
