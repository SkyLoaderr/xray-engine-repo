// File:		UITabButtonMP.cpp
// Description:	Tab Button designed special for TabControl in CUIBuyWeaponWnd
// Created:		17.02.2005
// Author:		Serge Vynnychenko
// Mail:		narrator@gsc-game.kiev.ua

// Copyright 2005 GSC Game World

#include "StdAfx.h"
#include "UITabButtonMP.h"
#include "../HUDManager.h"

const int TEXT_INDENTION = 43;

CUITabButtonMP::CUITabButtonMP()
{
	m_orientationVertical	= true;
	m_temp_index			= 0;
}


void CUITabButtonMP::SendMessage(CUIWindow* pWnd, s16 msg, void* pData)
{
	if (this == pWnd)
		m_bIsEnabled = true;

	CUITabButton::SendMessage(pWnd, msg, pData);
}

void CUITabButtonMP::UpdateTextAlign()
{
	if(m_orientationVertical)
	{
        if (CUIButton::BUTTON_PUSHED == m_eButtonState)
            m_TextOffset.y =  - TEXT_INDENTION;
		else
			m_TextOffset.y = 0;
	}
	else
	{
	if (m_bCursorOverWindow)
		m_TextOffset.x = 100;
	else
		m_TextOffset.x = 0;
	}

}


void CUITabButtonMP::Update()
{

	// cunning code : use Highlighting even if is disabled

	bool tempEnabled		= m_bIsEnabled;
	m_bIsEnabled			= m_bCursorOverWindow ? true : m_bIsEnabled;
	inherited::Update		();
	m_bIsEnabled			= tempEnabled;
	UpdateTextAlign			();
}

void CUITabButtonMP::Draw()
{
	UI()->PushScissor(UI()->ScreenRect(),true);

	CUITabButton::Draw();

	UI()->PopScissor();
}







