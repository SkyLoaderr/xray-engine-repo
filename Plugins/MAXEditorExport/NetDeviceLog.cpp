//----------------------------------------------------
// file: NetDeviceLog.cpp
//----------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "NetDeviceLog.h"
#include "MeshExpUtility.h"
#include "MeshExpUtility.rh"

//----------------------------------------------------

NetLog NLog( "objectexp.log" );
NetDeviceConsole NConsole;

//----------------------------------------------------

NetLog::NetLog( char *_FileName ){
	VERIFY( _FileName );
	strcpy( m_FileName, _FileName );
}

void NetLog::Msg( char *_Format, ... ){
	
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
	
	std::list<NetDeviceConsole::_ConsoleMsg>::iterator _F;
	std::list<NetDeviceConsole::_ConsoleMsg>::iterator _E;

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
			SendDlgItemMessage( hw, IDC_PROGRESS, PBM_SETRANGE,	0, MAKELPARAM(0, 100) );
			SendDlgItemMessage( hw, IDC_PROGRESS, PBM_SETPOS,		0, 0);
		}
		LeaveCriticalSection( &NConsole.m_CSection );
		break;
/*
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
*/
	case WM_CLOSE:
		SendMessage(U.hPanel,WM_COMMAND,IDC_CLOSE,0);
//		U.iu->CloseUtility();
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

void NetDeviceConsole::print(const char *_Format, ...){

	VERIFY( _Format );

	_ConsoleMsg buf;
	va_list l; 
	va_start( l, _Format );
	vsprintf( buf.buf, _Format, l );

	if( m_Messages.size() > 1000 ) m_Messages.pop_front();
	m_Messages.push_back( buf );

	if( !m_Valid )
		return;

	EnterCriticalSection( &m_CSection );
	if( SendDlgItemMessage( m_hWindow, IDC_MESSAGES, LB_GETCOUNT, 0, 0 ) > 1000 )
		SendDlgItemMessage( m_hWindow, IDC_MESSAGES,LB_DELETESTRING, 0, 0 );
	int k = SendDlgItemMessage( m_hWindow, IDC_MESSAGES,LB_ADDSTRING, 0, (LPARAM)buf.buf );
	SendDlgItemMessage( m_hWindow, IDC_MESSAGES,LB_SETCURSEL, k, 0 );
	DWORD dwCnt = SendMessage	( m_hWindow, LB_GETCOUNT, 0, 0);
	SendMessage	( m_hWindow, LB_SETTOPINDEX, dwCnt-1, 0);
	LeaveCriticalSection( &m_CSection );

	NLog.Msg(buf.buf);
}
/*
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
*/
bool NetDeviceConsole::valid(){
	return m_Valid;
}

NetDeviceConsole::NetDeviceConsole(){
	VERIFY( this == &NConsole );
	m_Valid = false;
}

NetDeviceConsole::~NetDeviceConsole(){
	VERIFY( m_Valid == false );
	m_Messages.clear();
}

//----------------------------------------------------
void NetDeviceConsole::ProgressStart(float max_val, const char* text){
	fMaxVal=max_val;
	fStatusProgress=0;
	Msg(text?text:"");
	ProgressUpdate(0);
}
void NetDeviceConsole::ProgressEnd(){
	ProgressUpdate(0);
}
void NetDeviceConsole::ProgressInc(){
	ProgressUpdate(fStatusProgress+1);
}
void NetDeviceConsole::ProgressUpdate(float val){
	if (fabsf(val-fStatusProgress)<1) return;
	fStatusProgress=val;
	EnterCriticalSection( &m_CSection );
    if (fMaxVal>0){
		DWORD progress = (DWORD)((fStatusProgress/fMaxVal)*100);
		SendDlgItemMessage( m_hWindow, IDC_PROGRESS,PBM_SETPOS, progress, 0 );
	}else{
		DWORD progress = 0;
		SendDlgItemMessage( m_hWindow, IDC_PROGRESS,PBM_SETPOS, progress, 0 );
	}
	LeaveCriticalSection( &m_CSection );
}
//----------------------------------------------------


void Msg(const char *msg, ... ){
	char buf[4096];
	va_list l; 
	va_start( l, msg );
	vsprintf( buf, msg, l );
	NConsole.print(buf);
}

void Log(const char *msg) {
	NConsole.print(msg);
}

void Log(const char *msg, const char *dop) {
	char buf[1024];
	if (dop)	sprintf(buf,"%s %s",msg,dop);
	else		sprintf(buf,"%s",msg);
	NConsole.print(buf);
}

void Log(const char *msg, DWORD dop) {
	char buf[1024];
	sprintf(buf,"%s %d",msg,dop);
	NConsole.print(buf);
}

void Log(const char *msg, int dop) {
	char buf[1024];
	sprintf(buf,"%s %d",msg,dop);
	NConsole.print(buf);
}

void Log(const char *msg, float dop) {
	char buf[1024];
	sprintf(buf,"%s %f",msg,dop);
	NConsole.print(buf);
}

void Log(const char *msg, const Fvector &dop) {
	char buf[1024];
	sprintf(buf,"%s (%f,%f,%f)",msg,dop.x,dop.y,dop.z);
	NConsole.print(buf);
}
