// UIDialogWnd.cpp: класс простого диалога, нужен для стандартного запуска
// разным менюшек путем вызова виртуальных Show() И Hide()
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "uidialogwnd.h"

#include "..\\hudmanager.h"
#include "..\\..\\xr_ioconsole.h"


CUIDialogWnd:: CUIDialogWnd()
{
}

CUIDialogWnd::~ CUIDialogWnd()
{
}

void CUIDialogWnd::Show()
{
	HUD().GetUI()->HideIndicators();
	HUD().GetUI()->ShowCursor();

	inherited::Enable(true);
	inherited::Show(true);

	m_bCrosshair = true;
	if(m_bCrosshair) Console.Execute("hud_crosshair 0");
}


void CUIDialogWnd::Hide()
{
	HUD().GetUI()->ShowIndicators();
	HUD().GetUI()->HideCursor();

	inherited::Enable(false);
	inherited::Show(false);
	
	if(m_bCrosshair) Console.Execute("hud_crosshair 1");
}


bool CUIDialogWnd::IR_OnKeyboardPress(int dik)
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
	else if(dik==MOUSE_2)
	{
		OnMouse(HUD().GetUI()->GetCursor()->GetPos().x,
				HUD().GetUI()->GetCursor()->GetPos().y,
                CUIWindow::RBUTTON_DOWN);
		return true;
	}

	
	return 	OnKeyboard(dik,	CUIWindow::KEY_PRESSED);
}
bool CUIDialogWnd::IR_OnKeyboardRelease(int dik)
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
	else if(dik==MOUSE_2)
	{
			OnMouse(HUD().GetUI()->GetCursor()->GetPos().x,
			 	    HUD().GetUI()->GetCursor()->GetPos().y,	
					CUIWindow::RBUTTON_UP);
		return true;
	}

	return 	OnKeyboard(dik,	CUIWindow::KEY_RELEASED);
}
bool CUIDialogWnd::IR_OnMouseMove(int dx, int dy)
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