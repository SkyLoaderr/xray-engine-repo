//----------------------------------------------------
// file: TB_AddSound.cpp
//----------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "NetDeviceLog.h"
#include "CustomControls.h"
#include "UI_Main.h"
#include "Scene.h"
#include "Sound.h"
#include "MSC_Main.h"
#include "MSC_AddSound.h"
#include "MenuTemplate.rh"

//----------------------------------------------------

TB_AddSound::TB_AddSound()
	:XRayEditorUIToolBox()
{
}

TB_AddSound::~TB_AddSound(){
}

//----------------------------------------------------

void TB_AddSound::Command( WPARAM wp, LPARAM lp ){
/*
	SceneObject *t = 0;

	switch( LOWORD( wp ) ){
	case IDC_SOUND_GRID_ALIGN:
		MSC_AddSound.m_GridAlign = IsChecked(m_ChildDialog,IDC_SOUND_GRID_ALIGN);
		break;
	}
*/
}

void TB_AddSound::Init( HWND hw ){
//	SetCheck(hw,IDC_SOUND_GRID_ALIGN,MSC_AddSound.m_GridAlign);
}

//----------------------------------------------------
