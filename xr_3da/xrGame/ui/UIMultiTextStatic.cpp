//=============================================================================
//  Filename:   UIMultiTextStatic.cpp
//	Created by Roman E. Marchenko, vortex@gsc-game.kiev.ua
//	Copyright 2004. GSC Game World
//	---------------------------------------------------------------------------
//  ������ ������� �� ������� ����� �������� ��������� �������� ����������
//	��������, ������� � ���� � ���������
//=============================================================================

#include "stdafx.h"
#include "UIMultiTextStatic.h"

//////////////////////////////////////////////////////////////////////////

CUIMultiTextStatic::SinglePhrase * CUIMultiTextStatic::AddPhrase()
{
	m_vPhrases.resize(m_vPhrases.size() + 1);
	return &m_vPhrases.back();
}

//////////////////////////////////////////////////////////////////////////

void CUIMultiTextStatic::Draw()
{
	inherited::Draw();

	for (Phrases_it it = m_vPhrases.begin(); it != m_vPhrases.end(); ++it)
	{
		it->effect.Out(it->outX, it->outY, *it->str);
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

//-----------------------------------------------------------------------------/
//  SinglePhrase struct
//-----------------------------------------------------------------------------/

//////////////////////////////////////////////////////////////////////////

CUIMultiTextStatic::SPh::SPh()
{
	effect.SetStyleParams(CUITextBanner::tbsNone, 0.0f);
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