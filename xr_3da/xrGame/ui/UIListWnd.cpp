//////////////////////////////////////////////////////////////////////
// UIListWnd.cpp: окно со списком
//////////////////////////////////////////////////////////////////////


#include"stdafx.h"

#include ".\uilistwnd.h"

CUIListWnd::CUIListWnd(void)
{
}

CUIListWnd::~CUIListWnd(void)
{
	//очистить список и удалить все элементы
	for(LIST_ITEM_LIST_it it=m_ItemList.begin(); it!=m_ItemList.end(); it++)
	{
		DetachChild(*it);
		if(*it!=NULL)xr_delete(*it);
	}

	m_ItemList.clear();
}

void CUIListWnd::Init(int x, int y, int width, int height)
{
	CUIWindow::Init(x, y, width, height);

	SetItemWidth(width - CUIScrollBar::SCROLLBAR_WIDTH);
	SetItemHeight(30);

	m_iFirstShownIndex = 0;
	m_iRowNum = height/m_iItemHeight;

	AttachChild(&m_ScrollBar);
	m_ScrollBar.Init(width-CUIScrollBar::SCROLLBAR_WIDTH,
						0,height, false);

	UpdateList();

	//обновить полосу прокрутки
	m_ScrollBar.SetRange(0,s16(m_ItemList.size()-1));
	m_ScrollBar.SetPageSize(s16(m_iRowNum));
	m_ScrollBar.SetScrollPos(s16(m_iFirstShownIndex));
}



bool CUIListWnd::AddItem(char*  str, void* pData)
{
	//создать новый элемент и добавить его в список
	CUIListItem* pItem = NULL;
	pItem = xr_new<CUIListItem>();
		
    if(!pItem) return false;

	pItem->Init(str, 0, GetSize()* m_iItemHeight, 
					m_iItemWidth, m_iItemHeight);

	AttachChild(pItem);

	pItem->SetData(pData);

	m_ItemList.push_back(pItem);
	pItem->SetIndex(m_ItemList.size()-1);

	UpdateList();

	//обновить полосу прокрутки
	m_ScrollBar.SetRange(0,s16(m_ItemList.size()-1));
	m_ScrollBar.SetPageSize(s16(m_iRowNum));
	m_ScrollBar.SetScrollPos(s16(m_iFirstShownIndex));


	return true;
}

void CUIListWnd::RemoveItem(int index)
{
	if(index<0 || index>=(int)m_ItemList.size()) return;

	LIST_ITEM_LIST_it it;

	//выбрать нужный элемент
	it = m_ItemList.begin();
	for(int i=0; i<index;i++, it++);

	R_ASSERT(it!=m_ItemList.end());

	DetachChild(*it);
	xr_delete(*it);

	m_ItemList.erase(it);

	UpdateList();

	//обновить полосу прокрутки
	m_ScrollBar.SetRange(0,s16(m_ItemList.size()-1));
	m_ScrollBar.SetPageSize(s16(m_iRowNum));
	m_ScrollBar.SetScrollPos(s16(m_iFirstShownIndex));

}

//убрать все элементы из списка
void CUIListWnd::RemoveAll()
{
	if(m_ItemList.empty()) return;

	LIST_ITEM_LIST_it it;

		
	while(!m_ItemList.empty())
	{
		it = m_ItemList.begin();
		
		DetachChild(*it);
		if(*it != NULL) xr_delete(*it);

		m_ItemList.erase(it);
	}

	m_iFirstShownIndex = 0;
	
	UpdateList();
	Reset();

	//обновить полосу прокрутки
	m_ScrollBar.SetRange(0,s16(m_ItemList.size()-1));
	m_ScrollBar.SetPageSize(s16(m_iRowNum));
	m_ScrollBar.SetScrollPos(s16(m_iFirstShownIndex));
}

void CUIListWnd::UpdateList()
{
	LIST_ITEM_LIST_it it=m_ItemList.begin();
	
	//спрятать все элементы до участка 
	//отображающейся в данный момент
	for(int i=0; i<_min(m_ItemList.size(),m_iFirstShownIndex);
					i++, it++)
	{
		(*it)->Show(false);
		(*it)->Enable(false);
	}
	   

	//показать текущий список
	for(i=m_iFirstShownIndex; 
			i<_min(m_ItemList.size(),m_iFirstShownIndex + m_iRowNum+1);
			i++, it++)
	{
		(*it)->SetWndRect(0, (i-m_iFirstShownIndex)* m_iItemHeight, 
							m_iItemWidth, m_iItemHeight);
		(*it)->Show(true);
		(*it)->Enable(true);
	}

	it--;

	//спрятать все после
	for(u32 k=m_iFirstShownIndex + m_iRowNum; 
			k<m_ItemList.size(); k++, it++)
	{
		(*it)->Show(false);
		(*it)->Enable(false);
	}
}




void CUIListWnd::SendMessage(CUIWindow *pWnd, s16 msg, void *pData)
{
	if(pWnd == &m_ScrollBar)
	{
		if(msg == CUIScrollBar::VSCROLL)
		{
			m_iFirstShownIndex = m_ScrollBar.GetScrollPos();
			UpdateList();
		}
	}
	else 
	{
		//если сообщение пришло от одного из элементов списка
		WINDOW_LIST_it it = std::find(m_ChildWndList.begin(), 
									  m_ChildWndList.end(), 
									  pWnd);
	
		if( it != m_ChildWndList.end() && msg == CUIListItem::BUTTON_CLICKED)
		{
			GetParent()->SendMessage(this, LIST_ITEM_CLICKED, *it);
		}
	}
	CUIWindow::SendMessage(pWnd, msg, pData);
}

void CUIListWnd::Draw()
{
	CUIWindow::Draw();
}

int CUIListWnd::GetSize()
{
	return (int)m_ItemList.size();
}


void CUIListWnd::SetItemWidth(int iItemWidth)
{
	m_iItemWidth = iItemWidth;
}

void CUIListWnd::SetItemHeight(int iItemHeight)
{
	m_iItemHeight = iItemHeight;
}

void CUIListWnd::Reset()
{
	for(LIST_ITEM_LIST_it it=m_ItemList.begin();  it != m_ItemList.end(); it++)
	{
		(*it)->Reset();
	}
}

//находит первый элемент с заданной pData, иначе -1
int CUIListWnd::FindItem(void* pData)
{
	int i=0;
	for(LIST_ITEM_LIST_it it=m_ItemList.begin();  it != m_ItemList.end(); it++,i++)
	{
		if((*it)->GetData()==pData) return i;
	}
	return -1;
}