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

	AttachChild(&m_UIButtonYesOk);
	AttachChild(&m_UIButtonNo);
	AttachChild(&m_UIButtonCancel);

	m_UIButtonYesOk.Init("ui\\ui_button_01", width/6 - BUTTON_WIDTH/2,
										height - BUTTON_UP_OFFSET,
										BUTTON_WIDTH,50);
	m_UIButtonNo.Init("ui\\ui_button_02", width/2 - BUTTON_WIDTH/2,
										height - BUTTON_UP_OFFSET,
										BUTTON_WIDTH,50);
	m_UIButtonCancel.Init("ui\\ui_button_03", 5*width/6 - BUTTON_WIDTH/2,
										height - BUTTON_UP_OFFSET,
										BUTTON_WIDTH,50);

	m_UIButtonYesOk.Show(false);
	m_UIButtonNo.Show(false);
	m_UIButtonCancel.Show(false);

	m_UIButtonYesOk.Enable(false);
	m_UIButtonNo.Enable(false);
	m_UIButtonCancel.Enable(false);

	inherited::Init(base_name, x,y, width, height);

	SetStyle(MESSAGEBOX_OK);
}

void CUIMessageBox::AutoCenter()
{
	if(!GetParent()) return;

	int x = (GetParent()->GetWidth() - GetWidth())/2;
	int y = (GetParent()->GetHeight() - GetHeight())/2;

	SetWndPos(x,y);
}

void CUIMessageBox::Show()
{
	BringAllToTop();
	inherited::Enable(true);
	inherited::Show(true);

	m_UIButtonYesOk.Reset();
	m_UIButtonNo.Reset();
	m_UIButtonCancel.Reset();

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
		if(pWnd == &m_UIButtonYesOk)
			if(msg == BUTTON_CLICKED)
				GetMessageTarget()->SendMessage(this, MESSAGE_BOX_OK_CLICKED);
		break;
	case MESSAGEBOX_YES_NO:
		if(pWnd == &m_UIButtonYesOk)
		{
			if(msg == BUTTON_CLICKED)
			{
				GetMessageTarget()->SendMessage(this, MESSAGE_BOX_YES_CLICKED);
			}
		}
		else if(pWnd == &m_UIButtonNo)
		{
			if(msg == BUTTON_CLICKED)
			{
				GetMessageTarget()->SendMessage(this, MESSAGE_BOX_NO_CLICKED);
			}
		}
		break;
	case MESSAGEBOX_YES_NO_CANCEL:
		if(pWnd == &m_UIButtonYesOk)
		{
			if(msg == BUTTON_CLICKED)
			{
				GetMessageTarget()->SendMessage(this, MESSAGE_BOX_YES_CLICKED);
			}
		}
		else if(pWnd == &m_UIButtonNo)
		{
			if(msg == BUTTON_CLICKED)
			{
				GetMessageTarget()->SendMessage(this, MESSAGE_BOX_NO_CLICKED);
			}
		}
		else if(pWnd == &m_UIButtonCancel)
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
		m_UIButtonYesOk.Show(true);
		m_UIButtonYesOk.Enable(true);
		m_UIButtonYesOk.SetText("OK");

		m_UIButtonYesOk.SetWndRect(width/2 - BUTTON_WIDTH/2, 
						 m_UIButtonYesOk.GetWndRect().top,
						 BUTTON_WIDTH,50);

		m_UIButtonNo.Show(false);
		m_UIButtonNo.Enable(false);
		m_UIButtonCancel.Show(false);
		m_UIButtonCancel.Enable(false);

		break;
	case MESSAGEBOX_YES_NO:
		m_UIButtonYesOk.Show(true);
		m_UIButtonYesOk.Enable(true);
		m_UIButtonYesOk.SetText("Yes");
		
		m_UIButtonYesOk.SetWndRect(width/4 - BUTTON_WIDTH/2, 
							 m_UIButtonYesOk.GetWndRect().top,
							 BUTTON_WIDTH,50);
				

		m_UIButtonNo.Show(true);
		m_UIButtonNo.Enable(true);
		m_UIButtonNo.SetText("No");


		m_UIButtonNo.SetWndRect(width/2 + BUTTON_WIDTH/2, 
							 m_UIButtonNo.GetWndRect().top,
							 BUTTON_WIDTH,50);

		m_UIButtonCancel.Show(false);
		m_UIButtonCancel.Enable(false);
		break;
	case MESSAGEBOX_YES_NO_CANCEL:
		m_UIButtonYesOk.Show(true);
		m_UIButtonYesOk.Enable(true);
		m_UIButtonYesOk.SetText("Yes");

		m_UIButtonNo.Show(true);
		m_UIButtonNo.Enable(true);
		m_UIButtonNo.SetText("No");

		m_UIButtonCancel.Show(true);
		m_UIButtonCancel.Enable(true);
		m_UIButtonCancel.SetText("Cancel");
		break;
	}
}

void CUIMessageBox::SetText(LPCSTR str)
{
	CGameFont *pFont = m_UIStatic.GetFont();
	if (pFont)
	{
		m_UIStatic.SetWndPos(GetWidth() / 2 - static_cast<int>(pFont->SizeOf(str) / 2),
			GetHeight() / 2 - static_cast<int>(pFont->CurrentHeight()));
	}
	m_UIStatic.SetText(str);
}
void CUIMessageBox::OnMouse(int x, int y, EUIMessages mouse_action)
{
	inherited::OnMouse(x, y, mouse_action);
}