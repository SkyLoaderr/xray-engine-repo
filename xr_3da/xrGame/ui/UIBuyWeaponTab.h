// File:		UIBuyWeaponTab.h
// Description:	Tab conorol for BuyWeaponWnd
// Created:		07.02.2005
// Author:		Serge Vynnychenko
// Mail:		narrator@gsc-game.kiev.ua

// Copyright 2005 GSC Game World

#pragma once

#include "UITabControl.h"
#include "UIMultiTextStatic.h"
#include "xrXMLParser.h"
#include "UIXmlInit.h"

class CUIBuyWeaponTab : public CUITabControl {
public:
	CUIBuyWeaponTab();
	virtual ~CUIBuyWeaponTab();

	virtual void Init(CUIXml& xml, char* path);
	virtual void OnTabChange(int iCur, int iPrev);
			void SetActiveState(bool bState = true);
private:
	void virtual Init(int x, int y, int width, int height);

	bool				m_bActiveState;
	CUIStatic			m_staticBackground;
};
