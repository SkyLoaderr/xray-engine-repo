#include "stdafx.h"
#include "uigamesp.h"

CUIGameSP::CUIGameSP(CUI* parent) : CUIGameCustom(parent) {
}

CUIGameSP::~CUIGameSP(void) {
}

void CUIGameSP::OnFrame() {
	if (InventoryMenu.Visible()) InventoryMenu.OnFrame();
}

bool CUIGameSP::IR_OnKeyboardPress(int dik) {
	if (InventoryMenu.Visible()&&InventoryMenu.IR_OnKeyboardPress(dik)) return true;
	switch (dik){
	case DIK_I: InventoryMenu.Show(); return true;
	}
	return false;
}

bool CUIGameSP::IR_OnKeyboardRelease(int dik) {
	if (InventoryMenu.Visible()&&InventoryMenu.IR_OnKeyboardRelease(dik)) return true;
	return false;
}
