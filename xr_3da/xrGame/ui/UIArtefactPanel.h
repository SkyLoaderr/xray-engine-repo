// File:        UIArtefactPanel.h
// Description: HUD artefact panel
// Created:     14.12.2004
// Author:      Serhiy 0. Vynnychenk0
// Mail:        narrator@gsc-game.kiev.ua
//
// Copyright 2004 GSC Game World
//

#pragma once
#include "UIWindow.h"
#include "UIStatic.h"
#include "../UIStaticItem.h"

class CUIArtefactPanel : public CUIWindow{
	typedef xr_vector<RECT>::const_iterator ITr;
	typedef xr_vector<CUIStaticItem*>::const_iterator ITsi;

public:
	// construction/destruction
	CUIArtefactPanel();
	~CUIArtefactPanel();

	virtual void InitIcons(const xr_vector<RECT>& vector);
	virtual void Draw();

protected:
	xr_vector<RECT>           m_vRects;
	CUIStaticItem             m_si;

};