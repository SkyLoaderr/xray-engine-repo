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
	m_vPhrases.back().effect.SetNewRenderMethod(true);
	return &m_vPhrases.back();
}

//////////////////////////////////////////////////////////////////////////

void CUIMultiTextStatic::Draw()
{
	RECT r = GetAbsoluteRect();

	for (Phrases_it it = m_vPhrases.begin(); it != m_vPhrases.end(); ++it)
	{
		it->effect.Out(it->outX + r.left, it->outY + r.top, *it->str);
	}

	inherited::Draw();
}

//////////////////////////////////////////////////////////////////////////

void CUIMultiTextStatic::Update()
{
	for (Phrases_it it = m_vPhrases.begin(); it != m_vPhrases.end(); ++it)
	{
		it->effect.Update();
	}

	inherited::Update();
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
	:	elipsisPos			(CUIStatic::eepEnd),
		maxWidth			(-1)
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

	if (maxWidth > 0 && elipsisPos != CUIStatic::eepNone)
	{
		STRING s;
		CUIStatic::SetText(*str, s);

		RECT r;
		r.left		= 0;
		r.right		= maxWidth;
		r.top		= 0;
		r.bottom	= 0;

		CUIStatic::Elipsis(s, r, elipsisPos, effect.GetFont());

		str = &s.front();
	}
}


void CUICaption::addCustomMessage(const ref_str& msg_name, float x, float y, float font_size, 
								  CGameFont *pFont, CGameFont::EAligment al, u32 color, LPCSTR def_str)
{
	R_ASSERT2( (m_indices.find(msg_name) == m_indices.end()),"message already defined !!!" );

	SinglePhrase * sp = AddPhrase();
	sp->outX = x;
	sp->outY = y;
	sp->effect.SetFontSize(font_size);
	sp->effect.SetFont(pFont);
	sp->effect.SetTextColor(color);
	sp->effect.SetFontAlignment(al);
	sp->SetText(def_str);

	m_indices[msg_name] = m_vPhrases.size()-1;

}

EffectParams* CUICaption::customizeMessage(const ref_str& msg_name, const CUITextBanner::TextBannerStyles styleName)
{
	R_ASSERT2( (m_indices.find(msg_name) != m_indices.end()),"message not defined !!!" );
	
	SinglePhrase * sp = GetPhraseByIndex( m_indices[msg_name] );
	sp->effect.PlayAnimation();
	return sp->effect.SetStyleParams(styleName);
	
}

void CUICaption::setCaption(const ref_str& msg_name, LPCSTR message_to_out, u32 color, bool replaceColor)
{
	R_ASSERT2( (m_indices.find(msg_name) != m_indices.end()),"message not defined !!!" );
	SinglePhrase * sp = GetPhraseByIndex(m_indices[msg_name]);
	sp->str = message_to_out;
	if(replaceColor)
		sp->effect.SetTextColor(color);
}

void CUICaption::Draw()
{
	inherited::Draw();
	inherited::Update();
}
