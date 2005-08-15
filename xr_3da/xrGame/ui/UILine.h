// File:		UILine.h
// Description:	Single text line
// Created:		11.03.2005
// Author:		Serge Vynnycheko
// Mail:		narrator@gsc-game.kiev.ua
//
// Copyright 2005 GSC Game World

#pragma once

#include "UISubLine.h"
#include "../HUDManager.h"
#include "UIColorAnimatorWrapper.h"

class Word{
public:
	Word()				{ len = 0; len_full = 0; pos = 0;}
	int  len;
	int  len_full;
	int  pos;
IC	int  last_symbol()	{ return pos + len - 1; }
IC	int  last_space()	{ return pos + len_full -1; }
IC	bool exist()		{ return len > 0; }
};

class Position{
public:
	Word word_1;
	Word word_2;
    u32  curr_subline;
IC	bool is_separated()	{ return 0 == word_2.pos; }
};

// Attention! Destructor is not virtual.
// if you want to inherit this class then make _coresponding_ changes
class CUILine {
	friend class CUILines;
public:    
	CUILine();
	CUILine(const CUILine& other);
CUILine& operator=(const CUILine& other);
	void AddSubLine(const xr_string& str, u32 color);
	void AddSubLine(const char* str, u32 color);
	void AddSubLine(const CUISubLine* subLine);
	void Clear();
	float  GetLength(CGameFont* pFont);
CUILine* CutByLength(CGameFont* pFont, float length);

	void Draw(CGameFont* pFont, float x, float y)						const;
    int	 DrawCursor(int pos, CGameFont* pFont, float x, float y, u32 color)	const;
static void DrawCursor(CGameFont* pFont, float x, float y, u32 color);

protected:
	int GetSize();
	bool GetWord(Word& w, const xr_string& text, int begin)				const;
CUILine* Cut2Pos(Position& pos, bool to_first = true);
	float GetLength_inclusiveWord_1(Position& pos, CGameFont* pFont)	const;
	float GetLength_inclusiveWord_2(Position& pos, CGameFont* pFont)	const;
	bool InitPos(Position& pos)											const;
	bool IncPos(Position& pos)											const;
    xr_vector<CUISubLine> m_subLines;

	static CUIColorAnimatorWrapper m_animation;

	CUILine* m_tmpLine;
};
