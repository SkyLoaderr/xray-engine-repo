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

#include "../artifact.h"

using namespace InventoryUtilities;

CUIArtefactPanel::CUIArtefactPanel(){		
}

CUIArtefactPanel::~CUIArtefactPanel(){

}

void CUIArtefactPanel::SetScaleXY(float x, float y){	
//.	m_si.SetScaleXY(x, y);
}

void CUIArtefactPanel::InitIcons(const xr_vector<const CArtefact*>& artefacts)
{
	m_si.SetShader(GetEquipmentIconsShader());
	m_vRects.clear();
	
	for(xr_vector<const CArtefact*>::const_iterator it = artefacts.begin();
		it != artefacts.end(); it++)
	{
		const CArtefact* artefact = *it;
		Irect rect;
		rect.left = artefact->GetXPos()*INV_GRID_WIDTH;
		rect.top = artefact->GetYPos()*INV_GRID_HEIGHT;
		rect.right = rect.left + artefact->GetGridWidth()*INV_GRID_WIDTH;
		rect.bottom = rect.top + artefact->GetGridHeight()*INV_GRID_HEIGHT;
		m_vRects.push_back(rect);
	}
}

void CUIArtefactPanel::Draw(){
	const int iIndent = 8;
	      int x = 0;
		  int y = 0;
		  int iHeight;
		  int iWidth;

	Irect rect = GetAbsoluteRect();
	x = rect.left;
	y = rect.top;	

	for (ITr it = m_vRects.begin(); it != m_vRects.end(); ++it)
	{
		const Irect& r = *it;		

		iHeight = (int)(m_si.GetScaleY()*(r.bottom - r.top));
		iWidth  = (int)(m_si.GetScaleX()*(r.right - r.left));

		m_si.SetOriginalRect(r.left, r.top, r.width(), r.height());
		m_si.SetRect(0, 0, iWidth, iHeight);

		m_si.SetPos(x, y);
		x = x + iIndent + iWidth;

        m_si.Render();
	}

	CUIWindow::Draw();
}
