// File:		CUIOutfitInfo.cpp
// Description:	outfit info window for inventoryWnd.
// Created:		31.01.2005
// Author:		Serge Vynnychenko
// Mail:		narrator@gsc-game.kiev.ua

// Copyright 2005 GSC Game World

#include "StdAfx.h"
#include "UIOutfitInfo.h"
#include "../HUDManager.h"
#include "../MainUI.h"

#define LIST_ITEM_HEIGHT	15
#define TITLE_INDENT_Y		32
#define TITLE_INDENT_X		10

CUIOutfitInfo::CUIOutfitInfo(){
	AttachChild(&m_listWnd);
	AttachChild(&m_staticTitle);
	m_pFont = UI()->Font()->pFontMedium;
	m_textColor = 0xfff0f0f0;
}

CUIOutfitInfo::~CUIOutfitInfo(){
}

void CUIOutfitInfo::Init(int x, int y, int width, int height){
	CUIWindow::Init(x, y, width, height);
	// init Caption
	m_staticTitle.Init(TITLE_INDENT_X, 0, width, 40);
	// init list
	m_listWnd.Init(0, TITLE_INDENT_Y, width, height);
	m_listWnd.SetItemHeight(LIST_ITEM_HEIGHT);
	m_listWnd.Enable(false);
		
	// header
	m_staticTitle.SetFont(UI()->Font()->pFontLetterica25);
	m_staticTitle.SetTextColor(0xfff0f0f0);

	m_listWnd.AddItem(m_itemBurnProtection = xr_new<CUIListItem>());		
	m_itemBurnProtection->SetFont(m_pFont);
	m_itemBurnProtection->SetTextColor(m_textColor);

	m_listWnd.AddItem(m_itemChemicalBurnProtection = xr_new<CUIListItem>());
	m_itemChemicalBurnProtection->SetFont(m_pFont);
	m_itemChemicalBurnProtection->SetTextColor(m_textColor);

	m_listWnd.AddItem(m_itemExplosionProtection = xr_new<CUIListItem>());
	m_itemExplosionProtection->SetFont(m_pFont);
	m_itemExplosionProtection->SetTextColor(m_textColor);

	m_listWnd.AddItem(m_itemFireWoundProtection = xr_new<CUIListItem>());
	m_itemFireWoundProtection->SetFont(m_pFont);
	m_itemFireWoundProtection->SetTextColor(m_textColor);

	m_listWnd.AddItem(m_itemShockProtection = xr_new<CUIListItem>());
	m_itemShockProtection->SetFont(m_pFont);
	m_itemShockProtection->SetTextColor(m_textColor);

	m_listWnd.AddItem(m_itemStrikeProtection = xr_new<CUIListItem>());
	m_itemStrikeProtection->SetFont(m_pFont);
	m_itemStrikeProtection->SetTextColor(m_textColor);	
}

void CUIOutfitInfo::SetText(const CStringTable& stringTable){
#ifdef DEBUG
	R_ASSERT2(m_itemBurnProtection,"Need Call Init first");
#endif

	// caption
	m_staticTitle.SetText(*stringTable("ui_inv_outfit_params"));

	// details	
	m_strBurnProtection = *stringTable("ui_inv_outfit_burn_protection");	
	m_strChemicalBurnProtection = *stringTable("ui_inv_outfit_chemical_burn_protection");
	m_strExplosionProtection = *stringTable("ui_inv_outfit_explosion_protection");
	m_strFireWoundProtection = *stringTable("ui_inv_outfit_fire_wound_protection");
	m_strShockProtection = *stringTable("ui_inv_outfit_shock_protection");
	m_strStrikeProtection = *stringTable("ui_inv_outfit_strike_protection");
}

void CUIOutfitInfo::Update(){
	// void
}

void CUIOutfitInfo::SetFont(CGameFont* pFont){
	if (m_itemBurnProtection)
	{
		m_itemBurnProtection->SetFont(pFont);
		m_itemChemicalBurnProtection->SetFont(pFont);
		m_itemExplosionProtection->SetFont(pFont);
		m_itemFireWoundProtection->SetFont(pFont);
		m_itemShockProtection->SetFont(pFont);
		m_itemStrikeProtection->SetFont(pFont);
	}
	else
		m_pFont = pFont;
}

void CUIOutfitInfo::SetTextColor(u32 color){
	if (m_itemBurnProtection)
	{
		m_itemBurnProtection->SetTextColor(color);
		m_itemChemicalBurnProtection->SetTextColor(color);
		m_itemExplosionProtection->SetTextColor(color);
		m_itemFireWoundProtection->SetTextColor(color);
		m_itemShockProtection->SetTextColor(color);
		m_itemStrikeProtection->SetTextColor(color);
	}
	else
		m_textColor = color;
}

void CUIOutfitInfo::SetTitleTextColor(u32 color){
	m_staticTitle.SetTextColor(color);
}

void CUIOutfitInfo::Update(CCustomOutfit& outfit){
	SetItem(ALife::eHitTypeBurn,		 outfit, m_itemBurnProtection,			m_strBurnProtection);
	SetItem(ALife::eHitTypeChemicalBurn, outfit, m_itemChemicalBurnProtection,	m_strChemicalBurnProtection);
	SetItem(ALife::eHitTypeExplosion,	 outfit, m_itemExplosionProtection,		m_strExplosionProtection);
	SetItem(ALife::eHitTypeFireWound,	 outfit, m_itemFireWoundProtection,		m_strFireWoundProtection);
	SetItem(ALife::eHitTypeShock,		 outfit, m_itemShockProtection,			m_strShockProtection);
	SetItem(ALife::eHitTypeStrike,		 outfit, m_itemStrikeProtection,		m_strStrikeProtection);
}

void CUIOutfitInfo::SetItem(ALife::EHitType hitType, CCustomOutfit& outfit, CUIListItem* listItem, shared_str sstr){
	char	str[256];
	float	fValue;
	int		iValue; char	strIVal[10];
//	int		decimal,   sign;
//	char*	buffer;
//	int		precision = 2;

	strcpy(str, *sstr);
	fValue = &outfit ? outfit.GetHitTypeProtection(hitType) : 0;
	iValue = (int)(fValue*100);
//	buffer = _ecvt(fValue, precision, &decimal, &sign );
	strcat(str, "  +");
	strcat(str, itoa(iValue, strIVal, 10));
	strcat(str, "%");
	listItem->SetText(str);
}

