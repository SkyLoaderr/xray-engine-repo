//////////////////////////////////////////////////////////////////////
// UIListWnd.cpp: окно со списком
//////////////////////////////////////////////////////////////////////


#include"stdafx.h"

#include ".\uilistwnd.h"

#define ACTIVE_BACKGROUND "ui\\ui_pop_up_active_back"
#define ACTIVE_BACKGROUND_WIDTH 16
#define ACTIVE_BACKGROUND_HEIGHT 16

CUIListWnd::CUIListWnd(void)
{
	m_bScrollBarEnabled = false;
	m_bActiveBackgroundEnable = false;
	m_bListActivity = true;

	m_iFocusedItem = -1;
	
	m_dwFontColor = 0xFFFFFFFF;

	SetItemHeight(DEFAULT_ITEM_HEIGHT);
}

CUIListWnd::~CUIListWnd(void)
{
	//очистить список и удалить все элементы
	for(LIST_ITEM_LIST_it it=m_ItemList.begin(); m_ItemList.end() != it; ++it)
	{
		DetachChild(*it);
		if(NULL != *it)xr_delete(*it);
	}

	m_ItemList.clear();
}

void CUIListWnd::Init(int x, int y, int width, int height)
{
	Init(x, y, width, height, DEFAULT_ITEM_HEIGHT);
}

void CUIListWnd::Init(int x, int y, int width, int height, int item_height)
{
	CUIWindow::Init(x, y, width, height);

	SetItemWidth(width - CUIScrollBar::SCROLLBAR_WIDTH);
	
	m_iFirstShownIndex = 0;

	SetItemHeight(item_height);
	m_iRowNum = height/m_iItemHeight;

	UpdateList();

	//добавить полосу прокрутки
	AttachChild(&m_ScrollBar);
	m_ScrollBar.Init(width-CUIScrollBar::SCROLLBAR_WIDTH,
						0,height, false);

	m_ScrollBar.SetRange(0,0);
	m_ScrollBar.SetPageSize(s16(0));
	m_ScrollBar.SetScrollPos(s16(m_iFirstShownIndex));

	m_ScrollBar.Show(false);
	m_ScrollBar.Enable(false);


	m_StaticActiveBackground.Init(ACTIVE_BACKGROUND,"hud\\default", 0,0,alNone);
	m_StaticActiveBackground.SetTile(m_iItemWidth/ACTIVE_BACKGROUND_WIDTH, 
									 m_iItemHeight/ACTIVE_BACKGROUND_HEIGHT,
									 m_iItemWidth%ACTIVE_BACKGROUND_WIDTH, 
									 m_iItemHeight%ACTIVE_BACKGROUND_HEIGHT);
}

void CUIListWnd::SetWidth(int width)
{
	inherited::SetWidth(width);
	m_StaticActiveBackground.SetTile(GetWidth()/ACTIVE_BACKGROUND_WIDTH, 
									 m_iItemHeight/ACTIVE_BACKGROUND_HEIGHT,
									 GetWidth()%ACTIVE_BACKGROUND_WIDTH, 
									 m_iItemHeight%ACTIVE_BACKGROUND_HEIGHT);
}


//добавляет элемент созданный извне
bool CUIListWnd::AddItem(CUIListItem* pItem)
{	
	AttachChild(pItem);
	pItem->Init(0, GetSize()* m_iItemHeight, 
				m_iItemWidth, m_iItemHeight);
	
	
	m_ItemList.push_back(pItem);
	pItem->SetIndex(m_ItemList.size()-1);
//	m_ItemList.push_front(pItem);
//pItem->SetIndex(0);
	
	

	UpdateList();

	//обновить полосу прокрутки
	m_ScrollBar.SetRange(0,s16(m_ItemList.size()-1));
	m_ScrollBar.SetPageSize(s16(
							(u32)m_iRowNum<m_ItemList.size()?m_iRowNum:m_ItemList.size()));
	m_ScrollBar.SetScrollPos(s16(m_iFirstShownIndex));

	UpdateScrollBar();

	return true;
}

bool CUIListWnd::AddItem(const char*  str, void* pData, int value)
{
	//создать новый элемент и добавить его в список
	CUIListItem* pItem = NULL;
	pItem = xr_new<CUIListItem>();

    if(!pItem) return false;


	pItem->Init(str, 0, GetSize()* m_iItemHeight, 
					m_iItemWidth, m_iItemHeight);

	pItem->SetData(pData);
	pItem->SetValue(value);
	pItem->SetTextColor(m_dwFontColor);

	return AddItem(pItem);
}

void CUIListWnd::RemoveItem(int index)
{
	if(index<0 || index>=(int)m_ItemList.size()) return;

	LIST_ITEM_LIST_it it;

	//выбрать нужный элемент
	it = m_ItemList.begin();
	for(int i=0; i<index;++i, ++it);

	R_ASSERT(m_ItemList.end() != it);

	DetachChild(*it);
	xr_delete(*it);

	m_ItemList.erase(it);

	UpdateList();

	//обновить полосу прокрутки
	if(m_ItemList.size()>0)
		m_ScrollBar.SetRange(0,s16(m_ItemList.size()-1));
	else
		m_ScrollBar.SetRange(0,0);

	m_ScrollBar.SetPageSize(s16((u32)m_iRowNum<m_ItemList.size()?
									 m_iRowNum:m_ItemList.size()));
	m_ScrollBar.SetScrollPos(s16(m_iFirstShownIndex));
}

CUIListItem* CUIListWnd::GetItem(int index)
{
	if(index<0 || index>=(int)m_ItemList.size()) return NULL;

	LIST_ITEM_LIST_it it;

	//выбрать нужный элемент
	it = m_ItemList.begin();
	for(int i=0; i<index;++i, ++it);

	R_ASSERT(m_ItemList.end() != it);

	return (*it);
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
		if(NULL != *it) xr_delete(*it);

		m_ItemList.erase(it);
	}

	m_iFirstShownIndex = 0;
	
	
	UpdateList();
	Reset();

	//обновить полосу прокрутки
	m_ScrollBar.SetRange(0,0);
	m_ScrollBar.SetPageSize(0);
	m_ScrollBar.SetScrollPos(s16(m_iFirstShownIndex));

	UpdateScrollBar();
}

void CUIListWnd::UpdateList()
{
	LIST_ITEM_LIST_it it=m_ItemList.begin();
	
	//спрятать все элементы до участка 
	//отображающейся в данный момент
	for(int i=0; i<_min(m_ItemList.size(),m_iFirstShownIndex);
					++i, ++it)
	{
		(*it)->Show(false);
		(*it)->Enable(false);
	}
	   

	//показать текущий список
	for(i=m_iFirstShownIndex; 
			i<_min(m_ItemList.size(),m_iFirstShownIndex + m_iRowNum+1);
			++i, ++it)
	{
		(*it)->SetWndRect(0, (i-m_iFirstShownIndex)* m_iItemHeight, 
							m_iItemWidth, m_iItemHeight);
		(*it)->Show(true);
		
		if(m_bListActivity) 
			(*it)->Enable(true);
		else
			(*it)->Enable(false);
	}

	--it;

	//спрятать все после
	for(u32 k=m_iFirstShownIndex + m_iRowNum; 
			k<m_ItemList.size(); ++k, ++it)
	{
		(*it)->Show(false);
		(*it)->Enable(false);
	}


	UpdateScrollBar();
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
	
		if( m_ChildWndList.end() != it)
		{
			CUIListItem* pListItem = dynamic_cast<CUIListItem*>(*it);
			R_ASSERT(pListItem);

			if(CUIListItem::BUTTON_CLICKED == msg)
			{
				GetParent()->SendMessage(this, LIST_ITEM_CLICKED, pListItem);
			}
			else if(CUIListItem::BUTTON_FOCUS_RECEIVED == msg)
			{
				m_iFocusedItem = pListItem->GetIndex();
			}
			else if(CUIListItem::BUTTON_FOCUS_LOST == msg)
			{
				if(pListItem->GetIndex() == m_iFocusedItem) m_iFocusedItem = -1;
			}
		}
	}
	CUIWindow::SendMessage(pWnd, msg, pData);
}

void CUIListWnd::Draw()
{
	CUIWindow::Draw();

	if(m_iFocusedItem != -1 && m_bActiveBackgroundEnable)
	{
		RECT rect = GetAbsoluteRect();
		m_StaticActiveBackground.SetPos(rect.left, rect.top + 
									(m_iFocusedItem-m_iFirstShownIndex)*GetItemHeight());
		m_StaticActiveBackground.Render();
	}
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
	for(LIST_ITEM_LIST_it it=m_ItemList.begin();  m_ItemList.end() != it; ++it)
	{
		(*it)->Reset();
	}

	ResetAll();

	inherited::Reset();
}

//находит первый элемент с заданной pData, иначе -1
int CUIListWnd::FindItem(void* pData)
{
	int i=0;
	for(LIST_ITEM_LIST_it it=m_ItemList.begin();  m_ItemList.end() != it; ++it,++i)
	{
		if((*it)->GetData()==pData) return i;
	}
	return -1;
}

void CUIListWnd::OnMouse(int x, int y, E_MOUSEACTION mouse_action)
{
	if(mouse_action == LBUTTON_DB_CLICK) 
	{
		mouse_action = CUIWindow::LBUTTON_DOWN;
	}
	inherited::OnMouse(x, y, mouse_action);
}

int CUIListWnd::GetLongestSignWidth()
{
	int max_width = m_ItemList.front()->GetSignWidht();
	
	LIST_ITEM_LIST_it it=m_ItemList.begin();
	++it;
	for(;  m_ItemList.end() != it; ++it)
	{
		if((*it)->GetSignWidht()>max_width) max_width = (*it)->GetSignWidht();
	}

	return max_width;
}

void CUIListWnd::UpdateScrollBar()
{
	//спрятать скорлинг, если он не нужен
	if(m_bScrollBarEnabled)
		if(m_ItemList.size()<=m_ScrollBar.GetPageSize())
			m_ScrollBar.Show(false);
		else
			m_ScrollBar.Show(true);
	
}

void CUIListWnd::EnableScrollBar(bool enable)
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

	UpdateScrollBar();
}

void CUIListWnd::ActivateList(bool activity)
{
	m_bListActivity = activity;
}

void CUIListWnd::ScrollToBegin()
{
	m_ScrollBar.SetScrollPos((s16)m_ScrollBar.GetMinRange());
	m_iFirstShownIndex = m_ScrollBar.GetScrollPos();
	UpdateList();
}
void CUIListWnd::ScrollToEnd()
{
	int pos = m_ScrollBar.GetMaxRange()- m_ScrollBar.GetPageSize() + 1;

	if(pos > m_ScrollBar.GetMinRange())
		m_ScrollBar.SetScrollPos((s16)pos);
	else
		m_ScrollBar.SetScrollPos((s16)m_ScrollBar.GetMinRange());

	m_iFirstShownIndex = m_ScrollBar.GetScrollPos();
	UpdateList();
}