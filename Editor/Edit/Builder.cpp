//----------------------------------------------------
// file: Builder.cpp
//----------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "Builder.h"
#include "UI_Main.h"
#include "Scene.h"
#include "AITraffic.h"
#include "PortalUtils.h"
#include "DetailObjects.h"
//----------------------------------------------------

SceneBuilder* Builder;
//----------------------------------------------------
SceneBuilder::SceneBuilder(){
	m_InProgress = false;
    bNeedAbort   = false;
    m_iDefaultSectorNum = 0;
    l_vertices	= 0;
    l_faces		= 0;
    l_vertices_cnt	= 0;
    l_faces_cnt 	= 0;
	l_vertices_it 	= 0;
	l_faces_it		= 0;
}

SceneBuilder::~SceneBuilder(){
	VERIFY( !m_InProgress );
}

bool SceneBuilder::Execute( ){
    bNeedAbort = false;
	if( m_InProgress ) return false;
	Log->Msg( mtInformation, "Building started..." );

    // save scene
	UI->Command(COMMAND_SAVE);

    // check debug
	if ((Scene->ObjCount(OBJCLASS_SECTOR)==0)&&(Scene->ObjCount(OBJCLASS_PORTAL)==0))
    	if (Log->DlgMsg(mtConfirmation,"Can't find sector and portal in scene. Create default?")==mrYes)
        	PortalUtils.CreateDebugCollection();

    
    // validate scene
    if (!Scene->Validate()){
    	Log->DlgMsg( mtError, "Invalid scene. Building failed." );
		UI->Command(COMMAND_RELOAD);
        return false;
    }

    // build
	m_InProgress = true;
	//---------------
	UI->BeginEState(esBuildLevel);
    Thread();
	UI->EndEState();
	//---------------
	UI->ProgressStart(2, "Restoring scene...");
	UI->ProgressUpdate(1);
	UI->Command(COMMAND_RELOAD);
	UI->ProgressEnd();

	return true;
}

bool SceneBuilder::MakeLTX( ){
    bNeedAbort = false;
	if( m_InProgress ) return false;
	Log->Msg( mtInformation, "Making started..." );

    // save scene
	UI->Command(COMMAND_SAVE);

    // validate scene
    if (!Scene->Validate(false,false)){
    	Log->DlgMsg( mtError, "Invalid scene. Building failed." );
		UI->Command(COMMAND_RELOAD);
        return false;
    }

    // build
	m_InProgress = true;
	//---------------
	UI->BeginEState(esBuildLevel);
    ThreadMakeLTX();
	UI->EndEState();
	//---------------
	UI->ProgressStart(2, "Restoring scene...");
	UI->ProgressUpdate(1);
	UI->Command(COMMAND_RELOAD);
	UI->ProgressEnd();

	return true;
}

bool SceneBuilder::MakeDetails(bool bOkMessage){
	bool error_flag = false;
	AnsiString error_text;
    do{
	    if( !PreparePath() ){
    	    error_text="*ERROR: Failed to prepare level path....";
        	error_flag = true;
	        break;
    	}
        AnsiString fn="level.details";
        m_LevelPath.Update(fn);
        // save details
		Scene->m_DetailObjects->Export(fn.c_str());
    }while(0);
	if( error_flag )  	Log->DlgMsg(mtError,error_text.c_str());
    else				if (bOkMessage) Log->DlgMsg(mtInformation,"Details succesfully exported.");

	return error_flag;
}

//----------------------------
DWORD SceneBuilder::Thread(){
	VERIFY( Scene );

	bool error_flag = false;
	AnsiString error_text;
	do{
		UI->Command( COMMAND_RESET_ANIMATION );

        if (NeedAbort()) break;
		if( !PreparePath() ){
			error_text="*ERROR: Failed to prepare level path....";
			error_flag = true;
			break;
		}

        if (NeedAbort()) break;
		if( !LightenObjects() ){
			error_text="*ERROR: Failed to prepare level folders....";
			error_flag = true;
			break;
		}

        if (NeedAbort()) break;
		if( !PrepareFolders() ){
			error_text="*ERROR: Failed to prepare level folders....";
			error_flag = true;
			break;
		}

        if (NeedAbort()) break;
		if( !UngroupAndUnlockObjects() ){
			error_text="*ERROR: Failed to ungroup and unlock objects...";
			error_flag = true;
			break;
		}

        if (NeedAbort()) break;
		if( !GetShift() ){
			error_text="*ERROR: Failed to acquire level shift....";
			error_flag = true;
			break;
		}


        if (NeedAbort()) break;
		if( !ShiftLevel() ){
			error_text="*ERROR: Failed to shift level....";
			error_flag = true;
			break;
		}
/*
        if (NeedAbort()) break;
		frmBuildProgress->SetStage( AnsiString("Resolving references...") );
		frmBuildProgress->SetProgress( 0 );
		if( !ResolveReferences() ){
			error_text="*ERROR: Failed to resolve references...";
			error_flag = true;
			break;
		}
        // unload object library
//	    UI->Command(COMMAND_UNLOAD_LIBMESHES);
*/
        
        if (NeedAbort()) break;
		if( !RenumerateSectors() ){
			error_text="*ERROR: Failed to renumerate sectors....";
			error_flag = true;
			break;
		}

        if (NeedAbort()) break;
		if( !RemoteStaticBuild() ){
			error_text="*ERROR: Failed static remote build....";
			error_flag = true;
			break;
		}

        if (NeedAbort()) break;
		if( !BuildLTX() ){
			error_text="*ERROR: Failed to build level description....";
			error_flag = true;
			break;
		}

        if (NeedAbort()) break;
		if( !BuildVCFModels() ){
			error_text="*ERROR: Failed to build collision form....";
			error_flag = true;
			break;
		}

        if (NeedAbort()) break;
		if( !BuildOGFModels() ){
			error_text="*ERROR: Failed to build OGF model....";
			error_flag = true;
			break;
		}

        if (NeedAbort()) break;
		// only implicit lighted
		if( !WriteTextures() ){
			error_text="*ERROR: Failed to write textures....";
			error_flag = true;
			break;
		}

	} while(0);

	if( error_flag ) 		Log->DlgMsg(mtError,error_text.c_str());
	else if ( bNeedAbort ) 	Log->DlgMsg(mtInformation,"Building terminated...");
    else					Log->DlgMsg(mtInformation,"Building OK...");

	m_InProgress = false;

	return 0;
}

//----------------------------
DWORD SceneBuilder::ThreadMakeLTX(){
	VERIFY( Scene );

	bool error_flag = false;
	AnsiString error_text;
	do{
		UI->Command( COMMAND_RESET_ANIMATION );

        if (NeedAbort()) break;
		if( !PreparePath() ){
			error_text="*ERROR: Failed to prepare level path....";
			error_flag = true;
			break;
		}

        if (NeedAbort()) break;
		if( !UngroupAndUnlockObjects() ){
			error_text="*ERROR: Failed to ungroup and unlock objects...";
			error_flag = true;
			break;
		}

        if (NeedAbort()) break;
		if( !GetShift() ){
			error_text="*ERROR: Failed to acquire level shift....";
			error_flag = true;
			break;
		}


        if (NeedAbort()) break;
		if( !ShiftLevel() ){
			error_text="*ERROR: Failed to shift level....";
			error_flag = true;
			break;
		}

        if (NeedAbort()) break;
		if( !BuildLTX() ){
			error_text="*ERROR: Failed to build level description....";
			error_flag = true;
			break;
		}
	} while(0);

	if( error_flag ) 		Log->DlgMsg(mtError,error_text.c_str());
	else if ( bNeedAbort ) 	Log->DlgMsg(mtInformation,"Building terminated...");
    else					Log->DlgMsg(mtInformation,"Building OK...");

	m_InProgress = false;

	return 0;
}


