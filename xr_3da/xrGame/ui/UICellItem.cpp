#include "stdafx.h"
#include "UICellItem.h"
#include "../xr_level_controller.h"
#include "../../xr_input.h"
#include "../HUDManager.h"
#include "../level.h"
#include "../object_broker.h"

CUICellItem::CUICellItem()
{
	m_pParentList	= NULL;
	m_pData			= NULL;
}

CUICellItem::~CUICellItem()
{
	delete_data(m_childs);
}

int kk = 0;
void CUICellItem::InitInternals()
{
	inherited::InitTexture							("ui\\ui_icon_equipment");
	Frect rect; rect.set							(500,450,550,500);
	inherited::SetOriginalRect						(rect);
	inherited::SetStretchTexture					(true);
	++kk;
	switch(kk%4){
	
		case 0:
			m_grid_size.set										(1,1);
			break;
		case 1:
			m_grid_size.set										(2,1);
			break;

		case 2:
			m_grid_size.set										(1,2);
			break;

		case 3:
			m_grid_size.set										(2,2);
			break;
	}
}

void CUICellItem::Draw()
{
	inherited::Draw();
};

bool CUICellItem::OnMouse(float x, float y, EUIMessages mouse_action)
{
	if(mouse_action == WINDOW_LBUTTON_DOWN){
		GetMessageTarget()->SendMessage(this, DRAG_DROP_ITEM_SELECTED, NULL);
		return true;
	}else
	if(mouse_action == WINDOW_MOUSE_MOVE && pInput->iGetAsyncBtnState(0)){
		GetMessageTarget()->SendMessage(this, DRAG_DROP_ITEM_DRAG, NULL);
		return true;
	}else
	if(mouse_action==WINDOW_LBUTTON_DB_CLICK){
		GetMessageTarget()->SendMessage(this, DRAG_DROP_ITEM_DB_CLICK, NULL);
		return true;
	}else
	if(mouse_action==WINDOW_RBUTTON_DOWN){
		GetMessageTarget()->SendMessage(this, DRAG_DROP_ITEM_RBUTTON_CLICK, NULL);
		return true;
	}
	
	return false;
};

CUIDragItem* CUICellItem::CreateDragItem()
{
	CUIDragItem* tmp;
	tmp = xr_new<CUIDragItem>(this);
	tmp->Init(GetShader(),GetAbsoluteRect(),GetUIStaticItem().GetOriginalRect());
	return tmp;
}

void CUICellItem::SetOwnerList(CUIDragDropListEx* p)	
{
	m_pParentList=p;
}

bool CUICellItem::EqualTo(CUICellItem* itm)
{
	return (m_grid_size.x==itm->GetGridSize().x) && (m_grid_size.y==itm->GetGridSize().y);
}

u32 CUICellItem::ChildsCount()
{
	return m_childs.size();
}

void CUICellItem::PushChild(CUICellItem* c)
{
	m_childs.push_back	(c);
	UpdateItemText		();
}

CUICellItem* CUICellItem::PopChild()
{
	CUICellItem* itm	= m_childs.back();
	m_childs.pop_back	();
	std::swap			(itm->m_pData, m_pData);
	UpdateItemText		();
	return				itm;
}

void CUICellItem::UpdateItemText()
{
	string32			str;
	if(ChildsCount())
		sprintf				(str,"x%d",ChildsCount()+1);
	else
		sprintf				(str,"");

	SetText				(str);
}

CUIDragItem::CUIDragItem(CUICellItem* parent)
{
	m_back_list						= NULL;
	m_pParent						= parent;
	AttachChild						(&m_static);
	Device.seqRender.Add			(this, REG_PRIORITY_LOW-5000);
	Device.seqFrame.Add				(this, REG_PRIORITY_LOW-5000);
	VERIFY							(m_pParent->GetMessageTarget());
}

CUIDragItem::~CUIDragItem()
{
	Device.seqRender.Remove			(this);
	Device.seqFrame.Remove			(this);
}

void CUIDragItem::Init(const ref_shader& sh, const Frect& rect, const Frect& text_rect)
{
	SetWndRect						(rect);
	m_static.SetShader				(sh);
	m_static.SetOriginalRect		(text_rect);
	m_static.SetWndPos				(0.0f,0.0f);
	m_static.SetWndSize				(GetWndSize());
	m_static.TextureAvailable		(true);
	m_static.TextureOn				();
	m_static.SetColor				(color_rgba(255,255,255,170));
	m_static.SetStretchTexture		(true);
	m_pos_offset.sub				(rect.lt, GetUICursor()->GetPos());
}

bool CUIDragItem::OnMouse(float x, float y, EUIMessages mouse_action)
{
	if(mouse_action == WINDOW_LBUTTON_UP){
		m_pParent->GetMessageTarget()->SendMessage(m_pParent,DRAG_DROP_ITEM_DROP,NULL);
		return true;
	}
	return false;
}

void CUIDragItem::OnRender()
{
	Draw			();
}

void CUIDragItem::OnFrame()
{
	Update			();
}

void CUIDragItem::Draw()
{
	Fvector2 tmp;
	tmp.sub					(GetWndPos(), GetUICursor()->GetPos());
	tmp.sub					(m_pos_offset);
	tmp.mul					(-1.0f);
	MoveWndDelta			(tmp);
	UI()->PushScissor		(UI()->ScreenRect(),true);

	inherited::Draw();

	UI()->PopScissor();
}

void CUIDragItem::SetBackList(CUIDragDropListEx*l)
{
	if(m_back_list!=l){
		m_back_list=l;
	}
}

Fvector2 CUIDragItem::GetPosition()
{
	return Fvector2().add(m_pos_offset, GetUICursor()->GetPos());
}
