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
//	m_pWindowDraw =  NULL;

	m_pParentWnd =  NULL;
	m_pMouseCapturer =  NULL;
	SetRect(&m_WndRect, 0,0,0,0);

    Show(true);
	Enable(true);

//	m_title_font = CUIWindowDraw::NORMAL_FONT;
}

CUIWindow::~CUIWindow()
{
	m_ChildWndList.clear();
}


void CUIWindow::Init(const char *title, int x, int y, int width, int height)
{
//	m_sTitle = title;
	SetRect(&m_WndRect, x, y, x+width, y+height);

	//m_background.Init("ui\\hud_health_back","hud\\default",250,250,alNone);
	//m_background.Init("ui\\ui_hud_frame_l","hud\\default",250,250,alNone);
//	m_background.Init("ui\\ui_hud_frame",x,y,width, height,alNone);
	
	//m_background.Init("ui\\ui_hud_frame",100,100,132, 132,alNone);
	m_background.Init("ui\\ui_hud_frame",x,y,width,height,alNone);
	//m_background.Init("ui\\ui_hud_frame",x,y,x+width,y+height,alNone);
	//m_background.SetSize(256,256);
	
	
	
	//m_background.SetRect(x, y, x+width, y+height);
	

	

	//координаты текста заголовка
//	SetRect(&m_title_rect, 2,2, width-2, height-2);
}

void CUIWindow::Init(const char *title, RECT* pRect)
{
	Init(title, pRect->left, pRect->top, 
				pRect->right - pRect->left, 
				pRect->bottom - pRect->top);
}




void  CUIWindow::Init(const char* xml_filename)
{
/*	m_inXml.Init(xml_filename); 


	string title;
	int x,y, width, height;
	

	m_inXml.ReadAttribInt("//window",0, x, 0);
	m_inXml.ReadAttribInt("//window",1, y, 0);
	m_inXml.ReadAttribInt("//window",2, width, 300);
	m_inXml.ReadAttribInt("//window",3, height, 300);

	//цвет окна
	LoadAttribColor(m_inXml,"//window",m_window_color,0xff000000);
	
	m_inXml.ReadAttribInt("//window",7,(int&)m_texture_num, 0);

	
	//заголовок окна
	m_inXml.Read("window//title//text", title, string("window"));
	LoadColor(m_inXml, string("//window//title//color"), m_title_color, 0xff0000ff);
	


	//вызов базовой функции для основной инициализации
	Init(title.c_str(), x, y, x+width, y+height);*/
}




//
// прорисовка окна
//
void CUIWindow::Draw()
{
/*	if(m_pWindowDraw == NULL) return;

	m_pWindowDraw->SetRectType(CUIWindowDraw::WINDOW_RECT);

	RECT rect = GetAbsoluteRect();
	m_pWindowDraw->Rectangle(rect.left, rect.top, rect.right, rect.bottom);


	//написать текст
	m_pWindowDraw->SetFontType(m_title_font);
	m_pWindowDraw->SetFontColor(m_title_color);
	m_pWindowDraw->TextOut(rect.left + m_title_rect.left,
						   rect.top + m_title_rect.top,	
							m_sTitle.c_str());
*/		
	RECT rect = GetAbsoluteRect();
	
	m_background.SetPos(rect.left, rect.top);
	m_background.Render();



	//перерисовать дочерние окна
	for(WINDOW_LIST_it it=m_ChildWndList.begin(); it!=m_ChildWndList.end(); it++)
	{
		if((*it)->IsShown())
			(*it)->Draw();
	}
}




//присоединить дочернее окно
void CUIWindow::AttachChild(CUIWindow* pChild)
{
	if(!pChild) return;

	pChild->SetParent(this);

//	pChild->SetDrawInterface(m_pWindowDraw);

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
								m_pMouseCapturer->GetWndRect().left /*- GetWndRect().left*/, 
								  cursor_pos.y - 
								m_pMouseCapturer->GetWndRect().top /*- GetWndRect().top*/, 
								  mouse_action);
		return;
	}

	//Проверка на попадание мыши в окно,
	//происходит в обратном порядке, чем рисование окон
	//(последние в списке имеют высший приоритет)
	WINDOW_LIST::reverse_iterator it = (WINDOW_LIST::reverse_iterator)m_ChildWndList.end();

//	for(WINDOW_LIST_it it = m_ChildWndList.begin(); it!=m_ChildWndList.end(); it++)
	for(u16 i=0; i<m_ChildWndList.size(); i++, it++)
	{
		RECT	l_tRect = (*it)->GetWndRect();
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
void CUIWindow::SendMessage(CUIWindow *pWnd, u16 msg, void *pData)
{
	//оповестить дочерние окна
	for(WINDOW_LIST_it it=m_ChildWndList.begin(); it!=m_ChildWndList.end(); it++)
	{
		(*it)->SendMessage(pWnd,msg,pData);
	}
}