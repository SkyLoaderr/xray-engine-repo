//=============================================================================
//  Filename:   UIMultiTextStatic.cpp
//	Created by Roman E. Marchenko, vortex@gsc-game.kiev.ua
//	Copyright 2004. GSC Game World
//	---------------------------------------------------------------------------
//  Статик контрол на котором можно выводить множество надписей различными
//	шрифтами, цветами и даже с анимацией
//=============================================================================

#include "stdafx.h"
#include "UIMultiTextStatic.h"

//////////////////////////////////////////////////////////////////////////

CUIMultiTextStatic::SinglePhrase * CUIMultiTextStatic::AddPhrase()
{
	m_vPhrases.resize(m_vPhrases.size() + 1);
	m_vPhrases.back().effect.SetNewRenderMethod(true);
	return &m_vPhrases.back();
}

//////////////////////////////////////////////////////////////////////////

void CUIMultiTextStatic::Draw()
{
	inherited::Draw();

	RECT r = GetAbsoluteRect();

	for (Phrases_it it = m_vPhrases.begin(); it != m_vPhrases.end(); ++it)
	{
		it->effect.Out(it->outX + r.left, it->outY + r.top, *it->str);
	}
}

//////////////////////////////////////////////////////////////////////////

void CUIMultiTextStatic::Update()
{
	inherited::Update();

	for (Phrases_it it = m_vPhrases.begin(); it != m_vPhrases.end(); ++it)
	{
		it->effect.Update();
	}
}

//////////////////////////////////////////////////////////////////////////

CUIMultiTextStatic::SinglePhrase * CUIMultiTextStatic::GetPhraseByIndex(u32 idx)
{
	R_ASSERT(idx < m_vPhrases.size());

	return &m_vPhrases[idx];
}

//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------/
//  SinglePhrase struct
//-----------------------------------------------------------------------------/

//////////////////////////////////////////////////////////////////////////

CUIMultiTextStatic::SPh::SPh()
{
	effect.SetStyleParams(CUITextBanner::tbsNone);
	effect.StopAnimation();
}

//////////////////////////////////////////////////////////////////////////

void CUIMultiTextStatic::SPh::SetText(const char *fmt, ...)
{
	va_list		Print;                                                                  
	string256	msg;
	std::string	buf;

	va_start(Print, fmt);
		vsprintf(msg, fmt, Print);
		buf += msg;
		msg[0] = '\n';
	va_end(Print);

	str = buf.c_str();
}