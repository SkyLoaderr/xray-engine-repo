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
	m_pFont = NULL;

	m_pParentWnd =  NULL;
	
	m_pMouseCapturer =  NULL;
	m_pOrignMouseCapturer = NULL;
	m_pMessageTarget = NULL;


	m_pKeyboardCapturer =  NULL;
	SetRect(&m_WndRect, 0,0,0,0);

	m_bAutoDelete = false;

    Show(true);
	Enable(true);
}

CUIWindow::~CUIWindow()
{
	DetachAll();
}


void CUIWindow::Init( int x, int y, int width, int height)
{
	SetRect(&m_WndRect, x, y, x+width, y+height);
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
	for(WINDOW_LIST_it it = m_ChildWndList.begin(); m_ChildWndList.end() != it; ++it)
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
	for(WINDOW_LIST_it it = m_ChildWndList.begin(); m_ChildWndList.end()!=it; ++it)
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
	if(m_pMouseCapturer == pChild) SetCapture(pChild, false);

	m_ChildWndList.remove(pChild);
}

void CUIWindow::DetachAll()
{
	if(m_ChildWndList.empty()) return;

	for(WINDOW_LIST_it it = m_ChildWndList.begin(); m_ChildWndList.end() != it; ++it)
	{
		CUIWindow* pChild = *it;
		if(pChild->IsAutoDelete()) 
			xr_delete(pChild);
	}

	m_ChildWndList.erase(m_ChildWndList.begin(), m_ChildWndList.end());
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

#define DOUBLE_CLICK_TIME 250

void CUIWindow::OnMouse(int x, int y, E_MOUSEACTION mouse_action)
{	
	//определить DoubleClick

	// В статческой переменной запоминаем адрес себя, для того чтобы другие контолы 
	// не инициализировали время реакции на даблклик. Т.е. реализовываем что-то типа
	// семафорчика

//	static CUIWindow *pWnd = NULL;

	if(mouse_action == LBUTTON_DOWN)
	{
		u32 dwCurTime = Device.TimerAsync();
		if(dwCurTime - m_dwLastClickTime < DOUBLE_CLICK_TIME)
		{
			 mouse_action = LBUTTON_DB_CLICK;
		}
//		else
//			pWnd = this;

		m_dwLastClickTime = dwCurTime;
	}

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
	if(NULL!=m_pMouseCapturer)
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

	for(u16 i=0; i<m_ChildWndList.size(); ++i, ++it)
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
	if(NULL != GetParent())
	{
		if(m_pOrignMouseCapturer == NULL || m_pOrignMouseCapturer == pChildWindow)
			GetParent()->SetCapture(this, capture_status);
	}

	if(capture_status)
	{
		//оповестить дочернее окно о потере фокуса мыши
		if(NULL!=m_pMouseCapturer)
			m_pMouseCapturer->SendMessage(this, MOUSE_CAPTURE_LOST);

		m_pMouseCapturer = pChildWindow;
	}
	else
	{
		/*if(m_pMouseCapturer == pChildWindow)*/
			m_pMouseCapturer = NULL;
	}
}


//реакция на клавиатуру
bool CUIWindow::OnKeyboard(int dik, E_KEYBOARDACTION keyboard_action)
{
	bool result;

	//если есть дочернее окно,захватившее клавиатуру, то
	//сообщение направляем ему сразу
	if(NULL!=m_pKeyboardCapturer)
	{
		result = m_pKeyboardCapturer->OnKeyboard(dik, keyboard_action);
		
		if(result) return true;
	}

	WINDOW_LIST::reverse_iterator it = (WINDOW_LIST::reverse_iterator)m_ChildWndList.end();

	for(u16 i=0; i<m_ChildWndList.size(); ++i, ++it)
	{
		if((*it)->IsEnabled())
		{
			result = (*it)->OnKeyboard(dik, keyboard_action);
			
			if(result)	return true;
		}
	}

	return false;
}


void CUIWindow::SetKeyboardCapture(CUIWindow* pChildWindow, bool capture_status)
{
	if(NULL != GetParent())
		GetParent()->SetCapture(this, capture_status);

	if(capture_status)
	{
		//оповестить дочернее окно о потере фокуса клавиатуры
		if(NULL!=m_pKeyboardCapturer)
			m_pKeyboardCapturer->SendMessage(this, KEYBOARD_CAPTURE_LOST);
			
		m_pKeyboardCapturer = pChildWindow;
	}
	else
		m_pKeyboardCapturer = NULL;
}


//обработка сообщений 
void CUIWindow::SendMessage(CUIWindow *pWnd, s16 msg, void *pData)
{
	//оповестить дочерние окна
	for(WINDOW_LIST_it it = m_ChildWndList.begin(); m_ChildWndList.end()!=it; ++it)
	{
		if((*it)->IsEnabled())
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

//для перевода окна и потомков в исходное состояние
void CUIWindow::Reset()
{
	m_pOrignMouseCapturer = m_pMouseCapturer = NULL;
}
void CUIWindow::ResetAll()
{
	for(WINDOW_LIST_it it = m_ChildWndList.begin(); m_ChildWndList.end()!=it; ++it)
	{
		(*it)->Reset();
	}
}


CUIWindow* CUIWindow::GetMessageTarget()
{
	return m_pMessageTarget?m_pMessageTarget:GetParent();
}

bool CUIWindow::IsChild(CUIWindow *pPossibleChild) const
{
	WINDOW_LIST::const_iterator it = std::find(m_ChildWndList.begin(), m_ChildWndList.end(), pPossibleChild);
	return it != m_ChildWndList.end();
}