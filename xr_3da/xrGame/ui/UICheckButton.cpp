// UICheckButton.cpp: класс кнопки, имеющей 2 состояния:
// с галочкой и без
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include ".\uicheckbutton.h"

CUICheckButton::CUICheckButton(void)
{
}

CUICheckButton::~CUICheckButton(void)
{
}

//реакция на мышь
void CUICheckButton::OnMouse(int x, int y, E_MOUSEACTION mouse_action)
{
	CUIButton::OnMouse(x, y, mouse_action);
	
	//если кнопка была только что нажата
	if(m_bButtonClicked)
		if(m_bIsChecked) 
		{
			m_bIsChecked = false;
			GetParent()->SendMessage(this, CHECK_BUTTON_RESET);
		}
		else
		{
			m_bIsChecked = true;
			GetParent()->SendMessage(this, CHECK_BUTTON_SET);
		}
}

//прорисовка окна
void CUICheckButton::Draw()
{
	CUIButton::Draw();
}


void CUICheckButton::Update()
{
	RECT rect = GetAbsoluteRect();

	//нарисовать галочку
	if(m_bIsChecked)
	{
		GetFont()->SetColor(0xFF00FF00);
		GetFont()->Out((float)rect.left, (float)rect.top+30,	"X");
	}

	CUIButton::Update();
}
