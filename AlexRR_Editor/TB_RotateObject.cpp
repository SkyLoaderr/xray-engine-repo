//----------------------------------------------------
// file: TB_RotateObject.cpp
//----------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "NetDeviceLog.h"
#include "CustomControls.h"
#include "UI_Main.h"
#include "Scene.h"
#include "Texture.h"
#include "MSC_Main.h"
#include "MSC_RotateObject.h"
#include "MenuTemplate.rh"

//----------------------------------------------------

TB_RotateObject::TB_RotateObject()
	:XRayEditorUIToolBox()
{
	m_RXv.set(1,0,0);
	m_RYv.set(0,1,0);
	m_RZv.set(0,0,1);
}

TB_RotateObject::~TB_RotateObject(){
}

//----------------------------------------------------

void TB_RotateObject::Command( WPARAM wp, LPARAM lp ){

	switch( LOWORD( wp ) ){
	case IDC_ROTATE_X:
		MSC_RotateObject.m_RVector.set(m_RXv);
		Init( m_ChildDialog );
		break;
	case IDC_ROTATE_Y:
		MSC_RotateObject.m_RVector.set(m_RYv);
		Init( m_ChildDialog );
		break;
	case IDC_ROTATE_Z:
		MSC_RotateObject.m_RVector.set(m_RZv);
		Init( m_ChildDialog );
		break;
	case IDC_LOCAL:
		MSC_RotateObject.m_Local = !!IsChecked(m_ChildDialog,IDC_LOCAL);
		break;
	case IDC_ASNAP:
		MSC_RotateObject.m_ASnap = !!IsChecked(m_ChildDialog,IDC_ASNAP);
		break;
	}
}

void TB_RotateObject::Init( HWND hw ){
	SetCheck(hw,IDC_ROTATE_X,MSC_RotateObject.m_RVector.cmp(m_RXv) );
	SetCheck(hw,IDC_ROTATE_Y,MSC_RotateObject.m_RVector.cmp(m_RYv) );
	SetCheck(hw,IDC_ROTATE_Z,MSC_RotateObject.m_RVector.cmp(m_RZv) );
	SetCheck(hw,IDC_LOCAL,MSC_RotateObject.m_Local);
	SetCheck(hw,IDC_ASNAP,MSC_RotateObject.m_ASnap );
}

//----------------------------------------------------
