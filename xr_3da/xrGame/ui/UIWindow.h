// Window.h: interface for the CUIWindow class.
//
// графический вывод через интерфейс CUIWindowDraw
//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "UIMessages.h"
#include "../script_export_space.h"
#include "uiabstract.h"

class CUIDragDropItem;


class CUIWindow  : public CUISimpleWindow
{
public:
	using CUISimpleWindow::Init;

				CUIWindow						();
	virtual		~CUIWindow						();

	////////////////////////////////////
	//инициализация
	virtual void			Init				(Frect* pRect);

	virtual CUIDragDropItem*cast_drag_drop_item	()								{return NULL;}

	////////////////////////////////////
	//работа с дочерними и родительскими окнами
	virtual void			AttachChild			(CUIWindow* pChild);
	virtual void			DetachChild			(CUIWindow* pChild);
	virtual bool			IsChild				(CUIWindow* pChild) const;
	virtual void			DetachAll			();
	int						GetChildNum			()								{return m_ChildWndList.size();} 

	void					SetParent			(CUIWindow* pNewParent);
	CUIWindow*				GetParent			()	const							{return m_pParentWnd;}
	
	//получить окно самого верхнего уровня
	CUIWindow*				GetTop				()								{if(m_pParentWnd == NULL) return  this; 
																				else return  m_pParentWnd->GetTop();}
	CUIWindow*				GetCurrentMouseHandler();
	CUIWindow*				GetChildMouseHandler();


	//поднять на вершину списка выбранное дочернее окно
	virtual bool			BringToTop			(CUIWindow* pChild);

	//поднять на вершину списка всех родителей окна и его самого
	virtual void			BringAllToTop		();
	


	virtual bool OnMouse(float x, float y, EUIMessages mouse_action);
	virtual void OnMouseMove();
	virtual void OnMouseScroll(float iDirection);
	virtual bool OnDbClick();
	virtual void OnMouseDown(bool left_button = true);
	virtual void OnMouseUp(bool left_button = true);
	virtual void OnFocusReceive();
	virtual void OnFocusLost();
			bool HasChildMouseHandler();

	//захватить/освободить мышь окном
	//сообщение посылается дочерним окном родительскому
	virtual void			SetCapture			(CUIWindow* pChildWindow, bool capture_status);
	virtual CUIWindow*		GetMouseCapturer	()													{return m_pMouseCapturer;}

	//окошко, которому пересылаются сообщения,
	//если NULL, то шлем на GetParent()
	virtual void			SetMessageTarget	(CUIWindow* pWindow)								{m_pMessageTarget = pWindow;}
	virtual CUIWindow*		GetMessageTarget	();

	//реакция на клавиатуру
	virtual bool			OnKeyboard			(int dik, EUIMessages keyboard_action);
	virtual void			SetKeyboardCapture	(CUIWindow* pChildWindow, bool capture_status);

	
	
	//обработка сообщений не предусмотреных стандартными обработчиками
	//ф-ция должна переопределяться
	//pWnd - указатель на окно, которое послало сообщение
	//pData - указатель на дополнительные данные, которые могут понадобиться
	virtual void			SendMessage			(CUIWindow* pWnd, s16 msg, void* pData = NULL);
	
	

	//запрещение/разрешение на ввод с клавиатуры
	virtual void			Enable				(bool status)									{m_bIsEnabled=status;}
	virtual bool			IsEnabled			()												{return m_bIsEnabled;}

	//убрать/показать окно и его дочерние окна
	virtual void			Show				(bool status)									{SetVisible(status); Enable(status); }
	IC		bool			IsShown				()												{return this->GetVisible();}
	
	//абсолютные координаты
	Frect					GetAbsoluteRect		() ;
	Fvector2				GetAbsolutePos		() 												{Frect abs = GetAbsoluteRect(); return Fvector2().set(abs.x1,abs.y1);}


	virtual void			SetWndRect_script(float x, float y, float width, float height)		{CUISimpleWindow::SetWndRect(x,y,width,height);}
	virtual void			SetWndRect_script(Frect rect)										{CUISimpleWindow::SetWndRect(rect);}

	//прорисовка окна
	virtual void			Draw				();
	virtual void			Draw				(float x, float y);
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


	IC bool					IsAutoDelete		()							{return m_bAutoDelete;}
	IC void					SetAutoDelete		(bool auto_delete)			{m_bAutoDelete = auto_delete;}

	// Name of the window
	const shared_str		WindowName			() const					{ return m_windowName; }
	void					SetWindowName		(LPCSTR wn)					{ m_windowName = wn; }
	LPCSTR					WindowName_script	()							{return *m_windowName;}
	CUIWindow*				FindChild			(const shared_str name);

	IC void					EnableDoubleClick	(bool value)				{ m_bDoubleClickEnabled = value; }
	IC bool					IsDBClickEnabled	() const					{ return m_bDoubleClickEnabled; }
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

	//разрешен ли ввод пользователя
	bool					m_bIsEnabled;

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
	bool					m_bClickable;
	IC void					SafeRemoveChild(CUIWindow* child){
								WINDOW_LIST_it it = std::find(m_ChildWndList.begin(),m_ChildWndList.end(),child);
								if(it!=m_ChildWndList.end())m_ChildWndList.erase(it);
	};

public:
	bool					CursorOverWindow() const				{ return m_bCursorOverWindow; }
	// Последняя позиция мышки
	Fvector2 cursor_pos;
	DECLARE_SCRIPT_REGISTER_FUNCTION

};

add_to_type_list(CUIWindow)
#undef script_type_list
#define script_type_list save_type_list(CUIWindow)
