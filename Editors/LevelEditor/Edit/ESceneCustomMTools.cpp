#include "stdafx.h"
#pragma hdrstop

#include "ESceneCustomMTools.h"
 
ESceneCustomMTools::ESceneCustomMTools(EObjClass cls)
{
    ClassID				= cls;
    m_Flags.set			(flVisible);
    // controls
    sub_target			= 0;
    pCurControl 		= 0;
    pFrame				= 0;
    action				= -1;
}

ESceneCustomMTools::~ESceneCustomMTools()
{
	OnDestroy			();
}
 
void ESceneCustomMTools::OnCreate()
{
    CreateControls		();
}
void ESceneCustomMTools::OnDestroy()
{
    RemoveControls		();
}

