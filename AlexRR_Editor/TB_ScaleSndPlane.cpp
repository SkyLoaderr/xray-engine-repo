//----------------------------------------------------
// file: TB_ScaleSndPlane.cpp
//----------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "NetDeviceLog.h"
#include "CustomControls.h"
#include "UI_Main.h"
#include "Scene.h"
#include "Texture.h"
#include "MSC_Main.h"
#include "MSC_ScaleSndPlane.h"
#include "MenuTemplate.rh"

//----------------------------------------------------

TB_ScaleSndPlane::TB_ScaleSndPlane()
	:XRayEditorUIToolBox()
{
}

TB_ScaleSndPlane::~TB_ScaleSndPlane(){
}

//----------------------------------------------------

void TB_ScaleSndPlane::Command( WPARAM wp, LPARAM lp ){

	ETexture *t = 0;

	switch( LOWORD( wp ) ){
	case IDC_RESTRICT_X:
		MSC_ScaleSndPlane.m_AxisEnable[0] = !IsChecked(m_ChildDialog,IDC_RESTRICT_X);
		break;
	case IDC_RESTRICT_Y:
		MSC_ScaleSndPlane.m_AxisEnable[1] = !IsChecked(m_ChildDialog,IDC_RESTRICT_Y);
		break;
	case IDC_RESTRICT_Z:
		MSC_ScaleSndPlane.m_AxisEnable[2] = !IsChecked(m_ChildDialog,IDC_RESTRICT_Z);
		break;
	case IDC_LOCAL:
		MSC_ScaleSndPlane.m_Local = !!IsChecked(m_ChildDialog,IDC_LOCAL);
		break;
	}
}

void TB_ScaleSndPlane::Init( HWND hw ){
	SetCheck(hw,IDC_RESTRICT_X,!MSC_ScaleSndPlane.m_AxisEnable[0]);
	SetCheck(hw,IDC_RESTRICT_Y,!MSC_ScaleSndPlane.m_AxisEnable[1]);
	SetCheck(hw,IDC_RESTRICT_Z,!MSC_ScaleSndPlane.m_AxisEnable[2]);
	SetCheck(hw,IDC_LOCAL,MSC_ScaleSndPlane.m_Local);
}

//----------------------------------------------------
