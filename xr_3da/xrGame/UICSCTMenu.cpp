#include "stdafx.h"
#include "UIBuyMenu.h"
#include "HUDManager.h"
#include "UICustomMenu.h"
#include "..\\xr_trims.h"

#define BUY_MENU_OFFS		200
#define BUY_MENU_OFFS_COL1	0
#define BUY_MENU_OFFS_COL2	200
//--------------------------------------------------------------------
BOOL CUIGameCS::CanBuy()
{
	game_cl_GameState::Player* P = Game().local_player;
	return (P->flags&GAME_PLAYER_FLAG_CS_ON_BASE)&&((Level().timeServer()-Game().start_time)<Game().buy_time);
}
//--------------------------------------------------------------------

void CUIGameCS::BuyItem(CCustomMenuItem* sender)
{
	CUIGameCS* G	= (CUIGameCS*)sender->tag; R_ASSERT(G);
	if (G)			G->SetFlag(CUIGameCustom::flShowBuyMenu,FALSE);

	if (G->CanBuy()&&sender->value1){
		string64 buf;
		sprintf(buf,"%s/cost=%s",sender->value1,sender->value0);
		// buy item
		CEntityAlive* E	= dynamic_cast<CEntityAlive*>(Level().CurrentEntity());
		if (E)		E->BuyItem(buf);
	}
}
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

void CUIBuyMenu::Load	(LPCSTR ini, CUIGameCustom* parent, OnExecuteEvent exec)
{
	// check ini exist
	menu_root			= UILoadMenu(ini,(int)parent,exec);
	menu_active			= menu_root;
}
//--------------------------------------------------------------------

void CUIBuyMenu::OnFrame()
{
	if (menu_active){
		CGameFont* F	= Level().HUD()->pFontMedium;
		F->SetColor		(0xFFFFFFFF);
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
				F->OutNext("%-2d %-20s",0,"Exit");
			}
		}
	}
}
//--------------------------------------------------------------------

void CUIBuyMenu::Render	()
{
}
//--------------------------------------------------------------------
bool CUIBuyMenu::OnKeyboardPress(int dik)
{
	int id = -1;
	switch (dik){
	case DIK_0:{
		CUIGameCustom* G	= Level().HUD()->GetUI()->UIGame();
		if (G)				G->SetFlag(CUIGameCustom::flShowBuyMenu,FALSE);
	}break;
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
		if (menu){
			if (menu->HasChildren())menu_active = menu;
			else menu->Execute();
			return true;
		}
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
