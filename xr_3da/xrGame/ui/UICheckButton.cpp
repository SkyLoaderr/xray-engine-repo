// UICheckButton.cpp: ����� ������, ������� 2 ���������:
// � �������� � ���
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

//������� �� ����
void CUICheckButton::OnMouse(int x, int y, EUIMessages mouse_action)
{
	inherited::OnMouse(x, y, mouse_action);
	
	//���� ������ ���� ������ ��� ������
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

//���������� ����
void CUICheckButton::Draw()
{
	inherited::Draw();

	RECT rect = GetAbsoluteRect();

	//���������� �������
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
