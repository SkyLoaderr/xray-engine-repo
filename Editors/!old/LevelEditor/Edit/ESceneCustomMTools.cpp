#include "stdafx.h"
#pragma hdrstop

#include "ESceneCustomMTools.h"

ESceneCustomMTools::ESceneCustomMTools(EObjClass cls)
{
    ClassID				= cls;
    // controls
    sub_target			= 0;
    pCurControl 		= 0;
    pFrame				= 0;
    action				= -1;
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

bool ESceneCustomMTools::Load			(IReader&){return true;}
void ESceneCustomMTools::Save           (IWriter&){;}
bool ESceneCustomMTools::LoadSelection  (IReader&){return true;}
void ESceneCustomMTools::SaveSelection  (IWriter&){;}

