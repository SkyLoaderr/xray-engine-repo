// File:        UIListWndEx.cpp
// Description: Extended ListItem
//              Requiered to use feature "Selected Item"
// Created:     
// Author:      Serhiy O. Vynnychenko
// Mail:        narrator@gsc-game.kiev.ua

// Copyright:   2004 GSC Game World

#include "stdafx.h"
#include ".\uilistitemex.h"

CUIListItemEx::CUIListItemEx(void)
{
	this->InitTexture("ui\\hud_map_point");
	this->SetStretchTexture(true);
	this->m_dwSelectionColor = color_argb(100, 0, 0, 255);
	this->SetColor(color_argb(0, 0, 0, 0));
	//this->m_bPerformTextLimit = false;
	//this->SetElipsis(CUIStatic::eepEnd, 3);
}

CUIListItemEx::~CUIListItemEx(void)
{
}

void CUIListItemEx::SendMessage(CUIWindow* pWnd, s16 msg, void* pData){
	//inherited::SendMessage(pWnd, msg, pData);

	switch (msg)
	{
	case LIST_ITEM_SELECT:
		this->SetColor(m_dwSelectionColor);	
		this->Draw();
		break;
	case LIST_ITEM_UNSELECT:
		this->SetColor(color_argb(0, 0, 0, 0));
		this->Draw();
		break;
	}
}

void CUIListItemEx::SetSelectionColor(u32 dwColor){
	m_dwSelectionColor = dwColor;
}

void CUIListItemEx::Draw(){
//	if (m_bPerformTextLimit)
//		this->PerformTextLengthLimit();
	inherited::Draw();	
}