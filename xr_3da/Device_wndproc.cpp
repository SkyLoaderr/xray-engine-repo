#include "stdafx.h"

//-----------------------------------------------------------------------------
// Name: WndProc()
// Desc: Static msg handler which passes messages to the application class.
//-----------------------------------------------------------------------------
LRESULT CALLBACK WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch(uMsg)
	{
	case WM_ACTIVATE:
		{
			u16 fActive	= LOWORD(wParam);
			BOOL fMinimized = (BOOL) HIWORD(wParam);
			Device.bActive	= (fActive!=WA_INACTIVE) && (!fMinimized);
			Log("Device.bActive=",Device.bActive?"true":"false");
			if (Device.bActive)	{
				Device.seqAppActivate.Process	(rp_AppActivate);
				if (!strstr(Core.Params, "-dedicated")) 
					ShowCursor	(FALSE);
			} else	{
				Device.seqAppDeactivate.Process(rp_AppDeactivate);
				ShowCursor	(TRUE);
			}
		}
		return 0;
	case WM_SETCURSOR:
		return 1;
	case WM_SYSCOMMAND:
		// Prevent moving/sizing and power loss in fullscreen mode
		switch( wParam ){
	case SC_MOVE:
	case SC_SIZE:
	case SC_MAXIMIZE:
	case SC_MONITORPOWER:
		return 1;
		break;
		}
		break;
	case WM_CLOSE:
		return 0;
	case WM_KEYDOWN:
		break;
	default:
		break;
	}
	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}
