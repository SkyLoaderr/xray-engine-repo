// File:        UIArtefactPanel.h
// Description: HUD artefact panel
// Created:     14.12.2004
// Author:      Serhiy 0. Vynnychenk0
// Mail:        narrator@gsc-game.kiev.ua
//
// Copyright 2004 GSC Game World
//

#include "StdAfx.h"
#include "UIArtefactPanel.h"

CUIArtefactPanel::CUIArtefactPanel(){	

}

CUIArtefactPanel::~CUIArtefactPanel(){

}

void CUIArtefactPanel::InitIcons(const xr_vector<RECT>& vect){	
	for (ITr it = vect.begin(); it != vect.end(); ++it)
	{
		const RECT &rect = *it;
		CUIStaticItem* si = new CUIStaticItem;		
		si->Init("ui\\ui_icon_equipment", "hud\\default", 0, 0, alNone);
		si->SetOriginalRect(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
		m_vIcons.push_back(si);
	}
}

void CUIArtefactPanel::Draw(){
	const int iWidth  = m_vIcons[1]->GetRect().width();
	const int iHeight = m_vIcons[1]->GetRect().height();
	const int iIndent = 8;
	      int x = 0;
	const int y = 0;

	for (ITsi it = m_vIcons.begin(); it != m_vIcons.end(); ++it)
	{
        CUIStaticItem& si = **it;
		si.SetPos(x,y);
		si.Render(0, 0, iWidth, iHeight);

		x = x + iWidth + iIndent;
	}
}
