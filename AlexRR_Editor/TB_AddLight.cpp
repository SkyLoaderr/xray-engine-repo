//----------------------------------------------------
// file: TB_AddLight.cpp
//----------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "NetDeviceLog.h"
#include "CustomControls.h"
#include "UI_Main.h"
#include "Scene.h"
#include "Light.h"
#include "MSC_Main.h"
#include "MSC_AddLight.h"
#include "MenuTemplate.rh"

//----------------------------------------------------

TB_AddLight::TB_AddLight()
	:XRayEditorUIToolBox()
{
}

TB_AddLight::~TB_AddLight(){
}

//----------------------------------------------------

void TB_AddLight::Command( WPARAM wp, LPARAM lp ){

	SceneObject *t = 0;

	switch( LOWORD( wp ) ){

	case IDC_LIGHT_DIR:
		MSC_AddLight.m_Directional = IsChecked(m_ChildDialog,IDC_LIGHT_DIR);
		break;

	case IDC_LIGHT_GRID_ALIGN:
		MSC_AddLight.m_GridAlign = IsChecked(m_ChildDialog,IDC_LIGHT_GRID_ALIGN);
		break;
	}
}

void TB_AddLight::Init( HWND hw ){
	SetCheck(hw,IDC_LIGHT_DIR,MSC_AddLight.m_Directional);
	SetCheck(hw,IDC_LIGHT_GRID_ALIGN,MSC_AddLight.m_GridAlign);
}

//----------------------------------------------------
