// UIMessageBox.cpp: 
//
// окшко с собщением и конпками
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UIMessageBox.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUIMessageBox::CUIMessageBox()
{
}

CUIMessageBox::~CUIMessageBox()
{
}
/*

	virtual void Init(LPSTR text, LPSTR caption, 
						LPCSTR base_name, int x, int y, int width, int height
						E_MB_STYLE messageBoxStyle);
	virtual void Init(LPSTR text, LPSTR caption,
						LPCSTR base_name, RECT* pRect,
						E_MB_STYLE messageBoxStyle);
*/

#define BUTTON_UP_OFFSET 55
#define BUTTON_WIDTH 140


void CUIMessageBox::Init(LPSTR text, LPSTR caption, E_MESSAGEBOX_STYLE messageBoxStyle,
						LPCSTR base_name, int x, int y, int width, int height)
{
	m_eMessageBoxStyle = messageBoxStyle;


	AttachChild(&m_UIButton1);
	AttachChild(&m_UIButton2);
	AttachChild(&m_UIButton3);

	m_UIButton1.Init("ui\\ui_button_01", width/6 - BUTTON_WIDTH/2,
										height - BUTTON_UP_OFFSET,
										BUTTON_WIDTH,50);
	m_UIButton2.Init("ui\\ui_button_02", width/2 - BUTTON_WIDTH/2,
										height - BUTTON_UP_OFFSET,
										BUTTON_WIDTH,50);
	m_UIButton3.Init("ui\\ui_button_03", 5*width/6 - BUTTON_WIDTH/2,
										height - BUTTON_UP_OFFSET,
										BUTTON_WIDTH,50);

	m_UIButton1.Show(false);
	m_UIButton2.Show(false);
	m_UIButton3.Show(false);

	m_UIButton1.Enable(false);
	m_UIButton2.Enable(false);
	m_UIButton3.Enable(false);

	switch(m_eMessageBoxStyle)
	{
	case MESSAGEBOX_OK:
		m_UIButton1.Show(true);
		m_UIButton1.Enable(true);
		m_UIButton1.SetText("OK");

		m_UIButton1.SetWndRect(width/2 - BUTTON_WIDTH/2, 
						 m_UIButton1.GetWndRect().top,
						 BUTTON_WIDTH,50);


		break;
	case MESSAGEBOX_YES_NO:
		m_UIButton1.Show(true);
		m_UIButton1.Enable(true);
		m_UIButton1.SetText("Yes");
		
		m_UIButton1.SetWndRect(width/4 - BUTTON_WIDTH/2, 
							 m_UIButton1.GetWndRect().top,
							 BUTTON_WIDTH,50);
				

		m_UIButton2.Show(true);
		m_UIButton2.Enable(true);
		m_UIButton2.SetText("No");


		m_UIButton2.SetWndRect(width/2 + BUTTON_WIDTH/2, 
							 m_UIButton1.GetWndRect().top,
							 BUTTON_WIDTH,50);
		break;
	case MESSAGEBOX_YES_NO_CANCEL:
		m_UIButton1.Show(true);
		m_UIButton1.Enable(true);
		m_UIButton1.SetText("Yes");

		m_UIButton2.Show(true);
		m_UIButton2.Enable(true);
		m_UIButton2.SetText("No");

		m_UIButton3.Show(true);
		m_UIButton3.Enable(true);
		m_UIButton3.SetText("Cancel");
		break;
	}
	
	CUIFrameWindow::Init(base_name, x,y, width, height);
}

void CUIMessageBox::Init(LPSTR text, LPSTR caption,E_MESSAGEBOX_STYLE messageBoxStyle,
						LPCSTR base_name, RECT* pRect)
{
	Init(text,  caption, messageBoxStyle,
			base_name, pRect->left, pRect->top, 
			pRect->right - pRect->left, 
			pRect->bottom - pRect->top);
}


void CUIMessageBox::SendMessage(CUIWindow *pWnd, s16 msg, void *pData)
{
	switch(m_eMessageBoxStyle)
	{
	case MESSAGEBOX_OK:
		if(pWnd == &m_UIButton1)
			if(msg == CUIButton::BUTTON_CLICKED)
				GetParent()->SendMessage(this, OK_CLICKED);
		break;
	case MESSAGEBOX_YES_NO:
		if(pWnd == &m_UIButton1)
		{
			if(msg == CUIButton::BUTTON_CLICKED)
			{
				GetParent()->SendMessage(this, YES_CLICKED);
			}
		}
		else if(pWnd == &m_UIButton2)
		{
			if(msg == CUIButton::BUTTON_CLICKED)
			{
				GetParent()->SendMessage(this, NO_CLICKED);
			}
		}
		break;
	case MESSAGEBOX_YES_NO_CANCEL:
		if(pWnd == &m_UIButton1)
		{
			if(msg == CUIButton::BUTTON_CLICKED)
			{
				GetParent()->SendMessage(this, YES_CLICKED);
			}
		}
		else if(pWnd == &m_UIButton2)
		{
			if(msg == CUIButton::BUTTON_CLICKED)
			{
				GetParent()->SendMessage(this, NO_CLICKED);
			}
		}
		else if(pWnd == &m_UIButton3)
		{
			if(msg == CUIButton::BUTTON_CLICKED)
			{
				GetParent()->SendMessage(this, CANCEL_CLICKED);
			}
		}
		break;
	}


	CUIFrameWindow::SendMessage(pWnd, msg, pData);
}