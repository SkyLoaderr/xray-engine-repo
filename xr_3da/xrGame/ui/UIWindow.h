// Window.h: interface for the CUIWindow class.
//
// графический вывод через интерфейс CUIWindowDraw
//////////////////////////////////////////////////////////////////////

#pragma once


class CUIWindow  
{
public:
	////////////////////////////////////
	//конструктор/деструктор
	CUIWindow();
	virtual ~CUIWindow();

	////////////////////////////////////
	//инициализация
	virtual void Init(int x, int y, int width, int height);
	virtual void Init(RECT* pRect);

	////////////////////////////////////
	//работа с дочерними и родительскими окнами
	void AttachChild(CUIWindow* pChild);
	void DetachChild(CUIWindow* pChild);

	void SetParent(CUIWindow* pNewParent) {m_pParentWnd = pNewParent;}
	CUIWindow* GetParent() {return m_pParentWnd;}

	////////////////////////////////////
	//работа с внешними сообщениями (мышь, клавиатура, другие окна)

	//реакция на мышь
	typedef enum{LBUTTON_DOWN, RBUTTON_DOWN, LBUTTON_UP, RBUTTON_UP, MOUSE_MOVE} E_MOUSEACTION;

	virtual void OnMouse(int x, int y, E_MOUSEACTION mouse_action);

	//захватить/освободить мышь окном
	//сообщение посылается дочерним окном родительскому
	void SetCapture(CUIWindow* pChildWindow, bool capture_status);

	
	//список перечисление, на которые должна быть предусмотрена реакция
	typedef enum{} E_MESSAGE;
	
	//обработка сообщений не предусмотреных стандартными обработчиками
	//ф-ция должна переопределяться
	//pWnd - указатель на окно, которое послало сообщение
	//pData - указатель на дополнительные данные, которые могут понадобиться
	virtual void SendMessage(CUIWindow* pWnd, u16 msg, void* pData = NULL);



	//запрещение/разрешение на ввод с клавиатуры
	void Enable(bool status) {m_bIsEnabled =  status;}
	bool IsEnabled() {return m_bIsEnabled;}

	//убрать/показать окно и его дочерние окна
	void Show(bool status) {m_bIsShown =  status;}
	bool IsShown() {return m_bIsShown;}
	
	


	////////////////////////////////////
	//положение и размеры окна

	//относительные координаты
	RECT GetWndRect() {return m_WndRect;}
	void SetWndRect(int x, int y, int width, int height) 
						{SetRect(&m_WndRect,x,y,x+width,y+height);}

	void MoveWindow(int x, int y)
					{int w = GetWidth();
					 int h = GetHeight();
					 SetRect(&m_WndRect,x,y,x+w,y+h);}



	//абсолютные координаты
	RECT GetAbsoluteRect();

	int GetWidth() {return m_WndRect.right-m_WndRect.left;}
	void SetWidth(int width) {SetRect(&m_WndRect,
										m_WndRect.left,
										m_WndRect.top,
										m_WndRect.left+width,
										m_WndRect.bottom);}

	int GetHeight() {return m_WndRect.bottom-m_WndRect.top;}
	void SetHeight(int height) {SetRect(&m_WndRect,
										m_WndRect.left,
										m_WndRect.top,
										m_WndRect.right,
										m_WndRect.top+height);}



	////////////////////////////////////
	//прорисовка окна
	virtual void Draw();
	//обновление окна передпрорисовкой
	virtual void Update();
	

protected:

	DEF_LIST (WINDOW_LIST, CUIWindow*);


	//список дочерних окон
	WINDOW_LIST m_ChildWndList;
	
	//указатель на родительское окно
	CUIWindow* m_pParentWnd;

	//дочернее окно которое, захватило ввод мыши
	CUIWindow* m_pMouseCapturer;

	//положение и размер окна, задается 
	//относительно родительского окна
	RECT m_WndRect;

	//разрешен ли ввод пользователя
	bool m_bIsEnabled;
	//показывать ли окно
	bool m_bIsShown;

};