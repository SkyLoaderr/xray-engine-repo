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

void CUIIconedListItem::SetIcon(ref_str textureName, u32 uIconSize)
{
	RECT r		= GetAbsoluteRect();
	m_uIconSize	= uIconSize;
	int iconTop	= (r.bottom - r.top) / 2 + r.top - m_uIconSize / 2;
	m_Icon.Init(*textureName, "hud\\default", GetAbsoluteRect().left, iconTop, alNone);
	m_bTexturePresent = true;
}

//////////////////////////////////////////////////////////////////////////

void CUIIconedListItem::Draw()
{
//	RECT r = GetAbsoluteRect();

	if (m_bTexturePresent) 
		m_Icon.Render();
	inherited::Draw();
}