//----------------------------------------------------
// file: Builder.cpp
//----------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "Builder.h"
#include "UI_Main.h"
#include "Scene.h"
#include "PortalUtils.h"
#include "DetailObjects.h"
//----------------------------------------------------

SceneBuilder Builder;
//----------------------------------------------------
SceneBuilder::SceneBuilder(){
	m_InProgress = false;
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
	UI.ResetBreak();
	if( m_InProgress ) return false;
	ELog.Msg( mtInformation, "Building started..." );

    // save scene
	UI.Command(COMMAND_SAVE);

    try{
        // check debug
        if ((Scene.ObjCount(OBJCLASS_SECTOR)==0)&&(Scene.ObjCount(OBJCLASS_PORTAL)==0)){
        	int res=ELog.DlgMsg(mtConfirmation,TMsgDlgButtons() << mbYes << mbNo,"Can't find sector and portal in scene. Create default?");
            switch (res){
            case mrYes: PortalUtils.CreateDebugCollection(); break;
            default: return false;
            }
        }


        // validate scene
        if (!Scene.Validate()){
            ELog.DlgMsg( mtError, "Invalid scene. Building failed." );
            UI.Command(COMMAND_RELOAD);
            return false;
        }

        // build
        m_InProgress = true;
        //---------------
        UI.BeginEState(esBuildLevel);
        Thread();
        UI.EndEState();
        //---------------
        UI.ProgressStart(2, "Restoring scene...");
        UI.ProgressUpdate(1);
        UI.Command(COMMAND_RELOAD);
        UI.ProgressEnd();
    }catch(...){
    	ELog.DlgMsg(mtError,"Error has occured in builder routine. Editor aborted.");
        abort();
    }

	return true;
}

bool SceneBuilder::MakeLTX( ){
	UI.ResetBreak();
	if( m_InProgress ) return false;
	ELog.Msg( mtInformation, "Making started..." );

    // save scene
	UI.Command(COMMAND_SAVE);

    // validate scene
    if (!Scene.Validate(false,false)){
    	ELog.DlgMsg( mtError, "Invalid scene. Building failed." );
		UI.Command(COMMAND_RELOAD);
        return false;
    }

    // build
	m_InProgress = true;
	//---------------
	UI.BeginEState(esBuildLevel);
    try{
	    ThreadMakeLTX();
    }catch(...){
    	ELog.DlgMsg(mtError,"Error has occured in builder routine. Editor aborted.");
        abort();
    }
	UI.EndEState();
	//---------------
	UI.ProgressStart(2, "Restoring scene...");
	UI.ProgressUpdate(1);
	UI.Command(COMMAND_RELOAD);
	UI.ProgressEnd();

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
		if (!Scene.m_DetailObjects->Export(fn.c_str())){
			error_text="Export failed.";
            error_flag=true;
        };
    }while(0);
	if( error_flag )  	ELog.DlgMsg(mtError,error_text.c_str());
    else				if (bOkMessage) ELog.DlgMsg(mtInformation,"Details succesfully exported.");

	return error_flag;
}

//----------------------------
DWORD SceneBuilder::Thread(){
	bool error_flag = false;
	AnsiString error_text;
	do{
		UI.Command( COMMAND_RESET_ANIMATION );

        if (UI.NeedAbort()) break;
		if( !PreparePath() ){
			error_text="*ERROR: Failed to prepare level path....";
			error_flag = true;
			break;
		}

        if (UI.NeedAbort()) break;
		if( !LightenObjects() ){
			error_text="*ERROR: Failed to prepare level folders....";
			error_flag = true;
			break;
		}

        if (UI.NeedAbort()) break;
		if( !PrepareFolders() ){
			error_text="*ERROR: Failed to prepare level folders....";
			error_flag = true;
			break;
		}

        if (UI.NeedAbort()) break;
		if( !UngroupAndUnlockObjects() ){
			error_text="*ERROR: Failed to ungroup and unlock objects...";
			error_flag = true;
			break;
		}

        if (UI.NeedAbort()) break;
		if( !GetShift() ){
			error_text="*ERROR: Failed to acquire level shift....";
			error_flag = true;
			break;
		}


        if (UI.NeedAbort()) break;
		if( !ShiftLevel() ){
			error_text="*ERROR: Failed to shift level....";
			error_flag = true;
			break;
		}

        if (UI.NeedAbort()) break;
		if( !RenumerateSectors() ){
			error_text="*ERROR: Failed to renumerate sectors....";
			error_flag = true;
			break;
		}

        if (UI.NeedAbort()) break;
		if( !RemoteStaticBuild() ){
			error_text="*ERROR: Failed static remote build....";
			error_flag = true;
			break;
		}

        if (UI.NeedAbort()) break;
		if( !BuildLTX() ){
			error_text="*ERROR: Failed to build level description....";
			error_flag = true;
			break;
		}

        if (UI.NeedAbort()) break;
        if( !BuildGame() ){
			error_text="*ERROR: Failed to build game....";
			error_flag = true;
			break;
        }

        if (UI.NeedAbort()) break;
		if( !BuildSkyModel() ){
			error_text="*ERROR: Failed to build OGF model....";
			error_flag = true;
			break;
		}

        if (UI.NeedAbort()) break;
		// only implicit lighted
		if( !WriteTextures() ){
			error_text="*ERROR: Failed to write textures....";
			error_flag = true;
			break;
		}

	} while(0);

	if( error_flag ) 		ELog.DlgMsg(mtError,error_text.c_str());
	else if ( UI.NeedAbort())ELog.DlgMsg(mtInformation,"Building terminated...");

    else					ELog.DlgMsg(mtInformation,"Building OK...");

	m_InProgress = false;

	return 0;
}

//----------------------------
DWORD SceneBuilder::ThreadMakeLTX(){
	bool error_flag = false;
	AnsiString error_text;
	do{
		UI.Command( COMMAND_RESET_ANIMATION );

        if (UI.NeedAbort()) break;
		if( !PreparePath() ){
			error_text="*ERROR: Failed to prepare level path....";
			error_flag = true;
			break;
		}

        if (UI.NeedAbort()) break;
		if( !UngroupAndUnlockObjects() ){
			error_text="*ERROR: Failed to ungroup and unlock objects...";
			error_flag = true;
			break;
		}

        if (UI.NeedAbort()) break;
		if( !GetShift() ){
			error_text="*ERROR: Failed to acquire level shift....";
			error_flag = true;
			break;
		}


        if (UI.NeedAbort()) break;
		if( !ShiftLevel() ){
			error_text="*ERROR: Failed to shift level....";
			error_flag = true;
			break;
		}

        if (UI.NeedAbort()) break;
		if( !BuildLTX() ){
			error_text="*ERROR: Failed to build level description....";
			error_flag = true;
			break;
		}

        if (UI.NeedAbort()) break;
        if( !BuildGame() ){
			error_text="*ERROR: Failed to build game....";
			error_flag = true;
			break;
        }

        if (UI.NeedAbort()) break;
		if( !BuildSkyModel() ){
			error_text="*ERROR: Failed to build OGF model....";
			error_flag = true;
			break;
		}
	} while(0);

	if( error_flag ) 		ELog.DlgMsg(mtError,error_text.c_str());
	else if ( UI.NeedAbort())ELog.DlgMsg(mtInformation,"Building terminated...");
    else					ELog.DlgMsg(mtInformation,"Building OK...");

	m_InProgress = false;

	return 0;
}


