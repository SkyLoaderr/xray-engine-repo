//----------------------------------------------------
// file: Builder.cpp
//----------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "Builder.h"
#include "Scene.h"
#include "PortalUtils.h"
#include "DetailObjects.h"
#include "ui_main.h"
//----------------------------------------------------

SceneBuilder Builder;
//----------------------------------------------------
SceneBuilder::SceneBuilder()
{
    m_iDefaultSectorNum = 0;
    l_verts			= 0;
    l_faces			= 0;
    l_vert_cnt		= 0;
    l_face_cnt 		= 0;
	l_vert_it	 	= 0;
	l_face_it		= 0;
}

SceneBuilder::~SceneBuilder()
{
}

//------------------------------------------------------------------------------
#define CHECK_BREAK     	if (UI.NeedAbort()) break;
#define VERIFY_COMPILE(x,c) CHECK_BREAK \
							if (!x){error_text.sprintf("Error: %s", c); break;}
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
	        // validate scene
    	    VERIFY_COMPILE(Scene.Validate(false,bTestPortal,true,true,true),"Validation failed. Invalid scene.");
        	// build
            VERIFY_COMPILE(PreparePath(),				"Failed to prepare level path.");
            VERIFY_COMPILE(PrepareFolders(),			"Failed to prepare level folders.");
            VERIFY_COMPILE(EvictResource(),				"Failed to evict resource.");
            VERIFY_COMPILE(GetBounding(),				"Failed to acquire level bounding volume.");
            VERIFY_COMPILE(RenumerateSectors(),			"Failed to renumerate sectors.");
            VERIFY_COMPILE(CompileStatic(),				"Failed static remote build.");
            VERIFY_COMPILE(BuildLTX(),					"Failed to build level description.");
            VERIFY_COMPILE(BuildGame(),					"Failed to build game.");
            VERIFY_COMPILE(BuildSkyModel(),				"Failed to build sky model.");
            BuildHOMModel();
			BuildAIMap();
        } while(0);

        if (!error_text.IsEmpty()) 	ELog.DlgMsg(mtError,error_text.c_str());
        else if (UI.NeedAbort())	ELog.DlgMsg(mtInformation,"Building terminated.");
        else						ELog.DlgMsg(mtInformation,"Building OK.");
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
	        // validate scene
    	    VERIFY_COMPILE(Scene.Validate(false,false,false,false,false),	"Validation failed. Invalid scene.");
        	// build
            VERIFY_COMPILE(PreparePath(),				"Failed to prepare level path.");
            VERIFY_COMPILE(GetBounding(),				"Failed to acquire level bounding volume.");
            VERIFY_COMPILE(BuildLTX(),					"Failed to build level description.");
            VERIFY_COMPILE(BuildGame(),					"Failed to build game.");
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

BOOL SceneBuilder::MakeSkydome()
{
	AnsiString error_text;
    do{
		VERIFY_COMPILE(PreparePath(),				"Failed to prepare level path.");
		VERIFY_COMPILE(BuildSkyModel(),				"Failed to build sky model.");
    }while(0);
    if (!error_text.IsEmpty()) 	ELog.DlgMsg(mtError,error_text.c_str());
    else if (UI.NeedAbort())	ELog.DlgMsg(mtInformation,"Building terminated.");
    else						ELog.DlgMsg(mtInformation,"Details succesfully exported.");

	return error_text.IsEmpty();
}
//------------------------------------------------------------------------------

BOOL SceneBuilder::MakeAIMap()
{
	AnsiString error_text;
    do{
		VERIFY_COMPILE(PreparePath(),				"Failed to prepare level path.");
		VERIFY_COMPILE(BuildAIMap(),				"Failed to build AI-Map.");
    }while(0);
    if (!error_text.IsEmpty()) 	ELog.DlgMsg(mtError,error_text.c_str());
    else if (UI.NeedAbort())	ELog.DlgMsg(mtInformation,"Building terminated.");
    else						ELog.DlgMsg(mtInformation,"AI-Map succesfully exported.");

	return error_text.IsEmpty();
}
//------------------------------------------------------------------------------

BOOL SceneBuilder::MakeDetails()
{
	AnsiString error_text;
    do{
		VERIFY_COMPILE(PreparePath(),				"Failed to prepare level path.");
        AnsiString fn			= m_LevelPath+"level.details";
        // save details
		VERIFY_COMPILE(Scene.GetMTools(OBJCLASS_DO)->Export(fn.c_str()), "Export failed.");
    }while(0);
    if (!error_text.IsEmpty()) 	ELog.DlgMsg(mtError,error_text.c_str());
    else if (UI.NeedAbort())	ELog.DlgMsg(mtInformation,"Building terminated.");
    else						ELog.DlgMsg(mtInformation,"Details succesfully exported.");

	return error_text.IsEmpty();
}
//------------------------------------------------------------------------------

BOOL SceneBuilder::MakeHOM( )
{
	AnsiString error_text="";
	UI.ResetBreak();
	if(UI.ContainEState(esBuildLevel)) return false;
	ELog.Msg( mtInformation, "Making started..." );

    UI.BeginEState(esBuildLevel);
    try{
        do{
        	// build
            VERIFY_COMPILE(PreparePath(),				"Failed to prepare level path.");
            VERIFY_COMPILE(BuildHOMModel(),				"Failed to build HOM model.");
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

