// SoundEditor.cpp: implementation of the CSoundEditor class.
//
//////////////////////////////////////////////////////////////////////
#include "Pch.h"
#pragma hdrstop

#include "NetDeviceLog.h"
#include "SoundEditor.h"
#include "Sound.h"
#include "SceneClassList.h"
#include "MenuTemplate.rh"

//----------------------------------------------------

CSoundEditor::CSoundEditor()
	:SceneClassEditor( IDD_EDITOR_SOUND )
{
}

CSoundEditor::~CSoundEditor(){
}

//----------------------------------------------------

void CSoundEditor::GetObjectsInfo(){
	_ASSERTE( !m_Objects.empty() );
	
	CSound *_L = 0;
	ObjectIt _F = m_Objects.begin();

	_ASSERTE( (*_F)->ClassID()==OBJCLASS_SOUND );
	_L = (CSound *)(*_F);

	m_Range.ObjFirsInit( _L->m_Range );
	_F++;
	
	for(;_F!=m_Objects.end();_F++){
		
		_ASSERTE( (*_F)->ClassID()==OBJCLASS_SOUND );
		_L = (CSound *)(*_F);

		m_Range.ObjNextInit( _L->m_Range );
	}
}

void CSoundEditor::ApplyObjectsInfo(){
	_ASSERTE( !m_Objects.empty() );
	
	CSound *_L = 0;
	ObjectIt _F = m_Objects.begin();

	for(;_F!=m_Objects.end();_F++){

		_ASSERTE( (*_F)->ClassID()==OBJCLASS_SOUND );
		_L = (CSound *)(*_F);

		m_Range.ObjApply( _L->m_Range );
	}
}

//----------------------------------------------------

void CSoundEditor::DlgExtract(){
	m_Range.DlgGet( m_DialogWindow, IDC_SOUND_RANGE );
}

void CSoundEditor::DlgSet(){
	m_Range.DlgSet( m_DialogWindow, IDC_SOUND_RANGE );
}

//----------------------------------------------------

void CSoundEditor::DlgInit( HWND hw ){
	GetObjectsInfo();
	DlgSet();
}

void CSoundEditor::Command( WPARAM wp, LPARAM lp ){
	
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
