//----------------------------------------------------
// file: TB_AddObject.cpp
//----------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "NetDeviceLog.h"
#include "CustomControls.h"
#include "UI_Main.h"
#include "Scene.h"
#include "Library.h"
#include "SObject2.h"
#include "MSC_Main.h"
#include "MSC_AddObject.h"
#include "MenuTemplate.rh"

//----------------------------------------------------

TB_AddObject::TB_AddObject()
	:XRayEditorUIToolBox()
{
}

TB_AddObject::~TB_AddObject(){
}

//----------------------------------------------------

void TB_AddObject::Command( WPARAM wp, LPARAM lp ){

	SceneObject *t = 0;

	switch( LOWORD( wp ) ){

	case IDC_SELOBJ:
		t = UI.SelectSObject();
		if( t ){
			Lib.oset( t );
			SetDlgItemText(m_ChildDialog,IDC_OBJNAME,Lib.o()?Lib.o()->GetName():"<no object>" );
		}
		break;

	case IDC_OBJREF:
		MSC_AddObject.m_AddReference = IsChecked(m_ChildDialog,IDC_OBJREF);
		break;

	case IDC_OBJALIGN:
		MSC_AddObject.m_GridAlign = IsChecked(m_ChildDialog,IDC_OBJALIGN);
		break;

	case IDC_QUADALIGN:
		MSC_AddObject.m_QCenterAlign = IsChecked(m_ChildDialog,IDC_QUADALIGN);
		break;
	}
}

void TB_AddObject::Init( HWND hw ){
	SetDlgItemText(hw,IDC_OBJNAME,Lib.o()?Lib.o()->GetName():"<no object>" );
	SetCheck(hw,IDC_OBJALIGN,MSC_AddObject.m_GridAlign);
	SetCheck(hw,IDC_QUADALIGN,MSC_AddObject.m_QCenterAlign);
	SetCheck(hw,IDC_OBJREF,MSC_AddObject.m_AddReference);
}

//----------------------------------------------------
