//----------------------------------------------------
// file: NetDeviceLog.cpp
//----------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "Log.h"
#include "splash.h"
//----------------------------------------------------

NetLog Log( "ed.log" );

//----------------------------------------------------
extern void AddToInfoBar(LPSTR s);

NetLog::NetLog( char *_FileName ){
    char f_path[1024];
    char _ExeName[1024];
    GetModuleFileName( GetModuleHandle(0), _ExeName, 1024 );
	_splitpath( _ExeName, f_path, 0, 0, 0 );
	_splitpath( _ExeName, 0, f_path+strlen(f_path), 0, 0 );

	_ASSERTE( _FileName );
    sprintf( m_FileName, "%s%s",f_path,_FileName );
	int hf = open( m_FileName, _O_WRONLY|_O_CREAT|_O_TRUNC| _O_BINARY, _S_IREAD | _S_IWRITE );
	_close( hf );
}

void __cdecl NetLog::Msg( char *_Format, ... ){
	char buf[4096];
	va_list l;
	va_start( l, _Format );
	vsprintf( buf, _Format, l );

    AddToInfoBar(buf);
    if (frmSplash) frmSplash->SetStatus(buf);

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
