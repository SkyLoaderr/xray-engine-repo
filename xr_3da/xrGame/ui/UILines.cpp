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
#include "../MainUI.h"


CUILines::CUILines()
{
	m_pFont = NULL;
	m_interval = 0.0f;
	m_eTextAlign = CGameFont::alLeft;
	m_eVTextAlign = valTop;
	m_dwTextColor = 0xffffffff;

	m_bShowMe = true;
	m_wndPos.x = 0;
	m_wndPos.y = 0;
	uFlags.zero();
	uFlags.set(flNeedReparse,	FALSE);
	uFlags.set(flComplexMode,	TRUE);
	uFlags.set(flPasswordMode,	FALSE);
	m_pFont = UI()->Font()->pFontLetterica16Russian;
	m_cursor_pos.set(0,0);
	m_iCursorPos = 0;
}

CUILines::~CUILines(){

}

void CUILines::SetTextComplexMode(bool mode){
	uFlags.set(flComplexMode, mode);
	if (mode)
		uFlags.set(flPasswordMode, FALSE);
}

bool CUILines::GetTextComplexMode(){
	return uFlags.test(flComplexMode) ? true : false;
}

void CUILines::SetPasswordMode(bool mode){
	uFlags.set(flPasswordMode, mode);
	if (mode)
		uFlags.set(flComplexMode, false);
}

void CUILines::Init(float x, float y, float width, float heigt){	
	CUISimpleWindow::Init(x, y, width, heigt);
}

void CUILines::SetText(const char* text){
	if (!m_pFont)
        m_pFont = UI()->Font()->pFontLetterica16Russian;

	if (text && xr_strlen(text) > 0)
	{
        m_text = text;
		uFlags.set(flNeedReparse, TRUE);
	}
	else
	{
		m_text = "";
		Reset();
	}
}

void CUILines::AddCharAtCursor(const char ch){
	uFlags.set(flNeedReparse, TRUE);
	m_text.insert(m_text.begin()+GetAbsCursorPos(),ch);
	IncCursorPos();
}

int CUILines::GetAbsCursorPos(){
	int abs_pos = 0;
	if (uFlags.test(flComplexMode))
		if (0 != m_cursor_pos.y)
		{
			int n = m_lines.size();
			if (n>m_cursor_pos.y)
				n = m_cursor_pos.y;
			for (int i = 0; i<n; i++)
				abs_pos += m_lines[i].GetSize();
		}	
	abs_pos += m_cursor_pos.x;
	return abs_pos;
}


void CUILines::MoveCursorToEnd(){
	const int sz = m_lines.size();
	if (sz >0)
	{
		m_cursor_pos.y = sz - 1;
		m_cursor_pos.x = m_lines[m_cursor_pos.y].GetSize();
	}
	else
	{
		m_cursor_pos.x = (int)m_text.size();
	}
}

void CUILines::DelChar(){
	const int pos = GetAbsCursorPos();
	const int sz = (int)m_text.size();
	if (pos < sz)
	{
        m_text.erase(m_text.begin()+pos);
        uFlags.set(flNeedReparse, TRUE);
	}
}

void CUILines::DelLeftChar(){
	const int pos = GetAbsCursorPos();
	if (pos>0)
	{
		DecCursorPos();
		DelChar();
	}	
}

const char* CUILines::GetText(){
	return m_text.c_str();
}

void CUILines::Reset(){
	m_lines.clear();
}

void CUILines::ParseText(){
	if (!uFlags.test(flComplexMode) || !uFlags.test(flNeedReparse))
		return;

	Reset();
	if (m_text.length() && NULL == m_pFont)
		R_ASSERT2(false, "can't parse text without font");
		
	if(NULL == m_pFont)
		return;

	CUILine* line = ParseTextToColoredLine(m_text.c_str());

	while (line->GetLength(m_pFont) > 0)
		m_lines.push_back(*line->CutByLength(m_pFont, m_wndSize.x));

	xr_delete(line);
	uFlags.set(flNeedReparse, FALSE);
}

float CUILines::GetVisibleHeight(){
	if (uFlags.test(flComplexMode))
	{
		if(uFlags.test(flNeedReparse))
			ParseText	();
		return (m_pFont->CurrentHeightRel() + m_interval)*m_lines.size() - m_interval;
	}
	else
		return m_pFont->CurrentHeightRel();
}

void CUILines::SetTextColor(u32 color){
	if (color == m_dwTextColor)
		return;
	uFlags.set(flNeedReparse, true);
	m_dwTextColor = color; 
}

void CUILines::SetFont(CGameFont* pFont){
	if (pFont == m_pFont)
		return;
	uFlags.set(flNeedReparse, true);
	m_pFont = pFont;
}

void CUILines::Draw(float x, float y){
	static string256 passText;

	if (m_text.empty())
		return;

	R_ASSERT(m_pFont);
	m_pFont->SetColor(m_dwTextColor);

	if (!uFlags.is(flComplexMode))
	{
		Fvector2 text_pos;
		text_pos.set(0,0);

		text_pos.x = x + GetIndentByAlign(m_pFont->SizeOfRel(m_text.c_str()));
		text_pos.y = y + GetVIndentByAlign();

		Frect r;
		r.x1=0.0f; r.x2=UI_BASE_WIDTH;
		r.y1=0.0f; r.y2=UI_BASE_HEIGHT;

		if (uFlags.test(flPasswordMode))
		{
			int sz = (int)m_text.size();
			for (int i = 0; i < sz; i++)
				passText[i] = '*';
			passText[sz] = 0;
			UI()->OutText(m_pFont, r, text_pos.x, text_pos.y, "%s", passText);
		}
		else
            UI()->OutText(m_pFont, r, text_pos.x, text_pos.y, "%s", m_text.c_str());
	}
	else
	{
		static float my_width = 0;

		if (m_oldWidth != m_wndSize.x)
		{
			uFlags.set(flNeedReparse, TRUE);
			m_oldWidth = m_wndSize.x;
		}

		if (uFlags.test(flNeedReparse))
			ParseText();

		Fvector2 pos;
		// get vertical indent
		pos.y			= y + GetVIndentByAlign();
		float height	= m_pFont->CurrentHeightRel();
		u32 size		= m_lines.size();

		for (int i=0; i<(int)size; i++)
		{
			pos.x = x + GetIndentByAlign(m_lines[i].GetLength(m_pFont));
			m_lines[i].Draw(m_pFont, pos.x, pos.y);
			pos.y+= height + m_interval;
		}

	}

	m_pFont->OnRender();
}

void CUILines::DrawCursor(float x, float y){
	int sz = (int)m_lines.size();
	int lnsz = (int)m_text.size();

	if (m_cursor_pos.y > sz)
		m_cursor_pos.y = sz;

	Fvector2 text_pos;
	text_pos.set(0,0);

	if (0 == lnsz)
	{
		text_pos.x = x + GetIndentByAlign(0);		
		text_pos.y = y + GetVIndentByAlign();
		CUILine::DrawCursor(m_pFont, text_pos.x, text_pos.y, m_dwTextColor);
		return;
	}
	
	if (!uFlags.test(flComplexMode))
	{
		text_pos.x = x + GetIndentByAlign(m_pFont->SizeOfRel(m_text.c_str()));		
		text_pos.y = y + GetVIndentByAlign();
		
		char tmp = m_text[m_cursor_pos.x];
		m_text[m_cursor_pos.x] = 0;

		if (uFlags.test(flPasswordMode))
			text_pos.x += m_pFont->SizeOfRel("*")*xr_strlen(m_text.c_str());
		else
			text_pos.x += iFloor(m_pFont->SizeOfRel(m_text.c_str()));

		m_text[m_cursor_pos.x] = tmp;
		
		CUILine::DrawCursor(m_pFont, text_pos.x, text_pos.y, m_dwTextColor);
	}
	else
	{
		if (0 == sz)
		{
			text_pos.x = x + GetIndentByAlign(0);		
			text_pos.y = y + GetVIndentByAlign();
			CUILine::DrawCursor(m_pFont, text_pos.x, text_pos.y, m_dwTextColor);
			return;
		}
		text_pos.y= y + GetVIndentByAlign();
		float height	= m_pFont->CurrentHeightRel();
		text_pos.y += (height + m_interval)*m_cursor_pos.y;
		text_pos.x = x + GetIndentByAlign(m_lines[m_cursor_pos.y].GetLength(m_pFont));
		m_lines[m_cursor_pos.y].DrawCursor(m_cursor_pos.x, m_pFont, text_pos.x, text_pos.y, m_dwTextColor);
	}	
}

float CUILines::GetDrawCursorPos(){
	R_ASSERT(!uFlags.test(flComplexMode));
	float x = GetIndentByAlign(m_pFont->SizeOfRel(m_text.c_str()));		
	char tmp = m_text[m_cursor_pos.x];
	m_text[m_cursor_pos.x] = 0;
	x += m_pFont->SizeOfRel(m_text.c_str());
	m_text[m_cursor_pos.x] = tmp;
	return x;
}

void CUILines::Draw(){
	Draw(m_wndPos.x, m_wndPos.y);
}

void CUILines::Update(){
}

void CUILines::OnDeviceReset(){
	uFlags.set(flNeedReparse, TRUE);
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

float CUILines::GetVIndentByAlign(){
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
//	CUIXmlInit xml;
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

void CUILines::SetWndSize_inline(const Fvector2& wnd_size){
	m_wndSize = wnd_size;
}

void CUILines::IncCursorPos(){
	const int txt_len = (int)m_text.size();

	if (0 == txt_len)
		return;

	if (m_iCursorPos < txt_len)
        m_iCursorPos++;

	UpdateCursor();
	return;
/**
    
	const int sz = (int)m_lines.size();
	if (uFlags.test(flComplexMode) && uFlags.test(flNeedReparse))
		ParseText();
	const int lnsz = uFlags.test(flComplexMode) ? m_lines[m_cursor_pos.y].GetSize() : (int)m_text.size();
	
	if (lnsz > m_cursor_pos.x)
		m_cursor_pos.x++;
	else
		if (sz > m_cursor_pos.y)
		{
			m_cursor_pos.x = 0;
			m_cursor_pos.y++;
		}
/**/
}

void CUILines::DecCursorPos(){
	const int txt_len = (int)m_text.size();

	if (0 == txt_len)
		return;

	if (m_iCursorPos > 0)
		m_iCursorPos--;

	UpdateCursor();
	return;
/**
	if (m_cursor_pos.x>0)
		m_cursor_pos.x--;
	else
		if (m_cursor_pos.y>0)
		{
			m_cursor_pos.y--;
			m_cursor_pos.x = m_lines[m_cursor_pos.y].GetSize();
		}
/**/
}

void CUILines::UpdateCursor(){
	if (uFlags.test(flComplexMode))
	{
		ParseText();
		const int sz = (int)m_lines.size();
		int len = 0;
		for (int i = 0; i < sz; i++)
		{
            int curlen = m_lines[i].GetSize();
			if (m_iCursorPos <= len + curlen)
			{
				m_cursor_pos.y = i;
				m_cursor_pos.x = m_iCursorPos - len;
				return;
			}
			len += curlen;
		}
//		R_ASSERT(false);
	}
	else
	{
		m_cursor_pos.y = 0;
		m_cursor_pos.x = m_iCursorPos;
	}	
}

