// File:		UILines.h
// Description:	Multilines Text Control
// Created:		11.03.2005
// Author:		Serge Vynnycheko
// Mail:		narrator@gsc-game.kiev.ua
//
// Copyright 2005 GSC Game World

#pragma once

#include "UIWindow.h"

static const int npos = -1;

class CUILines : public CUIWindow {
	typedef CGameFont::EAligment Alignment;
public:
	CUILines();
	virtual ~CUILines();

	virtual void Draw();

	// attributes
			void Clear();
			void AddLine(char* line);
			void ParseToWidth(char* line, int width);
			int  GetHeight();
			void SetLinesInterval(int i)			{ m_interval = i; }
			int  GetLinesInterval()					{ return m_interval; }
			void SetTextAlignment(Alignment align)	{ m_eTextAlign = align; }
	   Alignment GetTextAlignment()					{ return m_eTextAlign ;}

private:
			int  Get_words_Count(char* line, int length);
			int  Get_wholeWords_Count(char* line, int length);
IC			void Copy(char* line, int length, xr_string& str);
//IC			void NormilizeSpaces(xr_string& str);
	typedef xr_vector<xr_string>			LinesVector;
	typedef xr_vector<xr_string>::iterator	LinesVector_it;
	LinesVector	m_lines;
	u32			m_interval;

	Alignment	m_eTextAlign;
};