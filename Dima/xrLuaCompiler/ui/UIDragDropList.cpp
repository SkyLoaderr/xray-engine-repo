// UIDragDropList.cpp: список элементов Drag&Drop
//
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "uidragdroplist.h"

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
}

CUIDragDropList::~CUIDragDropList()
{
	m_vGridState.clear();
	m_vCellStatic.clear();
	
}

void CUIDragDropList::AttachChild(CUIDragDropItem* pChild)
{
	CUIWindow::AttachChild(pChild);
	PlaceItemInGrid(pChild);

	pChild->SetWidth(GetCellWidth()*pChild->GetGridWidth());
	pChild->SetHeight(GetCellHeight()*pChild->GetGridHeight());


	/*
	m_iCurrentFirstRow = pChild->GetGridRow();
	m_ScrollBar.SetScrollPos(s16(m_iCurrentFirstRow));
	m_iCurrentFirstRow = m_ScrollBar.GetScrollPos();
	UpdateList();
	*/

	pChild->ClipperOn();
//	pChild->TextureClipper();
}

void CUIDragDropList::DetachChild(CUIDragDropItem* pChild)
{
	WINDOW_LIST_it it = std::find(m_ChildWndList.begin(), 
								  m_ChildWndList.end(), 
								  pChild);
	
	if( it != m_ChildWndList.end())
	{
		RemoveItemFromGrid(pChild);
		CUIWindow::DetachChild(pChild);
	}
}

void CUIDragDropList::AttachChild(CUIWindow* pChild)
{
	inherited::AttachChild(pChild);
}
void CUIDragDropList::DetachChild(CUIWindow* pChild)
{
	inherited::DetachChild(pChild);
}

void CUIDragDropList::DropAll()
{
	CUIWindow::DetachAll();

	//освободить фокус мыши, если вдруг кто забыл это сделать!!!!
	//а ведь забывают, суки!!!!
	//выскакивает глюк, когда вместо одного элемента
	//тянется совсем другой.
	GetParent()->SetCapture(this, false);
	m_pMouseCapturer = NULL;

	for(u32 i=0; i<m_vCellStatic.size(); i++)
	{
		CUIWindow::AttachChild(&m_vCellStatic[i]);
	}
	CUIWindow::AttachChild(&m_ScrollBar);

/*	for(WINDOW_LIST_it it=m_ChildWndList.begin(); it!=m_ChildWndList.end(); it++)
	{
		CUIDragDropItem* pDragDropItem = dynamic_cast<CUIDragDropItem*>(*it);
		if(pDragDropItem) DetachChild(pDragDropItem);
	}*/

	

	int k,m;

	for(k=0; k<GetRows(); k++)
	{
		for(m=0; m<GetCols(); m++)
		{
			GetCell(k, m) = CELL_EMPTY;
		}
	}
}


//обработка сообщений для DragDrop
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
	else if(msg == CUIDragDropItem::ITEM_DRAG)
	{
		//принадлежит ли элемент отправивший сообщение ITEM_DRAG нашему списку
		WINDOW_LIST_it it = std::find(m_ChildWndList.begin(),
										m_ChildWndList.end(),
										pWnd);
		
		//элемент наш, поднять окно на вершину списка, 
		//чтоб оно могло выводить себя и этот элемент поверх всех окон
		//и получило сообщения DRAG в последнюю очередь
		if( it != m_ChildWndList.end())
		{
			//поднять окно вместе с родителями на вершину
			(*it)->BringAllToTop(); 

			//выбросить элемент из сетки
			RemoveItemFromGrid((CUIDragDropItem*)pWnd);

		}
	}
	else if(msg == CUIDragDropItem::ITEM_DROP)
	{	
		//принадлежит ли элемент отправивший сообщение ITEM_DROP нашему списку
		WINDOW_LIST_it it = std::find(m_ChildWndList.begin(), 
										m_ChildWndList.end(), 
										pWnd);

		CUIDragDropItem* pItem = (CUIDragDropItem*)pWnd;

		//элемени чужой, надо взять его себе, 
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

			if(PtInRect(&rect, pt_lt) || 
			   PtInRect(&rect, pt_lb) ||
			   PtInRect(&rect, pt_rt) ||
			   PtInRect(&rect, pt_rb))
			{

				//отсоединить у прошлого родителя
				//((CUIDragDropList*)pItem->GetParent())->DetachChild(pItem);


				////если есть куда брать и работает доп. проверка
				OnCustomPlacement();
				if(PlaceItemInGrid(pItem))
				{	
					//доп. проверка при помощи присоединенной функции
					bool additional_check;
				
					if(GetCheckProc())
						additional_check = (*(GetCheckProc()))(pItem, this);
					else
						additional_check = true;

					if(additional_check)
					{
						//отсоединить у прошлого родителя
						pItem->GetParent()->SetCapture(pItem, false);
						pItem->GetParent()->DetachChild(pItem);

						//присоединить нам
						CUIWindow::AttachChild(pItem);
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
			//pItem->MoveWindow(pItem->GetPreviousPos().x,
	 		//				  pItem->GetPreviousPos().y);
		}
	}
	
	CUIWindow::SendMessage(pWnd, msg, pData);
}



void CUIDragDropList::Init(int x, int y, int width, int height)
{
//	m_ScrollBar.Init(GetWidth()-CUIScrollBar::SCROLLBAR_WIDTH,0,
//					 GetHeight(), false);

	inherited::Init(x, y, width, height);
}
//инициализация сетки Drag&Drop
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



	SetWidth(m_iColsNum*GetCellWidth() + CUIScrollBar::SCROLLBAR_WIDTH);
	SetHeight(GetViewRows()*GetCellHeight());

	AttachChild(&m_ScrollBar);
	m_ScrollBar.Init(GetWidth() - CUIScrollBar::SCROLLBAR_WIDTH, 0, 
					GetHeight(), false);
	/*m_ScrollBar.SetRange(0,10);
	m_ScrollBar.SetPageSize(2);
	m_ScrollBar.SetScrollPos(0);*/


	m_vGridState.resize(m_iRowsNum*m_iColsNum, CELL_EMPTY);

	if(bGridVisible)
	{
		m_vCellStatic.resize(GetViewRows()*m_iColsNum);

		int i,j;

		//временно!
		//установить масштаб для клеточки
		float scale = GetCellWidth()/50.f;
		
		CELL_STATIC_IT it=m_vCellStatic.begin();
		
		for(i=0; i<GetViewRows(); i++)
		{
			for(j=0; j<GetCols(); j++)
			{

				(*it).Init("ui\\ui_inv_lattice", 
								j*GetCellWidth(),
								i*GetCellHeight(),
								GetCellWidth(),
								GetCellHeight());

				AttachChild(&(*it));
				
				(*it).SetTextureScale(scale);

				it++;
			}
		}

	}
}

//размещение элемента на свободном месте в сетке
bool CUIDragDropList::PlaceItemInGrid(CUIDragDropItem* pItem)
{
	R_ASSERT(pItem->GetGridHeight()>0);
	R_ASSERT(pItem->GetGridWidth()>0);

	R_ASSERT(GetRows()>0);
	R_ASSERT(GetCols()>0);

	int i,j;
	int k,m;

	int place_row = -1,  place_col = -1;

	bool found_place;
	bool can_place;

	found_place = false;

	//проверяем, можно ли разместить элемент на том месте на 
	//котором он сейчас
	RECT item_rect = pItem->GetAbsoluteRect();
	RECT rect = GetAbsoluteRect();
//	RECT wnd_rect = GetWndRect();

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
		place_col = item_center_x/GetCellWidth() - pItem->GetGridWidth()/2;
	}


	if(place_row == -1)
	{
		place_row = m_iCurrentFirstRow + 
					item_center_y/GetCellHeight() - pItem->GetGridHeight()/2;
	}


	if(place_col<0)
		place_col = 0;
	if(place_col>=GetCols())
		place_col = GetCols()- 1 - pItem->GetGridWidth()/2;
	if(place_row<0)
		place_row = 0;
	if(place_row>=GetRows())
		place_row = GetRows()- 1 - pItem->GetGridHeight()/2;

	//m_bCustomPlacement = false;
	if(m_bCustomPlacement && !m_bBlockCustomPlacement)
	{
		if(CanPlace(place_row, place_col, pItem))
		{
			found_place = true;
		}
		else if(CanPlace(place_row+1, place_col, pItem))
		{
			place_row++;
			place_col;
			found_place = true;
		}
		else if(CanPlace(place_row-1, place_col, pItem))
		{
			place_row--;
			place_col;
			found_place = true;
		}
		else if(CanPlace(place_row, place_col+1, pItem))
		{
			place_row;
			place_col++;
			found_place = true;
		}
		else if(CanPlace(place_row, place_col-1, pItem))
		{
			place_row;
			place_col--;
			found_place = true;
		}
	}

	


	//проверить можно ли разместить элемент,
	//проверяем последовательно каждую клеточку
	
	//found_place = false;
		

	for(i=0; (i<GetRows()-pItem->GetGridHeight()+1) && !found_place; i++)
	{
		for(j=0; (j<GetCols()-pItem->GetGridWidth()+1) && !found_place; j++)
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


	//разместить элемент на найденном месте
	if(found_place)
	{
		for(k=0; k<pItem->GetGridHeight(); k++)
		{
			for(m=0; m<pItem->GetGridWidth(); m++)
			{
				GetCell(place_row+k, place_col+m) = CELL_FULL;
				pItem->SetGridRow(place_row);
				pItem->SetGridCol(place_col);
			}
		}

		//разместить само окно элемента
		pItem->MoveWindow(place_col*GetCellWidth(),
						  (place_row-m_iCurrentFirstRow)*GetCellHeight());

		pItem->SetWidth(GetCellWidth()*pItem->GetGridWidth());
		pItem->SetHeight(GetCellHeight()*pItem->GetGridHeight());

		return true;
	}
	else
	{
		return false;
	}
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



	for(k=0; k<pItem->GetGridHeight(); k++)
	{
		for(m=0; m<pItem->GetGridWidth(); m++)
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
	int k, m;

	int place_row = pItem->GetGridRow();
	int place_col = pItem->GetGridCol();

    for(k=0; k<pItem->GetGridHeight(); k++)
	{
		for(m=0; m<pItem->GetGridWidth(); m++)
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
	for(WINDOW_LIST_it it=m_ChildWndList.begin(); it!=m_ChildWndList.end(); it++)
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