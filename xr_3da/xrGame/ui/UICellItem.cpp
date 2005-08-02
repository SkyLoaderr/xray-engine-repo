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

bool CUICellItem::OnMouse(float x, float y, EUIMessages mouse_action)
{
	if(mouse_action == WINDOW_LBUTTON_DOWN){
		GetMessageTarget()->SendMessage(this,DRAG_DROP_ITEM_SELECTED,NULL);
		return true;
	}
	if(mouse_action == WINDOW_MOUSE_MOVE && pInput->iGetAsyncBtnState(0)){
		GetMessageTarget()->SendMessage(this,DRAG_DROP_ITEM_DRAG,NULL);
		return true;
	}
	return false;
};

CUIFlyingItem* CUICellItem::CreateFlyingItem()
{
	CUIFlyingItem* res = xr_new<CUIFlyingItem>(this);
	res->Init(GetShader(),GetAbsoluteRect(),GetUIStaticItem().GetOriginalRect());
	return res;
}



CUIFlyingItem::CUIFlyingItem(CUICellItem* parent)
{
	m_pParent						= parent;
	m_static.SetAutoDelete			(false);
	AttachChild						(&m_static);
	HUD().GetUI()->StartStopMenu	(this,false);
//	GetUICursor()->SetMoveReceiver(CUICursor::CURSOR_MOVE_EVENT(this,&CUIFlyingItem::OnMouseMoved));
}

CUIFlyingItem::~CUIFlyingItem()
{
	HUD().GetUI()->StartStopMenu	(this,false);
}

void CUIFlyingItem::Init(const ref_shader& sh, const Frect& rect, const Frect& text_rect)
{
	SetWndRect						(rect);
	m_static.SetShader				(sh);
	m_static.SetOriginalRect		(text_rect);
	m_static.SetWndPos				(0.0f,0.0f);
	m_static.SetWndSize				(GetWndSize());
	m_static.TextureAvailable		(true);
	m_static.TextureOn				();

}

bool CUIFlyingItem::OnMouse(float x, float y, EUIMessages mouse_action)
{
	if(mouse_action == WINDOW_MOUSE_MOVE){
		MoveWndDelta(GetUICursor()->GetPosDelta());
		return true;
	}
	return false;
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
