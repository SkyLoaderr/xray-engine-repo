// File:		CUIOutfitInfo.h
// Description:	outfit info window for inventoryWnd.
// Created:		31.01.2005
// Author:		Serge Vynnychenko
// Mail:		narrator@gsc-game.kiev.ua

// Copyright 2005 GSC Game World

#pragma once

#include "UIListWnd.h"
#include "../CustomOutfit.h"
#include "../string_table.h"

class CUIOutfitInfo : public CUIStatic {
public:
	CUIOutfitInfo();
	~CUIOutfitInfo();
	virtual void Init(int x, int y, int widht, int height);

	virtual void Update();
	virtual void Update(CCustomOutfit& outfit);	
    virtual void SetText(const CStringTable& stringTable);
	virtual void SetTextColor(u32 color);
	virtual void SetTitleTextColor(u32 color);
	virtual void SetFont(CGameFont* pFont);
protected:
	// methods
	void SetItem(ALife::EHitType hitType, CCustomOutfit& outfit, CUIListItem* listItem, shared_str sstr);

	// data
	CUIListWnd m_listWnd;
	CUIStatic		m_staticTitle;
	CUIListItem*	m_itemBurnProtection;
	CUIListItem*	m_itemStrikeProtection;
	CUIListItem*	m_itemShockProtection;
	CUIListItem*	m_itemChemicalBurnProtection;
	CUIListItem*	m_itemExplosionProtection;
	CUIListItem*	m_itemFireWoundProtection;	

	shared_str	m_strBurnProtection;
	shared_str	m_strStrikeProtection;
	shared_str	m_strShockProtection;
	shared_str	m_strChemicalBurnProtection;
	shared_str	m_strExplosionProtection;
	shared_str	m_strFireWoundProtection;

	CGameFont*		m_pFont;
	u32				m_textColor;
};