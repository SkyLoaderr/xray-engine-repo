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
#include "UIXmlInit.h"


const u8			uIndicatorWidth				= 17;
const u8			uIndicatorHeight			= 27;

CUIBuyWeaponTab::CUIBuyWeaponTab(){
	AttachChild(&m_staticBackground);
	m_bActiveState = true;
}

CUIBuyWeaponTab::~CUIBuyWeaponTab(){

}

void CUIBuyWeaponTab::Init(CUIXml* xml, char* path){

	R_ASSERT3(xml->NavigateToNode(path,0), "XML node not found", path);
	
	CUIXmlInit::InitWindow(*xml, path, 0, this);
	int tabsCount = xml->GetNodesNum(path, 0, "button");

	XML_NODE* tab_node = xml->NavigateToNode(path,0);
	xml->SetLocalRoot(tab_node);

	for (int i = 0; i < tabsCount; ++i)
	{
		CUITabButtonMP *newButton = xr_new<CUITabButtonMP>();
		CUIXmlInit::Init3tButton(*xml, "button", i, newButton);
//		char ch[5];
//		newButton->SetNumber(itoa(i+1,ch,10));

		AddItem(newButton);
	}

	m_iStubIndex = tabsCount;	
	CUITabButtonMP *stubButton = xr_new<CUITabButtonMP>();
	AddItem(stubButton);

	SetNewActiveTab(m_iStubIndex);
	
	xml->SetLocalRoot(xml->GetRoot());

	SetActiveState();
}

void CUIBuyWeaponTab::Init(float x, float y, float width, float height){
	CUIWindow::Init(x, y, width, height);	
}

void CUIBuyWeaponTab::OnTabChange(int iCur, int iPrev){
	CUITabControl::OnTabChange(iCur, iPrev);

	if (m_iStubIndex != iCur)
        SetActiveState(false);	
}

void CUIBuyWeaponTab::SetActiveState(bool bState){
	m_bActiveState = bState;

	WINDOW_LIST::iterator it = m_ChildWndList.begin();

	for(u32 i=0; i<m_ChildWndList.size(); ++i, ++it)
		(*it)->Enable(bState);

	if (bState)
		SetNewActiveTab(m_iStubIndex);		

	m_TabsArr[m_iPushedIndex]->Enable(true);
}
