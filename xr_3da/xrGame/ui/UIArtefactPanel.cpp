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

CUIArtefactPanel::CUIArtefactPanel()
{		
	m_fScale = 1.0f;
}

CUIArtefactPanel::~CUIArtefactPanel()
{

}

void CUIArtefactPanel::SetScaleXY(float x, float y)
{	
	m_fScale = x;
}

void CUIArtefactPanel::InitIcons(const xr_vector<const CArtefact*>& artefacts)
{
	m_si.SetShader(GetEquipmentIconsShader());
	m_vRects.clear();
	
	for(xr_vector<const CArtefact*>::const_iterator it = artefacts.begin();
		it != artefacts.end(); it++)
	{
		const CArtefact* artefact = *it;
		Frect rect;
		rect.left = float(artefact->GetXPos()*INV_GRID_WIDTH);
		rect.top = float(artefact->GetYPos()*INV_GRID_HEIGHT);
		rect.right = rect.left + artefact->GetGridWidth()*INV_GRID_WIDTH;
		rect.bottom = rect.top + artefact->GetGridHeight()*INV_GRID_HEIGHT;
		m_vRects.push_back(rect);
	}
}

void CUIArtefactPanel::Draw(){
	const float iIndent = 1.0f;
	      float x = 0.0f;
		  float y = 0.0f;
		  float iHeight;
		  float iWidth;

	Frect rect;
	GetAbsoluteRect(rect);
	x = rect.left;
	y = rect.top;	

	for (ITr it = m_vRects.begin(); it != m_vRects.end(); ++it)
	{
		const Frect& r = *it;		

		iHeight = m_fScale*(r.bottom - r.top);
		iWidth  = m_fScale*(r.right - r.left);

		m_si.SetOriginalRect(r.left, r.top, r.width(), r.height());
		m_si.SetRect(0, 0, iWidth, iHeight);

		m_si.SetPos(x, y);
		x = x + iIndent + iWidth;

        m_si.Render();
	}

	CUIWindow::Draw();
}
