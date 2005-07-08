#include "stdafx.h"
#include "UIDragDropListEx.h"
#include "UIScrollBar.h"
#include "../object_broker.h"
#include "UICellItem.h"
#include "../MainUI.h"
#include <boost/function.hpp>
#include <boost/bind.hpp>

CUIDragDropListEx::CUIDragDropListEx()
{
	m_flags.zero				();
	m_container					= xr_new<CUICellContainer>(this);
	m_vScrollBar				= xr_new<CUIScrollBar>();
	m_background				= xr_new<CUIStatic>();

	m_container->SetAutoDelete	(false);
	m_vScrollBar->SetAutoDelete	(true);
	m_background->SetAutoDelete	(true);
	m_selected_item				= NULL;

	SetCellsCapacity			(Ivector2().set(0,0));
	SetCellSize					(Ivector2().set(50,50));

	AttachChild					(m_background);
	AttachChild					(m_container);
	AttachChild					(m_vScrollBar);

	m_vScrollBar->SetWindowName	("scroll_v");
	Register					(m_vScrollBar);
	AddCallback					("scroll_v",SCROLLBAR_VSCROLL,boost::bind(&CUIDragDropListEx::OnScrollV,this, _1, _2));
	AddCallback					("cell_item",DRAG_DROP_ITEM_DRAG,boost::bind(&CUIDragDropListEx::OnItemStartDragging,this, _1, _2));
	AddCallback					("cell_item",DRAG_DROP_ITEM_DROP,boost::bind(&CUIDragDropListEx::OnItemDrop,this, _1, _2));
	AddCallback					("cell_item",DRAG_DROP_ITEM_SELECTED,boost::bind(&CUIDragDropListEx::OnItemSelected,this, _1, _2));
	AddCallback					("cell_item",DRAG_DROP_ITEM_RBUTTON_CLICK,boost::bind(&CUIDragDropListEx::OnItemRButtonClick,this, _1, _2));
}

CUIDragDropListEx::~CUIDragDropListEx()
{
	delete_data					(m_container);
}

void CUIDragDropListEx::SendMessage(CUIWindow* pWnd, s16 msg, void* pData)
{
	CUIWndCallback::OnEvent(pWnd, msg, pData);
}

void CUIDragDropListEx::Init(float x, float y, float w, float h)
{
	inherited::SetWndRect				(x,y,w,h);
	m_vScrollBar->Init					(w-SCROLLBAR_HEIGHT, 0, h, false);
	m_background->Init					("ui\\ui_frame_02_back",0,0,w,h);

	Frect rect; rect.set				(0,0,64,64);
	m_background->GetUIStaticItem().SetOriginalRect(rect);
	m_background->SetStretchTexture		(true);
}

void CUIDragDropListEx::OnScrollV(CUIWindow* w, void* pData)
{
	m_container->SetWndPos		(m_container->GetWndPos().x, float(-m_vScrollBar->GetScrollPos()));
}

void CUIDragDropListEx::OnItemStartDragging(CUIWindow* w, void* pData)
{
	CUICellItem* selected_itm = smart_cast<CUICellItem*>(w);
	VERIFY(selected_itm==m_selected_item);

	CUIFlyingItem* fI = selected_itm->CreateFlyingItem();
	GetParent()->SetCapture(fI, true);
}

void CUIDragDropListEx::OnItemDrop(CUIWindow* w, void* pData)
{
}

void CUIDragDropListEx::OnItemSelected(CUIWindow* w, void* pData)
{
	m_selected_item = smart_cast<CUICellItem*>(w);
	VERIFY(m_selected_item);
	SendMessage(this,DRAG_DROP_LIST_SEL_CHANGED, (void*)m_selected_item);
}

void CUIDragDropListEx::OnItemRButtonClick(CUIWindow* w, void* pData)
{
}

void CUIDragDropListEx::GetClientArea(Frect& r)
{
	r	= GetAbsoluteRect				();
	if(m_vScrollBar->GetVisible())
		r.x2 -= m_vScrollBar->GetWidth	();
}

void CUIDragDropListEx::Draw()
{
//	UI()->PushScissor			(GetAbsoluteRect());
	inherited::Draw				();
//	UI()->PopScissor			();
}

void CUIDragDropListEx::Update()
{
	inherited::Update				();
}

void CUIDragDropListEx::ReinitScroll()
{
		float h1 = m_container->GetWndSize().y;
		float h2 = GetWndSize().y;
		m_vScrollBar->Show				( h1 > h2 );
		m_vScrollBar->Enable			( h1 > h2 );

		m_vScrollBar->SetRange		(0, _max(0,iFloor(h1-h2)) );
		m_vScrollBar->SetScrollPos	(0);
		m_vScrollBar->SetStepSize	(CellSize().y/3);
		m_vScrollBar->SetPageSize	(iFloor(GetWndSize().y/float(CellSize().y)));
}

void CUIDragDropListEx::OnMouse(float x, float y, EUIMessages mouse_action)
{
	inherited::OnMouse		(x,y,mouse_action);
}
const Ivector2& CUIDragDropListEx::CellsCapacity()
{
	return m_container->CellsCapacity();
};	
void CUIDragDropListEx::SetCellsCapacity(const Ivector2 c)
{
	m_container->SetCellsCapacity(c);
}
const Ivector2& CUIDragDropListEx::CellSize()
{
	return m_container->CellSize();
};	
void CUIDragDropListEx::SetCellSize(const Ivector2 new_sz)			
{
	m_container->SetCellSize(new_sz);
}
int CUIDragDropListEx::ScrollPos()
{
	return m_vScrollBar->GetScrollPos();
}

void CUIDragDropListEx::AddItem(CUICellItem*itm ,int x, int y)
{
	CUICell& C = m_container->GetCellAt(x,y);
	C.SetItem				(itm);
	C.m_item->SetWndRect	(float(CellSize().x*x), float(CellSize().y*y), float(CellSize().x), float(CellSize().y));
	m_container->AttachChild(itm);
	itm->SetAutoDelete		(false);
	itm->SetWindowName		("cell_item");
	Register				(itm);
}


CUICellContainer::CUICellContainer(CUIDragDropListEx* parent)
{
	m_flags.zero				();
	m_pParentDragDropList		= parent;
	hShader.create				("hud\\fog_of_war","ui\\ui_grid");
	hGeom.create				(FVF::F_TL, RCache.Vertex.Buffer(), 0);
}

CUICellContainer::~CUICellContainer()
{
}

void CUICellContainer::GetTexUVLT(Fvector2& uv, u32 col, u32 row)
{
	uv.set(0.0f,0.0f);

	if( (col%2==1 && row%2==1)||(col%2==0 && row%2==0) )
		uv.set(0.5f,0.0f);
}

void CUICellContainer::Draw()
{
	Frect clientArea;
	m_pParentDragDropList->GetClientArea(clientArea);

	Ivector2			cell_cnt = m_pParentDragDropList->CellsCapacity();
	if					(cell_cnt.x==0 || cell_cnt.y==0)	return;

	Ivector2			cell_sz = CellSize();

	Irect				tgt_cells;
	tgt_cells.lt		= TopVisibleCell();
	tgt_cells.x2		= iFloor( (float(clientArea.width())+float(cell_sz.x)-EPS)/float(cell_sz.x)) + tgt_cells.lt.x;
	tgt_cells.y2		= iFloor( (float(clientArea.height())+float(cell_sz.y)-EPS)/float(cell_sz.y)) + tgt_cells.lt.y;

	clamp				(tgt_cells.x2, 0, cell_cnt.x-1);
	clamp				(tgt_cells.y2, 0, cell_cnt.y-1);

	Fvector2 lt_abs_pos	= GetAbsolutePos();

	Ivector2	drawLT;
	drawLT.set			(lt_abs_pos.x+tgt_cells.lt.x*cell_sz.x, lt_abs_pos.y+tgt_cells.lt.y*cell_sz.y);

	const Fvector2 pts[6] =		{{0.0f,0.0f},{1.0f,0.0f},{1.0f,1.0f},
								 {0.0f,0.0f},{1.0f,1.0f},{0.0f,1.0f}};
#define ty 1.0f
#define tx 0.5f
	const Fvector2 uvs[6] =		{{0.0f,0.0f},{tx,0.0f},{tx,ty},
								 {0.0f,0.0f},{tx,ty},{0.0f,ty}};

	// calculate cell size in screen pixels
	float		fw				= cell_sz.x*UI()->GetScaleX();
	float		fh				= cell_sz.y*UI()->GetScaleY();

	// fill cell buffer
	u32 vOffset					= 0;
	FVF::TL* start_pv			= (FVF::TL*)RCache.Vertex.Lock	(tgt_cells.width()*tgt_cells.height()*6,hGeom.stride(),vOffset);
	FVF::TL* pv					= start_pv;
	for (int x=0; x<=tgt_cells.width(); ++x){
		for (int y=0; y<=tgt_cells.height(); ++y){
			Fvector2			tp;
			GetTexUVLT			(tp,tgt_cells.x1+x,tgt_cells.y1+y);
			for (u32 k=0; k<6; ++k,++pv){
				const Fvector2& p	= pts[k];
				const Fvector2& uv	= uvs[k];
				pv->set			(iFloor(drawLT.x + p.x*(fw) +fw*x)-0.5f, 
								 iFloor(drawLT.y + p.y*(fh) +fh*y)-0.5f, 
								 0xFFFFFFFF,tp.x+uv.x,tp.y+uv.y);
			}
		}
	}
	std::ptrdiff_t p_cnt		= (pv-start_pv)/3;
	RCache.Vertex.Unlock		(u32(pv-start_pv),hGeom.stride());

	UI()->PushScissor					(clientArea);

	if (p_cnt!=0){
		// draw grid
		RCache.set_Shader		(hShader);
		RCache.set_Geometry		(hGeom);
		RCache.Render			(D3DPT_TRIANGLELIST,vOffset,u32(p_cnt));
	}

	//draw shown items in range
	UI_CELLS_VEC cells_to_draw;
	if( GetCellsInRange(tgt_cells,cells_to_draw) ){
		UI_CELLS_VEC_IT it = cells_to_draw.begin();
		for(;it!=cells_to_draw.end();++it)
			if( !(*it).Empty() )
				(*it).m_item->Draw();
	}

	UI()->PopScissor			();
}

void CUICellContainer::SetCellsCapacity(const Ivector2 c)
{
	m_cellsCapacity				= c;
	m_cells.resize				(c.x*c.y);
	ReinitSize					();
}

void CUICellContainer::SetCellSize(const Ivector2 new_sz)
{
	m_cellSize					= new_sz;
	ReinitSize					();
}

Ivector2 CUICellContainer::TopVisibleCell()
{
	return Ivector2().set	(0, iFloor(m_pParentDragDropList->ScrollPos()/float(CellSize().y)));
}

CUICell& CUICellContainer::GetCellAt(int x, int y)
{
	VERIFY					(x>=0 && y>=0 && x<CellsCapacity().x && y<CellsCapacity().y);
	return m_cells[CellsCapacity().x*y+x];
}

u32 CUICellContainer::GetCellsInRange(const Irect rect, UI_CELLS_VEC& res)
{
	res.clear				();
	for(int x=rect.x1;x<=rect.x2;++x)
		for(int y=rect.y1;y<=rect.y2;++y)
			res.push_back	(GetCellAt(x,y));

	std::unique				(res.begin(), res.end());
	return res.size			();
}

void CUICellContainer::ReinitSize()
{
	Fvector2							sz;
	sz.set								(CellSize().x*CellsCapacity().x, CellSize().y*CellsCapacity().y);
	SetWndSize							(sz);
	m_pParentDragDropList->ReinitScroll	();
}

CTestDragDropWnd::CTestDragDropWnd()
{
		static Frect w_rect					= {200,100,216,200};
		static Ivector2 w_cell_sz			= {50,50};
		static Ivector2 w_cells				= {8,10};
		static int		dx					= 10;
		Init								(w_rect.x1, w_rect.y1, w_rect.x2*2+dx, w_rect.y2);
		m_list1 = xr_new<CUIDragDropListEx>	();
		AttachChild							(m_list1);
		m_list1->Init						(0,0,w_rect.x2,w_rect.y2);
		m_list1->SetAutoDelete				(true);
		m_list1->SetCellSize				(w_cell_sz);
		m_list1->SetCellsCapacity			(w_cells);

		m_list2 = xr_new<CUIDragDropListEx>	();
		AttachChild							(m_list2);
		m_list2->Init						(w_rect.x2+dx,0,w_rect.x2,w_rect.y2);
		m_list2->SetAutoDelete				(true);
		m_list2->SetCellSize				(w_cell_sz);
		m_list2->SetCellsCapacity			(w_cells);

		CUICellItem* itm = xr_new<CUICellItem>((void*)NULL);
		itm->InitInternals();
		m_list1->AddItem					(itm,0,0);

		itm = xr_new<CUICellItem>((void*)NULL);
		itm->InitInternals();
		m_list1->AddItem					(itm,1,1);

		itm = xr_new<CUICellItem>((void*)NULL);
		itm->InitInternals();
		m_list1->AddItem					(itm,2,2);

		itm = xr_new<CUICellItem>((void*)NULL);
		itm->InitInternals();
		m_list1->AddItem					(itm,3,3);

		itm = xr_new<CUICellItem>((void*)NULL);
		itm->InitInternals();
		m_list1->AddItem					(itm,0,4);

		itm = xr_new<CUICellItem>((void*)NULL);
		itm->InitInternals();
		m_list1->AddItem					(itm,1,5);

		itm = xr_new<CUICellItem>((void*)NULL);
		itm->InitInternals();
		m_list1->AddItem					(itm,2,6);

		itm = xr_new<CUICellItem>((void*)NULL);
		itm->InitInternals();
		m_list1->AddItem					(itm,3,7);


		itm = xr_new<CUICellItem>((void*)NULL);
		itm->InitInternals();
		m_list1->AddItem					(itm,0,8);

		itm = xr_new<CUICellItem>((void*)NULL);
		itm->InitInternals();
		m_list1->AddItem					(itm,1,9);

}

CTestDragDropWnd::~CTestDragDropWnd()
{
}
