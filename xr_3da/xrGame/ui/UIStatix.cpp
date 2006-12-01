#include "stdafx.h"

#include "UIStatix.h"

CUIStatix::CUIStatix(){
	m_bSelected		= false;
}

CUIStatix::~CUIStatix()
{}

void CUIStatix::start_anim()
{
	SetLightAnim	("ui_slow_blinking", true, true, true, true);
	ResetAnimation	();
}

void CUIStatix::stop_anim()
{
	SetLightAnim	(NULL, true, true, true, true);
}

void CUIStatix::Update(){
	if (m_bCursorOverWindow)
	{
		SetColor(0xff349F06);
	}

	CUIStatic::Update();
}

void CUIStatix::OnFocusLost()
{
	CUIStatic::OnFocusLost	();
	SetColor				(0xffffffff);
}

void CUIStatix::OnFocusReceive()
{
	CUIStatic::OnFocusReceive	();
	ResetAnimation				();
}

bool CUIStatix::OnMouseDown(bool left_button)
{
	GetMessageTarget()->SendMessage(this, BUTTON_CLICKED);
	return true;
}

void CUIStatix::SetSelectedState(bool state)
{
	bool b		= m_bSelected;
	m_bSelected = state;

	if(	b==m_bSelected )		return;
	
	if (!state)
		OnFocusLost();

	if(state)
		start_anim();
	else
		stop_anim();
}

bool CUIStatix::GetSelectedState(){
	return m_bSelected;
}
