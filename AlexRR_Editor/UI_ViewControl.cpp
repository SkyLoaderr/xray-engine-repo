//----------------------------------------------------
// file: UI_ViewControl.cpp
//----------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "NetDeviceLog.h"
#include "UI_ViewControl.h"

//----------------------------------------------------


static LRESULT CALLBACK VPControlWndProc( HWND hw, UINT msg, WPARAM wp, LPARAM lp){
	
	RECT rc0;
	POINT ptCurrent;
	UI_ViewControl *_Ctl;

	_Ctl = (UI_ViewControl *)GetWindowLong(hw,GWL_USERDATA);
	_ASSERTE( _Ctl );

	switch( msg ){

	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		if( !_Ctl->m_Captured ){

			GetCursorPos( &_Ctl->m_SaveCursor );

			GetWindowRect(0,&rc0);
			_Ctl->m_MoveStart.x = GetSystemMetrics(SM_CXSCREEN)/2;
			_Ctl->m_MoveStart.y = GetSystemMetrics(SM_CYSCREEN)/2;
			
			_Ctl->m_RelMove.x = 0;
			_Ctl->m_RelMove.y = 0;

			_Ctl->m_Alternate = (msg==WM_RBUTTONDOWN);
			_Ctl->m_Accelerated = !(0x80&GetKeyState(VK_SHIFT));
			_Ctl->m_Captured = true;
			_Ctl->MouseStartMove();

			ShowCursor(FALSE);
			SetCursorPos(_Ctl->m_MoveStart.x,_Ctl->m_MoveStart.y);
			SendMessage(hw,BM_SETSTATE,1,0);
			SetCapture( hw );
		}
		break;

	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		if( _Ctl->m_Captured ){
			ReleaseCapture();
			SetCursorPos(_Ctl->m_SaveCursor.x,_Ctl->m_SaveCursor.y);
			ShowCursor(TRUE);
			SendMessage(hw,BM_SETSTATE,0,0);
			_Ctl->MouseEndMove();
			_Ctl->m_Captured = false;
		}
		break;

	case WM_MOUSEMOVE:
		if( _Ctl->m_Captured ){
			GetCursorPos( &ptCurrent );
			ptCurrent.x -= _Ctl->m_MoveStart.x;
			ptCurrent.y -= _Ctl->m_MoveStart.y;
			if( ptCurrent.x || ptCurrent.y ){

				_Ctl->m_RelMove.x = ptCurrent.x;
				_Ctl->m_RelMove.y = ptCurrent.y;
				SetCursorPos(_Ctl->m_MoveStart.x,_Ctl->m_MoveStart.y);
				
				_Ctl->MouseMove();
			}
		}
		break;

	default:
		_ASSERTE( _Ctl->m_OriginalWndProc );
		return _Ctl->m_OriginalWndProc( hw, msg, wp, lp );
	}

	return 0l;
}


//----------------------------------------------------


UI_ViewControl::UI_ViewControl(){
	m_Window = 0;
	m_OriginalWndProc = 0;
}

UI_ViewControl::~UI_ViewControl(){
	_ASSERTE( m_Window==0 );
}

void UI_ViewControl::Create( HWND _Window ){
	m_Accelerated = false;
	m_Alternate = false;
	m_Captured = false;
	m_Window = _Window;
	m_OriginalWndProc = (WNDPROC)GetWindowLong(m_Window,GWL_WNDPROC);
	SetWindowLong(m_Window,GWL_USERDATA,(LONG)this);
	SetWindowLong(m_Window,GWL_WNDPROC,(LONG)VPControlWndProc);
	NConsole.print( "UI_ViewControl: 0x%08X subclassed", m_Window );
}

void UI_ViewControl::Restore(){
	_ASSERTE( m_Window );
	SetWindowLong(m_Window,GWL_WNDPROC,(LONG)m_OriginalWndProc);
	SetWindowLong(m_Window,GWL_USERDATA,0);
	m_Window = 0;
}

//----------------------------------------------------

