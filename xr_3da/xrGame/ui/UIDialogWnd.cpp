// UIDialogWnd.cpp: ����� �������� �������, ����� ��� ������������ �������
// ������ ������� ����� ������ ����������� Show() � Hide()
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "uidialogwnd.h"
#include "../hudmanager.h"
#include "../xr_level_controller.h"
#include "../../xr_ioconsole.h"
#include "../level.h"
#include "../GameObject.h"

CUIDialogWnd:: CUIDialogWnd()
{
	m_pHolder = NULL;
	work_phase = 0;
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

bool CUIDialogWnd::IR_OnKeyboardHold(int dik)
{
	if(!IR_process()) return false;
	if (OnKeyboardHold(dik)) 
		return true;

	if( !StopAnyMove() && g_pGameLevel ){
		CObject* O = Level().CurrentEntity();
		if( O ){
			IInputReceiver*		IR	= smart_cast<IInputReceiver*>( smart_cast<CGameObject*>(O) );
			if (!IR)
				return			(false);
			IR->IR_OnKeyboardHold(key_binding[dik]);
		}
	}
	return false;
}

bool CUIDialogWnd::IR_OnKeyboardPress(int dik)
{
	if(!IR_process()) return false;
	//mouse click
	if(dik==MOUSE_1 || dik==MOUSE_2)
	{
		Fvector2 cp = GetUICursor()->GetPos();
		if (OnMouse(cp.x,cp.y, dik==MOUSE_1? WINDOW_LBUTTON_DOWN : WINDOW_RBUTTON_DOWN))
            return true;
	}

	if (OnKeyboard(dik,	WINDOW_KEY_PRESSED))
		return true;

	if( !StopAnyMove() && g_pGameLevel ){
		CObject* O = Level().CurrentEntity();
		if( O ){
			IInputReceiver*		IR	= smart_cast<IInputReceiver*>( smart_cast<CGameObject*>(O) );
			if (!IR)
				return			(false);
			IR->IR_OnKeyboardPress(key_binding[dik]);
		}
	}
	return false;
}

bool CUIDialogWnd::IR_OnKeyboardRelease(int dik)
{
	if(!IR_process()) return false;
	
	//mouse click
	if(dik==MOUSE_1 || dik==MOUSE_2)
	{
		Fvector2 cp = GetUICursor()->GetPos();
		if (OnMouse(cp.x, cp.y, dik==MOUSE_1 ? WINDOW_LBUTTON_UP : WINDOW_RBUTTON_UP))
            return true;
	}

	if (OnKeyboard(dik,	WINDOW_KEY_RELEASED))
		return true;

	if( !StopAnyMove() && g_pGameLevel ){
		CObject* O = Level().CurrentEntity();
		if( O ){
			IInputReceiver*		IR	= smart_cast<IInputReceiver*>( smart_cast<CGameObject*>(O) );
			if (!IR)
				return			(false);
			IR->IR_OnKeyboardRelease(key_binding[dik]);
		}
	}
	return false;
}

bool CUIDialogWnd::IR_OnMouseWheel (int direction)
{
	if(!IR_process()) return false;
	Fvector2 pos = GetUICursor()->GetPos();

	if(direction>0)
		OnMouse(pos.x,pos.y,WINDOW_MOUSE_WHEEL_UP);
	else
		OnMouse(pos.x,pos.y,WINDOW_MOUSE_WHEEL_DOWN);

	return true;
}

bool CUIDialogWnd::IR_OnMouseMove(int dx, int dy)
{
	if(!IR_process()) return false;
	
	if (GetUICursor()->IsVisible())
	{ 
		GetUICursor()->MoveBy(float(dx), float(dy));
		Fvector2 cPos = GetUICursor()->GetPos();
		OnMouse(cPos.x, cPos.y , WINDOW_MOUSE_MOVE);
	}
	else if( !StopAnyMove() && g_pGameLevel ){
		CObject* O = Level().CurrentEntity();
		if( O ){
			IInputReceiver*		IR	= smart_cast<IInputReceiver*>( smart_cast<CGameObject*>(O) );
			if (!IR)
				return			(false);

			IR->IR_OnMouseMove(dx,dy);
		}
	};

	return true;
}

bool CUIDialogWnd::OnKeyboardHold(int dik)
{
	if(!IR_process()) return false;
	return inherited::OnKeyboardHold(dik);
}

bool CUIDialogWnd::OnKeyboard(int dik, EUIMessages keyboard_action)
{
	if(!IR_process()) return false;
	if (inherited::OnKeyboard(dik, keyboard_action) )
		return true;
	return false;
}

bool CUIDialogWnd::IR_process()
{
	if(!IsEnabled())					return false;
	if(UI()->IsActive())				return true;
	if(Device.Pause()&&!WorkInPause())	return false;
	return true;
}

void CUIDialogWnd::Update(){
	CUIWindow::Update();
	if (!CheckPhase()){
		UI()->StartStopMenu(this, true);
	}
}

void CUIDialogWnd::SetWorkPhase(u32 phase){
	work_phase = phase;
}

#include "../game_cl_base.h"
bool CUIDialogWnd::CheckPhase(){
	if (0 == work_phase)
		return true;
	else 
		return Game().Phase() == work_phase;
}
