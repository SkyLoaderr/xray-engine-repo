#include "stdafx.h"
#include "HUDmanager.h"
#include "..\xr_level_controller.h"
#include "..\xr_ioconsole.h"

// Обработка нажатия клавиш
ENGINE_API extern BOOL	ShowLM;
void CLevel::OnKeyboardPress(int key)
{
//	if (pHUD->IsUIActive())			
	if (pHUD->GetUI()->OnKeyboardPress(key)) return;

	switch (key) 
	{
	case DIK_RETURN:
		bDebug	= !bDebug;
		return;
	case DIK_F12:
		Render->Screenshot			();
		return;
	case DIK_RALT:
		ShowLM	= TRUE;
		return;
	case DIK_BACK:
		HW.Caps.SceneMode			= (HW.Caps.SceneMode+1)%3;
		return;
	case DIK_F6:
		net_Save					("quick.save");
		return;
	case DIK_F9:
		// SLS_Load					("quick.save");
		return;
	}

	switch (key_binding[key]) {
	case kCONSOLE:
		Console.Show				();
		break;
	case kQUIT:	
		Console.Execute				("quit");
		break;
	default:
		if (CurrentEntity())		CurrentEntity()->OnKeyboardPress(key_binding[key]);
		break;
	}
}

void CLevel::OnKeyboardRelease(int key)
{
//	if (pHUD->IsUIActive()) 
	if (pHUD->GetUI()->OnKeyboardRelease(key)) return;

	switch (key)
	{
	case DIK_RALT:
		ShowLM	= FALSE;
		return;
	}
	if (CurrentEntity())	CurrentEntity()->OnKeyboardRelease(key_binding[key]);
}

void CLevel::OnKeyboardHold(int key)
{
	if (CurrentEntity())	CurrentEntity()->OnKeyboardHold(key_binding[key]);
}

static int mouse_button_2_key []={MOUSE_1,MOUSE_2,MOUSE_3};
void CLevel::OnMousePress(int btn)
{	OnKeyboardPress(mouse_button_2_key[btn]);}
void CLevel::OnMouseRelease(int btn)
{	OnKeyboardRelease(mouse_button_2_key[btn]);}
void CLevel::OnMouseHold(int btn)
{	OnKeyboardHold(mouse_button_2_key[btn]);}

void CLevel::OnMouseMove( int dx, int dy )
{
	if (CurrentEntity())	CurrentEntity()->OnMouseMove(dx,dy);
}

void CLevel::OnMouseStop( int axis, int value)
{
}

