//----------------------------------------------------
// file: NetDeviceELog.cpp
//----------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "Log.h"
#ifdef _EDITOR
	#include "LogForm.h"
	#include "splash.h"
	#include "ui_main.h"
#endif
#ifdef _LW_PLUGIN
	#include <lwhost.h>
	extern "C" LWMessageFuncs	*g_msg;
#endif
#ifdef _MAX_EXPORT
	#include "NetDeviceLog.h"
#endif
//----------------------------------------------------

bool g_ErrorMode=false;

CLog ELog;
void Log	(const char *msg){ ELog.Msg(mtInformation,msg); }
void Log	(const char *msg, const char*	dop){ ELog.Msg(mtInformation,"%s%s",msg,dop); }
void Log	(const char *msg, DWORD			dop){ ELog.Msg(mtInformation,"%s%d",msg,dop); }
void Log	(const char *msg, int  			dop){ ELog.Msg(mtInformation,"%s%d",msg,dop); }
void Log	(const char *msg, float			dop){ ELog.Msg(mtInformation,"%s%f",msg,dop); }
void Log	(const char *msg, const Fvector& dop){ ELog.Msg(mtInformation,"%s [%f,%f,%f]",msg,dop.x,dop.y,dop.z); }
void Log	(const char *msg, const Fmatrix& dop){
	ELog.Msg(mtInformation,"%s\n[%f,%f,%f,%f]\n[%f,%f,%f,%f]\n[%f,%f,%f,%f]\n[%f,%f,%f,%f]\n",msg
																				,dop.i.x,dop.i.y,dop.i.z,dop._14_
																				,dop.j.x,dop.j.y,dop.j.z,dop._24_
																				,dop.k.x,dop.k.y,dop.k.z,dop._34_
																				,dop.c.x,dop.c.y,dop.c.z,dop._44_);
}

void __cdecl Msg	(LPCSTR format, ...){
	char buf[4096];
	va_list l;
	va_start( l, format );
	vsprintf( buf, format, l );

#ifdef _EDITOR
    if (frmSplash) frmSplash->SetStatus(buf);
#endif
    ELog.Msg(mtInformation,buf);
}
//----------------------------------------------------
void CLog::Create(LPCSTR _FileName, bool bContinue){
	VERIFY(!bReady);
    strcpy( m_FileName, _FileName );
    int hf=-1;
    if (bContinue) 	hf = open( m_FileName, _O_WRONLY|_O_CREAT|_O_APPEND|_O_BINARY );
	else			hf = open( m_FileName, _O_WRONLY|_O_CREAT|_O_TRUNC| _O_BINARY, _S_IREAD | _S_IWRITE );
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

	int res=0;
#ifdef _EDITOR
    UI.Command(COMMAND_RENDER_FOCUS);

    res=MessageDlg(buf, mt, btn, 0);
#endif
#ifdef _LW_PLUGIN
	switch(mt){
	case mtError:		g_msg->error(buf,0);	break;
	case mtInformation: g_msg->info(buf,0);		break;
	default:			g_msg->info(buf,0);		break;
	}
#endif
#ifdef _MAX_PLUGIN
	switch(mt){
	case mtError:		MessageBox(0,buf,"Error",		MB_OK|MB_ICONERROR);		break;
	case mtInformation: MessageBox(0,buf,"Information",	MB_OK|MB_ICONINFORMATION); break;
	default:			MessageBox(0,buf,"Information",	MB_OK|MB_ICONINFORMATION); break;
	}
#endif

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

    int res=0;
#ifdef _EDITOR
    UI.Command(COMMAND_RENDER_FOCUS);

    if (mt==mtConfirmation)	res=MessageDlg(buf, mt, TMsgDlgButtons() << mbYes << mbNo << mbCancel, 0);
    else                   	res=MessageDlg(buf, mt, TMsgDlgButtons() << mbOK, 0);
#else
	#ifdef _LW_PLUGIN
		g_msg->info(buf,0);
	#endif
#endif

    Msg(mt,buf);
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
    TfrmLog::AddMessage(mt,AnsiString(buf));
#endif
#ifdef _MAX_EXPORT
	EConsole.print(mt,buf);
#endif
#ifdef _LW_PLUGIN
	g_msg->info(buf,0);
#endif

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
