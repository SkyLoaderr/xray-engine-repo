// UIDragDropList.cpp: ������ ��������� Drag&Drop
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
}

CUIDragDropList::~CUIDragDropList()
{
	m_vGridState.clear();
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



//��������� ��������� ��� DragDrop
void CUIDragDropList::SendMessage(CUIWindow *pWnd, s16 msg, void *pData)
{
	if(msg == CUIDragDropItem::ITEM_DRAG)
	{
		//����������� �� ������� ����������� ��������� ITEM_DRAG ������ ������
		WINDOW_LIST_it it = std::find(m_ChildWndList.begin(), 
										m_ChildWndList.end(), 
										pWnd);
		
		//������� ���, ������� ���� �� ������� ������, 
		//���� ��� ����� �������� ���� � ���� ������� ������ ���� �����
		//� �������� ��������� DRAG � ��������� �������
		if( it != m_ChildWndList.end())
		{
			//������� ���� ������ � ���������� �� �������
			BringAllToTop(); 

			
			//��������� ������� �� �����
			RemoveItemFromGrid((CUIDragDropItem*)pWnd);

		}
	}
	else if(msg == CUIDragDropItem::ITEM_DROP)
	{	
		//����������� �� ������� ����������� ��������� ITEM_DROP ������ ������
		WINDOW_LIST_it it = std::find(m_ChildWndList.begin(), 
										m_ChildWndList.end(), 
										pWnd);

		CUIDragDropItem* pItem = (CUIDragDropItem*)pWnd;

		//������� �����, ���� ����� ��� ����, 
		//�� ������ ���� �� � ����� �������
		if( it == m_ChildWndList.end())
		{
			RECT rect = GetAbsoluteRect();
			POINT pt;
			pt.x = pWnd->GetAbsoluteRect().left;
			pt.y = pWnd->GetAbsoluteRect().top;
			
			if(PtInRect(&rect, pt))
			{
				////���� ���� ���� ����� 
				if(PlaceItemInGrid(pItem))
				{	
					//����������� � �������� ��������
					pItem->GetParent()->SetCapture(pItem, false);
					pItem->GetParent()->DetachChild(pItem);

					//������������ ���
					CUIWindow::AttachChild(pItem);
					pItem->BringAllToTop(); 
				}
			}
		}
		//������� ���, ������� ��� �� ����� ��� ��� ������
		else
		{
			PlaceItemInGrid(pItem);
			/*pItem->MoveWindow(pItem->GetPreviousPos().x,
	 						  pItem->GetPreviousPos().y);*/
		}
	}
	

	CUIWindow::SendMessage(pWnd, msg, pData);
}



//������������� ����� Drag&Drop
void CUIDragDropList::InitGrid(int iRowsNum, int iColsNum)
{
	m_iColsNum=iColsNum;
	m_iRowsNum=iRowsNum;

	m_vGridState.resize(0);
	m_vGridState.resize(m_iRowsNum*m_iColsNum, CELL_EMPTY);
  
}

//���������� �������� �� ��������� ����� � �����
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

	//��������� ����� �� ���������� �������,
	//��������� ��������������� ������ ��������
	
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


	//���������� ������� �� ��������� �����
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

		//���������� ���� ���� ��������
		pItem->MoveWindow(place_col*GetCellWidth(),
							place_row*GetCellHeight());

		return true;
	}
	else
	{
		return false;
	}
}

//�������� �������� �� �����
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