// CUIMapWnd.h:  диалог итерактивной карты
// 
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UIMapWnd.h"

#include "xrXMLParser.h"
#include "UIXmlInit.h"

#include "..\\HUDManager.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUIMapWnd::CUIMapWnd()
{
	Init();

	Show(false);
	Enable(false);
	
	SetFont(HUD().pFontHeaderRussian);
}

CUIMapWnd::~CUIMapWnd()
{
}

void CUIMapWnd::Init()
{
	inherited::Init(0,0, Device.dwWidth, Device.dwHeight);

	AttachChild(&UIMainMapFrame);
	UIMainMapFrame.Init("ui\\ui_frame", 100,100, 600, 400);

/*	UIMainPdaFrame.AttachChild(&UIPdaDialogWnd);
	UIPdaDialogWnd.Init(10,10,550,350);
	UIPdaDialogWnd.Hide();*/
}

void CUIMapWnd::InitMap()
{
}


void CUIMapWnd::SendMessage(CUIWindow* pWnd, s16 msg, void* pData)
{
	inherited::SendMessage(pWnd, msg, pData);
}

bool CUIMapWnd::IR_OnKeyboardPress(int dik)
{
	if(!IsEnabled()) return false;
			
	//mouse click
	if(dik==MOUSE_1)
	{
		OnMouse(HUD().GetUI()->GetCursor()->GetPos().x,
				HUD().GetUI()->GetCursor()->GetPos().y,
                CUIWindow::LBUTTON_DOWN);
		return true;
	}
	
	
	return 	OnKeyboard(dik,	CUIWindow::KEY_PRESSED);
}

bool CUIMapWnd::IR_OnKeyboardRelease(int dik)
{
	if(!IsEnabled()) return false;
			
	//mouse click
	if(dik==MOUSE_1)
	{
			OnMouse(HUD().GetUI()->GetCursor()->GetPos().x,
			 	    HUD().GetUI()->GetCursor()->GetPos().y,	
					CUIWindow::LBUTTON_UP);
		return true;
	}

	return 	OnKeyboard(dik,	CUIWindow::KEY_RELEASED);
}

bool CUIMapWnd::IR_OnMouseMove(int dx, int dy)
{
	if(!IsEnabled()) return false;
	
	if (HUD().GetUI()->GetCursor()->IsVisible())
	{ 
		HUD().GetUI()->GetCursor()->MoveBy(dx, dy);
		OnMouse(HUD().GetUI()->GetCursor()->GetPos().x, 
				HUD().GetUI()->GetCursor()->GetPos().y, 
				CUIWindow::MOUSE_MOVE);
	}

	return true;
}

void CUIMapWnd::Update()
{
	inherited::Update();
}

void CUIMapWnd::Draw()
{
	inherited::Draw();
}