///////////////////////////////////////////////////////////
/// Режим диалога в PDA
///////////////////////////////////////////////////////////
#include "stdafx.h"
#include "UIPdaWnd.h"

#include "..\\Pda.h"
#include "..\\HUDManager.h"


CUIPdaDialogWnd::CUIPdaDialogWnd()
{
	m_iMsgNum = 0;
}
CUIPdaDialogWnd::~CUIPdaDialogWnd()
{
}

void CUIPdaDialogWnd::Init(int x, int y, int width, int height)
{
	AttachChild(&UILogListWnd);
	UILogListWnd.Init(10,10, width-10,height-10);
	UILogListWnd.Enable(false);


	AttachChild(&UIBackButton);
	UIBackButton.Init("ui\\ui_button_02", 10,height - 50, 150, 40);
	UIBackButton.SetText("<< Back");

	
	AttachChild(&UIMsgButton1);
	UIMsgButton1.Init("ui\\ui_button_02", 10, 30 , 150, 40);
	UIMsgButton1.SetText("Yes");

	AttachChild(&UIMsgButton2);
	UIMsgButton2.Init("ui\\ui_button_02", 10, 90, 150, 40);
	UIMsgButton2.SetText("No");

	AttachChild(&UIMsgButton3);
	UIMsgButton3.Init("ui\\ui_button_02", 10, 150, 150, 40);
	UIMsgButton3.SetText("Get Lost!");

	AttachChild(&UIStaticContactName);
	UIStaticContactName.Init(10,10, 150, 40);

	AttachChild(&UIStaticContactStatus);
	UIStaticContactStatus.Init(10,60, 150, 40);
	UIStaticContactStatus.SetText("The contact has been lost.");
	UIStaticContactStatus.Show(false);

	
	
	inherited::Init(x, y, width, height);
}

void CUIPdaDialogWnd::SendMessage(CUIWindow *pWnd, s16 msg, void *pData)
{
	if(pWnd == &UIBackButton)
	{
			if(msg == CUIButton::BUTTON_CLICKED)
			{
				GetTop()->SendMessage(this, BACK_BUTTON_CLICKED);
			}
	}
	else if(pWnd == &UIMsgButton1)
	{
			if(msg == CUIButton::BUTTON_CLICKED)
			{
				m_iMsgNum = 0;
				GetTop()->SendMessage(this, MESSAGE_BUTTON_CLICKED);
			}
	}
	else if(pWnd == &UIMsgButton2)
	{
			if(msg == CUIButton::BUTTON_CLICKED)
			{
				m_iMsgNum = 1;
				GetTop()->SendMessage(this, MESSAGE_BUTTON_CLICKED);
			}
	}
	else if(pWnd == &UIMsgButton3)
	{
			if(msg == CUIButton::BUTTON_CLICKED)
			{
				m_iMsgNum = 2;
				GetTop()->SendMessage(this, MESSAGE_BUTTON_CLICKED);
			}
	}


	inherited::SendMessage(pWnd, msg, pData);
}


void CUIPdaDialogWnd::Show()
{
	inherited::Show(true);
	inherited::Enable(true);
	UIBackButton.Reset();
}
void CUIPdaDialogWnd::Hide()
{
	inherited::Show(false);
	inherited::Enable(false);
}


void CUIPdaDialogWnd::ContactLoss()
{
	UIStaticContactStatus.SetText("The contact has been lost.");
	UIStaticContactStatus.Show(true);
	
	UIMsgButton1.Show(false);
	UIMsgButton1.Enable(false);
	UIMsgButton2.Show(false);
	UIMsgButton2.Enable(false);
	UIMsgButton3.Show(false);
	UIMsgButton3.Enable(false);


}

void CUIPdaDialogWnd::ContactRestore()
{
	UIStaticContactStatus.Show(false);
	
	UIMsgButton1.Show(true);
	UIMsgButton1.Enable(true);
	UIMsgButton2.Show(true);
	UIMsgButton2.Enable(true);
	UIMsgButton3.Show(true);
	UIMsgButton3.Enable(true);

	UIMsgButton1.Reset();
	UIMsgButton2.Reset();
	UIMsgButton3.Reset();

}

void CUIPdaDialogWnd::ContactWaitForReply()
{
	UIStaticContactStatus.SetText("Waiting for reply.");
	UIStaticContactStatus.Show(true);
	
	UIMsgButton1.Show(false);
	UIMsgButton1.Enable(false);
	UIMsgButton2.Show(false);
	UIMsgButton2.Enable(false);
	UIMsgButton3.Show(false);
	UIMsgButton3.Enable(false);


}
