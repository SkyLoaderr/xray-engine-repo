//----------------------------------------------------
// file: TB_ScaleLandscape.cpp
//----------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "NetDeviceLog.h"
#include "CustomControls.h"
#include "UI_Main.h"
#include "Scene.h"
#include "Texture.h"
#include "MSC_Main.h"
#include "MSC_ScaleLandscape.h"
#include "MenuTemplate.rh"

//----------------------------------------------------

TB_ScaleLandscape::TB_ScaleLandscape()
	:XRayEditorUIToolBox()
{
}

TB_ScaleLandscape::~TB_ScaleLandscape(){
}

//----------------------------------------------------

void TB_ScaleLandscape::Command( WPARAM wp, LPARAM lp ){

	ETexture *t = 0;

	switch( LOWORD( wp ) ){
	case IDC_RESTRICT_X:
		MSC_ScaleLandscape.m_AxisEnable[0] = !IsChecked(m_ChildDialog,IDC_RESTRICT_X);
		break;
	case IDC_RESTRICT_Y:
		MSC_ScaleLandscape.m_AxisEnable[1] = !IsChecked(m_ChildDialog,IDC_RESTRICT_Y);
		break;
	case IDC_RESTRICT_Z:
		MSC_ScaleLandscape.m_AxisEnable[2] = !IsChecked(m_ChildDialog,IDC_RESTRICT_Z);
		break;
	}
}

void TB_ScaleLandscape::Init( HWND hw ){
	SetCheck(hw,IDC_RESTRICT_X,!MSC_ScaleLandscape.m_AxisEnable[0]);
	SetCheck(hw,IDC_RESTRICT_Y,!MSC_ScaleLandscape.m_AxisEnable[1]);
	SetCheck(hw,IDC_RESTRICT_Z,!MSC_ScaleLandscape.m_AxisEnable[2]);
}

//----------------------------------------------------
