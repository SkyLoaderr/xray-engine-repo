// UIPropertiesBox.cpp: 
//
// окшко с собщением для выбора действий, вызывается по нажатию правой 
// клавиши мыши
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UIPropertiesBox.h"
#include "../hudmanager.h"
#include "../level.h"

#define OFFSET_X 20
#define OFFSET_Y 18
#define FRAME_BORDER_WIDTH	20
#define FRAME_BORDER_HEIGHT	22

#define ITEM_HEIGHT ((int)GetFont()->CurrentHeight()+2)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUIPropertiesBox::CUIPropertiesBox()
{
	m_iClickedElement = -1;
	SetFont(HUD().pArialN21Russian);
}

CUIPropertiesBox::~CUIPropertiesBox()
{
}


void CUIPropertiesBox::Init(LPCSTR base_name, int x, int y, int width, int height)
{
	inherited::Init(base_name, x,y, width, height);

	AttachChild(&m_UIListWnd);
	
	m_UIListWnd.Init(OFFSET_X, OFFSET_Y, 
					 width - OFFSET_X*2, 
					 height - OFFSET_Y*2, ITEM_HEIGHT);
	m_UIListWnd.EnableActiveBackground(true);
	m_UIListWnd.EnableScrollBar(false);


	m_iClickedElement = -1;
}



void CUIPropertiesBox::SendMessage(CUIWindow *pWnd, s16 msg, void *pData)
{
	if(pWnd == &m_UIListWnd)
	{
		if(msg == LIST_ITEM_CLICKED)
		{
			m_iClickedElement = ((CUIListItem*)pData)->GetIndex();
			GetMessageTarget()->SendMessage(this, PROPERTY_CLICKED);
			
			Hide();
		}
	}

	inherited::SendMessage(pWnd, msg, pData);
}

bool CUIPropertiesBox::AddItem(const char*  str, void* pData, int value)
{
	return m_UIListWnd.AddItem<CUIListItem>(str, 0, pData, value);
}
void CUIPropertiesBox::RemoveItem(int index)
{
	m_UIListWnd.RemoveItem(index);
}

void CUIPropertiesBox::RemoveAll()
{
	m_UIListWnd.RemoveAll();
}

void CUIPropertiesBox::Show(int x, int y)
{
	int x_pos, y_pos;


	//выбрать позицию, чтобы окношко полностью влазило в экран
	if(x+GetWidth()<(int)Device.dwWidth)
		x_pos = x;
	else
		x_pos = x - (x+GetWidth()-(int)Device.dwWidth);
	
	if(y+GetHeight()<(int)Device.dwHeight)
		y_pos = y;
	else
		y_pos = y - (y+GetHeight()-(int)Device.dwHeight);
		
	MoveWindow(x_pos,y_pos);

	inherited::Show(true);
	inherited::Enable(true);

	ResetAll();

	//сразу захватываем ввод мыши
	GetParent()->SetCapture(this, true);
	//запретить дочерним окнам изментять захват мыши 
	//по отношению к нашему родительскому окну
	m_pOrignMouseCapturer = this;
	
	m_iClickedElement = -1;

	m_UIListWnd.Reset();
}
void CUIPropertiesBox::Hide()
{
	CUIWindow::Show(false);
	CUIWindow::Enable(false);

	m_pMouseCapturer = NULL;
	GetParent()->SetCapture(this, false);
}

void CUIPropertiesBox::OnMouse(int x, int y, EUIMessages mouse_action)
{
	bool cursor_on_box;


	if(x>=0 && x<GetWidth() && y>=0 && y<GetHeight())
		cursor_on_box = true;
	else
		cursor_on_box = false;


	if(mouse_action == WINDOW_LBUTTON_DOWN && !cursor_on_box)
	{
		Hide();
	}

	inherited::OnMouse(x, y, mouse_action);
}

void CUIPropertiesBox::AutoUpdateSize()
{
	SetHeight(m_UIListWnd.GetItemHeight()*m_UIListWnd.GetSize()+
			  FRAME_BORDER_HEIGHT*2);
	SetWidth(m_UIListWnd.GetLongestSignWidth()+FRAME_BORDER_WIDTH*2);
	m_UIListWnd.SetWidth(m_UIListWnd.GetLongestSignWidth());
}

int CUIPropertiesBox::GetClickedIndex() 
{
	return m_iClickedElement;
}
CUIListItem* CUIPropertiesBox::GetClickedItem()
{
	return m_UIListWnd.GetItem(GetClickedIndex());
}
void CUIPropertiesBox::Update()
{
	inherited::Update();
}
void CUIPropertiesBox::Draw()
{
	inherited::Draw();
}