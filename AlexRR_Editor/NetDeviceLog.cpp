//----------------------------------------------------
// file: NetDeviceLog.cpp
//----------------------------------------------------

#include "Pch.h"
//#include "stdafx.h"
#pragma hdrstop

#include "NetDeviceLog.h"
#include "MenuTemplate.rh"

//----------------------------------------------------

NetLog NLog( "netlog.log" );
NetDeviceConsole NConsole;

//----------------------------------------------------

NetLog::NetLog( char *_FileName ){
	_ASSERTE( _FileName );
	strcpy( m_FileName, _FileName );
}

void __cdecl NetLog::Msg( char *_Format, ... ){
	
	char buf[4096];
	va_list l; 
	va_start( l, _Format );
	vsprintf( buf, _Format, l );

#ifdef NLOG_CONSOLE_OUT
	printf( "\n%s", buf );
#endif

	strcat( buf, "\r\n" );

	int hf = _open( m_FileName, _O_WRONLY|_O_APPEND|_O_BINARY );
	if( hf<=0 )
		hf = _open( m_FileName,
			_O_WRONLY|_O_CREAT|_O_TRUNC| _O_BINARY,
			_S_IREAD | _S_IWRITE );

	_write( hf, buf, strlen(buf) );
	_close( hf );

}

//----------------------------------------------------

BOOL CALLBACK ConsoleDialogProc( HWND hw, UINT msg, WPARAM wp, LPARAM lp){
	
	list<NetDeviceConsole::_ConsoleMsg>::iterator _F;
	list<NetDeviceConsole::_ConsoleMsg>::iterator _E;

	switch( msg ){

	case WM_INITDIALOG:
		SetWindowPos(hw,HWND_TOPMOST,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);
		NConsole.m_hWindow = hw;
		EnterCriticalSection( &NConsole.m_CSection );
		if( !NConsole.m_Messages.empty() ){
			_F = NConsole.m_Messages.begin();
			_E = NConsole.m_Messages.end();
			for(;_F!=_E;_F++){
				int k = SendDlgItemMessage( hw, IDC_MESSAGES, LB_ADDSTRING, 0, (LPARAM)_F->buf );
				SendDlgItemMessage( hw, IDC_MESSAGES, LB_SETCURSEL, k, 0 );
			}
		}
		LeaveCriticalSection( &NConsole.m_CSection );
		break;

	case WM_COMMAND:
		switch( LOWORD(wp) ){
		case IDOK:
			EnterCriticalSection( &NConsole.m_CSection );
			GetDlgItemText( hw, IDC_COMMAND, NConsole.m_EnterBuffer, sizeof(NConsole.m_EnterBuffer) );
			NConsole.m_Enter = true;
			LeaveCriticalSection( &NConsole.m_CSection );
			break;
		}
		break;

	default:
		return FALSE;
	}
	return TRUE;
}

DWORD WINAPI ConsoleThreadProc( LPVOID ){
	DialogBox( NConsole.m_hInstance,
		MAKEINTRESOURCE(IDD_CONSOLE),
		0, ConsoleDialogProc );
	return 0;
}

bool NetDeviceConsole::Init( HINSTANCE _Inst, HWND _Window ){
	
	if( m_Valid ){
		SetForegroundWindow(m_hWindow);
		return true;}

	m_Enter = false;
	m_hInstance = _Inst;
	m_hParent = _Window;
	InitializeCriticalSection( &m_CSection );
	m_hThread = CreateThread( 0, 0, ConsoleThreadProc, 0, 0, &m_ThreadId );
	m_Valid = true;
	
	return true;
}

void NetDeviceConsole::Clear(){
	
	if( !m_Valid )
		return;

	m_Valid = false;
	TerminateThread( m_hThread, 0 );
	CloseHandle( m_hThread );
	DeleteCriticalSection( &m_CSection );
}

void __cdecl NetDeviceConsole::print( char *_Format, ... ){

	_ASSERTE( _Format );

	_ConsoleMsg buf;
	va_list l; 
	va_start( l, _Format );
	vsprintf( buf.buf, _Format, l );

	if( m_Messages.size() > 300 ) m_Messages.pop_front();
	m_Messages.push_back( buf );

	if( !m_Valid )
		return;

	EnterCriticalSection( &m_CSection );
	if( SendDlgItemMessage( m_hWindow, IDC_MESSAGES, LB_GETCOUNT, 0, 0 ) > 300 )
		SendDlgItemMessage( m_hWindow, IDC_MESSAGES,LB_DELETESTRING, 0, 0 );
	int k = SendDlgItemMessage( m_hWindow, IDC_MESSAGES,LB_ADDSTRING, 0, (LPARAM)buf.buf );
	SendDlgItemMessage( m_hWindow, IDC_MESSAGES,LB_SETCURSEL, k, 0 );
	LeaveCriticalSection( &m_CSection );
}

bool NetDeviceConsole::in( char *_Buffer ){

	if( !m_Valid )
		return false;

	bool result = false;

	bool ok = false;
	while( !ok ) {
		EnterCriticalSection( &m_CSection );
		ok = m_Enter;
		LeaveCriticalSection( &m_CSection );
		Sleep( 1 ); }

	EnterCriticalSection( &m_CSection );
	if( m_Enter ){
		m_Enter = false;
		SetDlgItemText( m_hWindow, IDC_COMMAND, "" );
		strcpy( _Buffer, m_EnterBuffer );
		result = true; }
	LeaveCriticalSection( &m_CSection );

	return result;
}

bool NetDeviceConsole::cmdtest( char *_Buffer ){

	if( !m_Valid )
		return false;

	bool result = false;

	EnterCriticalSection( &m_CSection );
	if( m_Enter ){
		m_Enter = false;
		SetDlgItemText( m_hWindow, IDC_COMMAND, "" );
		strcpy( _Buffer, m_EnterBuffer );
		result = true; }
	LeaveCriticalSection( &m_CSection );

	return result;
}

bool NetDeviceConsole::valid(){
	return m_Valid;
}

NetDeviceConsole::NetDeviceConsole(){
	_ASSERTE( this == &NConsole );
	m_Valid = false;
}

NetDeviceConsole::~NetDeviceConsole(){
	_ASSERTE( m_Valid == false );
	m_Messages.clear();
}

//----------------------------------------------------
