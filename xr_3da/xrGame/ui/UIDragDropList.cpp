// UIDragDropList.cpp: список элементов Drag&Drop
//
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "uidragdroplist.h"
#include "UIOutfitSlot.h"

#define SCROLLBAR_OFFSET_X 5
#define SCROLLBAR_OFFSET_Y 0


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CUIDragDropList::CUIDragDropList()
{
	SetCellHeight(50);
	SetCellWidth(50);

	m_vCellStatic.clear();
	m_vGridState.clear();


	m_pCheckProc = NULL;

	m_bCustomPlacement = false;
	m_bBlockCustomPlacement = false;

	m_iViewRowsNum = 0;
	m_iCurrentFirstRow = 0;

	m_DragDropItemsList.clear();

	m_bScrollBarEnabled = true;
}

CUIDragDropList::~CUIDragDropList()
{
	DetachAll();

	m_vGridState.clear();
	m_vCellStatic.clear();
	m_DragDropItemsList.clear();
	
}

void CUIDragDropList::AttachChild(CUIWindow* pChild)
{
	CUIDragDropItem* pDragDropItem = dynamic_cast<CUIDragDropItem*>(pChild);
	if(pDragDropItem) 
	{
		PlaceItemInGrid(pDragDropItem);

		pDragDropItem->SetWidth(GetCellWidth()*pDragDropItem->GetGridWidth());
		pDragDropItem->SetHeight(GetCellHeight()*pDragDropItem->GetGridHeight());

		pDragDropItem->SetCellWidth(GetCellWidth());
		pDragDropItem->SetCellHeight(GetCellHeight());

		m_DragDropItemsList.push_back(pDragDropItem);

		pDragDropItem->ClipperOn();
	}

	inherited::AttachChild(pChild);
}
void CUIDragDropList::DetachChild(CUIWindow* pChild)
{
	WINDOW_LIST_it it = std::find(m_ChildWndList.begin(), 
								  m_ChildWndList.end(), 
								  pChild);

	if( m_ChildWndList.end() != it)
	{

		CUIDragDropItem* pDragDropItem = dynamic_cast<CUIDragDropItem*>(pChild);
		if(pDragDropItem)
		{
			RemoveItemFromGrid(pDragDropItem);
			m_DragDropItemsList.remove(pDragDropItem);
		}
	}

	inherited::DetachChild(pChild);
}

void CUIDragDropList::DropAll()
{
	CUIWindow::DetachAll();
	m_DragDropItemsList.clear();

	//освободить фокус мыши, если вдруг кто забыл это сделать!!!!
	//а ведь забывают, суки!!!!
	//выскакивает глюк, когда вместо одного элемента
	//т€нетс€ совсем другой.
	if (GetParent())
		GetParent()->SetCapture(this, false);
	m_pMouseCapturer = NULL;


/*	for(DRAG_DROP_LIST_it it = m_DragDropItemsList.begin(); m_DragDropItemsList.end() != it; ++it)
	{
		DetachChild((CUIWindow*)*it);
	}
	m_DragDropItemsList.clear();*/

	for(u32 i=0; i<m_vCellStatic.size(); ++i)
	{
		AttachChild(&m_vCellStatic[i]);
	}
	AttachChild(&m_ScrollBar);


	int k,m;

	for(k=0; k<GetRows(); ++k)
	{
		for(m=0; m<GetCols(); ++m)
		{
			GetCell(k, m) = CELL_EMPTY;
		}
	}
}


//обработка сообщений дл€ DragDrop
void CUIDragDropList::SendMessage(CUIWindow *pWnd, s16 msg, void *pData)
{
	if(pWnd == &m_ScrollBar)
	{
		if(msg == CUIScrollBar::VSCROLL)
		{
			m_iCurrentFirstRow = m_ScrollBar.GetScrollPos();
			UpdateList();
		}
	}
	else if(dynamic_cast<CUIDragDropItem*>(pWnd) && msg == CUIDragDropItem::ITEM_DRAG)
	{
		//принадлежит ли элемент отправивший сообщение ITEM_DRAG нашему списку
		WINDOW_LIST_it it = std::find(m_ChildWndList.begin(),
										m_ChildWndList.end(),
										pWnd);
		
		//элемент наш, подн€ть окно на вершину списка, 
		//чтоб оно могло выводить себ€ и этот элемент поверх всех окон
		//и получило сообщени€ DRAG в последнюю очередь
		if( m_ChildWndList.end() != it)
		{
			//подн€ть окно вместе с родител€ми на вершину
			(*it)->BringAllToTop(); 

			pWnd->Show(true);

			//выбросить элемент из сетки
			RemoveItemFromGrid((CUIDragDropItem*)pWnd);
		}
	}
	else if(dynamic_cast<CUIDragDropItem*>(pWnd) && msg == CUIDragDropItem::ITEM_DROP)
	{	
		//принадлежит ли элемент отправивший сообщение ITEM_DROP нашему списку
		WINDOW_LIST_it it = std::find(m_ChildWndList.begin(), 
										m_ChildWndList.end(), 
										pWnd);

		CUIDragDropItem* pItem = (CUIDragDropItem*)pWnd;

		//элемени чужой, надо вз€ть его себе, 
		//но только если он в нашей области
		if( it == m_ChildWndList.end())
		{
			RECT rect = GetAbsoluteRect();
			POINT pt_lt, pt_lb, pt_rt, pt_rb;
			
			pt_lt.x = pWnd->GetAbsoluteRect().left;
			pt_lt.y = pWnd->GetAbsoluteRect().top;

			pt_lb.x = pWnd->GetAbsoluteRect().left;
			pt_lb.y = pWnd->GetAbsoluteRect().bottom;

			pt_rt.x = pWnd->GetAbsoluteRect().right;
			pt_rt.y = pWnd->GetAbsoluteRect().top;

			pt_rb.x = pWnd->GetAbsoluteRect().right;
			pt_rb.y = pWnd->GetAbsoluteRect().bottom;		

/*			if(PtInRect(&rect, pt_lt) || 
			   PtInRect(&rect, pt_lb) ||
			   PtInRect(&rect, pt_rt) ||
			   PtInRect(&rect, pt_rb))*/
			POINT pt_center;
			pt_center.x = (pWnd->GetAbsoluteRect().right+
						   pWnd->GetAbsoluteRect().left)/2;
			pt_center.y = (pWnd->GetAbsoluteRect().top+
						   pWnd->GetAbsoluteRect().bottom)/2;;


			if(PtInRect(&rect, pt_center) || pItem->NeedMoveWithoutRectCheck())
			{

				//отсоединить у прошлого родител€
				//((CUIDragDropList*)pItem->GetParent())->DetachChild(pItem);


				////если есть куда брать и работает доп. проверка
				OnCustomPlacement();
				int place_row, place_col;


				//доп. проверка при помощи присоединенной функции
				bool additional_check;

				if(GetCheckProc())
					additional_check = (*(GetCheckProc()))(pItem, this);
				else
					additional_check = true;

				if(additional_check)
				{

					if(CanPlaceItemInGrid(pItem, place_row, place_col))
					{	
						//отсоединить у прошлого родител€
						pItem->GetParent()->SetCapture(pItem, false);
						//((CUIDragDropList*)pItem->GetParent())->DetachChild(pItem);
						pItem->GetParent()->DetachChild(pItem);

						AttachChild(pItem);
						pItem->BringAllToTop(); 
					}
					else
					{
						RemoveItemFromGrid(pItem);
						//обратно к прошлому родителю
						//((CUIDragDropList*)pItem->GetParent())->AttachChild(pItem);
					}
				}
				OffCustomPlacement();
				//else

						//обратно к прошлому родителю
						//((CUIDragDropList*)pItem->GetParent())->AttachChild(pItem);
			}
		}
		//элемент наш, вернуть его на место где был раньше
		else
		{
			OnCustomPlacement();
			PlaceItemInGrid(pItem);
			OffCustomPlacement();
			pItem->GetParent()->SetCapture(pItem, false);
			// ѕросигнализировать о том, что если это был костюм, то надо его оп€ть спр€тать
			pItem->GetMessageTarget()->SendMessage(pItem ,CUIOutfitSlot::OUTFIT_RETURNED_BACK, NULL);
			//pItem->MoveWindow(pItem->GetPreviousPos().x,
	 		//				  pItem->GetPreviousPos().y);
		}
		// ѕриземл€ем объект.
	}

	CUIWindow::SendMessage(pWnd, msg, pData);
}



void CUIDragDropList::Init(int x, int y, int width, int height)
{
//	m_ScrollBar.Init(GetWidth()-CUIScrollBar::SCROLLBAR_WIDTH,0,
//					 GetHeight(), false);

	inherited::Init(x, y, width, height);
}
//инициализаци€ сетки Drag&Drop
void CUIDragDropList::InitGrid(int iRowsNum, int iColsNum, 
							   bool bGridVisible, int iViewRowsNum)
{
	OffCustomPlacement();

	m_iColsNum=iColsNum;
	m_iRowsNum=iRowsNum;

	if(iViewRowsNum>=m_iRowsNum || iViewRowsNum == 0)
	{
        m_iViewRowsNum = m_iRowsNum;
		m_ScrollBar.Show(false);
		m_ScrollBar.Enable(false);
	}
	else
	{
		m_iViewRowsNum = iViewRowsNum;

		//инициализировать скроллинг
		m_ScrollBar.SetRange(0, s16(GetRows()-1));
		m_ScrollBar.SetPageSize(s16(m_iViewRowsNum));
		m_ScrollBar.SetScrollPos(0);
	}



	SetWidth(m_iColsNum*GetCellWidth() + CUIScrollBar::SCROLLBAR_WIDTH + SCROLLBAR_OFFSET_X);
	SetHeight(GetViewRows()*GetCellHeight());

	AttachChild(&m_ScrollBar);
	m_ScrollBar.Init(GetWidth() - CUIScrollBar::SCROLLBAR_WIDTH, SCROLLBAR_OFFSET_Y, 
					GetHeight(), false);

	m_vGridState.resize(m_iRowsNum*m_iColsNum, CELL_EMPTY);

	if(bGridVisible)
	{
		m_vCellStatic.resize(GetViewRows()*m_iColsNum);

		int i,j;

		//временно!
		//установить масштаб дл€ клеточки
		float scale = GetCellWidth()/50.f;
		
		CELL_STATIC_IT it=m_vCellStatic.begin();

		for(i=0; i<GetViewRows(); ++i)
		{
			for(j=0; j<GetCols(); ++j)
			{

				(*it).Init("ui\\ui_inv_lattice", 
								j*GetCellWidth(),
								i*GetCellHeight(),
								GetCellWidth(),
								GetCellHeight());

				AttachChild(&(*it));
				
				(*it).SetTextureScale(scale);

				++it;
			}
		}

	}
}

//размещение элемента на свободном месте в сетке
bool CUIDragDropList::PlaceItemInGrid(CUIDragDropItem* pItem)
{
	int place_row = -1,  place_col = -1;
	bool found_place = CanPlaceItemInGrid(pItem, place_row, place_col);

	//разместить элемент на найденном месте
	if(found_place)
	{
		PlaceItemAtPos(place_row, place_col, pItem);
		return true;
	}
	else
		return false;
}

//размещение элемента на свободном месте в сетке
bool CUIDragDropList::CanPlaceItemInGrid(CUIDragDropItem* pItem, int& place_row, int& place_col)
{
	R_ASSERT(pItem->GetGridHeight()>0);
	R_ASSERT(pItem->GetGridWidth()>0);

	R_ASSERT(GetRows()>0);
	R_ASSERT(GetCols()>0);

	int i,j;
//	int k,m;

	place_row = -1;
	place_col = -1;

	bool found_place;
	bool can_place;

	found_place = false;

	//провер€ем, можно ли разместить элемент на том месте на 
	//котором он сейчас
	RECT item_rect = pItem->GetAbsoluteRect();
	RECT rect = GetAbsoluteRect();

	if(item_rect.left - rect.left<0)
		place_col = 0;
	else if(item_rect.right - rect.right>=0)
		place_col = GetCols()-pItem->GetGridWidth();

	if(item_rect.top - rect.top<0)
		place_row = m_iCurrentFirstRow;
	else if(item_rect.bottom - rect.bottom>=0)
		place_row = m_iCurrentFirstRow + GetViewRows()- pItem->GetGridHeight();


	int item_center_x = pItem->GetWidth()/2 + item_rect.left - rect.left;
	int item_center_y = pItem->GetHeight()/2 + item_rect.top - rect.top;


	if(place_col == -1)
	{
		place_col = iFloor((float)item_center_x/GetCellWidth() - (float)pItem->GetGridWidth()/2  + .5f);
	}
	if(place_row == -1)
	{
		place_row = m_iCurrentFirstRow + 
			//item_center_y/GetCellHeight()
			iFloor((float)item_center_y/GetCellHeight()
			- (float)pItem->GetGridHeight()/2  + .5f);
	}


	if(place_col<0)
		place_col = 0;
	if(place_col>=GetCols())
		place_col = GetCols()- 1 - pItem->GetGridWidth()/2;
	if(place_row<0)
		place_row = 0;
	if(place_row>=GetRows())
		place_row = GetRows()- 1 - pItem->GetGridHeight()/2;


	if(m_bCustomPlacement && !m_bBlockCustomPlacement)
	{
		if(CanPlace(place_row, place_col, pItem))
		{
			found_place = true;
		}
		else if(CanPlace(place_row, place_col+1, pItem))
		{
			place_row;
			++place_col;
			found_place = true;
		}
		else if(CanPlace(place_row+1, place_col, pItem))
		{
			++place_row;
			place_col;
			found_place = true;
		}
		else if(CanPlace(place_row-1, place_col, pItem))
		{
			--place_row;
			place_col;
			found_place = true;
		}
		else if(CanPlace(place_row, place_col-1, pItem))
		{
			place_row;
			--place_col;
			found_place = true;
		}
	}


	//проверить можно ли разместить элемент,
	//провер€ем последовательно каждую клеточку

	for(i=0; (i<GetRows()-pItem->GetGridHeight()+1) && !found_place; ++i)
	{
		for(j=0; (j<GetCols()-pItem->GetGridWidth()+1) && !found_place; ++j)
		{
			can_place = CanPlace(i,j, pItem);

			if(can_place)
			{
				found_place=true;	
				place_row = i;
				place_col = j;
			}

		}
	}

	return found_place;
}



void CUIDragDropList::PlaceItemAtPos(int place_row, int place_col, CUIDragDropItem* pItem)
{
	int k,m;

	for(k=0; k<pItem->GetGridHeight(); ++k)
	{
		for(m=0; m<pItem->GetGridWidth(); ++m)
		{
			GetCell(place_row+k, place_col+m) = CELL_FULL;
		}
	}

	pItem->SetGridRow(place_row);
	pItem->SetGridCol(place_col);


	//разместить само окно элемента
	pItem->MoveWindow(place_col*GetCellWidth(),
		(place_row-m_iCurrentFirstRow)*GetCellHeight());

	pItem->SetWidth(GetCellWidth()*pItem->GetGridWidth());
	pItem->SetHeight(GetCellHeight()*pItem->GetGridHeight());

	pItem->m_bInFloat = false;
}


bool CUIDragDropList::CanPlace(int row, int col, CUIDragDropItem* pItem)
{
	int k,m;

	
	if(row<0 || col<0)
		return false;
	if(row+pItem->GetGridHeight()-1 >= GetRows())
		return false;
	if(col+pItem->GetGridWidth()-1  >= GetCols())
		return false;



	for(k=0; k<pItem->GetGridHeight(); ++k)
	{
		for(m=0; m<pItem->GetGridWidth(); ++m)
		{
			if(GetCellState(row+k, col+m) == CELL_FULL)
				return false;
		}
	}
	
	return true;
}

//удаление элемента из сетки
void CUIDragDropList::RemoveItemFromGrid(CUIDragDropItem* pItem)
{
	if (pItem->m_bInFloat) return;
	pItem->m_bInFloat = true;

	int k, m;

	int place_row = pItem->GetGridRow();
	int place_col = pItem->GetGridCol();

    for(k=0; k<pItem->GetGridHeight(); ++k)
	{
		for(m=0; m<pItem->GetGridWidth(); ++m)
		{
			GetCell(place_row+k, place_col+m) = CELL_EMPTY;
		}
	}
}

E_CELL_STATE CUIDragDropList::GetCellState(int row, int col) 
{
	if(row<GetRows() && col<GetCols())
		return m_vGridState[row*GetCols() + col];
	else
		return CELL_FULL;
}


void CUIDragDropList::Draw()
{
	inherited::Draw();
}
void CUIDragDropList::Update()
{
	inherited::Update();
}

void CUIDragDropList::UpdateList()
{
	for(WINDOW_LIST_it it = m_ChildWndList.begin(); m_ChildWndList.end() != it; ++it)
	{
		CUIDragDropItem* pDragDropItem = dynamic_cast<CUIDragDropItem*>(*it);
		if(pDragDropItem)
		{
			int y = (pDragDropItem->GetGridRow()-m_iCurrentFirstRow)*GetCellHeight();
			int x = (pDragDropItem->GetGridCol())*GetCellWidth();
			pDragDropItem->MoveWindow(x, y);
		}
	}
}

//установление позиции скролинга
void CUIDragDropList::SetScrollPos(int iScrollPos)
{
	m_ScrollBar.SetScrollPos(s16(iScrollPos));
	m_iCurrentFirstRow = m_ScrollBar.GetScrollPos();
	UpdateList();
}
int CUIDragDropList::GetScrollPos()
{
	return m_iCurrentFirstRow;
}
void CUIDragDropList::DetachAll()
{
	//m_DragDropItemsList.clear();
	inherited::DetachAll();
}

void CUIDragDropList::EnableScrollBar(bool enable)
{
	m_bScrollBarEnabled = enable;

	if(m_bScrollBarEnabled)
	{
		m_ScrollBar.Enable(true);
		m_ScrollBar.Show(true);
	}
	else
	{
		m_ScrollBar.Enable(false);
		m_ScrollBar.Show(false);
	}

}