// File:		UIPdaKillMessage.cpp
// Description:	HUD message about player death. Implementation of visual behavior
// Created:		10.03.2005
// Author:		Serge Vynnychenko
// Mail:		narrator@gsc-game.kiev.ua
// 
// Copyright 2005 GSC GameWorld

#include "StdAfx.h"
#include "UIPdaKillMessage.h"

const int INDENT = 10;

CUIPdaKillMessage::CUIPdaKillMessage(){

}

CUIPdaKillMessage::CUIPdaKillMessage(int iDelay){
   	m_iDelay = iDelay;
	m_timeBegin = 0;
}

CUIPdaKillMessage::~CUIPdaKillMessage(){

}

void CUIPdaKillMessage::Init(KillMessageStruct& msg){
#ifdef DEBUG
	R_ASSERT2(xr_strlen(msg.m_victim.m_name), "CUIPdaKillMessage::Init(msg) - victim has no name");
	R_ASSERT2(GetWidth(),  "CUIPdaKillMessage::Init(msg) - need to call ::Init(x, y, width, height) before");
	R_ASSERT2(GetHeight(), "CUIPdaKillMessage::Init(msg) - need to call ::Init(x, y, width, height) before");
#endif	

	int		x = 0;
	int width = 0;

	width = InitText(m_victim_name, x, msg.m_victim);		x += width + INDENT;
	width = InitIcon(m_initiator,   x, msg.m_initiator);	x += width + INDENT;
	width = InitText(m_killer_name, x, msg.m_killer);		x += width + INDENT;
			InitIcon(m_ext_info,	x, msg.m_ext_info);

//	int		y = 0;
//	int		selfHeight = GetHeight();
//	float	scale = 0;
//	Irect	rect;
//	CGameFont* pFont = GetFont();

    // firtst of all init victim name
	//int width = (int)pFont->SizeOf(*msg.m_victim.m_name);
	//int height = (int)pFont->CurrentHeight();
	//y = (selfHeight - height)/2;

	//m_victim_name.Init(x, y, width, height);
	//m_victim_name.SetText(*msg.m_victim.m_name);
	//m_victim_name.SetTextColor(msg.m_victim.m_color);

    // init initiator icon

	

//	y = 0;
	
	//rect = msg.m_initiator.m_rect;
	//width = rect.width();
	//height = rect.height();
	//scale = ((float)selfHeight)/((float)height);
	//m_initiator.Init(x, y, width, height);
	//m_initiator.SetOriginalRect(msg.m_initiator.m_rect);
	//m_initiator.SetShader(msg.m_initiator.m_shader);
	//m_initiator.SetTextureScaleXY(scale, scale);

	

	m_killer_name.SetText(*msg.m_killer.m_name);

    
	
	m_ext_info.SetOriginalRect(msg.m_ext_info.m_rect);
	m_ext_info.SetShader(msg.m_ext_info.m_shader);
}

int CUIPdaKillMessage::InitText(CUIStatic& refStatic, int x, PlayerInfo& info){

	if ( 0 == xr_strlen(info.m_name))
		return 0;

	int		y = 0;
	int		selfHeight = GetHeight();
	CGameFont* pFont = GetFont();

	int width = (int)pFont->SizeOf(*info.m_name);
	int height = (int)pFont->CurrentHeight();
	y = (selfHeight - height)/2;

	refStatic.Init(x, y, width, height);
	refStatic.SetText(*info.m_name);
	refStatic.SetTextColor(info.m_color);

	return width;
}

int CUIPdaKillMessage::InitIcon(CUIStatic& refStatic, int x, IconInfo& info){
	if ( 0 == info.m_rect.width())
		return 0;

	int		y = 0;
	int		selfHeight = GetHeight();
	float	scale = 0;
	Irect	rect = info.m_rect;;

	int width = rect.width();
	int height = rect.height();
	scale = ((float)selfHeight)/((float)height);
	refStatic.Init(x, y, width, height);
	refStatic.SetOriginalRect(info.m_rect);
	refStatic.SetShader(info.m_shader);
	refStatic.SetTextureScaleXY(scale, scale);

	return width;
}

void CUIPdaKillMessage::Init(int x, int y, int width, int height){
	inherited::Init(x, y, width, height);
}

void CUIPdaKillMessage::SetFont(CGameFont* pFont){
	m_victim_name.SetFont(pFont);
	m_killer_name.SetFont(pFont);
	CUIStatic::SetFont(pFont);
}