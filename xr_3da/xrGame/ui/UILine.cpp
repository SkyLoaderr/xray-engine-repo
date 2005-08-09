// File:		UILine.cpp
// Description:	Single text line
// Created:		05.04.2005
// Author:		Serge Vynnycheko
// Mail:		narrator@gsc-game.kiev.ua
//
// Copyright 2005 GSC Game World


#include "StdAfx.h"
#include "UILine.h"
#include "uilinestd.h"
#include "UIColorAnimatorWrapper.h"

CUIColorAnimatorWrapper CUILine::m_animation;

CUILine::CUILine(){
	m_tmpLine = NULL;
	m_animation.SetColorAnimation("ui_map_area_anim");
	m_animation.Cyclic(true);
}

CUILine::~CUILine(){
	xr_delete(m_tmpLine);
}

CUILine::CUILine(const CUILine& other){
	m_subLines = other.m_subLines;
	m_tmpLine = NULL;
}

CUILine& CUILine::operator =(const CUILine& other){
	m_subLines = other.m_subLines;
	m_tmpLine = NULL;

	return (*this);
}

void CUILine::AddSubLine(const xr_string& str, u32 color){
	CUISubLine sline;
	sline.m_color = color;
	sline.m_text = str;
	m_subLines.push_back(sline);
}

void CUILine::AddSubLine(const char* str, u32 color){
	CUISubLine sline;
	sline.m_color = color;
	sline.m_text = str;
	m_subLines.push_back(sline);
}

void CUILine::AddSubLine(const CUISubLine* subLine){
	m_subLines.push_back(*subLine);
}

void CUILine::Clear(){
	m_subLines.clear();
}

float CUILine::GetLength(CGameFont* pFont) const{
	float length = 0.0f;
	int size = m_subLines.size();
	for (int i=0; i< size; i++)
	{
		if (i == size - 1)
			length += m_subLines[i].GetVisibleLength(pFont);
		else
            length += m_subLines[i].GetLength(pFont);
	}

    return length;	
}

void CUILine::Draw(CGameFont* pFont, float x, float y) const{
	float length = 0;
	int size = m_subLines.size();

	for (int i=0; i<size; i++)
	{
		m_subLines[i].Draw(pFont, x+length, y);
		length+=m_subLines[i].GetLength(pFont);
	}
}

int CUILine::DrawCursor(int pos, CGameFont* pFont, float x, float y, u32 color) const{
	int size = m_subLines.size();
	xr_string whole_line;
	for (int i=0; i<size; i++)
		whole_line += m_subLines[i].m_text;

	int sz = (int)whole_line.size();
		
	if (pos > sz)
		pos = sz;

	whole_line[pos] = 0;
	x += pFont->SizeOfRel(whole_line.c_str());
	DrawCursor(pFont, x, y, color);

	return pos;
}

int CUILine::GetSize(){
	int sz = 0;
	int size = m_subLines.size();
	for (int i=0; i<size; i++)
		sz += (int)m_subLines[i].m_text.size();

	return sz;
}

void CUILine::DrawCursor(CGameFont* pFont, float x, float y, u32 color){
	m_animation.Update();
	pFont->SetColor(subst_alpha(color, color_get_A(m_animation.GetColor())));

	Frect scr_rect;
	scr_rect.set(0,0,1024,768);
	UI()->OutText(pFont, scr_rect, x, y,  "|");
}

CUILine* CUILine::CutByLength(CGameFont* pFont, float length){
	R_ASSERT(GetLength(pFont) > 0);
	// if first sub line is void then delete it
	if (0 == m_subLines[0].GetLength(pFont)) 
		m_subLines.erase(m_subLines.begin());

	Position pos;
	InitPos(pos);

    if (!pos.word_2.exist())
		return Cut2Pos(pos);

	float len2w1 = GetLength_inclusiveWord_1(pos, pFont);
	float len2w2 = GetLength_inclusiveWord_2(pos, pFont);

	if (len2w1 > length)
	{
		// cut part of word
//		R_ASSERT(false);
		return Cut2Pos(pos);  // !!!DANGER!!! STUB
	}
	else if (len2w1 <= length && len2w2 > length)
	{
		// cut whole first word
		return Cut2Pos(pos);  // all right :)
	}
	else // if (len2w1 > length && len2w2 > length)
	{
		while (IncPos(pos))
		{
			len2w1 = GetLength_inclusiveWord_1(pos, pFont);
			len2w2 = GetLength_inclusiveWord_2(pos, pFont);
			if (len2w1 <= length && len2w2 > length)
				return Cut2Pos(pos);
		}
     
		return Cut2Pos(pos, false);
	}

}

bool CUILine::GetWord(Word& w, const xr_string& text, int begin) const{

	if (text.size() == 0)
		return false;

	StrSize first, last, lastsp/*last space*/;

	first  = text.find_first_not_of(' ', begin);

	if (npos == first)
		return false;

	last   = text.find_first_of(' ', first);
	lastsp = text.find_first_not_of(' ', last);

	if (npos == lastsp)
		lastsp = last;
	else
		--lastsp;

	if (npos == last && npos != first)
		last = text.size() - 1;
	else
		--last;

	if (npos == lastsp)
		lastsp = last;

	first = begin;

	w.pos		= (int)first;
	w.len		= (int)(last - first + 1);
	w.len_full	= (int)(lastsp - first + 1);

#ifdef DEBUG
	if (npos != first && (npos == last || npos == lastsp ))
		R_ASSERT2(false,"CUILine::InitPos -- impossible match");
#endif

	return true;        
}

bool CUILine::InitPos(Position& pos) const{
	Word w;
	pos.curr_subline = 0;
	if (GetWord(w, m_subLines[0].m_text, 0))
		pos.word_1 = w;
	else
		return false;

	if (GetWord(w, m_subLines[0].m_text, w.last_space() + 1))
		pos.word_2 = w;
	else if (m_subLines.size() > 1 && GetWord(w, m_subLines[1].m_text, 0))
		pos.word_2 = w;

	// find "\n"



	return true;
}

bool CUILine::IncPos(Position& pos) const{
	u32 totalLinesCount = m_subLines.size();
	if (totalLinesCount < pos.curr_subline)
		return false;

	Word w;
	u32 curLine = pos.curr_subline;

	if ( ! pos.is_separated() )
	{
		if (GetWord(w, m_subLines[curLine].m_text, pos.word_2.last_space() + 1))
		{
			pos.word_1 = pos.word_2;
			pos.word_2 = w;
			return true;
		}
		else if (curLine + 1 <= totalLinesCount - 1)
		{
			if (GetWord(w, m_subLines[curLine + 1].m_text, 0))
			{
				pos.word_1 = pos.word_2;
				pos.word_2 = w;
				return true;
			}
		}
		else
			return false;
	}
	else if (curLine + 1 <= totalLinesCount -1)
	{
		if (GetWord(w, m_subLines[curLine + 1].m_text, pos.word_2.last_space() + 1))
		{
			pos.word_1 = pos.word_2;
			pos.word_2 = w;
			pos.curr_subline = curLine + 1;
			return true;
		}
		else if (curLine + 2 <= totalLinesCount -1)
			if (GetWord(w, m_subLines[curLine + 2].m_text, 0))
			{
				pos.word_1 = pos.word_2;
				pos.word_2 = w;
				pos.curr_subline = curLine + 1;
				return true;
			}
			return false;
	}
	return false;
}

CUILine* CUILine::Cut2Pos(Position& pos, bool to_first){
	xr_delete(m_tmpLine);
	m_tmpLine = xr_new<CUILine>();

	int last;

	if (to_first || !pos.is_separated())
		last = pos.curr_subline - 1;
	else
		last = pos.curr_subline;


	for (int i = 0; i<= last; i++)
		m_tmpLine->AddSubLine(&m_subLines[i]);

	if (to_first)
		m_tmpLine->AddSubLine(m_subLines[last + 1].Cut2Pos(pos.word_1.last_space()));
	else
		m_tmpLine->AddSubLine(m_subLines[last + 1].Cut2Pos(pos.word_2.last_space()));

	for (int i = 0; i<= last; i++)
        m_subLines.erase(m_subLines.begin());

    return m_tmpLine;
}

float  CUILine::GetLength_inclusiveWord_1(Position& pos, CGameFont* pFont) const{
	R_ASSERT(pos.word_1.exist());
	float len = 0;

	for (u32 i = 0; i < pos.curr_subline; ++i)
		len += m_subLines[i].GetLength(pFont);

	xr_string str;
	str.assign(m_subLines[pos.curr_subline].m_text, 0, pos.word_1.pos + pos.word_1.len);

	
	len += int(pFont->SizeOfRel(str.c_str()));

	return len;
}

float  CUILine::GetLength_inclusiveWord_2(Position& pos, CGameFont* pFont) const{
	R_ASSERT(pos.word_2.exist());
	float len = 0;
	int last;

	if (!pos.is_separated())
		last = pos.curr_subline - 1;
	else
		last = pos.curr_subline;

	for (int i = 0; i <= last; i++)
		len += m_subLines[i].GetLength(pFont);

	xr_string str;
	str.assign(m_subLines[last + 1].m_text, 0, pos.word_2.pos + pos.word_2.len);

	len += pFont->SizeOfRel(str.c_str());

	return len;
}
