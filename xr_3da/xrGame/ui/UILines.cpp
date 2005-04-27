// File:		UILines.cpp
// Description:	Multilines Text Control
// Created:		12.03.2005
// Author:		Serge Vynnycheko
// Mail:		narrator@gsc-game.kiev.ua
//
// Copyright 2005 GSC Game World

#include "StdAfx.h"

#include "UILines.h"
#include "../HUDmanager.h"
#include "UIXmlInit.h"
#include "uilinestd.h"

CUILines::CUILines()
{
	m_pFont = UI()->Font()->pFontLetterica18Russian;
	m_interval = 0.3f;
	m_eTextAlign = CGameFont::alLeft;
	m_dwTextColor = 0xffffffff;

	m_bShowMe = true;
	m_wndPos.x = 0;
	m_wndPos.y = 0;
	
}

CUILines::~CUILines(){

}

void CUILines::SetWndRect(const Irect& rect){
	m_wndPos.x = rect.x1;
	m_wndPos.y = rect.y1;
	m_wndSize.x = rect.x2 - rect.x1;
	m_wndSize.y = rect.y2 - rect.y1;
}

Irect CUILines::GetWndRect() const {
	Irect r;

	r.x1 = m_wndPos.x;
	r.y1 = m_wndPos.y;
	r.x2 = m_wndPos.x + m_wndSize.x;
	r.y2 = m_wndPos.y + m_wndSize.y;

	return r;
}

void CUILines::SetText(const char* text){
	m_text = text;
	ParseText();
}

const char* CUILines::GetText() const{
	return m_text.c_str();
}

void CUILines::Reset(){
	m_lines.clear();
}

void CUILines::ParseText(){
	Reset();
	CUILine* line = ParseTextToColoredLine(m_text.c_str());

	while (line->GetLength(m_pFont) > 0)
		m_lines.push_back(*line->CutByLength(m_pFont, m_wndSize.x));

	xr_delete(line);
}

int CUILines::GetVisibleHeight() const{
	int interval = int(m_interval*m_pFont->CurrentHeightRel());
	return ((int)m_pFont->CurrentHeightRel() + interval)*m_lines.size() - interval;
}

void CUILines::Draw() const{
#ifdef DEBUG
	R_ASSERT(m_pFont);
#endif //DEBUG

	m_pFont->SetColor(m_dwTextColor);
	m_pFont->SetAligment(m_eTextAlign);

	Ivector2 pos;
	pos.y= m_wndPos.y + (m_wndSize.y - GetVisibleHeight())/2;
	int height = (int)m_pFont->CurrentHeightRel();
	int size = m_lines.size();

	int interval = int(m_interval*m_pFont->CurrentHeightRel());

	for (int i=0; i<size; i++)
	{
		pos.x = m_wndPos.x + GetIndentByAlign(m_lines[i].GetLength(m_pFont));
		m_lines[i].Draw(m_pFont, pos.x, pos.y);
		pos.y+= height + interval;
	}	
}

void CUILines::Update(){
}

void CUILines::OnDeviceReset(){
	ParseText();
}

u32 CUILines::GetIndentByAlign(u32 length)const{
	switch (m_eTextAlign)
	{
	case CGameFont::alCenter:
		return (m_wndSize.x - length)/2;
	case CGameFont::alLeft:
		return 0;
	case CGameFont::alRight:
		return (m_wndSize.x - length);
	default:
			NODEFAULT;
	}
	return 0;
}

// %c<255,255,255,255>
u32 CUILines::GetColorFromText(const xr_string& str)const{
//	typedef xr_string::size_type size;

	StrSize begin, end, comma1_pos, comma2_pos, comma3_pos;

	begin = str.find(BEGIN);
	end = str.find(END, begin);
	R_ASSERT2(npos != begin, "CUISubLine::GetColorFromText -- can't find beginning tag %c<");
	R_ASSERT2(npos != end, "CUISubLine::GetColorFromText -- can't find ending tag >");
	
	// try default color
	if (npos != str.find("%c<default>", begin, end - begin))
		return m_dwTextColor;

	// Try predefined in XML colors
	CUIXmlInit xml;
	for (CUIXmlInit::ColorDefs::const_iterator it = CUIXmlInit::GetColorDefs()->begin(); it != CUIXmlInit::GetColorDefs()->end(); ++it)
	{
		int cmp = str.compare(begin+3, end-begin-3, *it->first);			
		if (cmp == 0)
			return it->second;
	}

	// try parse values separated by commas
	comma1_pos = str.find(",", begin);
	comma2_pos = str.find(",", comma1_pos + 1);
	comma3_pos = str.find(",", comma2_pos + 1);

    R_ASSERT2(npos != comma1_pos, "CUISubLine::GetColorFromText -- can't find first comma");        
	R_ASSERT2(npos != comma2_pos, "CUISubLine::GetColorFromText -- can't find second comma");
	R_ASSERT2(npos != comma3_pos, "CUISubLine::GetColorFromText -- can't find third comma");
	

	u32 a, r, g, b;
	xr_string single_color;

	begin+=3;

	single_color = str.substr(begin, comma1_pos - 1);
	a = atoi(single_color.c_str());
	single_color = str.substr(comma1_pos + 1, comma2_pos - 1);
	r = atoi(single_color.c_str());
	single_color = str.substr(comma2_pos + 1, comma3_pos - 1);
	g = atoi(single_color.c_str());
	single_color = str.substr(comma3_pos + 1, end - 1);
	b = atoi(single_color.c_str());

    return color_argb(a,r,g,b);
}

CUILine* CUILines::ParseTextToColoredLine(const xr_string& str) const{
	CUILine* line = xr_new<CUILine>();
	xr_string tmp = str;
	xr_string entry;
	u32 color;

	do 
	{
		CutFirstColoredTextEntry(entry, color, tmp);
		line->AddSubLine(entry, color);
	} 
	while (tmp.size()>0);

	return line;
}

void CUILines::CutFirstColoredTextEntry(xr_string& entry, u32& color, xr_string& text) const {
	entry.clear();
	
	StrSize begin	= text.find(BEGIN);
	StrSize end	= text.find(END, begin);
	if (xr_string::npos == end)
		begin = end;
	StrSize begin2	= text.find(BEGIN, end);
	StrSize end2	= text.find(END,begin2);
	if (xr_string::npos == end2)
		begin2 = end2;

	// if we do not have any color entry or it is single with 0 position
	if (xr_string::npos == begin)
	{
		entry = text;
		color = m_dwTextColor;
		text.clear();
	}
	else if (0 == begin && xr_string::npos == begin2)
	{
		entry = text;
		color = GetColorFromText(entry);
		entry.replace(begin, end - begin + 1, "");
		text.clear();
	}
	// if we have color entry not at begin
	else if (0 != begin)
	{
		entry = text.substr(0, begin );
		color = m_dwTextColor;
		text.replace(0, begin, "");
	}
	// if we have two color entries. and first has 0 position
	else if (0 == begin && xr_string::npos != begin2)
	{
		entry = text.substr(0, begin2);
		color = GetColorFromText(entry);
		entry.replace(begin, end - begin + 1, "");
		text.replace(0, begin2, "");
	}
}
