// Window.h: interface for the CUIWindow class.
//
// ����������� ����� ����� ��������� CUIWindowDraw
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
	//�����������/����������
	CUIWindow();
	virtual ~CUIWindow();

	////////////////////////////////////
	//�������������
	virtual void			Init				(int x, int y, int width, int height);
	virtual void			Init				(Irect* pRect);

	virtual CUIDragDropItem*cast_drag_drop_item	()								{return NULL;}

	////////////////////////////////////
	//������ � ��������� � ������������� ������
	virtual void			AttachChild			(CUIWindow* pChild);
	virtual void			DetachChild			(CUIWindow* pChild);
	virtual bool			IsChild				(CUIWindow* pChild) const;
	virtual void			DetachAll			();
	int						GetChildNum			()								{return m_ChildWndList.size();} 

	void					SetParent			(CUIWindow* pNewParent);
	CUIWindow*				GetParent			()	const							{return m_pParentWnd;}
	
	//�������� ���� ������ �������� ������
	CUIWindow*				GetTop				()								{if(m_pParentWnd == NULL) return  this; 
																				else return  m_pParentWnd->GetTop();}


	//������� �� ������� ������ ��������� �������� ����
	virtual bool			BringToTop			(CUIWindow* pChild);

	//������� �� ������� ������ ���� ��������� ���� � ��� ������
	virtual void			BringAllToTop		();
	

	////////////////////////////////////
	//������ � �������� ����������� (����, ����������, ������ ����)

	//������� �� ����
//	typedef enum{LBUTTON_DOWN, RBUTTON_DOWN, LBUTTON_UP, RBUTTON_UP, MOUSE_MOVE,
//				 LBUTTON_DB_CLICK} E_MOUSEACTION;

	virtual void OnMouse(int x, int y, EUIMessages mouse_action);
	virtual void OnDbClick();

	//���������/���������� ���� �����
	//��������� ���������� �������� ����� �������������
	virtual void			SetCapture			(CUIWindow* pChildWindow, bool capture_status);
	virtual CUIWindow*		GetMouseCapturer	()													{return m_pMouseCapturer;}

	//������, �������� ������������ ���������,
	//���� NULL, �� ���� �� GetParent()
	virtual void			SetMessageTarget	(CUIWindow* pWindow)								{m_pMessageTarget = pWindow;}
	virtual CUIWindow*		GetMessageTarget	();

	//������� �� ����������
//	typedef enum{KEY_PRESSED, KEY_RELEASED} E_KEYBOARDACTION;
	virtual bool			OnKeyboard			(int dik, EUIMessages keyboard_action);
	virtual void			SetKeyboardCapture	(CUIWindow* pChildWindow, bool capture_status);

	
	//������ ������������, �� ������� ������ ���� ������������� �������
//	typedef enum{MOUSE_CAPTURE_LOST, 
//				 KEYBOARD_CAPTURE_LOST} E_MESSAGE;
	
	//��������� ��������� �� �������������� ������������ �������������
	//�-��� ������ ����������������
	//pWnd - ��������� �� ����, ������� ������� ���������
	//pData - ��������� �� �������������� ������, ������� ����� ������������
	virtual void			SendMessage			(CUIWindow* pWnd, s16 msg, void* pData = NULL);
	
	

	//����������/���������� �� ���� � ����������
	virtual void			Enable				(bool status)									{m_bIsEnabled=status;}
	virtual bool			IsEnabled			()												{return m_bIsEnabled;}

	//������/�������� ���� � ��� �������� ����
	virtual void			Show				(bool status)									{m_bIsShown= status; Enable(status); }
	virtual bool			IsShown				()												{return m_bIsShown;}
	
	////////////////////////////////////
	//��������� � ������� ����

	//������������� ����������
	const Ivector2&			GetWndPos			() 							{return m_WndRect.lt;}

	Irect					GetWndRect			()									{return m_WndRect;}
	void					SetWndRect			(int x, int y, int width, int height) 		
																					{m_WndRect.set(x,y,x+width,y+height); }

	void					SetWndRect			(Irect r)							{m_WndRect = r;}

	void					SetWndPos			(const Ivector2& pos)				{SetWndPos(pos.x, pos.y);}
	void					SetWndPos			(int x, int y)						{int w = GetWidth();
																					int h = GetHeight();
																					m_WndRect.set(x,y,x+w,y+h);}
	virtual void			MoveWndDelta		(const Ivector2& d)					{ MoveWndDelta(d.x, d.y);	};
	virtual void			MoveWndDelta		(int dx, int dy)					{	m_WndRect.x1+=dx;
																						m_WndRect.x2+=dx;
																						m_WndRect.y1+=dy;
																						m_WndRect.y2+=dy;}


	//���������� ����������
	Ivector2				GetAbsolutePos		() 							{Irect abs = GetAbsoluteRect(); 
																						return Ivector2().set(abs.x1,abs.y1);}
	Irect					GetAbsoluteRect		() ;

	virtual void			SetWidth			(int width)			{m_WndRect.right = m_WndRect.left+width;}

	virtual void			SetHeight			(int height)		{m_WndRect.bottom = m_WndRect.top+height;}

	virtual int				GetWidth			()					{return m_WndRect.width();}
	virtual int				GetHeight			()					{return m_WndRect.height();}




	////////////////////////////////////
	//���������� ����
	virtual void			Draw				();
	//���������� ���� ����������������
	virtual void			Update				();


	//��� �������� ���� � �������� � �������� ���������
	virtual void			Reset				();
	virtual void			ResetAll			();


	//��������!!!! (� ����� ��� � ���)
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
	//������ �������� ����
	WINDOW_LIST				m_ChildWndList;
	
	//��������� �� ������������ ����
	CUIWindow*				m_pParentWnd;

	//�������� ���� �������, ��������� ���� ����
	CUIWindow*				m_pMouseCapturer;
	
	//��� ���������� ����������
	//������ ������, ������ �� ������
	//����� ���� ����� � ����������
	CUIWindow*				m_pOrignMouseCapturer;


	//�������� ���� �������, ��������� ���� ����������
	CUIWindow*				m_pKeyboardCapturer;

	//���� ���� ���������
	CUIWindow*				m_pMessageTarget;

	//��������� � ������ ����, �������� 
	//������������ ������������� ����
	Irect					m_WndRect;

	//�������� �� ���� ������������
	bool					m_bIsEnabled;
	//���������� �� ����
	bool					m_bIsShown;


	/////////////
	//��������� �� ������������ �����
	//��������!!!!
	CGameFont*				m_pFont;


	//����� �������� ����� �����
	//��� ����������� DoubleClick
	u32						m_dwLastClickTime;

	//���� ��������������� �������� �� ����� ������ �����������
	bool					m_bAutoDelete;

	// ���� �����������/����������� ��������� ���������
	bool					m_bDoubleClickEnabled;

	// ���� ������ ��� �����
	bool					m_bCursorOverWindow;

public:
	bool					CursorOverWindow() const				{ return m_bCursorOverWindow; }
	virtual LPCSTR			GetHint			()						{return NULL;};
	// ��������� ������� �����
	Ivector2 cursor_pos;
	DECLARE_SCRIPT_REGISTER_FUNCTION

};

add_to_type_list(CUIWindow)
#undef script_type_list
#define script_type_list save_type_list(CUIWindow)
