// UIWindow.cpp: implementation of the CUIWindow class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "UIWindow.h"
#include "../MainUI.h"

//poolSS< std::_List_nod<WINDOW_LIST::_Myt, uialloc<WINDOW_LIST::_Myt> >, 128>	ui_allocator;
poolSS< _12b, 128>	ui_allocator;

//#define LOG_ALL_WNDS
#ifdef LOG_ALL_WNDS
	int ListWndCount = 0;
	struct DBGList{
		int				num;
		bool			closed;
	};
	xr_vector<DBGList>	dbg_list_wnds;
	void dump_list_wnd(){
		Msg("------Total  wnds %d",dbg_list_wnds.size());
		xr_vector<DBGList>::iterator _it = dbg_list_wnds.begin();
		for(;_it!=dbg_list_wnds.end();++_it)
			if(!(*_it).closed)
				Msg("--leak detected ---- wnd = %d",(*_it).num);
	}
#else
	void dump_list_wnd(){}
#endif

xr_vector<Frect> g_wnds_rects;
ref_shader  dbg_draw_sh;
ref_geom	dbg_draw_gm;

BOOL g_show_wnd_rect = FALSE;
void add_rect_to_draw(Frect r)
{
	if(!g_show_wnd_rect)	return;
		g_wnds_rects.push_back(r);
}
void draw_rect(Frect& r, u32 color)
{
	if(!g_show_wnd_rect)	return;
	if(!dbg_draw_sh){
		dbg_draw_sh.create("hud\\default","ui\\ui_pop_up_active_back");
		dbg_draw_gm.create(FVF::F_TL, RCache.Vertex.Buffer(), 0);
	}
	RCache.set_Shader			(dbg_draw_sh);
	u32							vOffset;
	FVF::TL* pv					= (FVF::TL*)RCache.Vertex.Lock	(5,dbg_draw_gm.stride(),vOffset);

	pv->set(r.lt.x, r.lt.y, color, 0,0); ++pv;
	pv->set(r.rb.x, r.lt.y, color, 0,0); ++pv;
	pv->set(r.rb.x, r.rb.y, color, 0,0); ++pv;
	pv->set(r.lt.x, r.rb.y, color, 0,0); ++pv;
	pv->set(r.lt.x, r.lt.y, color, 0,0); ++pv;

//	std::ptrdiff_t p_cnt		= 5;
	RCache.Vertex.Unlock		(5,dbg_draw_gm.stride());
	RCache.set_Geometry			(dbg_draw_gm);
	RCache.Render				(D3DPT_LINESTRIP,vOffset,4);

}
void draw_wnds_rects()
{

	if(!g_wnds_rects.size())	return;

	xr_vector<Frect>::iterator it = g_wnds_rects.begin();
	xr_vector<Frect>::iterator it_e = g_wnds_rects.end();

	for(;it!=it_e;++it)
	{
		Frect& r = *it;
		UI()->ClientToScreenScaled(r.lt, r.lt.x, r.lt.y);
		UI()->ClientToScreenScaled(r.rb, r.rb.x, r.rb.y);
		draw_rect				(r,color_rgba(255,0,0,255));
	};

	g_wnds_rects.clear();
}

void CUIWindow::SetPPMode()
{
	m_bPP					= true;
	UI()->RegisterPPDraw	(this);
	Show					(false);
};

void CUIWindow::ResetPPMode()
{
	if(	GetPPMode() ){
		UI()->UnregisterPPDraw	(this);
		m_bPP					= false;
	}
}

CUIWindow::CUIWindow()
{
//.	m_dbg_flag.zero			();
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
	m_bClickable			= false;
	m_bPP					= false;
	m_dwFocusReceiveTime	= 0;
#ifdef LOG_ALL_WNDS
	ListWndCount++;
	m_dbg_id = ListWndCount;
	dbg_list_wnds.push_back(DBGList());
	dbg_list_wnds.back().num		= m_dbg_id;
	dbg_list_wnds.back().closed		= false;
#endif
}

CUIWindow::~CUIWindow()
{
//.	VERIFY(	m_dbg_flag.get()==0);
	VERIFY( !(GetParent()&&IsAutoDelete()) );

	CUIWindow* parent	= GetParent();
	bool ad				= IsAutoDelete();
	if( parent && !ad )
		parent->CUIWindow::DetachChild( this );

	DetachAll();

	if(	GetPPMode() )
		UI()->UnregisterPPDraw	(this);

#ifdef LOG_ALL_WNDS
	xr_vector<DBGList>::iterator _it = dbg_list_wnds.begin();
	bool bOK = false;
	for(;_it!=dbg_list_wnds.end();++_it){
		if( (*_it).num==m_dbg_id && !(*_it).closed){
			bOK = true;
			(*_it).closed = true;
			dbg_list_wnds.erase(_it);
			break;
		}
		if( (*_it).num==m_dbg_id && (*_it).closed){
			Msg("--CUIWindow [%d] already deleted", m_dbg_id);
			bOK = true;
		}
	}
	if(!bOK)
		Msg("CUIWindow::~CUIWindow.[%d] cannot find window in list", m_dbg_id);
#endif
}


void CUIWindow::Init(Frect* pRect)
{
	SetWndRect			(*pRect);
}

void CUIWindow::Draw()
{
//.	m_dbg_flag.set(1,TRUE);
	//перерисовать дочерние окна
	for(WINDOW_LIST_it it = m_ChildWndList.begin(); m_ChildWndList.end() != it; ++it)
		if((*it)->IsShown())
			(*it)->Draw();
//.	m_dbg_flag.set(1,FALSE);
}

void CUIWindow::Draw(float x, float y){
	SetWndPos		(x,y);
	Draw			();
}

void CUIWindow::Update()
{
	if (GetUICursor()->IsVisible())
	{
		bool cursor_on_window;

		Fvector2			temp = GetUICursor()->GetPos();
		cursor_on_window	= !!GetAbsoluteRect().in(temp);
		if(cursor_on_window) add_rect_to_draw(GetAbsoluteRect());
		// RECEIVE and LOST focus
		if(m_bCursorOverWindow != cursor_on_window)
			if(cursor_on_window)
				OnFocusReceive();			
			else
				OnFocusLost();			
	}
	
//.	m_dbg_flag.set(2,TRUE);
	for(WINDOW_LIST_it it = m_ChildWndList.begin(); m_ChildWndList.end()!=it; ++it)
		if((*it)->IsShown())
			(*it)->Update();
//.	m_dbg_flag.set(2,FALSE);
}

void CUIWindow::AttachChild(CUIWindow* pChild)
{
//.	VERIFY(	m_dbg_flag.get()==0);
	if(!pChild) return;
	
	VERIFY( !IsChild(pChild) );
//.	VERIFY(	m_dbg_flag.get()==0);
	pChild->SetParent(this);
//.	VERIFY(	m_dbg_flag.get()==0);
	m_ChildWndList.push_back(pChild);
}

void CUIWindow::DetachChild(CUIWindow* pChild)
{
//.	VERIFY(	m_dbg_flag.get()==0);
	if(NULL==pChild)
		return;
	
	if(m_pMouseCapturer == pChild)
		SetCapture(pChild, false);

//.	VERIFY(	m_dbg_flag.get()==0);
	SafeRemoveChild(pChild);
//.	VERIFY(	m_dbg_flag.get()==0);
	pChild->SetParent(NULL);

//.	VERIFY(	m_dbg_flag.get()==0);
	if(pChild->IsAutoDelete())
		xr_delete(pChild);
}

void CUIWindow::DetachAll()
{
//.	VERIFY(	m_dbg_flag.get()==0);
	while( !m_ChildWndList.empty() ){
//.		VERIFY(	m_dbg_flag.get()==0);
		DetachChild( m_ChildWndList.back() );	
	}
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

	Frect rr		= GetWndRect	();
	rect.left		+= rr.left;
	rect.top		+= rr.top;
	rect.right		= rect.left + GetWidth();
	rect.bottom		= rect.top	+ GetHeight();

	return rect;
}

//реакция на мышь
//координаты курсора всегда, кроме начального вызова 
//задаются относительно текущего окна

#define DOUBLE_CLICK_TIME 250

bool CUIWindow::OnMouse(float x, float y, EUIMessages mouse_action)
{	
	Frect	wndRect = GetWndRect();

	cursor_pos.x = x;
	cursor_pos.y = y;


	// DOUBLE CLICK GENERATION
	if( ( WINDOW_LBUTTON_DOWN == mouse_action ) && m_bDoubleClickEnabled )
	{
		u32 dwCurTime = Device.dwTimeContinual;
		if(dwCurTime - m_dwLastClickTime < DOUBLE_CLICK_TIME)
            mouse_action = WINDOW_LBUTTON_DB_CLICK;

		m_dwLastClickTime = dwCurTime;
	}

	if(GetParent()== NULL) //вызов из главного окна у которого нет предков
	{
		if(!wndRect.in(cursor_pos))
            return false;
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
		return true;
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
			if(OnMouseDown())		return true;	break;
		case WINDOW_RBUTTON_DOWN:
			if(OnMouseDown(false))	return true;	break;
		case WINDOW_LBUTTON_DB_CLICK:
			if (OnDbClick()) return true;
			break;
		default:
            break;
	}

	//Проверка на попадание мыши в окно,
	//происходит в обратном порядке, чем рисование окон
	//(последние в списке имеют высший приоритет)
//.	m_dbg_flag.set(4,TRUE);
	WINDOW_LIST::reverse_iterator it = m_ChildWndList.rbegin();

	for(; it!=m_ChildWndList.rend(); ++it)
	{
		Frect wndRect = (*it)->GetWndRect();
		if (wndRect.in(cursor_pos) )
		{
			if((*it)->IsEnabled())
			{
				if( (*it)->OnMouse(cursor_pos.x -(*it)->GetWndRect().left, 
							   cursor_pos.y -(*it)->GetWndRect().top, mouse_action))return true;
			}
		}
		else if ((*it)->IsEnabled() && (*it)->CursorOverWindow())
		{
			if( (*it)->OnMouse(cursor_pos.x -(*it)->GetWndRect().left, 
						   cursor_pos.y -(*it)->GetWndRect().top, mouse_action))return true;
		}
	}
//.	m_dbg_flag.set(4,FALSE);

	return false;
}

bool CUIWindow::HasChildMouseHandler(){
//.	m_dbg_flag.set(8,TRUE);
	WINDOW_LIST::reverse_iterator it = m_ChildWndList.rbegin();

	for( ; it!=m_ChildWndList.rend(); ++it)
	{
		if ((*it)->m_bClickable)
		{
			Frect wndRect = (*it)->GetWndRect();
			if (wndRect.in(cursor_pos) )
				return true;
		}
	}

//.	m_dbg_flag.set(8,FALSE);
	return false;
}

void CUIWindow::OnMouseMove(){
}

void CUIWindow::OnMouseScroll(float iDirection){
}

bool CUIWindow::OnDbClick(){
	if (GetMessageTarget())
		GetMessageTarget()->SendMessage(this, WINDOW_LBUTTON_DB_CLICK);
	return false;
}

bool CUIWindow::OnMouseDown(bool left_button){
	return false;
}

void CUIWindow::OnMouseUp(bool left_button){
}

void CUIWindow::OnFocusReceive()
{
	m_dwFocusReceiveTime	= Device.dwTimeGlobal;
	m_bCursorOverWindow		= true;	
}

void CUIWindow::OnFocusLost()
{
	m_dwFocusReceiveTime	= 0;
	m_bCursorOverWindow		= false;	
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

//.	m_dbg_flag.set(16,TRUE);
	WINDOW_LIST::reverse_iterator it = m_ChildWndList.rbegin();

	for(; it!=m_ChildWndList.rend(); ++it)
	{
		if((*it)->IsEnabled())
		{
			result = (*it)->OnKeyboard(dik, keyboard_action);
			
			if(result)	return true;
		}
	}
//.	m_dbg_flag.set(16,FALSE);

	return false;
}

bool CUIWindow::OnKeyboardHold(int dik)
{
	bool result;

	if(NULL!=m_pKeyboardCapturer)
	{
		result = m_pKeyboardCapturer->OnKeyboardHold(dik);
		
		if(result) return true;
	}

	WINDOW_LIST::reverse_iterator it = m_ChildWndList.rbegin();

	for(; it!=m_ChildWndList.rend(); ++it)
	{
		if((*it)->IsEnabled())
		{
			result = (*it)->OnKeyboardHold(dik);
			
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
//.	m_dbg_flag.set(32,TRUE);
	for(WINDOW_LIST_it it = m_ChildWndList.begin(); m_ChildWndList.end()!=it; ++it)
	{
		if((*it)->IsEnabled())
			(*it)->SendMessage(pWnd,msg,pData);
	}
//.	m_dbg_flag.set(32,FALSE);
}

CUIWindow* CUIWindow::GetCurrentMouseHandler(){
	return GetTop()->GetChildMouseHandler();
}

CUIWindow* CUIWindow::GetChildMouseHandler(){
	CUIWindow* pWndResult;
//.	m_dbg_flag.set(64,TRUE);
	WINDOW_LIST::reverse_iterator it = m_ChildWndList.rbegin();

	for(; it!=m_ChildWndList.rend(); ++it)
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

//.	m_dbg_flag.set(64,FALSE);
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
//.	m_dbg_flag.set(128,TRUE);
	for(WINDOW_LIST_it it = m_ChildWndList.begin(); m_ChildWndList.end()!=it; ++it)
	{
		(*it)->Reset();
	}
//.	m_dbg_flag.set(128,FALSE);
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

//.	m_dbg_flag.set(256,TRUE);
	WINDOW_LIST::const_iterator it = m_ChildWndList.begin();
	WINDOW_LIST::const_iterator it_e = m_ChildWndList.end();
	for(;it!=it_e;++it){
		CUIWindow* pRes = (*it)->FindChild(name);
		if(pRes != NULL)
			return pRes;
	}

//.	m_dbg_flag.set(256,FALSE);
	return NULL;
}

void CUIWindow::SetParent(CUIWindow* pNewParent) 
{
	VERIFY( !(m_pParentWnd && m_pParentWnd->IsChild(this)) );

	m_pParentWnd = pNewParent;
}

void CUIWindow::ShowChildren(bool show){
//.	m_dbg_flag.set(512,TRUE);
	for(WINDOW_LIST_it it = m_ChildWndList.begin(); m_ChildWndList.end()!=it; ++it)		
			(*it)->Show(show);
//.	m_dbg_flag.set(512,FALSE);
}
