// UISleepWnd.cpp:  окошко для выбора того, сколько спать
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UISleepWnd.h"

CUISleepWnd::CUISleepWnd()
{
}

CUISleepWnd::~CUISleepWnd()
{
}

void CUISleepWnd::Init(int x, int y, int width, int height)
{

	inherited::Init("ui\\ui_frame", x, y, width, height);


	AttachChild(&UIPerformButton);
	UIPerformButton.Init("ui\\ui_button_01", 10, 250,150,40);
	UIPerformButton.SetText("perform");

	AttachChild(&UICloseButton);
	UICloseButton.Init("ui\\ui_button_01", 10, 300,150,40);
	UICloseButton.SetText("close");
}

void CUISleepWnd::InitSleepWnd()
{
}

void CUISleepWnd::SendMessage(CUIWindow *pWnd, s16 msg, void *pData)
{
	if(pWnd == &UIPerformButton && msg == CUIButton::BUTTON_CLICKED)
	{
		GetMessageTarget()->SendMessage(this, PERFORM_BUTTON_CLICKED);
	}
	else if(pWnd == &UICloseButton && msg == CUIButton::BUTTON_CLICKED)
	{
		GetMessageTarget()->SendMessage(this, CLOSE_BUTTON_CLICKED);
	}

	inherited::SendMessage(pWnd, msg, pData);
}


void CUISleepWnd::Show()
{
	ResetAll();
	inherited::Show(true);
	inherited::Enable(true);
}
void CUISleepWnd::Hide()
{
	inherited::Show(false);
	inherited::Enable(false);
}

void CUISleepWnd::Update()
{
	inherited::Update();
}