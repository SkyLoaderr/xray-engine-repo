#include "stdafx.h"
#include "uigamesp.h"

CUIGameSP::CUIGameSP(CUI* parent) : CUIGameCustom(parent) {
}

CUIGameSP::~CUIGameSP(void) {
}

void CUIGameSP::OnFrame() {
	if (InventoryMenu.Visible()) InventoryMenu.OnFrame();
}

bool CUIGameSP::OnKeyboardPress(int dik) {
	if (InventoryMenu.Visible()&&InventoryMenu.OnKeyboardPress(dik)) return true;
	switch (dik){
	case DIK_I: InventoryMenu.Show(); return true;
	}
	return false;
}

bool CUIGameSP::OnKeyboardRelease(int dik) {
	if (InventoryMenu.Visible()&&InventoryMenu.OnKeyboardRelease(dik)) return true;
	return false;
}
