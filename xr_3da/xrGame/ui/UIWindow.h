// Window.h: interface for the CUIWindow class.
//
// графический вывод через интерфейс CUIWindowDraw
//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "UIMessages.h"
#include "../script_export_space.h"

class CUIDragDropItem;

//////////////////////////////////////////////////////////////////////////

#define SHOW_UI_WINDOW(UI_WINDOW) (UI_WINDOW).Show(true); (UI_WINDOW).Enable(true);
#define HIDE_UI_WINDOW(UI_WINDOW) (UI_WINDOW).Show(false); (UI_WINDOW).Enable(false);

//////////////////////////////////////////////////////////////////////////

class CUIWindow  
{
public:
	////////////////////////////////////
	//конструктор/деструктор
	CUIWindow();
	virtual ~CUIWindow();

	////////////////////////////////////
	//инициализация
	virtual void			Init				(int x, int y, int width, int height);
	virtual void			Init				(Irect* pRect);

	virtual CUIDragDropItem*cast_drag_drop_item	()								{return NULL;}

	////////////////////////////////////
	//работа с дочерними и родительскими окнами
	virtual void			AttachChild			(CUIWindow* pChild);
	virtual void			DetachChild			(CUIWindow* pChild);
	virtual bool			IsChild				(CUIWindow* pChild) const;
	virtual void			DetachAll			();
	int						GetChildNum			()								{return m_ChildWndList.size();} 

	void					SetParent			(CUIWindow* pNewParent);
	CUIWindow*				GetParent			()								{return m_pParentWnd;}
	
	//получить окно самого верхнего уровня
	CUIWindow*				GetTop				()								{if(m_pParentWnd == NULL) return  this; 
																				else return  m_pParentWnd->GetTop();}


	//поднять на вершину списка выбранное дочернее окно
	virtual bool			BringToTop			(CUIWindow* pChild);

	//поднять на вершину списка всех родителей окна и его самого
	virtual void			BringAllToTop		();
	

	////////////////////////////////////
	//работа с внешними сообщениями (мышь, клавиатура, другие окна)

	//реакция на мышь
//	typedef enum{LBUTTON_DOWN, RBUTTON_DOWN, LBUTTON_UP, RBUTTON_UP, MOUSE_MOVE,
//				 LBUTTON_DB_CLICK} E_MOUSEACTION;

	virtual void OnMouse(int x, int y, EUIMessages mouse_action);
	virtual void OnMouseWheel(int direction);
	virtual void OnDbClick();

	//захватить/освободить мышь окном
	//сообщение посылается дочерним окном родительскому
	virtual void			SetCapture			(CUIWindow* pChildWindow, bool capture_status);
	virtual CUIWindow*		GetMouseCapturer	()													{return m_pMouseCapturer;}

	//окошко, которому пересылаются сообщения,
	//если NULL, то шлем на GetParent()
	virtual void			SetMessageTarget	(CUIWindow* pWindow)								{m_pMessageTarget = pWindow;}
	virtual CUIWindow*		GetMessageTarget	();

	//реакция на клавиатуру
//	typedef enum{KEY_PRESSED, KEY_RELEASED} E_KEYBOARDACTION;
	virtual bool			OnKeyboard			(int dik, EUIMessages keyboard_action);
	virtual void			SetKeyboardCapture	(CUIWindow* pChildWindow, bool capture_status);

	
	//список перечисление, на которые должна быть предусмотрена реакция
//	typedef enum{MOUSE_CAPTURE_LOST, 
//				 KEYBOARD_CAPTURE_LOST} E_MESSAGE;
	
	//обработка сообщений не предусмотреных стандартными обработчиками
	//ф-ция должна переопределяться
	//pWnd - указатель на окно, которое послало сообщение
	//pData - указатель на дополнительные данные, которые могут понадобиться
	virtual void			SendMessage			(CUIWindow* pWnd, s16 msg, void* pData = NULL);
	
	

	//запрещение/разрешение на ввод с клавиатуры
	virtual void			Enable				(bool status)									{m_bIsEnabled=status;}
	virtual bool			IsEnabled			()												{return m_bIsEnabled;}

	//убрать/показать окно и его дочерние окна
	virtual void			Show				(bool status)									{m_bIsShown= status; Enable(status); }
	virtual bool			IsShown				()												{return m_bIsShown;}
	
	////////////////////////////////////
	//положение и размеры окна

	//относительные координаты
	Ivector2				GetWndPos			() const							(return Ivector2().set(m_WndRect.x1,m_WndRect.y1);)

	Irect					GetWndRect			()									{return m_WndRect;}
	void					SetWndRect			(int x, int y, int width, int height) 		
																					{m_WndRect.set(x,y,x+width,y+height); }

	void					SetWndRect			(Irect r)							{m_WndRect = r;}

	void					SetWndPos			(int x, int y)						{int w = GetWidth();
																					int h = GetHeight();
																					m_WndRect.set(x,y,x+w,y+h);}
	virtual void			MoveWndDelta		(const Ivector2& d)					{ MoveWndDelta(d.x, d.y);	};
	virtual void			MoveWndDelta		(int dx, int dy)					{	m_WndRect.x1+=dx;
																						m_WndRect.x2+=dx;
																						m_WndRect.y1+=dy;
																						m_WndRect.y2+=dy;}


	//абсолютные координаты
	Ivector2				GetAbsolutePos		() const							{Irect abs = GetAbsoluteRect(); 
																					return Ivector2().set(abs.x1,abs.y1);)
	Irect					GetAbsoluteRect		();

	virtual void			SetWidth			(int width)			{m_WndRect.right = m_WndRect.left+width;}

	virtual void			SetHeight			(int height)		{m_WndRect.bottom = m_WndRect.top+height;}

	virtual int				GetWidth			()					{return m_WndRect.width();}
	virtual int				GetHeight			()					{return m_WndRect.height();}




	////////////////////////////////////
	//прорисовка окна
	virtual void			Draw				();
	//обновление окна передпрорисовкой
	virtual void			Update				();


	//для перевода окна и потомков в исходное состояние
	virtual void			Reset				();
	virtual void			ResetAll			();


	//временно!!!! (а может уже и нет)
	virtual void			SetFont				(CGameFont* pFont)			{ m_pFont = pFont;}
	CGameFont*				GetFont				()							{if(m_pFont) return m_pFont;
																				if(m_pParentWnd== NULL)	
																					return  m_pFont;
																				else
																					return  m_pParentWnd->GetFont();}

	DEF_LIST (WINDOW_LIST, CUIWindow*);
	WINDOW_LIST&			GetChildWndList		()							{return m_ChildWndList; }


	bool					IsAutoDelete		()							{return m_bAutoDelete;}
	void					SetAutoDelete		(bool auto_delete)			{m_bAutoDelete = auto_delete;}

	// Name of the window
	const shared_str		WindowName			() const					{ return m_windowName; }
	void					SetWindowName		(LPCSTR wn)					{ m_windowName = wn; }
	LPCSTR					WindowName_script	()							{return *m_windowName;}
	CUIWindow*				FindChild			(const shared_str name);

	void					EnableDoubleClick	(bool value)				{ m_bDoubleClickEnabled = value; }
	bool					IsDBClickEnabled	() const					{ return m_bDoubleClickEnabled; }
protected:
	shared_str				m_windowName;
	//список дочерних окон
	WINDOW_LIST				m_ChildWndList;
	
	//указатель на родительское окно
	CUIWindow*				m_pParentWnd;

	//дочернее окно которое, захватило ввод мыши
	CUIWindow*				m_pMouseCapturer;
	
	//кто изначально иницировал
	//захват фокуса, только он теперь
	//может весь фокус и освободить
	CUIWindow*				m_pOrignMouseCapturer;


	//дочернее окно которое, захватило ввод клавиатуры
	CUIWindow*				m_pKeyboardCapturer;

	//кому шлем сообщения
	CUIWindow*				m_pMessageTarget;

	//положение и размер окна, задается 
	//относительно родительского окна
	Irect					m_WndRect;

	//разрешен ли ввод пользователя
	bool					m_bIsEnabled;
	//показывать ли окно
	bool					m_bIsShown;


	/////////////
	//указатель на используемый шрифт
	//временно!!!!
	CGameFont*				m_pFont;


	//время прошлого клика мышки
	//для определения DoubleClick
	u32						m_dwLastClickTime;

	//флаг автоматического удаления во время вызова деструктора
	bool					m_bAutoDelete;

	// Флаг разрешающий/запрещающий генерацию даблклика
	bool					m_bDoubleClickEnabled;

	// Если курсор над окном
	bool					m_bCursorOverWindow;

public:
	bool					CursorOverWindow() const				{ return m_bCursorOverWindow; }
	// Последняя позиция мышки
	Ivector2 cursor_pos;
	DECLARE_SCRIPT_REGISTER_FUNCTION

};

add_to_type_list(CUIWindow)
#undef script_type_list
#define script_type_list save_type_list(CUIWindow)
