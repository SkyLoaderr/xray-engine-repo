// Window.h: interface for the CUIWindow class.
//
// ����������� ����� ����� ��������� CUIWindowDraw
//////////////////////////////////////////////////////////////////////

#pragma once

//#include "windows.h"

//��� ��������� 
//#include "WindowDraw.h"



//��� ������ � XML
//#include "IOXml.h"

class CUIWindow  
{
public:
	////////////////////////////////////
	//�����������/����������
	CUIWindow();
	virtual ~CUIWindow();

	////////////////////////////////////
	//�������������
	virtual void Init(const char* title, int x, int y, int width, int height);
	virtual void Init(const char* title, RECT* pRect);
	//�������� ���������� �� XML
	virtual void Init(const char* xml_filename);

	////////////////////////////////////
	//������ � ��������� � ������������� ������
	void AttachChild(CUIWindow* pChild);
	void DetachChild(CUIWindow* pChild);

	void SetParent(CUIWindow* pNewParent) {m_pParentWnd = pNewParent;}
	CUIWindow* GetParent() {return m_pParentWnd;}

	////////////////////////////////////
	//������ � �������� ����������� (����, ����������, ������ ����)

	//������� �� ����
	typedef enum{LBUTTON_DOWN, RBUTTON_DOWN, LBUTTON_UP, RBUTTON_UP, MOUSE_MOVE} E_MOUSEACTION;

	virtual void OnMouse(int x, int y, E_MOUSEACTION mouse_action);

	//���������/���������� ���� �����
	//��������� ���������� �������� ����� �������������
	void SetCapture(CUIWindow* pChildWindow, bool capture_status);

	
	//������ ������������, �� ������� ������ ���� ������������� �������
	typedef enum{} E_MESSAGE;
	
	//��������� ��������� �� �������������� ������������ �������������
	//�-��� ������ ����������������
	//pWnd - ��������� �� ����, ������� ������� ���������
	//pData - ��������� �� �������������� ������, ������� ����� ������������
	virtual void SendMessage(CUIWindow* pWnd, u16 msg, void* pData = NULL);



	//����������/���������� �� ���� � ����������
	void Enable(bool status) {m_bIsEnabled =  status;}
	bool IsEnabled() {return m_bIsEnabled;}

	//������/�������� ���� � ��� �������� ����
	void Show(bool status) {m_bIsShown =  status;}
	bool IsShown() {return m_bIsShown;}
	
	


	////////////////////////////////////
	//��������� � ������� ����

	//������������� ����������
	RECT GetWndRect() {return m_WndRect;}
	void SetWndRect(int x, int y, int width, int height) 
						{SetRect(&m_WndRect,x,y,x+width,y+height);}

	void MoveWindow(int x, int y)
					{int w = GetWidth();
					 int h = GetHeight();
					 SetRect(&m_WndRect,x,y,x+w,y+h);}



	//���������� ����������
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
	//���������� ����
	virtual void Draw();
	

protected:

	DEF_LIST (WINDOW_LIST, CUIWindow*);


	//������ �������� ����
	WINDOW_LIST m_ChildWndList;
	
	//��������� �� ������������ ����
	CUIWindow* m_pParentWnd;

	//�������� ���� �������, ��������� ���� ����
	CUIWindow* m_pMouseCapturer;

	//��������� ����
	//wstring m_sTitle;
	//string m_sTitle;

	//��������� � ������ ����, �������� 
	//������������ ������������� ����
	RECT m_WndRect;

	//�������� �� ���� ������������
	bool m_bIsEnabled;
	//���������� �� ����
	bool m_bIsShown;



	///////////////////////////////////////	
	//����������� ��������� ��� ���������
	///////////////////////////////////////
//	CUIWindowDraw* m_pWindowDraw;



	////////////////////////////////////	
	//�������� ��� ���������
	/////////////////////////////////////

	//��������� ������� ���������
//	RECT m_title_rect;

	//��� ������ ���������
//	CUIWindowDraw::E_FONTTYPE m_title_font;

	//���� ������
//	DWORD m_title_color;

	//��� ������ ���������� �� XML �����
//	CIOXml m_inXml;

};