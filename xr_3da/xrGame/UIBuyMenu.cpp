#include "stdafx.h"
#include "UIBuyMenu.h"
#include "HUDManager.h"
#include "UICustomMenu.h"
#include "..\\xr_trims.h"

#define BUY_MENU_OFFS		200
#define BUY_MENU_OFFS_COL1	0
#define BUY_MENU_OFFS_COL2	200
//--------------------------------------------------------------------
CUIBuyMenu::CUIBuyMenu	()
{
	CHUDManager* HUD	= (CHUDManager*)Level().HUD();
	menu_offs			= HUD->ClientToScreenY(BUY_MENU_OFFS,alLeft|alTop);
	menu_offs_col[0]	= HUD->ClientToScreenX(BUY_MENU_OFFS_COL1,alLeft|alTop);
	menu_offs_col[1]	= HUD->ClientToScreenX(BUY_MENU_OFFS_COL2,alLeft|alTop);
	menu_root			= 0;
	menu_active			= 0;
}
//--------------------------------------------------------------------

CUIBuyMenu::~CUIBuyMenu	()
{
	_DELETE(menu_root);
}
//--------------------------------------------------------------------

void CUIBuyMenu::Load	()
{
	// check ini exist
	menu_root			= UILoadMenu("cs.ltx",BuyItem);
	menu_active			= menu_root;
}
//--------------------------------------------------------------------

void CUIBuyMenu::OnFrame()
{
	if (menu_active){
		CGameFont* F	= Level().HUD()->pFontMedium;
		F->Color		(0xFFFFFFFF);
		if (menu_active->Parent()){
			F->OutSet	(float(menu_offs_col[1]),float(menu_offs));
			F->OutNext	("$ Cost");
		}
		F->OutSet		(float(menu_offs_col[0]),float(menu_offs));
		F->OutNext		("Buy %s",menu_active->caption);
		for (int col=0; col<2; col++){
			F->OutSet	(float(menu_offs_col[col]),float(menu_offs));
			F->OutSkip	(1.5f);
			int k=1;
			for (MIIt it=menu_active->FirstItem(); it!=menu_active->LastItem(); it++,k++)
				(*it)->DrawItem(F,k,col);
			if (0==col){
				F->OutSkip(0.5f);
				F->OutNext("%-2d. %-20s",0,"Exit");
			}
		}
	}
}
//--------------------------------------------------------------------

void CUIBuyMenu::Render	()
{
}
//--------------------------------------------------------------------
void CUIBuyMenu::BuyItem(CCustomMenuItem* sender)
{
	//
	Level().HUD()->GetUI()->ShowBuyMenu(FALSE);
}
//--------------------------------------------------------------------
bool CUIBuyMenu::OnKeyboardPress(int dik)
{
	int id = -1;
	switch (dik){
	case DIK_0: Level().HUD()->GetUI()->ShowBuyMenu(FALSE); break;
	case DIK_1: id=1; break;
	case DIK_2: id=2; break;
	case DIK_3: id=3; break;
	case DIK_4: id=4; break;
	case DIK_5: id=5; break;
	case DIK_6: id=6; break;
	case DIK_7: id=7; break;
	case DIK_8: id=8; break;
	case DIK_9: id=9; break;
	}
	if (menu_active&&(id>0)){
		CCustomMenuItem* menu	= menu_active->GetItem(id);
		R_ASSERT(menu);
		if (menu->HasChildren())menu_active = menu;
		else menu->Execute();
	}
	return false;
}
//--------------------------------------------------------------------

bool CUIBuyMenu::OnKeyboardRelease(int dik)
{
//	switch (dik){
//	}
	return false;
}
//--------------------------------------------------------------------

bool CUIBuyMenu::OnMouseMove(int dx, int dy)
{
	return false;
}
//--------------------------------------------------------------------
