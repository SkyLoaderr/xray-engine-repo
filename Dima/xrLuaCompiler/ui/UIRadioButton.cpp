//////////////////////////////////////////////////////////////////////
// UIRadioButton.cpp: класс кнопки, имеющей 2 состояния
// и работающей в группе с такими же кнопками
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include ".\uiradiobutton.h"

CUIRadioButton::CUIRadioButton(void)
{
}

CUIRadioButton::~CUIRadioButton(void)
{
}

//реакция на мышь
void CUIRadioButton::OnMouse(int x, int y, E_MOUSEACTION mouse_action)
{
	CUIButton::OnMouse(x, y, mouse_action);
	
	//если кнопка была только что нажата
	if(m_bButtonClicked)
		if(!m_bIsChecked) 
		{
			GetParent()->SendMessage(this, CUIRadioButton::RADIO_BUTTON_SET);
		}
}

#define RADIO_WIDTH 20

//прорисовка окна
void CUIRadioButton::Draw()
{
	CUIButton::Draw();
}


void CUIRadioButton::Update()
{
	RECT rect = GetAbsoluteRect();

	//нарисовать галочку
	if(m_bIsChecked)
	{
		GetFont()->SetColor(0xFF00FF00);
		GetFont()->Out((float)rect.left, (float)rect.top+30,	"O");
	}

	CUIButton::Update();
}