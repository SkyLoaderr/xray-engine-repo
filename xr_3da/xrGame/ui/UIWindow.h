// Window.h: interface for the CUIWindow class.
//
// ����������� ����� ����� ��������� CUIWindowDraw
//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "UIMessages.h"
#include "../script_export_space.h"

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
	virtual void Init(int x, int y, int width, int height);
	virtual void Init(RECT* pRect);

	////////////////////////////////////
	//������ � ��������� � ������������� ������
	virtual void AttachChild(CUIWindow* pChild);
	virtual void DetachChild(CUIWindow* pChild);
	virtual bool IsChild(CUIWindow* pChild) const;
	virtual void DetachAll();
	int GetChildNum() {return m_ChildWndList.size();} 

	void SetParent(CUIWindow* pNewParent) {m_pParentWnd = pNewParent;}
	CUIWindow* GetParent() {return m_pParentWnd;}
	
	//�������� ���� ������ �������� ������
	CUIWindow* GetTop() {if(m_pParentWnd == NULL)	
								return  this;
							else
								return  m_pParentWnd->GetTop();}


	//������� �� ������� ������ ��������� �������� ����
	virtual bool BringToTop(CUIWindow* pChild);

	//������� �� ������� ������ ���� ��������� ���� � ��� ������
	virtual void BringAllToTop();
	

	////////////////////////////////////
	//������ � �������� ����������� (����, ����������, ������ ����)

	//������� �� ����
//	typedef enum{LBUTTON_DOWN, RBUTTON_DOWN, LBUTTON_UP, RBUTTON_UP, MOUSE_MOVE,
//				 LBUTTON_DB_CLICK} E_MOUSEACTION;

	virtual void OnMouse(int x, int y, EUIMessages mouse_action);

	//���������/���������� ���� �����
	//��������� ���������� �������� ����� �������������
	virtual void SetCapture(CUIWindow* pChildWindow, bool capture_status);
	virtual CUIWindow* GetMouseCapturer(){return m_pMouseCapturer;}

	//������, �������� ������������ ���������,
	//���� NULL, �� ���� �� GetParent()
	virtual void SetMessageTarget(CUIWindow* pWindow) {	m_pMessageTarget = pWindow;}
	virtual CUIWindow* GetMessageTarget();

	//������� �� ����������
//	typedef enum{KEY_PRESSED, KEY_RELEASED} E_KEYBOARDACTION;
	virtual bool OnKeyboard(int dik, EUIMessages keyboard_action);
	virtual void SetKeyboardCapture(CUIWindow* pChildWindow, bool capture_status);

	
	//������ ������������, �� ������� ������ ���� ������������� �������
//	typedef enum{MOUSE_CAPTURE_LOST, 
//				 KEYBOARD_CAPTURE_LOST} E_MESSAGE;
	
	//��������� ��������� �� �������������� ������������ �������������
	//�-��� ������ ����������������
	//pWnd - ��������� �� ����, ������� ������� ���������
	//pData - ��������� �� �������������� ������, ������� ����� ������������
	virtual void SendMessage(CUIWindow* pWnd, s16 msg, void* pData = NULL);
	
	

	//����������/���������� �� ���� � ����������
	virtual void Enable(bool status) {m_bIsEnabled =  status;}
	virtual bool IsEnabled() {return m_bIsEnabled;}

	//������/�������� ���� � ��� �������� ����
	virtual void Show(bool status) {m_bIsShown =  status; Enable(status); }
	virtual bool IsShown() {return m_bIsShown;}
	
	////////////////////////////////////
	//��������� � ������� ����

	//������������� ����������
	RECT GetWndRect() {return m_WndRect;}
	void SetWndRect(int x, int y, int width, int height) 
						{SetRect(&m_WndRect,x,y,x+width,y+height);}

	void SetWndRect(RECT r){m_WndRect = r;}

	void MoveWindow(int x, int y)
					{int w = GetWidth();
					 int h = GetHeight();
					 SetRect(&m_WndRect,x,y,x+w,y+h);}



	//���������� ����������
	RECT GetAbsoluteRect();

	virtual int GetWidth() {return m_WndRect.right-m_WndRect.left;}
	virtual void SetWidth(int width) {SetRect(&m_WndRect,
										m_WndRect.left,
										m_WndRect.top,
										m_WndRect.left+width,
										m_WndRect.bottom);}

	virtual int GetHeight() {return m_WndRect.bottom-m_WndRect.top;}
	virtual void SetHeight(int height) {SetRect(&m_WndRect,
										m_WndRect.left,
										m_WndRect.top,
										m_WndRect.right,
										m_WndRect.top+height);}



	////////////////////////////////////
	//���������� ����
	virtual void Draw();
	//���������� ���� ����������������
	virtual void Update();


	//��� �������� ���� � �������� � �������� ���������
	virtual void Reset();
	virtual void ResetAll();


	//��������!!!! (� ����� ��� � ���)
	void SetFont(CGameFont* pFont) {m_pFont = pFont;}
	CGameFont* GetFont() {if(m_pFont) return m_pFont;
							if(m_pParentWnd== NULL)	
								return  m_pFont;
							else
								return  m_pParentWnd->GetFont();}

	DEF_LIST (WINDOW_LIST, CUIWindow*);
	WINDOW_LIST& GetChildWndList() {return m_ChildWndList;}


	bool IsAutoDelete() {return m_bAutoDelete;}
	void SetAutoDelete(bool auto_delete) {m_bAutoDelete = auto_delete;}

	// Name of the window
	virtual ref_str WindowName() { return ""; }
	LPCSTR	WindowName_script() {return *(WindowName());}
protected:
	//������ �������� ����
	WINDOW_LIST m_ChildWndList;
	
	//��������� �� ������������ ����
	CUIWindow* m_pParentWnd;

	//�������� ���� �������, ��������� ���� ����
	CUIWindow* m_pMouseCapturer;
	
	//��� ���������� ����������
	//������ ������, ������ �� ������
	//����� ���� ����� � ����������
	CUIWindow* m_pOrignMouseCapturer;


	//�������� ���� �������, ��������� ���� ����������
	CUIWindow* m_pKeyboardCapturer;

	//���� ���� ���������
	CUIWindow* m_pMessageTarget;

	//��������� � ������ ����, �������� 
	//������������ ������������� ����
	RECT m_WndRect;

	//�������� �� ���� ������������
	bool m_bIsEnabled;
	//���������� �� ����
	bool m_bIsShown;


	/////////////
	//��������� �� ������������ �����
	//��������!!!!
	CGameFont* m_pFont;


	//����� �������� ����� �����
	//��� ����������� DoubleClick
	u32 m_dwLastClickTime;

	//���� ��������������� �������� �� ����� ������ �����������
	bool m_bAutoDelete;

public:
	// ��������� ������� �����
	POINT cursor_pos;
	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CUIWindow)
#undef script_type_list
#define script_type_list save_type_list(CUIWindow)
