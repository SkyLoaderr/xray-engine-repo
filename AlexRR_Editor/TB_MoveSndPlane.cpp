//----------------------------------------------------
// file: TB_MoveSndPlane.cpp
//----------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "NetDeviceLog.h"
#include "CustomControls.h"
#include "UI_Main.h"
#include "Scene.h"
#include "Texture.h"
#include "MSC_Main.h"
#include "MSC_MoveSndPlane.h"
#include "MenuTemplate.rh"

//----------------------------------------------------

TB_MoveSndPlane::TB_MoveSndPlane()
	:XRayEditorUIToolBox()
{
}

TB_MoveSndPlane::~TB_MoveSndPlane(){
}

//----------------------------------------------------

void TB_MoveSndPlane::Command( WPARAM wp, LPARAM lp ){

	switch( LOWORD( wp ) ){
	case IDC_RESTRICT_X:
		MSC_MoveSndPlane.m_AxisEnable[0] = !IsChecked(m_ChildDialog,IDC_RESTRICT_X);
		break;
	case IDC_RESTRICT_Y:
		MSC_MoveSndPlane.m_AxisEnable[1] = !IsChecked(m_ChildDialog,IDC_RESTRICT_Y);
		break;
	case IDC_RESTRICT_Z:
		MSC_MoveSndPlane.m_AxisEnable[2] = !IsChecked(m_ChildDialog,IDC_RESTRICT_Z);
		break;
	case IDC_SNAP:
		MSC_MoveSndPlane.m_Snap = !!IsChecked(m_ChildDialog,IDC_SNAP);
		break;
	}
}

void TB_MoveSndPlane::Init( HWND hw ){
	SetCheck(hw,IDC_RESTRICT_X,!MSC_MoveSndPlane.m_AxisEnable[0]);
	SetCheck(hw,IDC_RESTRICT_Y,!MSC_MoveSndPlane.m_AxisEnable[1]);
	SetCheck(hw,IDC_RESTRICT_Z,!MSC_MoveSndPlane.m_AxisEnable[2]);
	SetCheck(hw,IDC_SNAP,!!MSC_MoveSndPlane.m_Snap);
}

//----------------------------------------------------
