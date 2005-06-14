// File:		UILines.cpp
// Description:	Multi-line Text Control
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
	m_pFont = NULL;
	m_interval = 0.3f;
	m_eTextAlign = CGameFont::alLeft;
	m_eVTextAlign = valTop;
	m_dwTextColor = 0xffffffff;

	m_bShowMe = true;
	m_wndPos.x = 0;
	m_wndPos.y = 0;
	uFlags.set(flNeedReparse, false);
}

CUILines::~CUILines(){

}

void CUILines::Init(float x, float y, float width, float heigt){	
	CUISimpleWindow::Init(x, y, width, heigt);
}

void CUILines::SetText(const char* text){
	if (!m_pFont)
		SetFont(UI()->Font()->pFontLetterica18Russian);

	if (text)
	{
        m_text = text;
		uFlags.set(flNeedReparse, true);
	}
	else
		Reset();
}

void CUILines::AddChar(const char ch){
	m_text.push_back(ch);
	uFlags.set(flNeedReparse, true);
}

void CUILines::AddChar(const char ch, int pos){
	m_text.insert(m_text.begin()+pos, ch);
	uFlags.set(flNeedReparse, true);
}

void CUILines::DelChar(int i){
	m_text.erase(i);
	uFlags.set(flNeedReparse, true);
}

const char* CUILines::GetText() const{
	return m_text.c_str();
}

void CUILines::Reset(){
	m_lines.clear();
}

void CUILines::ParseText(){
	VERIFY(g_bRendering);
	Reset();

	if (m_text.length() && NULL == m_pFont)
		R_ASSERT2(false, "can't parse text without font");
		
	if(NULL == m_pFont)
		return;

	CUILine* line = ParseTextToColoredLine(m_text.c_str());

	while (line->GetLength(m_pFont) > 0)
		m_lines.push_back(*line->CutByLength(m_pFont, m_wndSize.x));

	xr_delete(line);
	uFlags.set(flNeedReparse, false);
}

float CUILines::GetVisibleHeight() const{
	float interval = m_interval*m_pFont->CurrentHeightRel();
	return (m_pFont->CurrentHeightRel() + interval)*m_lines.size() - interval;
}

void CUILines::Draw(float x, float y){
	if (m_text.empty())
		return;

	if (uFlags.is(flNeedReparse)) 
		ParseText();

	R_ASSERT(m_pFont);

	m_pFont->SetColor(m_dwTextColor);
	m_pFont->SetAligment(m_eTextAlign);

	Fvector2 pos;
	// get vertical indent
	pos.y= y + GetVIndentByAlign();
	float height	= m_pFont->CurrentHeightRel();
	u32 size		= m_lines.size();

	float interval = m_interval*m_pFont->CurrentHeightRel();

	for (int i=0; i<(int)size; i++)
	{
 		pos.x = x + GetIndentByAlign(m_lines[i].GetLength(m_pFont));
		m_lines[i].Draw(m_pFont, pos.x, pos.y);
		pos.y+= height + interval;
	}

	m_pFont->OnRender();
}

void CUILines::Draw(){
	Draw(m_wndPos.x, m_wndPos.y);
}

void CUILines::Update(){
}

void CUILines::OnDeviceReset(){
	uFlags.set(flNeedReparse, true);
}

float CUILines::GetIndentByAlign(float length)const{
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

float CUILines::GetVIndentByAlign()const{
	switch(m_eVTextAlign) {
	case valTop: 
		return 0;
	case valCenter:
		return (m_wndSize.y - GetVisibleHeight())/2;
	case valBotton:
		return m_wndSize.y - GetVisibleHeight();
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
