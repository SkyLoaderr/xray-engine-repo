// File:		UIPdaKillMessage.cpp
// Description:	HUD message about player death. Implementation of visual behavior
// Created:		10.03.2005
// Author:		Serge Vynnychenko
// Mail:		narrator@gsc-game.kiev.ua
// 
// Copyright 2005 GSC GameWorld

#include "StdAfx.h"
#include "UIPdaKillMessage.h"
#include "UIInventoryUtilities.h"


using namespace InventoryUtilities;

const int INDENT = 10;

CUIPdaKillMessage::CUIPdaKillMessage(){
	AttachChild(&m_victim_name);
	AttachChild(&m_killer_name);	
	AttachChild(&m_initiator);
	AttachChild(&m_ext_info);
}

CUIPdaKillMessage::CUIPdaKillMessage(int iDelay){
	AttachChild(&m_victim_name);
	AttachChild(&m_killer_name);	
	AttachChild(&m_initiator);
	AttachChild(&m_ext_info);

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

	refStatic.Init(x, y, width + 1, height);
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
	refStatic.SetShader(info.m_shader/*GetEquipmentIconsShader()*/);
//	refStatic.SetTextureScaleXY(scale, scale);
	refStatic.SetStretchTexture(true);

	return (int)((float)width*scale);
}

void CUIPdaKillMessage::Init(int x, int y, int width, int height){
	inherited::Init(x, y, width, height);
}

void CUIPdaKillMessage::SetFont(CGameFont* pFont){
	m_victim_name.SetFont(pFont);
	m_killer_name.SetFont(pFont);
	CUIStatic::SetFont(pFont);
}