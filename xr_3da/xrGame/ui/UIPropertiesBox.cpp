// UIPropertiesBox.cpp: 
//
// ����� � ��������� ��� ������ ��������, ���������� �� ������� ������ 
// ������� ����
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UIPropertiesBox.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUIPropertiesBox::CUIPropertiesBox()
{
	m_iClickedElement = -1;
}

CUIPropertiesBox::~CUIPropertiesBox()
{
}


void CUIPropertiesBox::Init(LPCSTR base_name, int x, int y, int width, int height)
{
	CUIFrameWindow::Init(base_name, x,y, width, height);

	AttachChild(&m_UIListWnd);
	m_UIListWnd.Init(x,y, width, height);

	m_iClickedElement = -1;
}



void CUIPropertiesBox::SendMessage(CUIWindow *pWnd, s16 msg, void *pData)
{
	if(pWnd == &m_UIListWnd)
	{
			if(msg == CUIListWnd::LIST_ITEM_CLICKED)
			{
				GetParent()->SendMessage(this, PROPERTY_CLICKED);
				m_iClickedElement = ((CUIListItem*)pData)->GetIndex();

				Hide();
			}
	}

	CUIFrameWindow::SendMessage(pWnd, msg, pData);
}

bool CUIPropertiesBox::AddItem(char*  str, void* pData)
{
	return m_UIListWnd.AddItem(str, pData);
}
void CUIPropertiesBox::RemoveItem(int index)
{
	m_UIListWnd.RemoveItem(index);
}

void CUIPropertiesBox::Show(int x, int y)
{
	int x_pos, y_pos;

	if(x+GetWidth()<(int)Device.dwWidth)
		x_pos = x;
	else
		x_pos = x - (x+GetWidth()-(int)Device.dwWidth);
	
	if(y+GetHeight()<(int)Device.dwHeight)
		y_pos = y;
	else
		y_pos = y - (y+GetHeight()-(int)Device.dwHeight);
		
	MoveWindow(x_pos,y_pos);

	CUIWindow::Show(true);
	CUIWindow::Enable(true);

	//����� ����������� ���� ����
	GetParent()->SetCapture(this, true);
	//��������� �������� ����� ��������� ������ ���� 
	//�� ��������� � ������ ������������� ����
	m_pOrignMouseCapturer = this;
	
	m_iClickedElement = -1;

	m_UIListWnd.Reset();
}
void CUIPropertiesBox::Hide()
{
	CUIWindow::Show(false);
	CUIWindow::Enable(false);

	GetParent()->SetCapture(this, false);
}

void CUIPropertiesBox::OnMouse(int x, int y, E_MOUSEACTION mouse_action)
{
	bool cursor_on_box;

	if(x>=0 && x<GetWidth() && y>=0 && y<GetHeight())
		cursor_on_box = true;
	else
		cursor_on_box = false;


	if(mouse_action == LBUTTON_DOWN && !cursor_on_box)
	{
		Hide();
		
		//��������� ��������� �������������
		/*RECT rect = GetAbsoluteRect();
		GetTop()->OnMouse(x + rect.left,
			                 y + rect.top,
							 LBUTTON_DOWN);*/
	}

	inherited::OnMouse(x, y, mouse_action);
}
