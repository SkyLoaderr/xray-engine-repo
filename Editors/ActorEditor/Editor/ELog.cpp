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

//----------------------------------------------------
int CLog::DlgMsg (TMsgDlgType mt, TMsgDlgButtons btn, LPCSTR _Format, ...)
{
    in_use = true;
	char buf[4096];
	va_list l;
	va_start( l, _Format );
	vsprintf( buf, _Format, l );

	int res=0;
#ifdef _EDITOR
    UI.Command(COMMAND_RENDER_FOCUS);

    res=MessageDlg(buf, mt, btn, 0);
    if (mtConfirmation==mt){
        switch (res){
        case mrYes: 	strcat(buf," - Yes."); 	break;
        case mrNo: 		strcat(buf," - No."); 	break;
        case mrCancel:  strcat(buf," - Cancel.");	break;
        default: strcat(buf," - Something.");
        }
    }
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
	case mtInformation: MessageBox(0,buf,"Information",	MB_OK|MB_ICONINFORMATION);	break;
	default:			MessageBox(0,buf,"Information",	MB_OK|MB_ICONINFORMATION);	break;
	}
#endif

    Msg(mt, buf);
	strcat( buf, "\r\n" );
    g_ErrorMode = false;
    return res;
}


int CLog::DlgMsg (TMsgDlgType mt, LPCSTR _Format, ...)
{
    in_use = true;
	char buf[4096];
	va_list l;
	va_start( l, _Format );
	vsprintf( buf, _Format, l );

    int res=0;
#ifdef _EDITOR
    UI.Command(COMMAND_RENDER_FOCUS);

    if (mtConfirmation==mt)	res=MessageDlg(buf, mt, TMsgDlgButtons() << mbYes << mbNo << mbCancel, 0);
    else                   	res=MessageDlg(buf, mt, TMsgDlgButtons() << mbOK, 0);

    if (mtConfirmation==mt){
        switch (res){
        case mrYes: 	strcat(buf," - Yes."); 	break;
        case mrNo: 		strcat(buf," - No."); 	break;
        case mrCancel:  strcat(buf," - Cancel.");	break;
        default: strcat(buf," - Something.");
        }
    }
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
	case mtInformation: MessageBox(0,buf,"Information",	MB_OK|MB_ICONINFORMATION);	break;
	default:			MessageBox(0,buf,"Information",	MB_OK|MB_ICONINFORMATION);	break;
	}
#endif

    Msg(mt,buf);
    g_ErrorMode = false;
    return res;
}

void CLog::Msg(TMsgDlgType mt, LPCSTR _Format, ...)
{
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

    ::Msg(buf);
}
//----------------------------------------------------
