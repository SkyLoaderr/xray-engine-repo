//----------------------------------------------------
// file: XRayMapEditor.cpp
//----------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "NetDeviceLog.h"
#include "UI_Main.h"
#include "FileSystem.h"
#include "Scene.h"
#include "Library.h"
#include "D3DUtils.h"

#include "MenuTemplate.rh"


int WINAPI WinMain( HINSTANCE _Instance, HINSTANCE, LPSTR s, int ){

	timeBeginPeriod(1);
	DU_InitUtilLibrary();

	char _FileName[MAX_PATH]="";
	GetModuleFileName( 0, _FileName, MAX_PATH-1 );

	NConsole.Init( _Instance, 0 );

	_ASSERTE( s );
	if( strstr(s,"HIGHPRIORITY") ){
		SetPriorityClass( GetCurrentProcess(), HIGH_PRIORITY_CLASS );
		NConsole.print( "Using high priority ..." );
	} else if( strstr(s,"REALTIMEPRIORITY") ){
		SetPriorityClass( GetCurrentProcess(), REALTIME_PRIORITY_CLASS );
		NConsole.print( "Using realtime priority ..." );
	} else {
		NConsole.print( "Using default priority ..." );
	}
	
	UI.Init( _Instance );
	FS.Init( _FileName );
	Lib.Init();
	Scene.Init();

	UI.UpdateCaption();

	NConsole.Clear();

	MSG msg;
	HACCEL accel;
	char _B[128];

	accel = LoadAccelerators( _Instance,
		MAKEINTRESOURCE(IDR_MAINACCEL) );

	while(1)
		if( PeekMessage( &msg,0,0,0, PM_REMOVE) ){
			if( msg.message == WM_QUIT )
				break;

			if( accel ){
				if( 0==TranslateAccelerator(UI.wnd(),accel,&msg) )
					TranslateMessage( &msg );
			} else {
				TranslateMessage( &msg );
			}

			DispatchMessage( &msg );

		} else {

			Sleep(0);
			UI.Idle();

			if( NConsole.cmdtest(_B) ){

				if( 0==stricmp(_B,"quit") ){
					break;

				} else if( 0==stricmp(_B,"close") ) {
					NConsole.Clear();

				} else if( 0==stricmp(_B,"camera") ) {
					UI.PrintCamera();

				} else if( 0==stricmp(_B,"restart") ) {
					// restart video
				
				} else {
					NConsole.print( "Error: Unknown command" );
				}


			}
		}

	NConsole.Init( _Instance, 0 );
	
	Scene.Unload();
	Scene.UndoClear();
	Scene.Clear();
	Lib.Clear();
	UI.Clear();

	NConsole.Clear();

	DU_UninitUtilLibrary();
	timeEndPeriod(1);

	return 0;
}
