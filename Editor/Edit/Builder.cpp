//----------------------------------------------------
// file: Builder.cpp
//----------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "Builder.h"
#include "UI_Main.h"
#include "BuildProgress.h"
#include "Scene.h"
//----------------------------------------------------

SceneBuilder Builder;
//----------------------------------------------------
SceneBuilder::SceneBuilder(){
	m_Scene = 0;
	m_InProgress = false;
	m_SceneSaved = false;

// remote static data     
    bStaticRemoteInProgress = false;
}

SceneBuilder::~SceneBuilder(){
	_ASSERTE( !m_InProgress );
    if (bStaticRemoteInProgress) RemoteStaticFinalize();
}

bool SceneBuilder::Execute( EScene *scene ){
	
	if( m_InProgress )
		return false;

	m_InProgress = true;
	m_Scene = scene;
	m_SceneSaved = false;

	Log.Msg( "Building started..." );

    frmBuildProgress->Show();
    Thread();
    frmBuildProgress->Close();

	return true;
}

bool SceneBuilder::Stop(){
	if( m_InProgress ){
		Log.Msg( "Restoring scene..." );
		if( m_SceneSaved ){
			m_Scene->Unload();
			m_Scene->Load( m_TempFilename );
			m_SceneSaved = false;
		}
		m_InProgress = false;
		Log.Msg( "Building terminated..." );
		return true;
	}
	Log.Msg( "Stop() called when no progress..." );
	return false;
}

//----------------------------

DWORD SceneBuilder::Thread(){

	_ASSERTE( m_Scene );

	frmBuildProgress->SetStage( "Saving scene..." );
	frmBuildProgress->SetProgress( 0 );
	Sleep(300);

	strcpy( m_TempFilename, "build.tmp" );
	FS.m_Temp.Update( m_TempFilename );
	m_Scene->Save( m_TempFilename );
	m_SceneSaved = true;

	m_TexNames.clear();

	bool error_flag = false;
	char error_text[256];
	do{
		frmBuildProgress->SetStage( "Setting up folders..." );
		frmBuildProgress->SetProgress( 0 );
		if( !PrepareFolders() ){
			strcpy( error_text, "Failed to prepare level folders...." );
			error_flag = true;
			break;
		}

		frmBuildProgress->SetStage( "Getting level shift..." );
		frmBuildProgress->SetProgress( 0 );
		if( !GetShift() ){
			strcpy( error_text, "Failed to acquire level shift...." );
			error_flag = true;
			break;
		}


		frmBuildProgress->SetStage( "Shifting level..." );
		frmBuildProgress->SetProgress( 0 );
		if( !ShiftLevel() ){
			strcpy( error_text, "Failed to shift level...." );
			error_flag = true;
			break;
		}

		frmBuildProgress->SetStage( "Resolving references..." );
		frmBuildProgress->SetProgress( 0 );
		if( !ResolveReferences() ){
			strcpy( error_text, "Failed to resolve references...." );
			error_flag = true;
			break;
		}

		frmBuildProgress->SetStage( "Static remote build..." );
		frmBuildProgress->SetProgress( 0 );
		if( !RemoteStaticBuild() ){
			strcpy( error_text, "Failed static remote build...." );
			error_flag = true;
			break;
		}

		frmBuildProgress->SetStage( "Building description (ltx)..." );
		frmBuildProgress->SetProgress( 0 );
		if( !BuildLTX() ){
			strcpy( error_text, "Failed to build level description...." );
			error_flag = true;
			break;
		}

		frmBuildProgress->SetStage( "Writing textures..." );
		frmBuildProgress->SetProgress( 0 );
		if( !WriteTextures() ){
			strcpy( error_text, "Failed to write textures...." );
			error_flag = true;
			break;
		}

		frmBuildProgress->SetStage( "Wait static remote build..." );
		frmBuildProgress->SetProgress( 0 );
		if( !RemoteStaticFinalize() ){
			strcpy( error_text, "Failed wait static remote build...." );
			error_flag = true;
			break;
		}
	} while(0);

	m_TexNames.clear();

	frmBuildProgress->SetStage( "Restoring scene..." );
	frmBuildProgress->SetProgress( 0 );
	Sleep(300);

	//---------------
	if( m_SceneSaved ){
		m_Scene->Unload();
		m_Scene->Load( m_TempFilename );
		m_SceneSaved = false;
	}

	m_InProgress = false;

	if( error_flag ){
		frmBuildProgress->SetStage( error_text );
		frmBuildProgress->SetProgress( 100 );
	} else {
		frmBuildProgress->SetStage( "Building OK..." );
		frmBuildProgress->SetProgress( 100 );
	}

	return 0;
}


