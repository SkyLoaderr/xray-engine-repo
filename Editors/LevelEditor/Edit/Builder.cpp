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
    m_iDefaultSectorNum = 0;
    l_vertices	= 0;
    l_faces		= 0;
    l_vertices_cnt	= 0;
    l_faces_cnt 	= 0;
	l_vertices_it 	= 0;
	l_faces_it		= 0;
}

SceneBuilder::~SceneBuilder(){
}

//------------------------------------------------------------------------------
#define CHECK_BREAK     	if (UI.NeedAbort()) break;
#define VERIFY_COMPILE(x,c) CHECK_BREAK \
							if (!x){ELog.DlgMsg( mtError, "Error: %s.", c); break;}
//------------------------------------------------------------------------------
BOOL SceneBuilder::Compile()
{
	AnsiString error_text="";
	UI.ResetBreak();
	if(UI.ContainEState(esBuildLevel)) return false;
	ELog.Msg( mtInformation, "Building started..." );
    
    UI.BeginEState(esBuildLevel);
    try{
        do{
            UI.Command( COMMAND_RESET_ANIMATION );
	        // check debug
            bool bTestPortal = Scene.ObjCount(OBJCLASS_SECTOR)||Scene.ObjCount(OBJCLASS_PORTAL);
//    	    if ((Scene.ObjCount(OBJCLASS_SECTOR)==0)&&(Scene.ObjCount(OBJCLASS_PORTAL)==0))
//        	    if (mrYes==ELog.DlgMsg(mtConfirmation,TMsgDlgButtons()<<mbYes<<mbNo,"Can't find sector and portal in scene. Create default?")) PortalUtils.CreateDebugCollection();
//            	else { error_text="Building canceled"; break; }
	        // validate scene
    	    VERIFY_COMPILE(Scene.Validate(false,bTestPortal),"Validation failed. Invalid scene.");
        	// build
            VERIFY_COMPILE(PreparePath(),				"Failed to prepare level path.");
            VERIFY_COMPILE(LightenObjects(),			"Failed to prepare level folders.");
            VERIFY_COMPILE(PrepareFolders(),			"Failed to prepare level folders.");
            VERIFY_COMPILE(GetBounding(),				"Failed to acquire level bounding volume.");
            VERIFY_COMPILE(RenumerateSectors(),			"Failed to renumerate sectors.");
            VERIFY_COMPILE(CompileStatic(),				"Failed static remote build.");
            VERIFY_COMPILE(BuildLTX(),					"Failed to build level description.");
            VERIFY_COMPILE(BuildGame(),					"Failed to build game.");
            VERIFY_COMPILE(BuildSkyModel(),				"Failed to build OGF model.");
            VERIFY_COMPILE(WriteTextures(),				"Failed to write textures."); 				// only implicit lighted
        } while(0);

        if (!error_text.IsEmpty()) 	ELog.DlgMsg(mtError,error_text.c_str());
        else if (UI.NeedAbort())	ELog.DlgMsg(mtInformation,"Building terminated...");
        else						ELog.DlgMsg(mtInformation,"Building OK...");
    }catch(...){
    	ELog.DlgMsg(mtError,"Error has occured in builder routine. Editor aborted.");
        abort();
    }
    UI.EndEState();

	return error_text.IsEmpty();
}
//------------------------------------------------------------------------------

BOOL SceneBuilder::MakeGame( )
{
	AnsiString error_text="";
	UI.ResetBreak();
	if(UI.ContainEState(esBuildLevel)) return false;
	ELog.Msg( mtInformation, "Making started..." );
    
    UI.BeginEState(esBuildLevel);
    try{
        do{
            UI.Command( COMMAND_RESET_ANIMATION );
	        // validate scene
    	    VERIFY_COMPILE(Scene.Validate(false,false),	"Validation failed. Invalid scene.");
        	// build
            VERIFY_COMPILE(PreparePath(),				"Failed to prepare level path.");
            VERIFY_COMPILE(PrepareFolders(),			"Failed to prepare level folders.");
            VERIFY_COMPILE(GetBounding(),				"Failed to acquire level bounding volume.");
            VERIFY_COMPILE(BuildLTX(),					"Failed to build level description.");
            VERIFY_COMPILE(BuildGame(),					"Failed to build game.");
            VERIFY_COMPILE(BuildSkyModel(),				"Failed to build OGF model.");
        } while(0);

        if (!error_text.IsEmpty()) 	ELog.DlgMsg(mtError,error_text.c_str());
        else if (UI.NeedAbort())	ELog.DlgMsg(mtInformation,"Building terminated...");
        else						ELog.DlgMsg(mtInformation,"Building OK...");
    }catch(...){
    	ELog.DlgMsg(mtError,"Error has occured in builder routine. Editor aborted.");
        abort();
    }
    UI.EndEState();

	return error_text.IsEmpty();
}
//------------------------------------------------------------------------------

BOOL SceneBuilder::MakeDetails()
{
	AnsiString error_text;
    do{
		VERIFY_COMPILE(PreparePath(),				"Failed to prepare level path.");
        AnsiString fn="level.details";
        m_LevelPath.Update(fn);
        // save details
		VERIFY_COMPILE(Scene.m_DetailObjects->Export(fn.c_str()), "Export failed.");
    }while(0);
    if (!error_text.IsEmpty()) 	ELog.DlgMsg(mtError,error_text.c_str());
    else if (UI.NeedAbort())	ELog.DlgMsg(mtInformation,"Building terminated.");
    else						ELog.DlgMsg(mtInformation,"Details succesfully exported.");

	return error_text.IsEmpty();
}
//------------------------------------------------------------------------------

