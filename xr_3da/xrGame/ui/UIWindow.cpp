// UIWindow.cpp: implementation of the CUIWindow class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UIWindow.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUIWindow::CUIWindow()
{
	m_pParentWnd =  NULL;
	m_pMouseCapturer =  NULL;
	SetRect(&m_WndRect, 0,0,0,0);

    Show(true);
	Enable(true);
}

CUIWindow::~CUIWindow()
{
	m_ChildWndList.clear();
}


void CUIWindow::Init( int x, int y, int width, int height)
{
	SetRect(&m_WndRect, x, y, x+width, y+height);

	//
	//m_background.Init("ui\\hud_health_back","hud\\default",250,250,alNone);
	//m_background.Init("ui\\ui_hud_frame_l","hud\\default",250,250,alNone);
//	m_background.Init("ui\\ui_hud_frame",x,y,width, height,alNone);
	
	//m_background.Init("ui\\ui_hud_frame",100,100,132, 132,alNone);
	//m_background.Init("ui\\ui_hud_frame",x,y,width,height,alNone);
	//m_background.Init("ui\\ui_hud_frame",x,y,x+width,y+height,alNone);
	//m_background.SetSize(256,256);
	
	
	
	//m_background.SetRect(x, y, x+width, y+height);
	

	

	//координаты текста заголовка
//	SetRect(&m_title_rect, 2,2, width-2, height-2);
}

void CUIWindow::Init(RECT* pRect)
{
	Init(pRect->left, pRect->top, 
		pRect->right - pRect->left, 
		pRect->bottom - pRect->top);
}

//
// прорисовка окна
//
void CUIWindow::Draw()
{
	//перерисовать дочерние окна
	for(WINDOW_LIST_it it=m_ChildWndList.begin(); it!=m_ChildWndList.end(); it++)
	{
		if((*it)->IsShown())
			(*it)->Draw();
	}
}


//
// обновление окна перед прорисовкой
//
void CUIWindow::Update()
{
	//перерисовать дочерние окна
	for(WINDOW_LIST_it it=m_ChildWndList.begin(); it!=m_ChildWndList.end(); it++)
	{
		if((*it)->IsShown())
			(*it)->Update();
	}
}


//присоединить дочернее окно
void CUIWindow::AttachChild(CUIWindow* pChild)
{
	if(!pChild) return;

	pChild->SetParent(this);

	m_ChildWndList.push_back(pChild);
}




//отсоединить дочернее окно
void CUIWindow::DetachChild(CUIWindow* pChild)
{
	m_ChildWndList.remove(pChild);
}




//абсолютные координаты, от начала экрана
RECT CUIWindow::GetAbsoluteRect()
{
	RECT rect;


	//окно самого верхнего уровня
	if(GetParent() == NULL)
		return GetWndRect();


	//рекурсивно вычисляем абсолютные координаты
	rect = GetParent()->GetAbsoluteRect();

	rect.left += GetWndRect().left;
	rect.top += GetWndRect().top;
	rect.right = rect.left + GetWidth();
	rect.bottom = rect.top + GetHeight();

	return rect;
}



//реакция на мышь
//координаты курсора всегда, кроме начального вызова 
//задаются относительно текущего окна

void CUIWindow::OnMouse(int x, int y, E_MOUSEACTION mouse_action)
{
	POINT cursor_pos;
	
	cursor_pos.x = x;
	cursor_pos.y = y;

	//вызов из главного окна у которого нет предков
	if(GetParent()== NULL)
	{
		RECT	l_tRect = GetWndRect();
		if(!PtInRect(&l_tRect, cursor_pos))
			return;

		//получить координаты относительно окна
		cursor_pos.x -= GetWndRect().left;
		cursor_pos.y -= GetWndRect().top;
	}



	//если есть дочернее окно,захватившее мышь, то
	//сообщение направляем ему сразу
	if(m_pMouseCapturer!=NULL)
	{
		m_pMouseCapturer->OnMouse(cursor_pos.x - 
								m_pMouseCapturer->GetWndRect().left, 
								  cursor_pos.y - 
								m_pMouseCapturer->GetWndRect().top, 
								  mouse_action);
		return;
	}

	//Проверка на попадание мыши в окно,
	//происходит в обратном порядке, чем рисование окон
	//(последние в списке имеют высший приоритет)
	WINDOW_LIST::reverse_iterator it = (WINDOW_LIST::reverse_iterator)m_ChildWndList.end();

	for(u16 i=0; i<m_ChildWndList.size(); i++, it++)
	{
		RECT l_tRect = (*it)->GetWndRect();
		if (PtInRect(&l_tRect, cursor_pos))
		{
			if((*it)->IsEnabled())
			{
				(*it)->OnMouse(cursor_pos.x -(*it)->GetWndRect().left, 
							   cursor_pos.y -(*it)->GetWndRect().top, mouse_action);
				return;
			}
		}
	}
}


//Сообщение, посылаемое дочерним окном,
//о том, что окно хочет захватить мышь,
//все сообщения от нее будут направляться только
//ему в независимости от того где мышь
void CUIWindow::SetCapture(CUIWindow *pChildWindow, bool capture_status)
{
	if(GetParent() != NULL)
		GetParent()->SetCapture(this, capture_status);

	if(capture_status)
		m_pMouseCapturer = pChildWindow;
	else
		m_pMouseCapturer = NULL;
}




//обработка сообщений 
void CUIWindow::SendMessage(CUIWindow *pWnd, s16 msg, void *pData)
{
	//оповестить дочерние окна
	for(WINDOW_LIST_it it=m_ChildWndList.begin(); it!=m_ChildWndList.end(); it++)
	{
		(*it)->SendMessage(pWnd,msg,pData);
	}
}

//перемесчтить окно на вершину.
//false если такого дочернего окна нет
bool CUIWindow::BringToTop(CUIWindow* pChild)
{
	//найти окно в списке
	WINDOW_LIST_it it = std::find(m_ChildWndList.begin(), 
										m_ChildWndList.end(), 
										pChild);

	if( it == m_ChildWndList.end()) return false;

	//удалить со старого места
	m_ChildWndList.erase(it);
	//поместить на вершину списка
	m_ChildWndList.push_back(pChild);

	return true;
}

//поднять на вершину списка всех родителей окна и его самого
void CUIWindow::BringAllToTop()
{
	if(GetParent() == NULL)	
			return;
	else
	{
		GetParent()->BringToTop(this);	
		GetParent()->BringAllToTop();
	}
}
	
