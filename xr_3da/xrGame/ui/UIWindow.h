// Window.h: interface for the CUIWindow class.
//
// ����������� ����� ����� ��������� CUIWindowDraw
//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "UIMessages.h"
#include "../script_export_space.h"
#include "uiabstract.h"

class CUIDragDropItem;

enum EWindowAlignment{
	waNone		=0,
	waLeft		=1,
	waRight		=2,
	waTop		=4,
	waBottom	=8,
	waCenter	=16
};

class CUIWindow  : public CUISimpleWindow
{
public:
	using CUISimpleWindow::Init;
	////////////////////////////////////
	//�����������/����������
	CUIWindow();
	virtual ~CUIWindow();

	////////////////////////////////////
	//�������������
//	virtual void			Init				(int x, int y, int width, int height);
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
	CUIWindow*				GetCurrentMouseHandler();
	CUIWindow*				GetChildMouseHandler();


	//������� �� ������� ������ ��������� �������� ����
	virtual bool			BringToTop			(CUIWindow* pChild);

	//������� �� ������� ������ ���� ��������� ���� � ��� ������
	virtual void			BringAllToTop		();
	


	virtual void OnMouse(int x, int y, EUIMessages mouse_action);
	virtual void OnMouseMove();
	virtual void OnMouseScroll(int iDirection);
	virtual void OnDbClick();
	virtual void OnMouseDown(bool left_button = true);
	virtual void OnMouseUp(bool left_button = true);
	virtual void OnFocusReceive();
	virtual void OnFocusLost();

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
	virtual void			Show				(bool status)									{SetVisible(status); Enable(status); }
	IC		bool			IsShown				()												{return this->GetVisible();}
	
	////////////////////////////////////
	//��������� � ������� ����

	//������������� ����������
//	IC const Ivector2&		GetWndPos			() 									{return m_iWndPos;}
//	IC void					SetWndPos			(int x, int y)						{m_iWndPos.set(x, y); }
//	IC void					SetWndPos			(const Ivector2& pos)				{SetWndPos(pos.x, pos.y);}

//	Irect					GetWndRect			()									;//	{return Irect().set(m_iWndPos.x,m_iWndPos.y,m_iWndPos.x+m_iWndSize.x,m_iWndPos.y+m_iWndSize.y);}
	void					SetWndRect			(int x, int y, int width, int height) {m_wndPos.set(x,y); m_wndSize.set(width,height); }
	IC void					SetWndRect			(Irect r)							{SetWndRect(r.x1,r.y1,r.width(),r.height());}

	virtual void			MoveWndDelta		(int dx, int dy)					{m_wndPos.x+=dx;m_wndPos.y+=dy;}
	virtual void			MoveWndDelta		(const Ivector2& d)					{ MoveWndDelta(d.x, d.y);	};

	//���������� ����������
	Irect					GetAbsoluteRect		() ;
	Ivector2				GetAbsolutePos		() 									{Irect abs = GetAbsoluteRect(); return Ivector2().set(abs.x1,abs.y1);}

//	virtual void			SetWidth			(int width)			{m_iWndSize.x=width;}
//	virtual void			SetHeight			(int height)		{m_iWndSize.y=height;}

//	virtual int				GetWidth			()					{return m_iWndSize.x;}
//	virtual int				GetHeight			()					{return m_iWndSize.y;}

	////////////////////////////////////
	//���������� ����
	virtual void			Draw				();
	virtual void			Draw				(int x, int y);
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
//	DEF_VECTOR (WINDOW_LIST, CUIWindow*);
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
	IC void					SetAlignment		(EWindowAlignment al)		{m_alignment = al;}
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
//	Ivector2				m_iWndPos;
//	Ivector2				m_iWndSize;

	//�������� �� ���� ������������
	bool					m_bIsEnabled;
	//���������� �� ����
//	bool					m_bIsShown;


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

	IC void					SafeRemoveChild(CUIWindow* child){
								WINDOW_LIST_it it = std::find(m_ChildWndList.begin(),m_ChildWndList.end(),child);
								if(it!=m_ChildWndList.end())m_ChildWndList.erase(it);
	};
private:
	EWindowAlignment		m_alignment;
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
