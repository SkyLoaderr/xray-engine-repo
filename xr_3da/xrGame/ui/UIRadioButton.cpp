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
void CUIRadioButton::OnMouse(int x, int y, EUIMessages mouse_action)
{
	CUIButton::OnMouse(x, y, mouse_action);
	
	//���� ������ ���� ������ ��� ������
	if(m_bButtonClicked)
		if(!m_bIsChecked) 
		{
			GetMessageTarget()->SendMessage(this, RADIOBUTTON_SET);
			GetParent()->SendMessage(this, RADIOBUTTON_SET);
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
		float dy = ((float)rect.bottom-(float)rect.top)/2.0f;
		UI()->OutText(GetFont(), GetClipRect(), (float)rect.left, (float)rect.top+dy,	"O");
	}

	GetFont()->OnRender();
}


void CUIRadioButton::Update()
{
	CUIButton::Update();
}