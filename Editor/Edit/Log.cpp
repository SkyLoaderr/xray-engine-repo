//----------------------------------------------------
// file: NetDeviceELog.cpp
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

CLog ELog;
BOOL CLog::bReady = false;
char CLog::m_FileName[MAX_PATH];

void Log	(const char *msg){ ELog.Msg(mtInformation,msg); }
void Log	(const char *msg, const char*	dop){ ELog.Msg(mtInformation,"%s%s",msg,dop); }
void Log	(const char *msg, DWORD			dop){ ELog.Msg(mtInformation,"%s%d",msg,dop); }
void Log	(const char *msg, int  			dop){ ELog.Msg(mtInformation,"%s%d",msg,dop); }
void Log	(const char *msg, float			dop){ ELog.Msg(mtInformation,"%s%f",msg,dop); }
void Log	(const char *msg, const Fvector& dop){ ELog.Msg(mtInformation,"%s [%f,%f,%f]",msg,dop); }
void Log	(const char *msg, const Fmatrix& dop){
	ELog.Msg(mtInformation,"%s [%f,%f,%f,%f]\n[%f,%f,%f,%f]\n[%f,%f,%f,%f]\n[%f,%f,%f,%f]\n",msg,
	dop.i.x,dop.i.y,dop.i.z,dop._14_, dop.k.x,dop.k.y,dop.k.z,dop._34_,dop.c.x,dop.c.y,dop.c.z,dop._44_);
}

void __cdecl Msg	(LPCSTR format, ...){
	char buf[4096];
	va_list l;
	va_start( l, format );
	vsprintf( buf, format, l );

#ifdef _EDITOR
    if (frmSplash) frmSplash->SetStatus(buf);
#endif
    TfrmLog::AddMessage(mtInformation,AnsiString(buf));

	strcat( buf, "\r\n" );
    ELog.Msg(mtInformation,buf);
}
//----------------------------------------------------
void CLog::Create(LPCSTR _FileName){
	VERIFY(!bReady);
    char f_path[1024];
    char _ExeName[1024];
    GetModuleFileName( GetModuleHandle(0), _ExeName, 1024 );
	_splitpath( _ExeName, f_path, 0, 0, 0 );
	_splitpath( _ExeName, 0, f_path+strlen(f_path), 0, 0 );

	VERIFY( _FileName );
    sprintf( m_FileName, "%s%s",f_path,_FileName );
	int hf = open( m_FileName, _O_WRONLY|_O_CREAT|_O_TRUNC| _O_BINARY, _S_IREAD | _S_IWRITE );
	_close( hf );
    bReady = TRUE;
}

int CLog::DlgMsg (TMsgDlgType mt, TMsgDlgButtons btn, LPCSTR _Format, ...){
	if(!bReady) return -1;
    g_ErrorMode = true;
	char buf[4096];
	va_list l;
	va_start( l, _Format );
	vsprintf( buf, _Format, l );

#ifdef _EDITOR
    if (UI) UI->Command(COMMAND_RENDER_FOCUS);
#endif

    int res=MessageDlg(buf, mt, btn, 0);

    Msg(mt, buf);
	strcat( buf, "\r\n" );
    g_ErrorMode = false;
    return res;
}


int CLog::DlgMsg (TMsgDlgType mt, LPCSTR _Format, ...){
	if(!bReady) return -1;
    g_ErrorMode = true;
	char buf[4096];
	va_list l;
	va_start( l, _Format );
	vsprintf( buf, _Format, l );

#ifdef _EDITOR
    if (UI) UI->Command(COMMAND_RENDER_FOCUS);
#endif

    int res=mrNone;
    if (mt==mtConfirmation)	res=MessageDlg(buf, mt, TMsgDlgButtons() << mbYes << mbNo << mbCancel, 0);
    else                   	res=MessageDlg(buf, mt, TMsgDlgButtons() << mbOK, 0);

    Msg(mt,buf);
	strcat( buf, "\r\n" );
    g_ErrorMode = false;
    return res;
}

void CLog::Msg(TMsgDlgType mt, LPCSTR _Format, ...){
	if(!bReady) return;
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
