//////////////////////////////////////////////////////////////////////
// UIRadioButton.cpp: класс кнопки, имеющей 2 состояния
// и работающей в группе с такими же кнопками
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include ".\uiradiobutton.h"
#include "../Level.h"
#include "../HUDManager.h"

CUIRadioButton::CUIRadioButton(void)
{
}

CUIRadioButton::~CUIRadioButton(void)
{
}

//реакция на мышь
void CUIRadioButton::OnMouse(int x, int y, EUIMessages mouse_action)
{
	CUIButton::OnMouse(x, y, mouse_action);
	
	//если кнопка была только что нажата
	if(m_bButtonClicked)
		if(!m_bIsChecked) 
		{
			GetMessageTarget()->SendMessage(this, RADIOBUTTON_SET);
			GetParent()->SendMessage(this, RADIOBUTTON_SET);
		}
}

#define RADIO_WIDTH 20

//прорисовка окна
void CUIRadioButton::Draw()
{
	CUIButton::Draw();

	RECT rect = GetAbsoluteRect();

	//нарисовать галочку
	if(m_bIsChecked)
	{
		GetFont()->SetColor(0xFF00FF00);
		float dy = ((float)rect.bottom-(float)rect.top)/2.0f;
		HUD().OutText(GetFont(), GetClipRect(), (float)rect.left, (float)rect.top+dy,	"O");
	}

	GetFont()->OnRender();
}


void CUIRadioButton::Update()
{
	CUIButton::Update();
}