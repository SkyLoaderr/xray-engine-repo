//----------------------------------------------------
// file: TB_AddSndPlane.cpp
//----------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "NetDeviceLog.h"
#include "UI_Main.h"
#include "MSC_AddSndPlane.h"
#include "Scene.h"
#include "MenuTemplate.rh"

//----------------------------------------------------

TB_AddSndPlane::TB_AddSndPlane()
	:XRayEditorUIToolBox()
{
}

TB_AddSndPlane::~TB_AddSndPlane(){
}

//----------------------------------------------------

void TB_AddSndPlane::Command( WPARAM wp, LPARAM lp ){

	switch( LOWORD( wp ) ){

	case IDC_BSIZE_1:
		MSC_AddSndPlane.m_PlaneSize = 1;
		UpdateBSButtons( m_ChildDialog );
		break;
	case IDC_BSIZE_2:
		MSC_AddSndPlane.m_PlaneSize = 2;
		UpdateBSButtons( m_ChildDialog );
		break;
	case IDC_BSIZE_3:
		MSC_AddSndPlane.m_PlaneSize = 3;
		UpdateBSButtons( m_ChildDialog );
		break;
	case IDC_BSIZE_4:
		MSC_AddSndPlane.m_PlaneSize = 4;
		UpdateBSButtons( m_ChildDialog );
		break;
	}
}

void TB_AddSndPlane::Init( HWND hw ){
	UpdateBSButtons( hw );
}

//----------------------------------------------------

void TB_AddSndPlane::UpdateBSButtons( HWND hw ){
	CheckDlgButton( hw, IDC_BSIZE_1, MSC_AddSndPlane.m_PlaneSize == 1 );
	CheckDlgButton( hw, IDC_BSIZE_2, MSC_AddSndPlane.m_PlaneSize == 2 );
	CheckDlgButton( hw, IDC_BSIZE_3, MSC_AddSndPlane.m_PlaneSize == 3 );
	CheckDlgButton( hw, IDC_BSIZE_4, MSC_AddSndPlane.m_PlaneSize == 4 );
}

