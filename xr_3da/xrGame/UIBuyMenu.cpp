#include "stdafx.h"
#include "UIBuyMenu.h"
#include "HUDManager.h"

#define BUY_MENU_OFFS 200
//--------------------------------------------------------------------
CUIBuyMenu::CUIBuyMenu()
{
	CHUDManager* HUD	= (CHUDManager*)Level().HUD();
	menu_offs			= HUD->ClientToScreenScaledY(BUY_MENU_OFFS,alLeft|alTop)/HUD->pHUDFont->GetScale();
}
//--------------------------------------------------------------------

CUIBuyMenu::~CUIBuyMenu()
{
}
//--------------------------------------------------------------------

void CUIBuyMenu::Load()
{
/*
    menu_items.push_back(CMenuItem("Pistols"));
	CMenuItem& M = menu_items.back();
	M.AppendItem(CValueItem("PM",	200));
	M.AppendItem(CValueItem("FORT",	350));
	menu_items.push_back(CMenuItem("Automats"));
	M = menu_items.back();
	M.AppendItem(CValueItem("AK-74",	1000));
	M.AppendItem(CValueItem("LR-300",	1500));
	M.AppendItem(CValueItem("FN-2000",	2500));
*/
}
//--------------------------------------------------------------------

void CUIBuyMenu::OnFrame()
{
/*
	if (menu_active){
		CFontHUD* F		= (CFontHUD*)Level().HUD()->pHUDFont;
		F->OutSet		(0,menu_offs);
		F->Color		(0xFFFFFFFF);
		int k=0;
		for (MIIt it=menu_active->menu_items.begin(); it!=menu_active->menu_items.end(); it++,k++){
			F->OutNext	("%-2d: %-32s",k,it->caption);
		}
	}
	F->OutNext			("1: FN 2000");
	F->OutNext			("2: LR 300");
	F->OutNext			("3: PM");
*/
}
//--------------------------------------------------------------------

void CUIBuyMenu::Render()
{
}
//--------------------------------------------------------------------
bool CUIBuyMenu::OnKeyboardPress(int dik)
{
	switch (dik){
	}
	return false;
}
//--------------------------------------------------------------------

bool CUIBuyMenu::OnKeyboardRelease(int dik)
{
	switch (dik){
	}
	return false;
}
//--------------------------------------------------------------------

bool CUIBuyMenu::OnMouseMove	(int dx, int dy)
{
	return false;
}
//--------------------------------------------------------------------
