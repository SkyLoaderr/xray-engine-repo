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
#include "UIInventoryUtilities.h"

using namespace InventoryUtilities;

CUIArtefactPanel::CUIArtefactPanel(){	
}

CUIArtefactPanel::~CUIArtefactPanel(){

}

void CUIArtefactPanel::InitIcons(const xr_vector<RECT>& vect){

	m_si.SetShader(GetEquipmentIconsShader());

	this->m_vRects.clear();
	this->m_vRects.assign(vect.begin(), vect.end());	
}

void CUIArtefactPanel::Draw(){
	const int iIndent = 8;
	      int x = 0;
		  int y = 0;

	RECT rect = GetAbsoluteRect();
	x = rect.left;
	y = rect.top;	

	for (ITr it = m_vRects.begin(); it != m_vRects.end(); ++it)
	{
		const RECT& r = *it;		

		m_si.SetOriginalRect(r.left, r.top, 0, 0);
		m_si.SetRect(0, 0, r.right - r.left, r.bottom - r.top);

		m_si.SetPos(x, y);

		x = x + iIndent + (r.right - r.left);

        m_si.Render();
	}

	CUIWindow::Draw();
}
