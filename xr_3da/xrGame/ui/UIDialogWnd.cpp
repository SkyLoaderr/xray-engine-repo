// UIDialogWnd.cpp: класс простого диалога, нужен для стандартного запуска
// разным менюшек путем вызова виртуальных Show() И Hide()
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "uidialogwnd.h"
#include "../hudmanager.h"
#include "../xr_level_controller.h"
#include "../../xr_ioconsole.h"

CUIDialogWnd:: CUIDialogWnd()
{
	m_pHolder = NULL;
	Hide();
}

CUIDialogWnd::~ CUIDialogWnd()
{
}

void CUIDialogWnd::Show()
{
	inherited::Enable(true);
	inherited::Show(true);


	ResetAll();
}


void CUIDialogWnd::Hide()
{

	inherited::Enable(false);
	inherited::Show(false);
	
}


bool CUIDialogWnd::IR_OnKeyboardPress(int dik)
{
	if(!IsEnabled()) return false;
			
	//mouse click
	if(dik==MOUSE_1)
	{
		OnMouse(static_cast<int>(GetUICursor()->GetPos().x /* / UI()->GetScaleX() */),
				static_cast<int>(GetUICursor()->GetPos().y /* / UI()->GetScaleY() */),
                WINDOW_LBUTTON_DOWN);
		return true;
	}
	else if(dik==MOUSE_2)
	{
		OnMouse(static_cast<int>(GetUICursor()->GetPos().x /* / UI()->GetScaleX() */),
				static_cast<int>(GetUICursor()->GetPos().y /* / UI()->GetScaleY() */),
                WINDOW_RBUTTON_DOWN);
		return true;
	}

	
	return 	OnKeyboard(dik,	WINDOW_KEY_PRESSED);
}

bool CUIDialogWnd::IR_OnKeyboardRelease(int dik)
{
	if(!IsEnabled()) return false;
	

	//mouse click
	if(dik==MOUSE_1 || dik==MOUSE_2)
	{
		int x = static_cast<int>(GetUICursor()->GetPos().x /* / UI()->GetScaleX() */);
		int y = static_cast<int>(GetUICursor()->GetPos().y /* / UI()->GetScaleY() */);
		
		if(dik==MOUSE_1)
			OnMouse(x,   y,	WINDOW_LBUTTON_UP);
		else
			OnMouse(x,   y,	WINDOW_RBUTTON_UP);

		return true;
	}

	return 	OnKeyboard(dik,	WINDOW_KEY_RELEASED);
}

bool CUIDialogWnd::IR_OnMouseWheel (int direction)
{
	OnMouseWheel(direction);
	return true;
}

bool CUIDialogWnd::IR_OnMouseMove(int dx, int dy)
{
	if(!IsEnabled()) return false;
	
	if (GetUICursor()->IsVisible())
	{ 
		GetUICursor()->MoveBy(dx, dy);
		Ivector2 cPos = GetUICursor()->GetPos();
		OnMouse(cPos.x, cPos.y , WINDOW_MOUSE_MOVE);
	}

	return true;
}

