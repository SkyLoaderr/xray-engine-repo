#include "stdafx.h"
#pragma hdrstop

#include "ESceneCustomMTools.h"
#include "ui_main.h"
#include "ui_levelmain.h"
#include "scene.h"

ESceneCustomMTools::ESceneCustomMTools(ObjClassID cls)
{
    ClassID				= cls;
    // controls
    sub_target			= 0;
    pCurControl 		= 0;
    pFrame				= 0;
    action				= -1;
    m_bEnabled			= TRUE;
}

ESceneCustomMTools::~ESceneCustomMTools()
{
}
 
void ESceneCustomMTools::OnCreate()
{
    OnDeviceCreate		();
    CreateControls		();
}
void ESceneCustomMTools::OnDestroy()
{
    OnDeviceDestroy		();
    RemoveControls		();
}

BOOL ESceneCustomMTools::Enable(BOOL val)
{
    m_bEnabled			= val;
	if (val){
    	BOOL bRes 		= ExecCommand(COMMAND_LOAD_LEVEL_PART,ClassID);
        ExecCommand		(COMMAND_REFRESH_UI_BAR);
        return			bRes;
    }else{
        if (!Scene->IfModified()){
        	m_bEnabled	= TRUE;
            return		FALSE;
        }else{
			Clear		();
		    ExecCommand	(COMMAND_CHANGE_TARGET,OBJCLASS_SCENEOBJECT);
			ExecCommand	(COMMAND_REFRESH_UI_BAR);
            return		TRUE;
        }
    }
}

