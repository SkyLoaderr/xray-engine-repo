#include "stdafx.h"
#include "UIBuyMenu.h"
#include "HUDManager.h"

#define BUY_MENU_OFFS 200
//--------------------------------------------------------------------
CUIBuyMenu::CUIBuyMenu()
{
	CHUDManager* HUD	= (CHUDManager*)Level().HUD();
	menu_offs			= HUD->ClientToScreenScaledY(BUY_MENU_OFFS,alLeft|alTop)/HUD->pHUDFont->GetScale();
	menu_root			= 0;
	menu_active			= 0;
}
//--------------------------------------------------------------------

CUIBuyMenu::~CUIBuyMenu()
{
	_DELETE(menu_root);
}
//--------------------------------------------------------------------

void CUIBuyMenu::Load()
{
	menu_root = new CMenuItem("root",0);
	CMenuItem* I=0;
	I = new CMenuItem("Pistols",0);
	I->AppendItem(new CMenuItem("PM",200));
	I->AppendItem(new CMenuItem("FORT",350));
	menu_root->AppendItem(I);
	I = new CMenuItem("Automats",0);
	I->AppendItem(new CMenuItem("AK-74",1000));
	I->AppendItem(new CMenuItem("LR-300",1500));
	I->AppendItem(new CMenuItem("FN-200",2500));
	menu_root->AppendItem(I);
	menu_active = menu_root;
}
//--------------------------------------------------------------------

void CUIBuyMenu::OnFrame()
{
	if (menu_active){
		CFontHUD* F		= (CFontHUD*)Level().HUD()->pHUDFont;
		F->OutSet		(0,menu_offs);
		F->Color		(0xFFFFFFFF);
		int k=0;
		for (MIIt it=menu_active->items.begin(); it!=menu_active->items.end(); it++,k++)
			(*it)->OnItemDraw(F,k);
	}
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
