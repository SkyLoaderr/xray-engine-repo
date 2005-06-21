// UICheckButton.cpp: класс кнопки, имеющей 2 состояния:
// с галочкой и без
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include ".\uicheckbutton.h"
#include "../HUDManager.h"
#include "../../xr_ioconsole.h"

CUICheckButton::CUICheckButton(void)
{	
	m_lines.SetTextAlignment(CGameFont::alLeft);
}

CUICheckButton::~CUICheckButton(void)
{
}

void CUICheckButton::SetDefaultValue(){
	BOOL val;
	Console->GetBool(m_entry.c_str(), val);
	SetCheck(val ? true : false);
}

void CUICheckButton::SaveValue(){
	char buf[16];
	xr_string command = m_entry;
	command += " ";
	command += itoa(GetCheck(), buf, 10);
	Console->Execute(command.c_str());
}



void CUICheckButton::Init(float x, float y, float width, float height){
	CUIWindow::Init(x,y,width,height);
	InitTexture();
	m_lines.Init(x,y,width,m_background.GetE()->GetStaticItem()->GetRect().height());
}

void CUICheckButton::InitTexture(){
	CUI3tButton::InitTexture("ui_checker");
	Frect r = m_background.GetE()->GetStaticItem()->GetOriginalRect();
	CUI3tButton::SetTextX(r.width());	
}

void CUICheckButton::OnMouse(float x, float y, EUIMessages mouse_action)
{
	CUIWindow::OnMouse(x, y, mouse_action);
}

void CUICheckButton::OnMouseDown(bool left_button){
	if (left_button)
	{
		if (m_eButtonState == BUTTON_NORMAL)
			m_eButtonState = BUTTON_PUSHED;
		else
			m_eButtonState = BUTTON_NORMAL;
	}
}
