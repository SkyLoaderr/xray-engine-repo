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

void CUISubLine::Draw(CGameFont* pFont, float x, float y) const{
	pFont->SetColor(m_color);
	pFont->SetAligment(CGameFont::alLeft);
	Frect r;
	r.x1=0.0f; r.x2=UI_BASE_WIDTH;
	r.y1=0.0f; r.y2=UI_BASE_HEIGHT;
	UI()->OutText(pFont, r, x, y, "%s", m_text.c_str());
}

float CUISubLine::GetLength(CGameFont* pFont) const{
	return (pFont->SizeOfRel(m_text.c_str()));
}

float CUISubLine::GetVisibleLength(CGameFont* pFont){
	int end = (int)m_text.find_last_not_of(' ');
	bool b = (end!=(int)m_text.size()-1);
	if(b)
		m_text[end+1] = 0;	
	float res = (pFont->SizeOfRel(m_text.c_str()));	
	if(b)
		m_text[end+1] = ' ';
	return res;
}