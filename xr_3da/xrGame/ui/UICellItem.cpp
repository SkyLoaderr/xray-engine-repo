#include "stdafx.h"
#include "UICellItem.h"
#include "../xr_level_controller.h"
#include "../../xr_input.h"
#include "../HUDManager.h"
#include "../level.h"

CUICellItem::CUICellItem(void* pData)
{
	m_pData.clear		();
//	m_parentItm			= NULL;
}

CUICellItem::~CUICellItem()
{
}

void CUICellItem::InitInternals()
{
	inherited::Init									("ui\\ui_icon_equipment",0,0,50,50);
	Frect rect; rect.set							(500,450,550,500);
	inherited::SetOriginalRect						(rect);
	inherited::SetStretchTexture					(true);
}

void CUICellItem::Draw()
{
	inherited::Draw();
};
void CUICellItem::OnMouse(float x, float y, EUIMessages mouse_action)
{
	if(mouse_action == WINDOW_LBUTTON_DOWN){
		GetMessageTarget()->SendMessage(this,DRAG_DROP_ITEM_SELECTED,NULL);
	}
	if(mouse_action == WINDOW_MOUSE_MOVE && pInput->iGetAsyncBtnState(0)){
		GetMessageTarget()->SendMessage(this,DRAG_DROP_ITEM_DRAG,NULL);
	}
};

CUIFlyingItem* CUICellItem::CreateFlyingItem()
{
	CUIFlyingItem* res = xr_new<CUIFlyingItem>(this);
	res->GetUIStaticItem().SetShader(GetShader());
	res->SetOriginalRect(GetUIStaticItem().GetOriginalRect());
	res->SetWndSize(GetWndSize());
	res->SetWndPos(GetAbsolutePos());
	res->TextureAvailable			(true);
	res->TextureOn					();
	return res;
}



CUIFlyingItem::CUIFlyingItem(CUICellItem* parent)
{
	m_pParent = parent;
	HUD().GetUI()->AddDialogToRender(this);
	GetUICursor()->SetMoveReceiver(CUICursor::CURSOR_MOVE_EVENT(this,&CUIFlyingItem::OnMouseMoved));
}

CUIFlyingItem::~CUIFlyingItem()
{
	HUD().GetUI()->RemoveDialogToRender(this);

}

void CUIFlyingItem::OnMouse(float x, float y, EUIMessages mouse_action)
{
//	if(mouse_action == WINDOW_MOUSE_MOVE){
//		MoveWndDelta(GetUICursor()->GetPosDelta());
//	}
}

void CUIFlyingItem::OnMouseMoved(Fvector2 delta)
{
	MoveWndDelta(delta);
}

void CUIFlyingItem::Draw()
{
	UI()->PushScissor(UI()->ScreenRect(),true);

	inherited::Draw();

	UI()->PopScissor();
}
