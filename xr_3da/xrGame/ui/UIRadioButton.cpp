//////////////////////////////////////////////////////////////////////
// UIRadioButton.cpp: ����� ������, ������� 2 ���������
// � ���������� � ������ � ������ �� ��������
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

//������� �� ����
void CUIRadioButton::OnMouse(int x, int y, E_MOUSEACTION mouse_action)
{
	CUIButton::OnMouse(x, y, mouse_action);
	
	//���� ������ ���� ������ ��� ������
	if(m_bButtonClicked)
		if(!m_bIsChecked) 
		{
			GetMessageTarget()->SendMessage(this, CUIRadioButton::RADIO_BUTTON_SET);
		}
}

#define RADIO_WIDTH 20

//���������� ����
void CUIRadioButton::Draw()
{
	CUIButton::Draw();

	RECT rect = GetAbsoluteRect();

	//���������� �������
	if(m_bIsChecked)
	{
		GetFont()->SetColor(0xFF00FF00);
		HUD().OutText(GetFont(), GetClipRect(), (float)rect.left, (float)rect.top+30,	"O");
	}

	GetFont()->OnRender();
}


void CUIRadioButton::Update()
{
	CUIButton::Update();
}