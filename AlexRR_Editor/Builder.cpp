//----------------------------------------------------
// file: Builder.cpp
//----------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "Builder.h"
#include "UI_Main.h"
#include "MenuTemplate.rh"
//----------------------------------------------------

SceneBuilder Builder;
BuilderWindow BWindow;

//----------------------------------------------------

DWORD WINAPI ThreadOfBuild( LPVOID param ){
	SceneBuilder *builder = (SceneBuilder*)param;
	return builder->Thread();
}

//----------------------------------------------------

BOOL CALLBACK InfoDialogProc( HWND hw, UINT msg, WPARAM wp, LPARAM lp){
	Sleep(0);
	switch( msg ){
	case WM_INITDIALOG:
		SendDlgItemMessage(hw,IDC_PROGRESS,PBM_SETRANGE,0,MAKELONG(0,100));
		SetDlgItemText(hw,IDC_STAGE,BWindow.m_LastStage);
		SendDlgItemMessage(hw,IDC_PROGRESS,PBM_SETPOS,(int)BWindow.m_LastProgress,0);
		BWindow.m_Window = hw;
		break;
	case WM_DESTROY:
		BWindow.m_Window = 0;
		break;
	case WM_COMMAND:
		switch(LOWORD(wp)){
		case IDCANCEL:
			EndDialog( hw, IDCANCEL );
			break;
		}
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

DWORD WINAPI ThreadOfDialog( LPVOID param ){
	DialogBox(UI.inst(),MAKEINTRESOURCE(IDD_BUILD_PROGRESS),
		UI.wnd(), InfoDialogProc);
	NConsole.print( "Exitting information thread...");
	if(Builder.InProgress())
		Builder.Stop();
	return 0;
}

void BuilderWindow::SetStage( char *stage ){
	_ASSERTE(stage);
	strcpy(m_LastStage,stage);
	Sleep(0);
	if( m_Window )
		SetDlgItemText(m_Window,IDC_STAGE,m_LastStage);
}

void BuilderWindow::SetProgress( float progress ){
	progress = floorf(progress);
	if( progress != m_LastProgress ){
		Sleep(0);
		m_LastProgress = progress;
		if( m_Window ){
			SendDlgItemMessage(m_Window,
				IDC_PROGRESS,PBM_SETPOS,(int)m_LastProgress,0);
		}
	}
}

void BuilderWindow::Open(){

	if( m_DialogThread ){
		NConsole.print( "Closing previous information thread...");
		CloseHandle(m_DialogThread);
		m_DialogThread = 0;
	}

	NConsole.print( "Creating new information thread...");
	m_DialogThread = CreateThread( 0, 0, ThreadOfDialog, this,
		0, &m_DialogThreadID );
}

BuilderWindow::BuilderWindow(){
	m_Window = 0;
	m_DialogThread = 0;
	m_DialogThreadID = 0;
	m_LastStage[0] = 0;
	m_LastProgress = 0.f;
}

BuilderWindow::~BuilderWindow(){
	_ASSERTE( m_Window==0 );
	if( m_DialogThread )
		CloseHandle( m_DialogThread );
}


//----------------------------------------------------

VisMap::VisMap(){
	m_Map = 0;
}

VisMap::~VisMap(){
	SAFE_DELETE_A( m_Map );
};

void VisMap::Allocate( int _dx, int _dz ){
	if( m_Map )
		Free();
	dx = _dx;
	dz = _dz;
	m_Map = new VisItem[dx*dz];
}

void VisMap::Free( ){
	SAFE_DELETE_A( m_Map );
}

//----------------------------------------------------

SceneBuilder::SceneBuilder(){
	m_Scene = 0;
	m_InProgress = false;
	m_Thread = 0;
	m_ThreadID = 0;
	m_SceneSaved = false;
}

SceneBuilder::~SceneBuilder(){
	_ASSERTE( !m_InProgress );
}

bool SceneBuilder::Execute( EScene *scene ){
	
	if( m_InProgress )
		return false;

	m_InProgress = true;
	m_Scene = scene;
	m_SceneSaved = false;

	NConsole.print( "Building started..." );

	if( m_Thread ){
		NConsole.print( "Closing previous thread ..." );
		CloseHandle( m_Thread );
		m_Thread = 0; }

	NConsole.print( "Creating new thread ..." );
	m_Thread = CreateThread( 0, 0, ThreadOfBuild, this,
		CREATE_SUSPENDED, &m_ThreadID );
	_ASSERTE( m_Thread );

	NConsole.print( "Opening information window..." );
	BWindow.Open();

	NConsole.print( "Setting up priority..." );
	SetThreadPriority( m_Thread, THREAD_PRIORITY_ABOVE_NORMAL );
	ResumeThread( m_Thread );

	return true;
}

bool SceneBuilder::Stop(){
	if( m_InProgress ){

		NConsole.print( "Terminationg thread..." );
		
		_ASSERTE( m_Thread );
		TerminateThread( m_Thread, 0 );
		CloseHandle( m_Thread );
		m_Thread = 0;

		NConsole.print( "Restoring scene..." );

		if( m_SceneSaved ){
			m_Scene->Unload();
			m_Scene->Load( m_TempFilename );
			m_SceneSaved = false;
		}

		m_InProgress = false;
		NConsole.print( "Building terminated..." );

		return true;
	}

	NConsole.print( "Stop() called when no progress..." );
	return false;
}

//----------------------------

DWORD SceneBuilder::Thread(){

	_ASSERTE( m_Scene );

	BWindow.SetStage( "Saving scene..." );
	BWindow.SetProgress( 0 );
	Sleep(300);

	strcpy( m_TempFilename, "build.tmp" );
	FS.m_Temp.Update( m_TempFilename );
	m_Scene->Save( m_TempFilename );
	m_SceneSaved = true;

	m_TexNames.clear();
	m_Statics.clear();
	m_VMap.Free();

	bool error_flag = false;
	char error_text[256];
	do{
		BWindow.SetStage( "Setting up folders..." );
		BWindow.SetProgress( 0 );
		if( !PrepareFolders() ){
			strcpy( error_text, "Failed to prepare level folders...." );
			error_flag = true;
			break;
		}

		BWindow.SetStage( "Converting landscape..." );
		BWindow.SetProgress( 0 );
		if( !ConvertLandscape() ){
			strcpy( error_text, "Failed to convert landscape...." );
			error_flag = true;
			break;
		}

		BWindow.SetStage( "Getting level shift..." );
		BWindow.SetProgress( 0 );
		if( !GetShift() ){
			strcpy( error_text, "Failed to acquire level shift...." );
			error_flag = true;
			break;
		}

		BWindow.SetStage( "Shifting level..." );
		BWindow.SetProgress( 0 );
		if( !ShiftLevel() ){
			strcpy( error_text, "Failed to shift level...." );
			error_flag = true;
			break;
		}

		BWindow.SetStage( "Resolving references..." );
		BWindow.SetProgress( 0 );
		if( !ResolveReferences() ){
			strcpy( error_text, "Failed to resolve references...." );
			error_flag = true;
			break;
		}

		BWindow.SetStage( "Building collision model..." );
		BWindow.SetProgress( 0 );
		if( !BuildCollisionModel() ){
			strcpy( error_text, "Failed to build collision model...." );
			error_flag = true;
			break;
		}

		if( m_Scene->m_BOPLightMaps ){
			BWindow.SetStage( "Calculating lightmaps..." );
			BWindow.SetProgress( 0 );
			if( !TextureLighting() ){
				strcpy( error_text, "Failed to calculate lightmaps...." );
				error_flag = true;
				break;
			}
		}

		BWindow.SetStage( "Calculating vertex colors..." );
		BWindow.SetProgress( 0 );
		if( !VertexLighting() ){
			strcpy( error_text, "Failed to calculate vertex colors...." );
			error_flag = true;
			break;
		}

		if( m_Scene->m_BOPOptimize ){
			BWindow.SetStage( "Optimizing (pass 1 of 2) ..." );
			BWindow.SetProgress( 0 );
			if( !OptimizeRenderObjects() ){
				strcpy( error_text, "Failed to optimize render objects...." );
				error_flag = true;
				break;
			}
			if( m_Scene->m_BOPLightMaps ){
				BWindow.SetStage( "Optimizing lightmaps (pass 2 of 3)..." );
				BWindow.SetProgress( 0 );
				if( !OptimizeLightmaps() ){
					strcpy( error_text, "Failed to optimize lightmaps ...." );
					error_flag = true;
					break;
				}
			}
			BWindow.SetStage( "Optimizing (pass 3 of 3) ..." );
			BWindow.SetProgress( 0 );
			if( !OptimizeRenderObjects2() ){
				strcpy( error_text, "Failed to optimize render objects...." );
				error_flag = true;
				break;
			}
		} else {
			if( m_Scene->m_BOPLightMaps ){
				BWindow.SetStage( "Optimizing lightmaps ..." );
				BWindow.SetProgress( 0 );
				if( !OptimizeLightmaps() ){
					strcpy( error_text, "Failed to optimize lightmaps ...." );
					error_flag = true;
					break;
				}
			}
		}

		BWindow.SetStage( "Building light model..." );
		BWindow.SetProgress( 0 );
		if( !BuildLightModel() ){
			strcpy( error_text, "Failed to build light model...." );
			error_flag = true;
			break;
		}

		BWindow.SetStage( "Building render model..." );
		BWindow.SetProgress( 0 );
		if( !BuildRenderModel() ){
			strcpy( error_text, "Failed to build render model...." );
			error_flag = true;
			break;
		}

		BWindow.SetStage( "Building visibility maps..." );
		BWindow.SetProgress( 0 );
		if( !BuildVisibility() ){
			strcpy( error_text, "Failed to build visibility maps...." );
			error_flag = true;
			break;
		}

		BWindow.SetStage( "Building fake collision..." );
		BWindow.SetProgress( 0 );
		if( !BuildFakeCollision() ){
			strcpy( error_text, "Failed to build fake collision...." );
			error_flag = true;
			break;
		}

		BWindow.SetStage( "Building description (ltx)..." );
		BWindow.SetProgress( 0 );
		if( !BuildLTX() ){
			strcpy( error_text, "Failed to build level description...." );
			error_flag = true;
			break;
		}

		BWindow.SetStage( "Compiling description..." );
		BWindow.SetProgress( 0 );
		if( !CompileText() ){
			strcpy( error_text, "Failed to compile level description...." );
			error_flag = true;
			break;
		}

		BWindow.SetStage( "Writing textures..." );
		BWindow.SetProgress( 0 );
		if( !WriteTextures() ){
			strcpy( error_text, "Failed to write textures...." );
			error_flag = true;
			break;
		}

	} while(0);

	m_TexNames.clear();
	m_Statics.clear();
	m_VMap.Free();

	BWindow.SetStage( "Restoring scene..." );
	BWindow.SetProgress( 0 );
	Sleep(300);

	//---------------
	//if( m_SceneSaved ){
	//	m_Scene->Unload();
	//	m_Scene->Load( m_TempFilename );
	//	m_SceneSaved = false;
	//}

	m_InProgress = false;

	if( error_flag ){
		BWindow.SetStage( error_text );
		BWindow.SetProgress( 100 );
	} else {
		BWindow.SetStage( "Building OK..." );
		BWindow.SetProgress( 100 );
	}

	return 0;
}


