#include "stdafx.h"
#include "UIBuyMenu.h"
#include "HUDManager.h"
#include "..\\xr_trims.h"
#include <functional>

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
void CUIBuyMenu::ParseMenu(CInifile* ini, CMenuItem* root, LPCSTR sect)
{
	CMenuItem* I=0;
	string256 buf,buf1;
	int ln_cnt = ini->LineCount(sect);
	for (int i=1; i<ln_cnt; i++){
		// append if exist menu item
		sprintf(buf,"menu_%d",i);
		if (ini->LineExists(sect,buf)){
			LPCSTR line		= ini->ReadSTRING(sect,buf);	R_ASSERT(_GetItemCount(line)==2);
			LPCSTR	name	= _GetItem(line,0,buf);
			LPCSTR	new_sect= strlwr(_GetItem(line,1,buf1));
			I				= new CMenuItem(root,name,0);
			ParseMenu(ini,I,new_sect);
			root->AppendItem(I);
		}
		// append if exist value item
		sprintf(buf,"item_%d",i);
		if (ini->LineExists(sect,buf)){
			LPCSTR line		= ini->ReadSTRING(sect,buf);	R_ASSERT(_GetItemCount(line)==2);
			LPCSTR	name	= _GetItem(line,0,buf);
			int		val		= atoi(_GetItem(line,1,buf1));
			root->AppendItem(new CMenuItem(root,name,val,BuyItem));
		}
	}
}
//--------------------------------------------------------------------

void CUIBuyMenu::Load()
{
	// check ini exist
	string256 fn;
	if (Engine.FS.Exist(fn,Path.GameData,"cs.ltx")){
		menu_root = new CMenuItem(0,"root",0);
		CInifile* ini			= CInifile::Create(fn);
		ParseMenu(ini,menu_root,"cs_buy_menu");
		CInifile::Destroy		(ini);
	}
	menu_active = menu_root;
}
//--------------------------------------------------------------------

void CUIBuyMenu::OnFrame()
{
	if (menu_active){
		CGameFont* F	= Level().HUD()->pHUDFont;
		F->OutSet		(0,menu_offs);
		F->Color		(0xFFFFFFFF);
		if (menu_active->m_Parent)	F->OutNext("Buy %-20s %6s",menu_active->caption,"$ Cost");
		else						F->OutNext("Buy Items");
		F->OutSkip		();
		int k=1;
		for (MIIt it=menu_active->items.begin(); it!=menu_active->items.end(); it++,k++)
			(*it)->OnItemDraw(F,k);
		F->OutSkip		();
		F->OutNext		("%-2d. %-20s",0,"Exit");
	}
}
//--------------------------------------------------------------------

void CUIBuyMenu::Render()
{
}
//--------------------------------------------------------------------
void CUIBuyMenu::BackItem(CMenuItem* sender)
{
}
//--------------------------------------------------------------------
void CUIBuyMenu::BuyItem(CMenuItem* sender)
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
		CMenuItem* menu		= menu_active->GetItem(id);
		R_ASSERT(menu);
		if (menu->IsMenu())	menu_active = menu;
		else menu->Execute();
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
