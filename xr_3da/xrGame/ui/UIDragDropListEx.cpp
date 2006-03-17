#include "stdafx.h"
#include "UIDragDropListEx.h"
#include "UIScrollBar.h"
#include "../object_broker.h"
#include "UICellItem.h"
#include "../MainUI.h"
#include <boost/function.hpp>
#include <boost/bind.hpp>

CUIDragItem* CUIDragDropListEx::m_drag_item = NULL;

CUIDragDropListEx::CUIDragDropListEx()
{

	m_flags.zero				();
	m_container					= xr_new<CUICellContainer>(this);
	m_vScrollBar				= xr_new<CUIScrollBar>();
//.	m_background				= xr_new<CUIStatic>();

	m_vScrollBar->SetAutoDelete	(true);
//.	m_background->SetAutoDelete	(true);
	m_selected_item				= NULL;

	SetCellsCapacity			(Ivector2().set(0,0));
	SetCellSize					(Ivector2().set(50,50));

//.	AttachChild					(m_background);
	AttachChild					(m_container);
	AttachChild					(m_vScrollBar);

	m_vScrollBar->SetWindowName	("scroll_v");
	Register					(m_vScrollBar);
	AddCallback					("scroll_v",SCROLLBAR_VSCROLL,boost::bind(&CUIDragDropListEx::OnScrollV,this, _1, _2));
	AddCallback					("cell_item",DRAG_DROP_ITEM_DRAG,boost::bind(&CUIDragDropListEx::OnItemStartDragging,this, _1, _2));
	AddCallback					("cell_item",DRAG_DROP_ITEM_DROP,boost::bind(&CUIDragDropListEx::OnItemDrop,this, _1, _2));
	AddCallback					("cell_item",DRAG_DROP_ITEM_SELECTED,boost::bind(&CUIDragDropListEx::OnItemSelected,this, _1, _2));
	AddCallback					("cell_item",DRAG_DROP_ITEM_RBUTTON_CLICK,boost::bind(&CUIDragDropListEx::OnItemRButtonClick,this, _1, _2));
	AddCallback					("cell_item",DRAG_DROP_ITEM_DB_CLICK,boost::bind(&CUIDragDropListEx::OnItemDBClick,this, _1, _2));
}

CUIDragDropListEx::~CUIDragDropListEx()
{

	if(m_selected_item && m_drag_item && m_drag_item->ParentItem()==m_selected_item)
		delete_data					(m_drag_item);

	delete_data					(m_container);
}
void CUIDragDropListEx::SetAutoGrow(bool b)						
{
	m_flags.set(flAutoGrow,b);
}
bool CUIDragDropListEx::IsAutoGrow()								
{
	return !!m_flags.test(flAutoGrow);
}
void CUIDragDropListEx::SetGrouping(bool b)						
{
	m_flags.set(flGroupSimilar,b);
}
bool CUIDragDropListEx::IsGrouping()
{
	return !!m_flags.test(flGroupSimilar);
}
void CUIDragDropListEx::SetCustomPlacement(bool b)
{
	m_flags.set(flCustomPlacement,b);
}

bool CUIDragDropListEx::GetCustomPlacement()
{
	return !!m_flags.test(flCustomPlacement);
}

void CUIDragDropListEx::SendMessage(CUIWindow* pWnd, s16 msg, void* pData)
{
	CUIWndCallback::OnEvent(pWnd, msg, pData);
}

void CUIDragDropListEx::Init(float x, float y, float w, float h)
{
	inherited::SetWndRect				(x,y,w,h);
	m_vScrollBar->Init					(w-SCROLLBAR_HEIGHT, 0, h, false);
/*
//.	m_background->Init					(0,0,w,h);
//.	m_background->Init					("ui\\ui_frame_02_back",0,0,w,h);
//.	Frect rect; rect.set				(0,0,64,64);
//.	m_background->GetUIStaticItem().SetOriginalRect(rect);
//.	m_background->SetStretchTexture		(true);
*/
}

void CUIDragDropListEx::OnScrollV(CUIWindow* w, void* pData)
{
	m_container->SetWndPos		(m_container->GetWndPos().x, float(-m_vScrollBar->GetScrollPos()));
}

void CUIDragDropListEx::CreateDragItem(CUICellItem* itm)
{
	R_ASSERT							(!m_drag_item);
	m_drag_item							= itm->CreateDragItem();
	GetParent()->SetCapture				(m_drag_item, true);
}

void CUIDragDropListEx::DestroyDragItem()
{
	GetParent()->SetCapture				(NULL, false);
	delete_data							(m_drag_item);
}

Fvector2 CUIDragDropListEx::GetDragItemPosition()
{
	return m_drag_item->GetPosition();
}

void CUIDragDropListEx::OnItemStartDragging(CUIWindow* w, void* pData)
{
	CUICellItem* itm		= smart_cast<CUICellItem*>(w);

	if(itm!=m_selected_item)	return;
	
	if(m_f_item_start_drag && m_f_item_start_drag(itm) ) return;

	CreateDragItem						(itm);
}

void CUIDragDropListEx::OnItemDrop(CUIWindow* w, void* pData)
{
	CUICellItem*		itm				= smart_cast<CUICellItem*>(w);
	VERIFY								(itm->OwnerList() == itm->OwnerList());

	if(m_f_item_drop && m_f_item_drop(itm) ){
		DestroyDragItem						();
		return;
	}

	CUIDragDropListEx*	old_owner		= itm->OwnerList();
	CUIDragDropListEx*	new_owner		= m_drag_item->BackList();

	bool b				= (old_owner==new_owner)&&!GetCustomPlacement();

	if(old_owner&&new_owner && !b)
	{
		CUICellItem* i					= old_owner->RemoveItem(itm, (old_owner==new_owner) );
		new_owner->SetItem				(i,old_owner->GetDragItemPosition());
	}
	DestroyDragItem						();
}

void CUIDragDropListEx::OnItemDBClick(CUIWindow* w, void* pData)
{
	CUICellItem*		itm				= smart_cast<CUICellItem*>(w);

	if(m_f_item_db_click && m_f_item_db_click(itm) ){
		DestroyDragItem						();
		return;
	}

	CUIDragDropListEx*	old_owner		= itm->OwnerList();
	VERIFY								(m_drag_item==NULL);
	VERIFY								(old_owner == this);

	if(old_owner&&old_owner->GetCustomPlacement())
	{
		CUICellItem* i					= old_owner->RemoveItem(itm, true);
		old_owner->SetItem				(i);
	}

	DestroyDragItem						();
}

void CUIDragDropListEx::OnItemSelected(CUIWindow* w, void* pData)
{
	m_selected_item						= smart_cast<CUICellItem*>(w);
	VERIFY								(m_selected_item);
	if(m_f_item_selected)
		m_f_item_selected(m_selected_item);
}

void CUIDragDropListEx::OnItemRButtonClick(CUIWindow* w, void* pData)
{
	CUICellItem*		itm				= smart_cast<CUICellItem*>(w);
	if(m_f_item_rbutton_click) 
		m_f_item_rbutton_click(itm);
}

void CUIDragDropListEx::GetClientArea(Frect& r)
{
	r	= GetAbsoluteRect				();
	if(m_vScrollBar->GetVisible())
		r.x2 -= m_vScrollBar->GetWidth	();
}

void CUIDragDropListEx::ClearAll(bool bDestroy)
{
	m_container->ClearAll	(bDestroy);
	m_selected_item			= NULL;
}

#include "../HUDManager.h"
void CUIDragDropListEx::Draw()
{
	inherited::Draw				();

	if(0 && bDebug){
		CGameFont* F		= UI()->Font()->pFontDI;
		F->SetAligment		(CGameFont::alCenter);
		F->SetSizeI			(0.02f);
		F->OutSetI			(0.f,-0.5f);
		F->SetColor			(0xffffffff);
		Ivector2			pt = m_container->PickCell(GetUICursor()->GetPos());
		F->OutNext			("%d-%d",pt.x, pt.y);
	};

}

void CUIDragDropListEx::Update()
{
	inherited::Update			();

	if( m_drag_item ){
		Frect	wndRect		= GetAbsoluteRect();
		Fvector2 cp			= GetUICursor()->GetPos();
		if(wndRect.in(cp)){
			if(NULL==m_drag_item->BackList())
				m_drag_item->SetBackList(this);
		}else
			if( this==m_drag_item->BackList() )
				m_drag_item->SetBackList(NULL);
	}
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

bool CUIDragDropListEx::OnMouse(float x, float y, EUIMessages mouse_action)
{
	bool b = inherited::OnMouse		(x,y,mouse_action);

	if(m_vScrollBar->IsShown())
	{
		switch(mouse_action){
		case WINDOW_MOUSE_WHEEL_DOWN:
				m_vScrollBar->TryScrollInc();
				return true;
				break;

		case WINDOW_MOUSE_WHEEL_UP:
				m_vScrollBar->TryScrollDec();
				return true;
				break;
		}
	}
	return b;
}

const Ivector2& CUIDragDropListEx::CellsCapacity()
{
	return m_container->CellsCapacity();
}

void CUIDragDropListEx::SetCellsCapacity(const Ivector2 c)
{
	m_container->SetCellsCapacity(c);
}

const Ivector2& CUIDragDropListEx::CellSize()
{
	return m_container->CellSize();
}

void CUIDragDropListEx::SetCellSize(const Ivector2 new_sz)			
{
	m_container->SetCellSize(new_sz);
}

int CUIDragDropListEx::ScrollPos()
{
	return m_vScrollBar->GetScrollPos();
}

void CUIDragDropListEx::SetItem(CUICellItem* itm) //auto
{
	if(m_container->AddSimilar(itm))	return;

	Ivector2 dest_cell_pos =	m_container->FindFreeCell(itm->GetGridSize());

	SetItem						(itm,dest_cell_pos);
}

void CUIDragDropListEx::SetItem(CUICellItem* itm, Fvector2 abs_pos) // start at cursor pos
{
	if(m_container->AddSimilar(itm))	return;

	const Ivector2 dest_cell_pos =	m_container->PickCell		(abs_pos);

	if(m_container->ValidCell(dest_cell_pos) && m_container->IsRoomFree(dest_cell_pos,itm->GetGridSize()))
		SetItem						(itm, dest_cell_pos);
	else
		SetItem						(itm);
}

void CUIDragDropListEx::SetItem(CUICellItem* itm, Ivector2 cell_pos) // start at cell
{
	if(m_container->AddSimilar(itm))	return;
	VERIFY						(m_container->IsRoomFree(cell_pos, itm->GetGridSize()));

	m_container->PlaceItemAtPos	(itm, cell_pos);

	itm->SetWindowName			("cell_item");
	Register					(itm);
	itm->SetOwnerList			(this);
}

CUICellItem* CUIDragDropListEx::RemoveItem(CUICellItem* itm, bool force_root)
{
	CUICellItem* i				= m_container->RemoveItem		(itm, force_root);
	i->SetOwnerList				((CUIDragDropListEx*)NULL);
	return						i;
}

u32 CUIDragDropListEx::ItemsCount()
{
	return m_container->GetChildWndList().size();
}

CUICellItem* CUIDragDropListEx::GetItemIdx(u32 idx)
{
	VERIFY(idx<ItemsCount());
	WINDOW_LIST_it it = m_container->GetChildWndList().begin();
	std::advance	(it, idx);
	return smart_cast<CUICellItem*>(*it);
}

CUICellContainer::CUICellContainer(CUIDragDropListEx* parent)
{
	m_pParentDragDropList		= parent;
	hShader.create				("hud\\fog_of_war","ui\\ui_grid");
	hGeom.create				(FVF::F_TL, RCache.Vertex.Buffer(), 0);
}

CUICellContainer::~CUICellContainer()
{
}

bool CUICellContainer::AddSimilar(CUICellItem* itm)
{
	if(!m_pParentDragDropList->IsGrouping())	return false;

	CUICellItem* i = FindSimilar(itm);
	if(i)	
		i->PushChild	(itm);
	
	return (i!=NULL);
}

CUICellItem* CUICellContainer::FindSimilar(CUICellItem* itm)
{
	for(WINDOW_LIST_it it = m_ChildWndList.begin(); m_ChildWndList.end()!=it; ++it)
	{
#ifdef DEBUG
		CUICellItem* i = smart_cast<CUICellItem*>(*it);
#else
		CUICellItem* i = (CUICellItem*)(*it);
#endif
		if(i->EqualTo(itm))
			return i;
	}
	return NULL;
}

void CUICellContainer::PlaceItemAtPos(CUICellItem* itm, Ivector2& cell_pos)
{
	Ivector2 cs				= itm->GetGridSize();
	for(int x=0; x<cs.x; ++x)
		for(int y=0; y<cs.y; ++y){
			CUICell& C		= GetCellAt(Ivector2().set(x,y).add(cell_pos));
			C.SetItem		(itm);
		}

	itm->SetWndPos			( Fvector2().set( (m_cellSize.x*cell_pos.x), (m_cellSize.y*cell_pos.y))	);
	itm->SetWndSize			( Fvector2().set( (m_cellSize.x*cs.x),		(m_cellSize.y*cs.y)		 )	);

	AttachChild				(itm);
	itm->OnAfterChild		();
}

CUICellItem* CUICellContainer::RemoveItem(CUICellItem* itm, bool force_root)
{
	if(!force_root && itm->ChildsCount())
		return				itm->PopChild();

	Ivector2 pos			= GetItemPos(itm);
	Ivector2 cs				= itm->GetGridSize();

	for(int x=0; x<cs.x;++x)
		for(int y=0; y<cs.y;++y){
			CUICell& C		= GetCellAt(Ivector2().set(x,y).add(pos));
			C.Clear			();
		}

	DetachChild				(itm);
	return					itm;
}

Ivector2 CUICellContainer::FindFreeCell	(const Ivector2& size)
{
	Ivector2 tmp;
	for(tmp.y=0; tmp.y<=m_cellsCapacity.y-size.y; ++tmp.y )
		for(tmp.x=0; tmp.x<=m_cellsCapacity.x-size.x; ++tmp.x )
			if(IsRoomFree(tmp,size))
				return  tmp;

	if(m_pParentDragDropList->IsAutoGrow())
	{
		Grow	();
		return							FindFreeCell	(size);
	}else{
		R_ASSERT2		(0,"there are no free room to place item");
	}
	return			tmp;
}

bool CUICellContainer::IsRoomFree(const Ivector2& pos, const Ivector2& size)
{
	Ivector2 tmp;

	for(tmp.x =pos.x; tmp.x<pos.x+size.x; ++tmp.x)
		for(tmp.y =pos.y; tmp.y<pos.y+size.y; ++tmp.y)
		{
			if(!ValidCell(tmp))		return		false;

			CUICell& C				= GetCellAt(tmp);

			if(!C.Empty())			return		false;
		}
	return true;
}

void CUICellContainer::GetTexUVLT(Fvector2& uv, u32 col, u32 row)
{
	uv.set(0.0f,0.0f);

//.	if( (col%2==1 && row%2==1)||(col%2==0 && row%2==0) )
//.		uv.set(0.5f,0.0f);
}


void CUICellContainer::SetCellsCapacity(const Ivector2& c)
{
	m_cellsCapacity				= c;
	m_cells.resize				(c.x*c.y);
	ReinitSize					();
}

void CUICellContainer::SetCellSize(const Ivector2& new_sz)
{
	m_cellSize					= new_sz;
	ReinitSize					();
}

Ivector2 CUICellContainer::TopVisibleCell()
{
	return Ivector2().set	(0, iFloor(m_pParentDragDropList->ScrollPos()/float(CellSize().y)));
}

CUICell& CUICellContainer::GetCellAt(const Ivector2& pos)
{
	VERIFY				(ValidCell(pos));
	CUICell&	c		= m_cells[m_cellsCapacity.x*pos.y+pos.x];
	return				c;
}

Ivector2 CUICellContainer::GetItemPos(CUICellItem* itm)
{
	for(int x=0; x<m_cellsCapacity.x ;++x)
		for(int y=0; y<m_cellsCapacity.y ;++y){
			Ivector2 p;
			p.set(x,y);
		if(GetCellAt(p).m_item==itm)
			return p;
		}

		R_ASSERT(0);
		return Ivector2().set(-1,-1);
}

u32 CUICellContainer::GetCellsInRange(const Irect& rect, UI_CELLS_VEC& res)
{
	res.clear				();
	for(int x=rect.x1;x<=rect.x2;++x)
		for(int y=rect.y1;y<=rect.y2;++y)
			res.push_back	(GetCellAt(Ivector2().set(x,y)));

	std::unique				(res.begin(), res.end());
	return res.size			();
}

void CUICellContainer::ReinitSize()
{
	Fvector2							sz;
	sz.set								(CellSize().x*m_cellsCapacity.x, CellSize().y*m_cellsCapacity.y);
	SetWndSize							(sz);
	m_pParentDragDropList->ReinitScroll	();
}

void CUICellContainer::Grow()
{
	SetCellsCapacity	(Ivector2().set(m_cellsCapacity.x,m_cellsCapacity.y+1));
}

void CUICellContainer::Shrink()
{
}

bool CUICellContainer::ValidCell(const Ivector2& pos) const
{
	return !(pos.x<0 || pos.y<0 || pos.x>=m_cellsCapacity.x || pos.y>=m_cellsCapacity.y);
}

void CUICellContainer::ClearAll(bool bDestroy)
{
	while( !m_ChildWndList.empty() )
	{
		CUIWindow* w			= m_ChildWndList.back();
		VERIFY					(!w->IsAutoDelete());
		DetachChild				(w);	

		if(bDestroy)
			delete_data			(w);
	}

	{
		UI_CELLS_VEC_IT it		= m_cells.begin();
		UI_CELLS_VEC_IT it_e	= m_cells.end();
		for(;it!=it_e;++it)
			(*it).Clear();
	}
}

Ivector2 CUICellContainer::PickCell(const Fvector2& abs_pos)
{
	Ivector2 res;
	Fvector2 ap = GetAbsolutePos			();
	ap.sub									(abs_pos);
	ap.mul									(-1);
	res.x									= iFloor(ap.x/m_cellSize.x);
	res.y									= iFloor(ap.y/m_cellSize.y);
	if(!ValidCell(res))						res.set(-1, -1);
	return res;
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

	Fvector2	drawLT;
	drawLT.set			(lt_abs_pos.x+tgt_cells.lt.x*cell_sz.x, lt_abs_pos.y+tgt_cells.lt.y*cell_sz.y);
	drawLT.x			*= UI()->GetScaleX();
	drawLT.y			*= UI()->GetScaleY();

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
	FVF::TL* start_pv			= (FVF::TL*)RCache.Vertex.Lock	((tgt_cells.width()+1)*(tgt_cells.height()+1)*6,hGeom.stride(),vOffset);
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
#include "UICellItemFactory.h"
#include "../inventory.h"
#include "../actor.h"

CTestDragDropWnd::CTestDragDropWnd()
{
		static Frect w_rect					= {100,100,300,300};
		static Ivector2 w_cell_sz			= {50,50};
		static Ivector2 w_cell_sz2			= {50,50};
		static Ivector2 w_cells				= {6,7};
		static Ivector2 w_cells2			= {6,7};
		static int		dx					= 10;
//.		Init								(w_rect.x1, w_rect.y1, w_rect.x2*2+dx, w_rect.y2);
		Init								(0, 0, 1024, 768);
		m_list1 = xr_new<CUIDragDropListEx>	();
		AttachChild							(m_list1);
		m_list1->Init						(w_rect.x1, w_rect.y1, w_rect.x2, w_rect.y2);
		m_list1->SetAutoDelete				(true);
		m_list1->SetCellSize				(w_cell_sz);
		m_list1->SetCellsCapacity			(w_cells);
		m_list1->SetAutoGrow				(true);
		m_list1->SetGrouping				(true);

		m_list2 = xr_new<CUIDragDropListEx>	();
		AttachChild							(m_list2);
		m_list2->Init						(w_rect.x1+w_rect.x2+dx, w_rect.y1, w_rect.x2, w_rect.y2);
		m_list2->SetAutoDelete				(true);
		m_list2->SetCellSize				(w_cell_sz2);
		m_list2->SetCellsCapacity			(w_cells2);
		m_list2->SetAutoGrow				(true);
		m_list2->SetGrouping				(false);


		TIItemContainer::iterator it		= Actor()->inventory().m_all.begin();
		TIItemContainer::iterator it_e		= Actor()->inventory().m_all.end();

		CUICellItem* itm = NULL;
		for(;it!=it_e;++it){
			itm = create_cell_item(*it);
			m_list1->SetItem(itm);
		}

		m_list1->m_f_item_drop				= CUIDragDropListEx::DRAG_DROP_EVENT(this,&CTestDragDropWnd::OnItemDrop);
		m_list1->m_f_item_start_drag		= CUIDragDropListEx::DRAG_DROP_EVENT(this,&CTestDragDropWnd::OnItemStartDrag);
		m_list1->m_f_item_db_click			= CUIDragDropListEx::DRAG_DROP_EVENT(this,&CTestDragDropWnd::OnItemDbClick);
		m_list1->m_f_item_selected			= CUIDragDropListEx::DRAG_DROP_EVENT(this,&CTestDragDropWnd::OnItemSelected);
		m_list1->m_f_item_rbutton_click		= CUIDragDropListEx::DRAG_DROP_EVENT(this,&CTestDragDropWnd::OnItemRButtonClick);
}

bool CTestDragDropWnd::OnItemDrop			(CUICellItem* itm)
{
	CUIDragDropListEx*	old_owner		= itm->OwnerList();
	CUIDragDropListEx*	new_owner		= CUIDragDropListEx::m_drag_item->BackList();
	
	if(old_owner && new_owner)
	{
		CUICellItem* i					= old_owner->RemoveItem(itm, (old_owner==new_owner) );
		new_owner->SetItem				(i,old_owner->GetDragItemPosition());
	}
	return true;
}

bool CTestDragDropWnd::OnItemStartDrag		(CUICellItem* itm)
{
	return false;
}

bool CTestDragDropWnd::OnItemDbClick		(CUICellItem* itm)
{
	return false;
}

bool CTestDragDropWnd::OnItemSelected		(CUICellItem* itm)
{
	return false;
}

bool CTestDragDropWnd::OnItemRButtonClick	(CUICellItem* itm)
{
	return false;
}
