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

CUILines::CUILines()
	:m_interval(5)
{

}

CUILines::~CUILines(){

}

void CUILines::Clear(){
	m_lines.clear();
}

void CUILines::AddLine(char* line){
	m_lines.push_back(line);
}

void CUILines::ParseToWidth(char* line, int width){
	Clear();
}

int CUILines::Get_words_Count(char* line, int length){
    xr_string str;

	int real_length = xr_strlen(line);

	char after_last = 0;

	if (real_length < length)
		length = real_length;  
	else
		after_last = line[length]; 

	// copy line to xr_string str
	Copy(line, length, str);

    // if there are no spaces, then the word is only one :)
	int pos = (int)str.find(' ');
	if (npos == pos)
		return 1;

	while (' ' == str[pos])	
		pos++;

	return NULL;
}

void CUILines::Copy(char* line, int length, xr_string& str){
	for ( int i = 0; i < length; ++i)
        str.push_back(line[i]);
}

//void CUILines::NormilizeSpaces(xr_string& str){
//	static const int npos = -1;
//
//	int pos = (int)str.find(' ');
//
//	if (npos == pos)
//		return;	
//}

int CUILines::Get_wholeWords_Count(char* line, int length){
    
	return NULL;
}

int CUILines::GetHeight(){
	return ((int)GetFont()->CurrentHeight())*(m_lines.size() + m_interval) - m_interval;
}

void CUILines::Draw(){
	CGameFont* pFont = GetFont();
	LinesVector_it it;

	Ivector2 pos;
	Irect rect = GetAbsoluteRect();
	pos.x = rect.x1;
	pos.y = rect.y1;
	int height = (int)pFont->CurrentHeight();

	switch (m_eTextAlign)
	{
	case CGameFont::alCenter:
			pos.x = rect.x1 + rect.width()/2;	break;
	case CGameFont::alLeft:
			pos.x = rect.x1;					break;
	case CGameFont::alRight:
			pos.x = rect.x2;					break;
		default:
            NODEFAULT;
	}

	pFont->SetAligment(m_eTextAlign);

	for ( it = m_lines.begin(); it != m_lines.end(); ++it)
	{
		UI()->OutText(pFont, rect, (float)pos.x, (float)pos.y, (*it).c_str());
		pos.y += m_interval + height;
	}
//	
}
