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

#define BUTTON_UP_OFFSET 75
#define BUTTON_WIDTH 140


void CUIMessageBox::Init(LPCSTR base_name, int x, int y, int width, int height)
{
	AttachChild(&m_UIStatic);
	m_UIStatic.Init(0,0, width, height);
	m_UIStatic.SetTextAlign(CGameFont::alLeft);

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

	inherited::Init(base_name, x,y, width, height);

	SetStyle(MESSAGEBOX_OK);
}

void CUIMessageBox::AutoCenter()
{
	if(!GetParent()) return;

	int x = (GetParent()->GetWidth() - GetWidth())/2;
	int y = (GetParent()->GetHeight() - GetHeight())/2;

	MoveWindow(x,y);
}

void CUIMessageBox::Show()
{
	BringAllToTop();
	inherited::Enable(true);
	inherited::Show(true);

	m_UIButton1.Reset();
	m_UIButton2.Reset();
	m_UIButton3.Reset();

}
void CUIMessageBox::Hide()
{
	inherited::Enable(false);
	inherited::Show(false);
}

void CUIMessageBox::SendMessage(CUIWindow *pWnd, s16 msg, void *pData)
{
	switch(m_eMessageBoxStyle)
	{
	case MESSAGEBOX_OK:
		if(pWnd == &m_UIButton1)
			if(msg == BUTTON_CLICKED)
				GetMessageTarget()->SendMessage(this, MESSAGE_BOX_OK_CLICKED);
		break;
	case MESSAGEBOX_YES_NO:
		if(pWnd == &m_UIButton1)
		{
			if(msg == BUTTON_CLICKED)
			{
				GetMessageTarget()->SendMessage(this, MESSAGE_BOX_YES_CLICKED);
			}
		}
		else if(pWnd == &m_UIButton2)
		{
			if(msg == BUTTON_CLICKED)
			{
				GetMessageTarget()->SendMessage(this, MESSAGE_BOX_NO_CLICKED);
			}
		}
		break;
	case MESSAGEBOX_YES_NO_CANCEL:
		if(pWnd == &m_UIButton1)
		{
			if(msg == BUTTON_CLICKED)
			{
				GetMessageTarget()->SendMessage(this, MESSAGE_BOX_YES_CLICKED);
			}
		}
		else if(pWnd == &m_UIButton2)
		{
			if(msg == BUTTON_CLICKED)
			{
				GetMessageTarget()->SendMessage(this, MESSAGE_BOX_NO_CLICKED);
			}
		}
		else if(pWnd == &m_UIButton3)
		{
			if(msg == BUTTON_CLICKED)
			{
				GetMessageTarget()->SendMessage(this, MESSAGE_BOX_CANCEL_CLICKED);
			}
		}
		break;
	}
	inherited::SendMessage(pWnd, msg, pData);
}

void CUIMessageBox::SetStyle(E_MESSAGEBOX_STYLE messageBoxStyle)
{
	m_eMessageBoxStyle = messageBoxStyle;

	int width = GetWidth();

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
}

void CUIMessageBox::SetText(LPCSTR str)
{
	CGameFont *pFont = m_UIStatic.GetFont();
	if (pFont)
	{
		m_UIStatic.MoveWindow(GetWidth() / 2 - static_cast<int>(pFont->SizeOf(str) / 2),
			GetHeight() / 2 - static_cast<int>(pFont->CurrentHeight()));
	}
	m_UIStatic.SetText(str);
}
void CUIMessageBox::OnMouse(int x, int y, EUIMessages mouse_action)
{
	inherited::OnMouse(x, y, mouse_action);
}