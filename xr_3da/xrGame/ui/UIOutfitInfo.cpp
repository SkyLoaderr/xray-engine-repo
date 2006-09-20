// File:		CUIOutfitInfo.cpp
// Description:	outfit info window for inventoryWnd.
// Created:		31.01.2005
// Author:		Serge Vynnychenko
// Mail:		narrator@gsc-game.kiev.ua

// Copyright 2005 GSC Game World

#include "StdAfx.h"
#include "UIOutfitInfo.h"
#include "../HUDManager.h"
#include "../level.h"
#include "../actor.h"
#include "../game_base_space.h"

#define LIST_ITEM_HEIGHT	15
#define TITLE_INDENT_Y		34
#define TITLE_INDENT_X		132

CUIOutfitInfo::CUIOutfitInfo(){
	AttachChild(&m_listWnd);
	AttachChild(&m_staticTitle);
	m_pFont = UI()->Font()->pFontLetterica16Russian;
	m_textColor = 0xfff0D9B6;
}

CUIOutfitInfo::~CUIOutfitInfo(){
}

#define ADD_ITEM(x)	x = xr_new<CUIListItem>();		\
					m_listWnd.AddItem(x);			\
					x->SetFont(m_pFont);			\
					x->SetAutoDelete(true);			\
					x->SetTextColor(m_textColor)

void CUIOutfitInfo::Init(float x, float y, float width, float height){
	CUIWindow::Init(x, y, width, height);
	// init list
	
	if(UI()->is_16_9_mode() )
		m_staticTitle.Init(TITLE_INDENT_X/1.328f, 0, width, 40);
	else
		m_staticTitle.Init(TITLE_INDENT_X, 0, width, 40);

	m_listWnd.Init(0, TITLE_INDENT_Y, width, height);
	m_listWnd.SetItemHeight(LIST_ITEM_HEIGHT);
	m_listWnd.Enable(false);
		
	// header
	m_staticTitle.SetFont(UI()->Font()->pFontGraffiti22Russian);
	m_staticTitle.SetTextColor(0xffE79916);
	
	if (GameID() == GAME_SINGLE)
	{
		ADD_ITEM(m_itemBurnProtection);
		ADD_ITEM(m_itemChemicalBurnProtection);
	}

	ADD_ITEM(m_itemExplosionProtection);
	ADD_ITEM(m_itemFireWoundProtection);
	ADD_ITEM(m_itemShockProtection);
	ADD_ITEM(m_itemStrikeProtection);
}

void CUIOutfitInfo::SetText(){
#ifdef DEBUG
	R_ASSERT2(m_itemExplosionProtection,"Need Call Init first");
#endif

	// caption
	m_staticTitle.SetText			(*CStringTable().translate("ui_inv_outfit_params"));

	// details	
	m_strBurnProtection				= *CStringTable().translate("ui_inv_outfit_burn_protection");	
	m_strChemicalBurnProtection		= *CStringTable().translate("ui_inv_outfit_chemical_burn_protection");
	m_strExplosionProtection		= *CStringTable().translate("ui_inv_outfit_explosion_protection");
	m_strFireWoundProtection		= *CStringTable().translate("ui_inv_outfit_fire_wound_protection");
	m_strShockProtection			= *CStringTable().translate("ui_inv_outfit_shock_protection");
	m_strStrikeProtection			= *CStringTable().translate("ui_inv_outfit_strike_protection");
}

void CUIOutfitInfo::Update(){
	// void
}

void CUIOutfitInfo::SetFont(CGameFont* pFont){
	if (m_itemBurnProtection)
	{
		if (GameID() == GAME_SINGLE)
		{
			m_itemBurnProtection->SetFont	(pFont);
			m_itemChemicalBurnProtection->SetFont(pFont);
		}
		
		m_itemExplosionProtection->SetFont	(pFont);
		m_itemFireWoundProtection->SetFont	(pFont);
		m_itemShockProtection->SetFont		(pFont);
		m_itemStrikeProtection->SetFont		(pFont);
	}
	else
		m_pFont = pFont;
}

void CUIOutfitInfo::SetTextColor(u32 color){
	if (m_itemBurnProtection)
	{
		if (GameID() == GAME_SINGLE)
		{
			m_itemBurnProtection->SetTextColor(color);
            m_itemChemicalBurnProtection->SetTextColor(color);
		}
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
	if (GameID() == GAME_SINGLE)
	{
        SetItem(ALife::eHitTypeBurn,		 outfit, m_itemBurnProtection,			m_strBurnProtection);
        SetItem(ALife::eHitTypeChemicalBurn, outfit, m_itemChemicalBurnProtection,	m_strChemicalBurnProtection);
	}
	SetItem(ALife::eHitTypeExplosion,	 outfit, m_itemExplosionProtection,		m_strExplosionProtection);
	SetItem(ALife::eHitTypeFireWound,	 outfit, m_itemFireWoundProtection,		m_strFireWoundProtection);
	SetItem(ALife::eHitTypeShock,		 outfit, m_itemShockProtection,			m_strShockProtection);
	SetItem(ALife::eHitTypeStrike,		 outfit, m_itemStrikeProtection,		m_strStrikeProtection);
}

void CUIOutfitInfo::SetItem(ALife::EHitType hitType, CCustomOutfit& outfit, CUIListItem* listItem, shared_str sstr){
	char	str[256];
	float	fValue;
	int		iValue; char	strIVal[10];

	strcpy(str, *sstr);
	fValue = &outfit ? outfit.GetDefHitTypeProtection(hitType) : 1.0f;
	iValue = 100-( iFloor(fValue*100+0.5f) );
	strcat(str, "  + ");
	strcat(str, itoa(iValue, strIVal, 10));
	strcat(str, "%");
//	listItem->SetTextComplexMode(false);	

	if (GameID() == GAME_SINGLE){
		fValue = Actor()->HitArtefactsOnBelt(1, hitType);
		iValue = 100-( iFloor(fValue*100+0.5f) );

        if (iValue)
		{
			strcat(str, "%c<green> + ");
			strcat(str, itoa(iValue, strIVal, 10));
			strcat(str, "%");
		}
	}

	listItem->SetText(str);
}

void CUIOutfitInfo::SetItem(ALife::EHitType hitType, int value,			 CUIListItem* listItem, shared_str sstr){
	char	str[256];
	char	buff[16];

	strcpy(str, *sstr);
	strcat(str, "  +");
	strcat(str, itoa(value, buff, 10));
	strcat(str, "%");
	listItem->SetText(str);
}

