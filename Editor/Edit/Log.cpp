//----------------------------------------------------
// file: NetDeviceLog->cpp
//----------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "Log.h"
#include "LogForm.h"
#ifdef _EDITOR
	#include "splash.h"
	#include "ui_main.h"
#endif
//----------------------------------------------------

CLog* Log;// ( "ed.log" );

//----------------------------------------------------
CLog::CLog( char *_FileName ){
    char f_path[1024];
    char _ExeName[1024];
    GetModuleFileName( GetModuleHandle(0), _ExeName, 1024 );
	_splitpath( _ExeName, f_path, 0, 0, 0 );
	_splitpath( _ExeName, 0, f_path+strlen(f_path), 0, 0 );

	VERIFY( _FileName );
    sprintf( m_FileName, "%s%s",f_path,_FileName );
	int hf = open( m_FileName, _O_WRONLY|_O_CREAT|_O_TRUNC| _O_BINARY, _S_IREAD | _S_IWRITE );
	_close( hf );
}

int CLog::DlgMsg ( TMsgDlgType mt, TMsgDlgButtons btn, char *_Format, ... ){
    g_ErrorMode = true;
	char buf[4096];
	va_list l;
	va_start( l, _Format );
	vsprintf( buf, _Format, l );

#ifdef _EDITOR
    UI->Command(COMMAND_RENDER_FOCUS);
#endif

    int res=MessageDlg(buf, mt, btn, 0);

    Msg(mt, buf);
	strcat( buf, "\r\n" );
    g_ErrorMode = false;
    return res;
}


int CLog::DlgMsg ( TMsgDlgType mt, char *_Format, ... ){
    g_ErrorMode = true;
	char buf[4096];
	va_list l;
	va_start( l, _Format );
	vsprintf( buf, _Format, l );

#ifdef _EDITOR
    UI->Command(COMMAND_RENDER_FOCUS);
#endif

    int res=mrNone;
    if (mt==mtConfirmation)	res=MessageDlg(buf, mt, TMsgDlgButtons() << mbYes << mbNo << mbCancel, 0);
    else                   	res=MessageDlg(buf, mt, TMsgDlgButtons() << mbOK, 0);

    Msg(mt,buf);
	strcat( buf, "\r\n" );
    g_ErrorMode = false;
    return res;
}

void CLog::Msg( TMsgDlgType mt, char *_Format, ... ){
	char buf[4096];
	va_list l;
	va_start( l, _Format );
	vsprintf( buf, _Format, l );

#ifdef _EDITOR
    if (frmSplash) frmSplash->SetStatus(buf);
#endif
    TfrmLog::AddMessage(mt,AnsiString(buf));

	strcat( buf, "\r\n" );

	int hf = open( m_FileName, _O_WRONLY|_O_APPEND|_O_BINARY );
	if( hf<=0 )
		hf = open( m_FileName,
			_O_WRONLY|_O_CREAT|_O_TRUNC| _O_BINARY,
			_S_IREAD | _S_IWRITE );

	_write( hf, buf, strlen(buf) );
	_close( hf );
}
//----------------------------------------------------
