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
}

void CUIDragDropList::DetachChild(CUIDragDropItem* pChild)
{
	CUIWindow::DetachChild(pChild);
}



//обработка сообщений для DragDrop
void CUIDragDropList::SendMessage(CUIWindow *pWnd, s16 msg, void *pData)
{
	if(msg == CUIDragDropItem::ITEM_DRAG)
	{
		//принадлежит ли элемент отправивший сообщение ITEM_DRAG нашему списку
		WINDOW_LIST_it it = std::find(m_ChildWndList.begin(), 
										m_ChildWndList.end(), 
										pWnd);
		
		//элемент наш, поднять окно на вершину списка, 
		//чтоб оно могло выводить себя и этот элемент поверх всех оконо
		//и получило сообщения DRAG в последнюю очередь
		if( it != m_ChildWndList.end())
		{
			//поднять окно вместе с родителями на вершину
			BringAllToTop(); 

			
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
			POINT pt;
			pt.x = pWnd->GetAbsoluteRect().left;
			pt.y = pWnd->GetAbsoluteRect().top;
			
			if(PtInRect(&rect, pt))
			{
				////если есть куда брать 
				if(PlaceItemInGrid(pItem))
				{	
					//отсоединить у прошлого родителя
					pItem->GetParent()->SetCapture(pItem, false);
					pItem->GetParent()->DetachChild(pItem);

					//присоединить нам
					CUIWindow::AttachChild(pItem);
					pItem->BringAllToTop(); 
				}
			}
		}
		//элемент наш, вернуть его на место где был раньше
		else
		{
			PlaceItemInGrid(pItem);
			/*pItem->MoveWindow(pItem->GetPreviousPos().x,
	 						  pItem->GetPreviousPos().y);*/
		}
	}
	

	CUIWindow::SendMessage(pWnd, msg, pData);
}



//инициализация сетки Drag&Drop
void CUIDragDropList::InitGrid(int iRowsNum, int iColsNum, bool bGridVisible)
{
	m_iColsNum=iColsNum;
	m_iRowsNum=iRowsNum;

	
	m_vGridState.resize(m_iRowsNum*m_iColsNum, CELL_EMPTY);

	if(bGridVisible)
	{
		m_vCellStatic.resize(m_iRowsNum*m_iColsNum);

		int i,j;

		
		CELL_STATIC_IT it=m_vCellStatic.begin();
		
		for(i=0; i<GetRows(); i++)
		{
			for(j=0; j<GetCols(); j++)
			{

				(*it).Init("ui\\ui_inv_lattice", 
									j*GetCellWidth(),
									i*GetCellHeight(),
									GetCellWidth(),
									GetCellHeight());

				CUIWindow::AttachChild(&(*it));
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

	int place_row = 0,  place_col = 0;

	bool found_place;
	bool can_place;

	//проверить можно ли разместить элемент,
	//проверяем последовательно каждую клеточку
	
	found_place = false;
	

	for(i=0; (i<GetRows()-pItem->GetGridHeight()+1) && !found_place; i++)
	{
		for(j=0; (j<GetCols()-pItem->GetGridWidth()+1) && !found_place; j++)
		{
			can_place = true;

			for(k=0; (k<pItem->GetGridHeight()) && can_place; k++)
			{
				for(m=0; (m<pItem->GetGridWidth()) && can_place; m++)
				{
					if(GetCell(i+k, j+m) == CELL_FULL)
							can_place =  false;
				}
			}
			
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
							place_row*GetCellHeight());

		return true;
	}
	else
	{
		return false;
	}
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