// UIDialogWnd.cpp: ����� �������� �������, ����� ��� ������������ �������
// ������ ������� ����� ������ ����������� Show() � Hide()
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "uidialogwnd.h"
#include "../hudmanager.h"
#include "../xr_level_controller.h"
#include "../../xr_ioconsole.h"
#include "../level.h"


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

//	m_bCrosshair = !!psHUD_Flags.test(HUD_CROSSHAIR);
//	if(m_bCrosshair) 
//		psHUD_Flags.set(HUD_CROSSHAIR, FALSE);


	ResetAll();
}


void CUIDialogWnd::Hide()
{

	inherited::Enable(false);
	inherited::Show(false);
	
//	if(m_bCrosshair) 
//		psHUD_Flags.set(HUD_CROSSHAIR, TRUE);
}


bool CUIDialogWnd::IR_OnKeyboardPress(int dik)
{
	if(!IsEnabled()) return false;
			
	//mouse click
	if(dik==MOUSE_1)
	{
		OnMouse(static_cast<int>(GetUICursor()->GetPos().x / UI()->GetScale()),
				static_cast<int>(GetUICursor()->GetPos().y / UI()->GetScale()),
                WINDOW_LBUTTON_DOWN);
		return true;
	}
	else if(dik==MOUSE_2)
	{
		OnMouse(static_cast<int>(GetUICursor()->GetPos().x / UI()->GetScale()),
				static_cast<int>(GetUICursor()->GetPos().y / UI()->GetScale()),
                WINDOW_RBUTTON_DOWN);
		return true;
	}

	
	return 	OnKeyboard(dik,	WINDOW_KEY_PRESSED);
}
bool CUIDialogWnd::IR_OnKeyboardRelease(int dik)
{
	if(!IsEnabled()) return false;
			
	//mouse click
	if(dik==MOUSE_1)
	{
			OnMouse(static_cast<int>(GetUICursor()->GetPos().x / UI()->GetScale()),
			 	    static_cast<int>(GetUICursor()->GetPos().y / UI()->GetScale()),	
					WINDOW_LBUTTON_UP);
		return true;
	}
	else if(dik==MOUSE_2)
	{
			OnMouse(static_cast<int>(GetUICursor()->GetPos().x / UI()->GetScale()),
			 	    static_cast<int>(GetUICursor()->GetPos().y / UI()->GetScale()),	
					WINDOW_RBUTTON_UP);
		return true;
	}

	return 	OnKeyboard(dik,	WINDOW_KEY_RELEASED);
}
bool CUIDialogWnd::IR_OnMouseMove(int dx, int dy)
{
	if(!IsEnabled()) return false;
	
	if (GetUICursor()->IsVisible())
	{ 
		GetUICursor()->MoveBy(dx, dy);
		OnMouse(static_cast<int>(GetUICursor()->GetPos().x / UI()->GetScale()), 
				static_cast<int>(GetUICursor()->GetPos().y / UI()->GetScale()), 
				WINDOW_MOUSE_MOVE);
	}

	return true;
}

