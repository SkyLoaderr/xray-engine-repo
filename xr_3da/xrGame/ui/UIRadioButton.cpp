//////////////////////////////////////////////////////////////////////
// UIRadioButton.cpp: ����� ������, ������� 2 ���������
// � ���������� � ������ � ������ �� ��������
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include ".\uiradiobutton.h"

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
}


void CUIRadioButton::Update()
{
	RECT rect = GetAbsoluteRect();

	//���������� �������
	if(m_bIsChecked)
	{
		GetFont()->SetColor(0xFF00FF00);
		GetFont()->Out((float)rect.left, (float)rect.top+30,	"O");
	}

	CUIButton::Update();
}