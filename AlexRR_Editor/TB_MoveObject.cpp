//----------------------------------------------------
// file: TB_MoveObject.cpp
//----------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "NetDeviceLog.h"
#include "CustomControls.h"
#include "UI_Main.h"
#include "Scene.h"
#include "Texture.h"
#include "MSC_Main.h"
#include "MSC_MoveObject.h"
#include "MenuTemplate.rh"

//----------------------------------------------------

TB_MoveObject::TB_MoveObject()
	:XRayEditorUIToolBox()
{
}

TB_MoveObject::~TB_MoveObject(){
}

//----------------------------------------------------

void TB_MoveObject::Command( WPARAM wp, LPARAM lp ){

	switch( LOWORD( wp ) ){
	case IDC_RESTRICT_X:
		MSC_MoveObject.m_AxisEnable[0] = !IsChecked(m_ChildDialog,IDC_RESTRICT_X);
		break;
	case IDC_RESTRICT_Y:
		MSC_MoveObject.m_AxisEnable[1] = !IsChecked(m_ChildDialog,IDC_RESTRICT_Y);
		break;
	case IDC_RESTRICT_Z:
		MSC_MoveObject.m_AxisEnable[2] = !IsChecked(m_ChildDialog,IDC_RESTRICT_Z);
		break;
	case IDC_SNAP:
		MSC_MoveObject.m_Snap = !!IsChecked(m_ChildDialog,IDC_SNAP);
		break;
	}
}

void TB_MoveObject::Init( HWND hw ){
	SetCheck(hw,IDC_RESTRICT_X,!MSC_MoveObject.m_AxisEnable[0]);
	SetCheck(hw,IDC_RESTRICT_Y,!MSC_MoveObject.m_AxisEnable[1]);
	SetCheck(hw,IDC_RESTRICT_Z,!MSC_MoveObject.m_AxisEnable[2]);
	SetCheck(hw,IDC_SNAP,!!MSC_MoveObject.m_Snap);
}

//----------------------------------------------------
