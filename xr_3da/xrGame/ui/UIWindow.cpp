// UIWindow.cpp: implementation of the CUIWindow class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UIWindow.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//#define LOG_ALL_WINDOWS

#ifdef LOG_ALL_WINDOWS
xr_vector<CUIWindow*> g_globalWnds;
#endif

void dump_all_wnds()
{
#ifdef LOG_ALL_WINDOWS
	xr_vector<CUIWindow*>::iterator it = g_globalWnds.begin();
	Msg("---g_globalWnds.size()==[%d]",g_globalWnds.size());
	for(;it!=g_globalWnds.end();++it)
	{
		CUIWindow* w = *it;
		shared_str name = w->WindowName();
	}
#endif
}

CUIWindow::CUIWindow()
{
	m_bMouseDown			= false;
	m_alignment				= waNone;
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
#ifdef LOG_ALL_WINDOWS
	g_globalWnds.push_back(this);
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

#ifdef LOG_ALL_WINDOWS
	xr_vector<CUIWindow*>::iterator it = std::find(g_globalWnds.begin(),g_globalWnds.end(),this);
	VERIFY(it!=g_globalWnds.end());
	g_globalWnds.erase(it);
#endif
}


void CUIWindow::Init( int x, int y, int width, int height)
{
	SetWndRect(x, y, width, height);
}

void CUIWindow::Init(Irect* pRect)
{
	SetWndRect(*pRect);
}

//
// ���������� ����
//
void CUIWindow::Draw()
{
	//������������ �������� ����
	for(WINDOW_LIST_it it = m_ChildWndList.begin(); m_ChildWndList.end() != it; ++it)
	{
		if((*it)->IsShown())
			(*it)->Draw();
	}
}

void CUIWindow::Update()
{
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



//����������� �������� ����
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

//���������� ����������, �� ������ ������
Irect CUIWindow::GetAbsoluteRect() 
{
	Irect rect;


	//���� ������ �������� ������
	if(GetParent() == NULL)
		return GetWndRect();


	//���������� ��������� ���������� ����������
	rect = GetParent()->GetAbsoluteRect();

	rect.left += GetWndRect().left;
	rect.top += GetWndRect().top;
	rect.right = rect.left + GetWidth();
	rect.bottom = rect.top + GetHeight();

	return rect;
}



//������� �� ����
//���������� ������� ������, ����� ���������� ������ 
//�������� ������������ �������� ����

#define DOUBLE_CLICK_TIME 250

void CUIWindow::OnMouse(int x, int y, EUIMessages mouse_action)
{	
	Irect	wndRect = GetWndRect();

	cursor_pos.x = x;
	cursor_pos.y = y;


	bool cursor_on_window;

	cursor_on_window = (x>=0 && x<GetWidth() && y>=0 && y<GetHeight());
	
	// RECEIVE and LOST focus
	if(m_bCursorOverWindow != cursor_on_window)
		if(cursor_on_window)
            OnFocusReceive();			
		else
			OnFocusLost();			
	m_bCursorOverWindow = cursor_on_window;

	// DOUBLE CLICK GENERATION
	if( ( WINDOW_LBUTTON_DOWN == mouse_action ) && m_bDoubleClickEnabled )
	{
		u32 dwCurTime = Device.TimerAsync();
		if(dwCurTime - m_dwLastClickTime < DOUBLE_CLICK_TIME)
            mouse_action = WINDOW_LBUTTON_DB_CLICK;

		m_dwLastClickTime = dwCurTime;
	}

	if(GetParent()== NULL) //����� �� �������� ���� � �������� ��� �������
	{
		if(!wndRect.in(cursor_pos))
            return;
		//�������� ���������� ������������ ����
		cursor_pos.x -= wndRect.left;
		cursor_pos.y -= wndRect.top;
	}


	//���� ���� �������� ����,����������� ����, ��
	//��������� ���������� ��� �����
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

	//�������� �� ��������� ���� � ����,
	//���������� � �������� �������, ��� ��������� ����
	//(��������� � ������ ����� ������ ���������)
	WINDOW_LIST::reverse_iterator it = (WINDOW_LIST::reverse_iterator)m_ChildWndList.end();

	for(u16 i=0; i<m_ChildWndList.size(); ++i, ++it)
	{
		Irect wndRect = (*it)->GetWndRect();
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

void CUIWindow::OnMouseScroll(int iDirection){
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


//���������, ���������� �������� �����,
//� ���, ��� ���� ����� ��������� ����,
//��� ��������� �� ��� ����� ������������ ������
//��� � ������������� �� ���� ��� ����
void CUIWindow::SetCapture(CUIWindow *pChildWindow, bool capture_status)
{
	if(NULL != GetParent())
	{
		if(m_pOrignMouseCapturer == NULL || m_pOrignMouseCapturer == pChildWindow)
			GetParent()->SetCapture(this, capture_status);
	}

	if(capture_status)
	{
		//���������� �������� ���� � ������ ������ ����
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


//������� �� ����������
bool CUIWindow::OnKeyboard(int dik, EUIMessages keyboard_action)
{
	bool result;

	//���� ���� �������� ����,����������� ����������, ��
	//��������� ���������� ��� �����
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
		//���������� �������� ���� � ������ ������ ����������
		if(NULL!=m_pKeyboardCapturer)
			m_pKeyboardCapturer->SendMessage(this, WINDOW_KEYBOARD_CAPTURE_LOST);
			
		m_pKeyboardCapturer = pChildWindow;
	}
	else
		m_pKeyboardCapturer = NULL;
}


//��������� ��������� 
void CUIWindow::SendMessage(CUIWindow *pWnd, s16 msg, void *pData)
{
	//���������� �������� ����
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
		Irect wndRect = (*it)->GetWndRect();
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

//������������ ���� �� �������.
//false ���� ������ ��������� ���� ���
bool CUIWindow::BringToTop(CUIWindow* pChild)
{
	//����� ���� � ������
/*	WINDOW_LIST_it it = std::find(m_ChildWndList.begin(), 
										m_ChildWndList.end(), 
										pChild);
*/
	if( !IsChild(pChild) ) return false;

	//������� �� ������� �����
	SafeRemoveChild(pChild);
//	m_ChildWndList.remove(pChild);
	//��������� �� ������� ������
	m_ChildWndList.push_back(pChild);

	return true;
}

//������� �� ������� ������ ���� ��������� ���� � ��� ������
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

//��� �������� ���� � �������� � �������� ���������
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

Irect CUIWindow::GetWndRect()
{
	switch (m_alignment){
		case waNone:
			return Irect().set(m_iWndPos.x,m_iWndPos.y,m_iWndPos.x+m_iWndSize.x,m_iWndPos.y+m_iWndSize.y);
			break;
		case waCenter:{
				Irect res;
				int half_w = iFloor(float(m_iWndSize.x)/2.0f);
				int half_h = iFloor(float(m_iWndSize.y)/2.0f);
				res.set(m_iWndPos.x - half_w,
						m_iWndPos.y - half_h,
						m_iWndPos.x + half_w,
						m_iWndPos.y + half_h);
				return res;
			}break;
		default:
			NODEFAULT;
	};
	return Irect().null();
}

void CUIWindow::SetWndRect(int x, int y, int width, int height)
{
	switch (m_alignment){
		case waNone:
		case waCenter:{
				m_iWndPos.set(x,y); 
				m_iWndSize.set(width,height);
			}break;
		default:
			NODEFAULT;
	};
}
