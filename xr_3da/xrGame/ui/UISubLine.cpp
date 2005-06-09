// File:		UISubLine.cpp
// Description:	Text line. Owns color attribute
// Created:		04.04.2005
// Author:		Serge Vynnycheko
// Mail:		narrator@gsc-game.kiev.ua
//
// Copyright 2005 GSC Game World

#include "StdAfx.h"
#include "UISubLine.h"
#include "uilinestd.h"
#include "../MainUI.h"

CUISubLine::CUISubLine(const CUISubLine& other){
	m_color = other.m_color;
	m_text = other.m_text;
	m_pTempLine = NULL;
}

CUISubLine& CUISubLine::operator=(const CUISubLine& other){
	m_color = other.m_color;
	m_text = other.m_text;
	m_pTempLine = NULL;
	return (*this);
}

CUISubLine::CUISubLine(){
	m_color = 0;
	m_pTempLine = NULL;
}

CUISubLine::~CUISubLine(){
	xr_delete(m_pTempLine);
}

const CUISubLine* CUISubLine::Cut2Pos(int i){
	R_ASSERT2( (u32)i < m_text.size(), "CUISubLine::Cut2Pos - invalid parameter");

	xr_delete(m_pTempLine);
	m_pTempLine = xr_new<CUISubLine>();
	m_pTempLine->m_color = m_color;
	m_pTempLine->m_text.assign(m_text,0,i+1);	
	m_text.replace(0, i+1, "");

	return m_pTempLine;
}

void CUISubLine::FreeBuffer(){
	xr_delete(m_pTempLine);
}

void CUISubLine::Draw(CGameFont* pFont, int x, int y) const{
	pFont->SetColor(m_color);
	pFont->SetAligment(CGameFont::alLeft);
	Irect r;
	r.x1=0; r.x2=1024;
	r.y1=0; r.y2=768;
	UI()->OutText(pFont, r, (float)x, (float)y, "%s", m_text.c_str());
}

u32 CUISubLine::GetLength(CGameFont* pFont) const{
	return (u32)(pFont->SizeOfRel(m_text.c_str()));
}

u32 CUISubLine::GetVisibleLength(CGameFont* pFont) const{
	xr_string text;
	StrSize end = m_text.find_last_not_of(' ');

	text.assign(m_text, 0, end + 1);

	return (u32)(pFont->SizeOfRel(text.c_str()));
}