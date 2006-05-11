// File:		UIDebugFonts.cpp
// Description:	Output list of all fonts
// Created:		22.03.2005
// Author:		Serge Vynnychenko
// Mail:		narrator@gsc-game.kiev.ua
//
// Copyright 2005 GSC Game World

#include "StdAfx.h"
#include "UIDebugFonts.h"
#include "dinput.h"
#include "../hudmanager.h"


CUIDebugFonts::CUIDebugFonts(){
	AttachChild(&m_background);
	AttachChild(&m_list);	
	Init(0, 0, 1024, 768);
}

CUIDebugFonts::~CUIDebugFonts(){

}

void CUIDebugFonts::Init(float x, float y, float width, float height){
	CUIDialogWnd::Init(x, y, width, height);

	m_list.Init(x, y, width, height);
	m_list.SetItemHeight(60);
	FillUpList();

	m_background.Init(x, y, width, height);
	m_background.InitTexture("ui\\ui_debug_font");
}

bool CUIDebugFonts::OnKeyboard(int dik, EUIMessages keyboard_action){
	if (DIK_ESCAPE == dik)
		this->GetHolder()->StartStopMenu(this, true);

	if (DIK_F12 == dik)
		return false;

    return true;
}

void CUIDebugFonts::FillUpList(){
	CUIListItem* pItem = xr_new<CUIListItem>();
	pItem->SetFont(UI()->Font()->pFontDI);
	pItem->SetText("pFontDI 1 2 3 4 5 6 7 8 9 0");
	m_list.AddItem(pItem);

	pItem = xr_new<CUIListItem>();
	pItem->SetFont(UI()->Font()->pFontGraffiti19Russian);
	pItem->SetText("pFontGraffiti19Russian 1 2 3 4 5 6 7 8 9 0");
	m_list.AddItem(pItem);	

	pItem = xr_new<CUIListItem>();
	pItem->SetFont(UI()->Font()->pFontGraffiti22Russian);
	pItem->SetText("pFontGraffiti22Russian 1 2 3 4 5 6 7 8 9 0");
	m_list.AddItem(pItem);	

	pItem = xr_new<CUIListItem>();
	pItem->SetFont(UI()->Font()->pFontGraffiti32Russian);
	pItem->SetText("pFontGraffiti32Russian 1 2 3 4 5 6 7 8 9 0");
	m_list.AddItem(pItem);	

	pItem = xr_new<CUIListItem>();
	pItem->SetFont(UI()->Font()->pFontGraffiti50Russian);
	pItem->SetText("pFontGraffiti50Russian 1 2 3 4 5 6 7 8 9 0");
	m_list.AddItem(pItem);	

	pItem = xr_new<CUIListItem>();
	pItem->SetFont(UI()->Font()->pFontHeaderEurope);
	pItem->SetText("pFontHeaderEurope 1 2 3 4 5 6 7 8 9 0");
	m_list.AddItem(pItem);	

	pItem = xr_new<CUIListItem>();
	pItem->SetFont(UI()->Font()->pFontHeaderRussian);
	pItem->SetText("pFontHeaderRussian 1 2 3 4 5 6 7 8 9 0");
	m_list.AddItem(pItem);

	pItem = xr_new<CUIListItem>();
	pItem->SetFont(UI()->Font()->pFontLetterica16Russian);
	pItem->SetText("pFontLetterica16Russian 1 2 3 4 5 6 7 8 9 0");
	m_list.AddItem(pItem);

	pItem = xr_new<CUIListItem>();
	pItem->SetFont(UI()->Font()->pFontLetterica18Russian);
	pItem->SetText("pFontLetterica18Russian 1 2 3 4 5 6 7 8 9 0");
	m_list.AddItem(pItem);

	pItem = xr_new<CUIListItem>();
	pItem->SetFont(UI()->Font()->pFontLetterica25);
	pItem->SetText("pFontLetterica25 1 2 3 4 5 6 7 8 9 0");
	m_list.AddItem(pItem);

	pItem = xr_new<CUIListItem>();
	pItem->SetFont(UI()->Font()->pFontMedium);
	pItem->SetText("pFontMedium 1 2 3 4 5 6 7 8 9 0");
	m_list.AddItem(pItem);

	pItem = xr_new<CUIListItem>();
	pItem->SetFont(UI()->Font()->pFontSmall);
	pItem->SetText("pFontSmall 1 2 3 4 5 6 7 8 9 0");
	m_list.AddItem(pItem);


	pItem = xr_new<CUIListItem>();
	pItem->SetFont(UI()->Font()->pFontStat);
	pItem->SetText("pFontStat 1 2 3 4 5 6 7 8 9 0");
	m_list.AddItem(pItem);	

}