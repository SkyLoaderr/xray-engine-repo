// File:		UIBuyWeaponTab.cpp
// Description:	Tab conorol for BuyWeaponWnd
// Created:		07.02.2005
// Author:		Serge Vynnychenko
// Mail:		narrator@gsc-game.kiev.ua

// Copyright 2005 GSC Game World

#include "StdAfx.h"
#include "UIBuyWeaponTab.h"
#include "UITabButtonMP.h"
#include "../string_table.h"
#include "../HUDManager.h"
#include "../UI.h"


const u8			uIndicatorWidth				= 17;
const u8			uIndicatorHeight			= 27;

CUIBuyWeaponTab::CUIBuyWeaponTab(){
	AttachChild(&m_staticBackground);
	m_bActiveState = true;
}

CUIBuyWeaponTab::~CUIBuyWeaponTab(){

}

void CUIBuyWeaponTab::Init(CUIXml& xml, char* path){

#pragma todo("Satan to Satan: delete all this shit form textures folder")

	//static const shared_str	inactiveItems[]	= { "ui\\ui_mp_icon_small_weapons", 
	//											"ui\\ui_mp_icon_main_guns",
	//											"ui\\ui_mp_icon_grenades",
	//											"ui\\ui_mp_icon_suits",
	//											"ui\\ui_mp_icon_equipment" };

	//static const shared_str	activeItems[]	= { "ui\\ui_mp_icon_small_weapons_over",
	//											"ui\\ui_mp_icon_main_guns_over",
	//											"ui\\ui_mp_icon_grenades_over",
	//											"ui\\ui_mp_icon_suits_over",
	//											"ui\\ui_mp_icon_equipment_over" };

	//static const shared_str	activeItems[]	= { "ui\\ui_btn_sec_t",
	//											"ui\\ui_btn_prim_t",
	//											"ui\\ui_btn_gren_t",
	//											"ui\\ui_btn_suit_t",
	//											"ui\\ui_btn_equip_t" };

	R_ASSERT3(xml.NavigateToNode(path,0), "XML node not found", path);
	
	CUIXmlInit::InitWindow(xml, path, 0, this);
	int tabsCount = xml.GetNodesNum(path, 0, "button");

	XML_NODE* tab_node = xml.NavigateToNode(path,0);
	xml.SetLocalRoot(tab_node);

	for (int i = 0; i < tabsCount; ++i)
	{
		CUITabButtonMP *newButton = xr_new<CUITabButtonMP>();
		CUIXmlInit::Init3tButton(xml, "button", i, newButton);
//		newButton->InitTexture(*activeItems[i]);
//		newButton->SetTextColor(color_argb(255, 181, 170, 148));
//		newButton->SetTextColorT(color_argb(255, 255, 187, 51));
//		newButton->SetTextColorH(color_argb(255, 255, 187, 51));
		char ch[5];
		newButton->SetNumber(itoa(i+1,ch,10));

		AddItem(newButton);
	}
	
	xml.SetLocalRoot(xml.GetRoot());

	SetActiveState();
}

void CUIBuyWeaponTab::Init(int x, int y, int width, int height){
	CUIWindow::Init(x, y, width, height);	
}

void CUIBuyWeaponTab::OnTabChange(int iCur, int iPrev){
	CUITabControl::OnTabChange(iCur, iPrev);
	SetActiveState(false);
}

void CUIBuyWeaponTab::SetActiveState(bool bState){
	m_bActiveState = bState;

	WINDOW_LIST::iterator it = m_ChildWndList.begin();

	for(u32 i=0; i<m_ChildWndList.size(); ++i, ++it)
		(*it)->Enable(bState);

	m_TabsArr[m_iPushedIndex]->Enable(true);
}
