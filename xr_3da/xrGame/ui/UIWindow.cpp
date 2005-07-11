// UIWindow.cpp: implementation of the CUIWindow class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UIWindow.h"
#include "../MainUI.h"

//#define LOG_ALL_WNDS
#ifdef LOG_ALL_WNDS
	int ListWndCount = 0;
	struct DBGList{
		CUIWindow*		wnd;
		int				num;
	};
	xr_vector<DBGList>	dbg_list_wnds;
	void dump_list_wnd(){
		Msg("------Total  wnds %d",dbg_list_wnds.size());
		xr_vector<DBGList>::iterator _it = dbg_list_wnds.begin();
		for(;_it!=dbg_list_wnds.end();++_it)
			Msg("--leak detected ---- wnd = %d",(*_it).num);
	}
#else
	void dump_list_wnd(){}
#endif


CUIWindow::CUIWindow()
{
	m_pFont					= NULL;

	m_pParentWnd			= NULL;
	
	m_pMouseCapturer		= NULL;
	m_pOrignMouseCapturer	= NULL;
	m_pMessageTarget		= NULL;


	m_pKeyboardCapturer		=  NULL;
	SetWndRect				(0,0,0,0);

	m_bAutoDelete			= false;

    Show					(true);
	Enable					(true);
	EnableDoubleClick		(true);
	m_bCursorOverWindow		= false;

#ifdef LOG_ALL_WNDS
	ListWndCount++;
	dbg_list_wnds.push_back(DBGList());
	dbg_list_wnds.back().wnd = this;
	dbg_list_wnds.back().num = ListWndCount;
#endif
}

CUIWindow::~CUIWindow()
{
	VERIFY( !(GetParent()&&IsAutoDelete()) );

	CUIWindow* parent	= GetParent();
	bool ad				= IsAutoDelete();
	if( parent && !ad )
		parent->CUIWindow::DetachChild( this );

	DetachAll();

#ifdef LOG_ALL_WNDS
	xr_vector<DBGList>::iterator _it = dbg_list_wnds.begin();
	bool bOK = false;
	for(;_it!=dbg_list_wnds.end();++_it){
		if((*_it).wnd == this){
			bOK = true;
			dbg_list_wnds.erase(_it);
			break;
		}
	}
	if(!bOK)
		Msg("CUIWindow::~CUIWindow.!!!!!!!!!!!!!!!!!!!!!!! cannot find window in list");
#endif

}

//
//void CUIWindow::Init( int x, int y, int width, int height)
//{
//	SetWndRect(x, y, width, height);
//}

void CUIWindow::Init(Frect* pRect)
{
	SetWndRect(*pRect);
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

void CUIWindow::Draw(float x, float y){
	SetWndPos(x, y);
	Draw();
}

void CUIWindow::Update()
{
	bool cursor_on_window;

	cursor_on_window = GetAbsoluteRect().in(GetUICursor()->GetPos());

	// RECEIVE and LOST focus
	if(m_bCursorOverWindow != cursor_on_window)
		if(cursor_on_window)
			OnFocusReceive();			
		else
			OnFocusLost();			
	m_bCursorOverWindow = cursor_on_window;	
	for(WINDOW_LIST_it it = m_ChildWndList.begin(); m_ChildWndList.end()!=it; ++it)
	{
		if((*it)->IsShown())
			(*it)->Update();
	}
}

void CUIWindow::AttachChild(CUIWindow* pChild)
{
	if(!pChild) return;
	
	VERIFY( !IsChild(pChild) );
	pChild->SetParent(this);
	m_ChildWndList.push_back(pChild);
}



//отсоединить дочернее окно
void CUIWindow::DetachChild(CUIWindow* pChild)
{
	if(NULL==pChild)
		return;
	
	if(m_pMouseCapturer == pChild)
		SetCapture(pChild, false);

	SafeRemoveChild(pChild);
	pChild->SetParent(NULL);

	if(pChild->IsAutoDelete())
		xr_delete(pChild);


}

void CUIWindow::DetachAll()
{
	while( !m_ChildWndList.empty() )
		DetachChild( m_ChildWndList.back() );	
}

//абсолютные координаты, от начала экрана
Frect CUIWindow::GetAbsoluteRect() 
{
	Frect rect;


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

void CUIWindow::OnMouse(float x, float y, EUIMessages mouse_action)
{	
	Frect	wndRect = GetWndRect();

	cursor_pos.x = x;
	cursor_pos.y = y;


	//bool cursor_on_window;

	//cursor_on_window = (x>=0 && x<GetWidth() && y>=0 && y<GetHeight());
	//
	//// RECEIVE and LOST focus
	//if(m_bCursorOverWindow != cursor_on_window)
	//	if(cursor_on_window)
 //           OnFocusReceive();			
	//	else
	//		OnFocusLost();			
	//m_bCursorOverWindow = cursor_on_window;

	// DOUBLE CLICK GENERATION
	if( ( WINDOW_LBUTTON_DOWN == mouse_action ) && m_bDoubleClickEnabled )
	{
		u32 dwCurTime = Device.TimerAsyncMM();
		if(dwCurTime - m_dwLastClickTime < DOUBLE_CLICK_TIME)
            mouse_action = WINDOW_LBUTTON_DB_CLICK;

		m_dwLastClickTime = dwCurTime;
	}

	if(GetParent()== NULL) //вызов из главного окна у которого нет предков
	{
		if(!wndRect.in(cursor_pos))
            return;
		//получить координаты относительно окна
		cursor_pos.x -= wndRect.left;
		cursor_pos.y -= wndRect.top;
	}


	//если есть дочернее окно,захватившее мышь, то
	//сообщение направляем ему сразу
	if(m_pMouseCapturer)
	{
		m_pMouseCapturer->OnMouse(cursor_pos.x - m_pMouseCapturer->GetWndRect().left, 
								  cursor_pos.y - m_pMouseCapturer->GetWndRect().top, 
								  mouse_action);
		return;
	}

	// handle any action
	switch (mouse_action){
		case WINDOW_MOUSE_MOVE:
			OnMouseMove();							break;
		case WINDOW_MOUSE_WHEEL_DOWN:
			OnMouseScroll(WINDOW_MOUSE_WHEEL_DOWN); break;
		case WINDOW_MOUSE_WHEEL_UP:
			OnMouseScroll(WINDOW_MOUSE_WHEEL_UP);	break;
		case WINDOW_LBUTTON_DOWN:
			OnMouseDown();							break;
		case WINDOW_RBUTTON_DOWN:
			OnMouseDown(/*left_button = */false);	break;
		case WINDOW_LBUTTON_DB_CLICK:
			OnDbClick();							break;
		default:
            break;
	}

	//Проверка на попадание мыши в окно,
	//происходит в обратном порядке, чем рисование окон
	//(последние в списке имеют высший приоритет)
	WINDOW_LIST::reverse_iterator it = (WINDOW_LIST::reverse_iterator)m_ChildWndList.end();

	for(u16 i=0; i<m_ChildWndList.size(); ++i, ++it)
	{
		Frect wndRect = (*it)->GetWndRect();
		if (wndRect.in(cursor_pos) )
		{
			if((*it)->IsEnabled())
			{
				(*it)->OnMouse(cursor_pos.x -(*it)->GetWndRect().left, 
							   cursor_pos.y -(*it)->GetWndRect().top, mouse_action);
				return;
			}
		}
		else if ((*it)->IsEnabled() && (*it)->CursorOverWindow())
		{
			(*it)->OnMouse(cursor_pos.x -(*it)->GetWndRect().left, 
						   cursor_pos.y -(*it)->GetWndRect().top, mouse_action);
		}
	}


}

void CUIWindow::OnMouseMove(){
	// do nothing 
}

void CUIWindow::OnMouseScroll(float iDirection){
	; // do nothing
}

void CUIWindow::OnDbClick(){
	;
}

void CUIWindow::OnMouseDown(bool left_button){

}

void CUIWindow::OnMouseUp(bool left_button){

}

void CUIWindow::OnFocusReceive(){
	;
}

void CUIWindow::OnFocusLost(){
	;
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
			m_pMouseCapturer->SendMessage(this, WINDOW_MOUSE_CAPTURE_LOST);

		m_pMouseCapturer = pChildWindow;
	}
	else
	{
		/*if(m_pMouseCapturer == pChildWindow)*/
			m_pMouseCapturer = NULL;
	}
}


//реакция на клавиатуру
bool CUIWindow::OnKeyboard(int dik, EUIMessages keyboard_action)
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
		GetParent()->SetKeyboardCapture(this, capture_status);

	if(capture_status)
	{
		//оповестить дочернее окно о потере фокуса клавиатуры
		if(NULL!=m_pKeyboardCapturer)
			m_pKeyboardCapturer->SendMessage(this, WINDOW_KEYBOARD_CAPTURE_LOST);
			
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

CUIWindow* CUIWindow::GetCurrentMouseHandler(){
	return GetTop()->GetChildMouseHandler();
}

CUIWindow* CUIWindow::GetChildMouseHandler(){
	CUIWindow* pWndResult;
	WINDOW_LIST::reverse_iterator it = (WINDOW_LIST::reverse_iterator)m_ChildWndList.end();

	for(u32 i=0; i<m_ChildWndList.size(); ++i, ++it)
	{
		Frect wndRect = (*it)->GetWndRect();
		// very strange code.... i can't understand difference between
		// first and second condition. I Got It from OnMouse() method;
		if (wndRect.in(cursor_pos) )
		{
			if((*it)->IsEnabled())
			{
				return pWndResult = (*it)->GetChildMouseHandler();				
			}
		}
		else if ((*it)->IsEnabled() && (*it)->CursorOverWindow())
		{
			return pWndResult = (*it)->GetChildMouseHandler();
		}
	}

    return this;
}

//перемесчтить окно на вершину.
//false если такого дочернего окна нет
bool CUIWindow::BringToTop(CUIWindow* pChild)
{
	//найти окно в списке
/*	WINDOW_LIST_it it = std::find(m_ChildWndList.begin(), 
										m_ChildWndList.end(), 
										pChild);
*/
	if( !IsChild(pChild) ) return false;

	//удалить со старого места
	SafeRemoveChild(pChild);
//	m_ChildWndList.remove(pChild);
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


CUIWindow*	CUIWindow::FindChild(const shared_str name)
{
	if(WindowName()==name)
		return this;

	WINDOW_LIST::const_iterator it = m_ChildWndList.begin();
	WINDOW_LIST::const_iterator it_e = m_ChildWndList.end();
	for(;it!=it_e;++it){
		CUIWindow* pRes = (*it)->FindChild(name);
		if(pRes != NULL)
			return pRes;
	}

	return NULL;
}

void CUIWindow::SetParent(CUIWindow* pNewParent) 
{
	VERIFY( !(m_pParentWnd && m_pParentWnd->IsChild(this)) );

	m_pParentWnd = pNewParent;
}
