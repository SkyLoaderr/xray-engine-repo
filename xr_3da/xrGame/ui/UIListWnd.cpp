//////////////////////////////////////////////////////////////////////
// UIListWnd.cpp: ���� �� �������
//////////////////////////////////////////////////////////////////////


#include"stdafx.h"

#include ".\uilistwnd.h"
#include "UIInteractiveListItem.h"

#define ACTIVE_BACKGROUND "ui\\ui_pop_up_active_back"
#define ACTIVE_BACKGROUND_WIDTH 16
#define ACTIVE_BACKGROUND_HEIGHT 16

// ����������� ��� ������������� ����� � �����
static const char cSeparatorChar = '%';


CUIListWnd::CUIListWnd(void)
{
	m_bScrollBarEnabled = false;
	m_bActiveBackgroundEnable = false;
	m_bListActivity = true;

	m_iFocusedItem = -1;
	m_iFocusedItemGroupID = -1;

	m_dwFontColor = 0xFFFFFFFF;

	SetItemHeight(DEFAULT_ITEM_HEIGHT);

	m_bVertFlip = false;

	m_bUpdateMouseMove = false;

	m_bForceFocusedItem = false;

	m_iLastUniqueID = 0;
}

CUIListWnd::~CUIListWnd(void)
{
	//�������� ������ � ������� ��� ��������
	for(LIST_ITEM_LIST_it it=m_ItemList.begin(); m_ItemList.end() != it; ++it)
	{
		DetachChild(*it);
		if(NULL != *it && !(*it)->IsManualDelete())xr_delete(*it);
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

	//�������� ������ ���������
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


//��������� ������� ��������� �����

void CUIListWnd::RemoveItem(int index)
{
	if(index<0 || index>=(int)m_ItemList.size()) return;

	LIST_ITEM_LIST_it it;

	//������� ������ �������
	it = m_ItemList.begin();
	for(int i=0; i<index;++i, ++it);

	R_ASSERT(m_ItemList.end() != it);

	DetachChild(*it);
	if (!(*it)->IsManualDelete())
		xr_delete(*it);

	m_ItemList.erase(it);

	UpdateList();

	//�������� ������ ���������
	if(m_ItemList.size()>0)
		m_ScrollBar.SetRange(0,s16(m_ItemList.size()-1));
	else
		m_ScrollBar.SetRange(0,0);

	m_ScrollBar.SetPageSize(s16((u32)m_iRowNum<m_ItemList.size()?
									 m_iRowNum:m_ItemList.size()));
	m_ScrollBar.SetScrollPos(s16(m_iFirstShownIndex));
	m_ScrollBar.Refresh();

	//�������������� ������� ������
	i=0;
	for(LIST_ITEM_LIST_it it=m_ItemList.begin();  m_ItemList.end() != it; ++it,i++)
	{
		(*it)->SetIndex(i);
	}

}

CUIListItem* CUIListWnd::GetItem(int index)
{
	if(index<0 || index>=(int)m_ItemList.size()) return NULL;

	LIST_ITEM_LIST_it it;

	//������� ������ �������
	it = m_ItemList.begin();
	for(int i=0; i<index;++i, ++it);

	R_ASSERT(m_ItemList.end() != it);

	return (*it);
}

//������ ��� �������� �� ������
void CUIListWnd::RemoveAll()
{
	if(m_ItemList.empty()) return;

	LIST_ITEM_LIST_it it;

		
	while(!m_ItemList.empty())
	{
		it = m_ItemList.begin();
		
		DetachChild(*it);
		if(NULL != *it && !(*it)->IsManualDelete()) xr_delete(*it);

		m_ItemList.erase(it);
	}

	m_iFirstShownIndex = 0;
	
	
	UpdateList();
	Reset();

	//�������� ������ ���������
	m_ScrollBar.SetRange(0,0);
	m_ScrollBar.SetPageSize(0);
	m_ScrollBar.SetScrollPos(s16(m_iFirstShownIndex));

	UpdateScrollBar();
}

void CUIListWnd::UpdateList()
{
	LIST_ITEM_LIST_it it=m_ItemList.begin();
	
	//�������� ��� �������� �� ������� 
	//�������������� � ������ ������
	for(int i=0; i<_min(m_ItemList.size(),m_iFirstShownIndex);
					++i, ++it)
	{
		(*it)->Show(false);
		(*it)->Enable(false);
	}
	   

	//�������� ������� ������
	for(i=m_iFirstShownIndex; 
			i<_min(m_ItemList.size(),m_iFirstShownIndex + m_iRowNum+1);
			++i, ++it)
	{
		(*it)->SetWndRect((*it)->GetWndRect().left, m_bVertFlip?GetHeight()-(i-m_iFirstShownIndex)* m_iItemHeight-m_iItemHeight:(i-m_iFirstShownIndex)* m_iItemHeight, 
							m_iItemWidth, m_iItemHeight);
		(*it)->Show(true);
		
		if(m_bListActivity) 
			(*it)->Enable(true);
		else
			(*it)->Enable(false);
	}

	--it;

	//�������� ��� �����
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
		//���� ��������� ������ �� ������ �� ��������� ������
		WINDOW_LIST_it it = std::find(m_ChildWndList.begin(), 
									  m_ChildWndList.end(), 
									  pWnd);
	
		if( m_ChildWndList.end() != it)
		{
			CUIListItem* pListItem2, *pListItem = dynamic_cast<CUIListItem*>(*it);
			R_ASSERT(pListItem);

			if(CUIListItem::BUTTON_CLICKED == msg)
			{
				GetMessageTarget()->SendMessage(this, LIST_ITEM_CLICKED, pListItem);
			}
			else if(CUIListItem::BUTTON_FOCUS_RECEIVED == msg)
			{
				if (!m_bForceFocusedItem)
				{
					m_iFocusedItem = pListItem->GetIndex();
					m_iFocusedItemGroupID = pListItem->GetGroupID();
				}
				else if (m_iFocusedItem >= 0)
						m_iFocusedItemGroupID = GetItem(m_iFocusedItem)->GetGroupID();


				// prototype code
				
				for (it = m_ChildWndList.begin(); it != m_ChildWndList.end(); ++it)
				{
					pListItem2 = dynamic_cast<CUIListItem*>(*it);
					if (!pListItem2) continue;
					if (pListItem2->GetGroupID() == -1) continue;
					if (pListItem2->GetGroupID() == pListItem->GetGroupID())
						pListItem2->SetHighlightText(true);
					else
						pListItem2->SetHighlightText(false);
				}
				// end prototype code
			}
			else if(CUIListItem::BUTTON_FOCUS_LOST == msg)
			{
				if(pListItem->GetIndex() == m_iFocusedItem && !m_bForceFocusedItem) m_iFocusedItem = -1;

				for (it = m_ChildWndList.begin(); it != m_ChildWndList.end(); ++it)
				{
					pListItem2 = dynamic_cast<CUIListItem*>(*it);
					if (!pListItem2) continue;
					pListItem2->SetHighlightText(false);
				}
				m_bUpdateMouseMove = true;

				/*for (it = m_ChildWndList.begin(); (it != m_ChildWndList.end()) && (m_iFocusedItem == -1); ++it)
				{
					pListItem2 = dynamic_cast<CUIListItem*>(*it);
					if (!pListItem2) continue;
					if (pWnd != pListItem2)
						pListItem2->OnMouse(cursor_pos.x - pListItem2->GetWndRect().left, cursor_pos.y - pListItem2->GetWndRect().top, MOUSE_MOVE);
				}*/
			}
		}
	}
	CUIWindow::SendMessage(pWnd, msg, pData);
}

void CUIListWnd::Draw()
{
	WINDOW_LIST_it it;

	if(m_iFocusedItem != -1 && m_bActiveBackgroundEnable)
	{
		RECT rect = GetAbsoluteRect();
		for (it = m_ChildWndList.begin(); it != m_ChildWndList.end(); ++it)
		{
			CUIListItem *pListItem2 = dynamic_cast<CUIListItem*>(*it);
			if (!pListItem2) continue;
			if (pListItem2->GetGroupID() == -1) continue;
			if ((pListItem2->GetGroupID() == m_iFocusedItemGroupID) && 
				((pListItem2->GetIndex() >= m_iFirstShownIndex) && 
				(pListItem2->GetIndex() <= m_iRowNum + m_iFirstShownIndex - 1)))
			{
				m_StaticActiveBackground.SetPos(rect.left, rect.top + 
								(pListItem2->GetIndex() - m_iFirstShownIndex)*GetItemHeight());
				m_StaticActiveBackground.Render();
			}
		}
	}

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
	for(LIST_ITEM_LIST_it it=m_ItemList.begin();  m_ItemList.end() != it; ++it)
	{
		(*it)->Reset();
	}

	ResetAll();

	inherited::Reset();
}

//������� ������ ������� � �������� pData, ����� -1
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
	//�������� ��������, ���� �� �� �����
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

void CUIListWnd::Update()
{
	if(m_bUpdateMouseMove)
	{
		OnMouse(cursor_pos.x, cursor_pos.y, MOUSE_MOVE);
		m_bUpdateMouseMove = false;
	}

	inherited::Update();
}

void CUIListWnd::SetFocusedItem(int iNewFocusedItem)
{ 
	m_iFocusedItem = iNewFocusedItem; 
	m_bForceFocusedItem = true;
	EnableActiveBackground(true);
	if (m_iFocusedItem >= 0)
		m_iFocusedItemGroupID = GetItem(m_iFocusedItem)->GetGroupID();
}

//=============================================================================
//  Interactive element insertion
//=============================================================================

xr_vector<int> CUIListWnd::AddInteractiveItem(const char *str2, const int shift,
											  const u32 &MsgColor, CGameFont* pFont, int pushAfter)
{
	string1024	str = {0};
	// ��������� ������ ��� ����������� ������
	std::strcpy(str, str2);
	// ������������ ������, ��� ����������� �������������� ���� ������ � ������� ���
	// ������������� ���������
	char				*pSep = str;
	int					count = 0;
	xr_vector<char *>	vSubItems;
	xr_vector<int>		IDs;

	vSubItems.clear();

	// ������� ���������� ������-������������
	while (pSep < str + xr_strlen(str) + 1)
	{
		pSep = std::find(pSep, str + xr_strlen(str) + 1, cSeparatorChar);
		if (pSep < str + xr_strlen(str) + 1)
		{
			++count;
			vSubItems.push_back(pSep - count + 1 );
			pSep++;
		}
	}

	// ��������� ������ �����
	if (count != 0)
	{
		char *b = std::remove(str, str + xr_strlen(str) + 1, cSeparatorChar);
		*b = '\n';
	}

	// ���� ��� �� ������ �����-�����������, ��� �� ���������� �������, �� �������, ��� ������ 
	// �� ������������
	if (count % 2 == 1 || count == 0)
	{
		// ���������� NULL ���� ������ �� ������������.
		// � ���� ������ ListWnd ������ ��� �������� ������ ������� ��������
		CUIString A;
		A.SetText(str);
		AddParsedItem<CUIListItem>(A, shift, MsgColor, pFont);
		return IDs;
	}
	int k = 0;
	while (k < count / 2)
	{
		IDs.push_back(m_iLastUniqueID++);
		++k;
	}

	// ���� ������ ���� ������������, �� ������������ ������������� ��������� - ���� �����
	CUIInteractiveListItem *pILItem = xr_new<CUIInteractiveListItem>();
	AddItem<CUIListItem>(pILItem, pushAfter);
	pILItem->Init(str, vSubItems, IDs, GetItemHeight());
	pILItem->SetFont(pFont);
	pILItem->SetTextColor(MsgColor);
	pILItem->SetMessageTarget(GetMessageTarget());
	return IDs;
}

//////////////////////////////////////////////////////////////////////////

int CUIListWnd::GetItemPos(CUIListItem *pItem)
{
	LIST_ITEM_LIST_it it = m_ItemList.begin();
	for (u32 i = 0; i < m_ItemList.size(); ++i)
	{
		if (*it == pItem) return i;
		++it;
	}

	return -1;
}