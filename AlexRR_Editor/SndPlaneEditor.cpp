//----------------------------------------------------
// file: SndPlaneEditor.cpp
//----------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "NetDeviceLog.h"
#include "SndPlaneEditor.h"
#include "SndPlane.h"
#include "SceneClassList.h"
#include "MenuTemplate.rh"

//----------------------------------------------------

SndPlaneEditor::SndPlaneEditor()
	:SceneClassEditor( IDD_EDITOR_SNDPLANE )
{
}

SndPlaneEditor::~SndPlaneEditor(){
}

//----------------------------------------------------

void SndPlaneEditor::GetObjectsInfo(){
	_ASSERTE( !m_Objects.empty() );
	
	SndPlane *_L = 0;
	ObjectIt _F = m_Objects.begin();

	_ASSERTE( (*_F)->ClassID()==OBJCLASS_SNDPLANE );
	_L = (SndPlane *)(*_F);

	m_Dencity.ObjFirsInit( _L->m_Dencity );
	_F++;
	
	for(;_F!=m_Objects.end();_F++){
		
		_ASSERTE( (*_F)->ClassID()==OBJCLASS_SNDPLANE );
		_L = (SndPlane *)(*_F);

		m_Dencity.ObjNextInit( _L->m_Dencity );
	}
}

void SndPlaneEditor::ApplyObjectsInfo(){
	_ASSERTE( !m_Objects.empty() );
	
	SndPlane *_L = 0;
	ObjectIt _F = m_Objects.begin();

	for(;_F!=m_Objects.end();_F++){

		_ASSERTE( (*_F)->ClassID()==OBJCLASS_SNDPLANE );
		_L = (SndPlane *)(*_F);

		m_Dencity.ObjApply( _L->m_Dencity );
	}
}

//----------------------------------------------------

void SndPlaneEditor::DlgExtract(){
	m_Dencity.DlgGet( m_DialogWindow, IDC_PLANE_DENCITY );
}

void SndPlaneEditor::DlgSet(){
	m_Dencity.DlgSet( m_DialogWindow, IDC_PLANE_DENCITY );
}

//----------------------------------------------------

void SndPlaneEditor::DlgInit( HWND hw ){
	GetObjectsInfo();
	DlgSet();
}

void SndPlaneEditor::Command( WPARAM wp, LPARAM lp ){
	
	switch( LOWORD(wp) ){
	
	case IDAPPLY:
		DlgExtract();
		ApplyObjectsInfo();
		GetObjectsInfo();
		DlgSet();
		break;

	case IDOK:
		DlgExtract();
		ApplyObjectsInfo();
		EndDialog( m_DialogWindow, IDOK );
		break;

	case IDCANCEL:
		EndDialog( m_DialogWindow, IDCANCEL );
		break;

	}
}

//----------------------------------------------------
