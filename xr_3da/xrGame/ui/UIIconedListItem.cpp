//=============================================================================
//  Filename:   UIIconedListItem.cpp
//	Created by Roman E. Marchenko, vortex@gsc-game.kiev.ua
//	Copyright 2004. GSC Game World
//	---------------------------------------------------------------------------
//  Ћист айтем с иконкой в начале первой строки текста
//=============================================================================

#include "stdafx.h"
#include "UIIconedListItem.h"

//////////////////////////////////////////////////////////////////////////

void CUIIconedListItem::SetIcon(shared_str textureName, u32 uIconSize)
{
	RECT r		= GetAbsoluteRect();
	m_uIconSize	= uIconSize;
	int iconTop	= (r.bottom - r.top) / 2 + r.top - m_uIconSize / 2;
	m_Icon.Init(*textureName, "hud\\default", GetAbsoluteRect().left, iconTop, alNone);
	m_bTexturePresent = true;
	m_OldAbsoluteWndRect = r;
}

//////////////////////////////////////////////////////////////////////////

void CUIIconedListItem::Draw()
{
//	RECT r = GetAbsoluteRect();

	if (m_bTexturePresent && m_bIconDraw) 
		m_Icon.Render();
	inherited::Draw();
}

//////////////////////////////////////////////////////////////////////////

void CUIIconedListItem::SetIconOffset(const int oX, const int oY)
{
	m_Icon.SetPos(m_Icon.GetPosX() + oX, m_Icon.GetPosY() + oY);
}

//////////////////////////////////////////////////////////////////////////

void CUIIconedListItem::Show(bool status)
{
	inherited::Show(status);
	m_bIconDraw = status;
}

//////////////////////////////////////////////////////////////////////////

void CUIIconedListItem::Update()
{
	RECT r = GetAbsoluteRect();

	if (m_OldAbsoluteWndRect.left	!= r.left		||
		m_OldAbsoluteWndRect.top	!= r.top		||
		m_OldAbsoluteWndRect.right	!= r.right		||
		m_OldAbsoluteWndRect.bottom	!= r.bottom)
	{
		m_Icon.SetPos(m_Icon.GetPosX() + r.left - m_OldAbsoluteWndRect.left,
					  m_Icon.GetPosY() + r.top - m_OldAbsoluteWndRect.top);
		m_OldAbsoluteWndRect = r;
	}
}
