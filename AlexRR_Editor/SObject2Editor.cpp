//----------------------------------------------------
// file: SObject2Editor.cpp
//----------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "NetDeviceLog.h"
#include "SObject2Editor.h"
#include "SObject2.h"
#include "SceneClassList.h"
#include "MenuTemplate.rh"

//----------------------------------------------------

SObject2Editor::SObject2Editor()
	:SceneClassEditor( IDD_EDITOR_OBJECT )
{
}

SObject2Editor::~SObject2Editor(){
}

//----------------------------------------------------

void SObject2Editor::GetObjectsInfo(){
	_ASSERTE( !m_Objects.empty() );
	
	m_MultiSelection = false;

	SObject2 *_O = 0;
	ObjectIt _F = m_Objects.begin();

	_ASSERTE( (*_F)->ClassID()==OBJCLASS_SOBJECT2 );
	_O = (SObject2 *)(*_F);

	if( m_Reference = _O->IsReference() ){
		strcpy( m_RefName, _O->GetRefName() );
	} else {
		m_RefName[0] = 0;
	}

	strcpy( m_ObjectScript, _O->GetClassScript() );
	strcpy( m_ObjectName, _O->GetName() );
	strcpy( m_ObjectClass, _O->GetClassName() );

	m_MakeUnique.ObjFirsInit( _O->m_MakeUnique );
	m_DynamicList.ObjFirsInit( _O->m_DynamicList );

	_F++;
	
	for(;_F!=m_Objects.end();_F++){
		_ASSERTE( (*_F)->ClassID()==OBJCLASS_SOBJECT2 );
		m_MultiSelection = true;
		m_MakeUnique.ObjNextInit( _O->m_MakeUnique );
		m_DynamicList.ObjNextInit( _O->m_DynamicList );
	}

	if( !m_MultiSelection ){
		_O = (SObject2 *) m_Objects.front();
		if( !_O->IsReference() ){
			m_EObject = _O;
		} else {
			m_EObject = 0;
		}
	} else {
		m_EObject = 0;
	}
}

void SObject2Editor::ApplyObjectsInfo(){
	_ASSERTE( !m_Objects.empty() );
	
	SObject2 *_O = 0;
	ObjectIt _F = m_Objects.begin();

	for(;_F!=m_Objects.end();_F++){

		_ASSERTE( (*_F)->ClassID()==OBJCLASS_SOBJECT2 );
		_O = (SObject2 *)(*_F);

		m_MakeUnique.ObjApply( _O->m_MakeUnique );
		m_DynamicList.ObjApply( _O->m_DynamicList );

		if( !m_MultiSelection ){
			strcpy( _O->GetClassScript(), m_ObjectScript );
			strcpy( _O->GetName(), m_ObjectName );
			strcpy( _O->GetClassName(), m_ObjectClass );
		}
	}
}

//----------------------------------------------------

void SObject2Editor::DlgExtract(){

	m_MakeUnique.DlgGet( m_DialogWindow, IDC_MAKE_UNIQUE );
	m_DynamicList.DlgGet( m_DialogWindow, IDC_MAKE_DYNAMIC );
	
	if( !m_MultiSelection ){
		GetDlgItemText( m_DialogWindow, IDC_OBJ_NAME, m_ObjectName, sizeof(m_ObjectName) );
		GetDlgItemText( m_DialogWindow, IDC_OBJ_CLASS, m_ObjectClass, sizeof(m_ObjectClass) );
		GetDlgItemText( m_DialogWindow, IDC_OBJ_SCRIPT, m_ObjectScript, sizeof(m_ObjectScript) );
	}
}

void SObject2Editor::DlgSet(){

	m_MakeUnique.DlgSet( m_DialogWindow, IDC_MAKE_UNIQUE );
	m_DynamicList.DlgSet( m_DialogWindow, IDC_MAKE_DYNAMIC );
	
	if( m_MultiSelection ){
		SetDlgItemText( m_DialogWindow, IDC_OBJ_NAME, "<Multiple selection>" );
		SetDlgItemText( m_DialogWindow, IDC_OBJ_CLASS, "<Multiple selection>");
		SetDlgItemText( m_DialogWindow, IDC_OBJ_SCRIPT, "<Multiple selection>" );

	} else {
		SetDlgItemText( m_DialogWindow, IDC_OBJ_NAME, m_ObjectName );
		SetDlgItemText( m_DialogWindow, IDC_OBJ_CLASS, m_ObjectClass );
		SetDlgItemText( m_DialogWindow, IDC_OBJ_SCRIPT, m_ObjectScript );
	}

	EnableWindow( GetDlgItem(m_DialogWindow,IDC_OBJ_NAME), !m_MultiSelection );
	EnableWindow( GetDlgItem(m_DialogWindow,IDC_OBJ_CLASS), !m_MultiSelection );
	EnableWindow( GetDlgItem(m_DialogWindow,IDC_OBJ_SCRIPT), !m_MultiSelection );

	if( !m_MultiSelection ){
		if( m_Reference ){
			char b[256];
			sprintf(b,"Object properties - reference to '%s'", m_RefName );
			SetWindowText( m_DialogWindow, b );
		} else {
			SetWindowText( m_DialogWindow, "Object properties" );
		}
	} else {
		SetWindowText( m_DialogWindow, "Object properties - multiple selection" );
	}


	if( m_EObject ){

		EnableWindow( GetDlgItem(m_DialogWindow,IDC_ELEMENT_LIST), TRUE );
		EnableWindow( GetDlgItem(m_DialogWindow,IDC_EDIT_ELEMENT), TRUE );

		SendDlgItemMessage( m_DialogWindow, IDC_ELEMENT_LIST, LB_RESETCONTENT, 0, 0 );
		SObjMeshIt it = m_EObject->m_Meshes.begin();
		for(;it!=m_EObject->m_Meshes.end();it++){
			char buffer[MAX_PATH];
			sprintf( buffer, "%s (%s)", it->m_Name, it->m_FileName );
			int itemid = SendDlgItemMessage(
				m_DialogWindow, IDC_ELEMENT_LIST,
				LB_ADDSTRING, 0, (LPARAM)buffer );
			if( itemid != LB_ERR ){
				SendDlgItemMessage(
					m_DialogWindow, IDC_ELEMENT_LIST,
					LB_SETITEMDATA, itemid, (LPARAM)&(*it) );
			}
		}
	} else {
		EnableWindow( GetDlgItem(m_DialogWindow,IDC_ELEMENT_LIST), FALSE );
		EnableWindow( GetDlgItem(m_DialogWindow,IDC_EDIT_ELEMENT), FALSE );
	}
}

//----------------------------------------------------

void SObject2Editor::DlgInit( HWND hw ){
	GetObjectsInfo();
	DlgSet();
}

void SObject2Editor::Command( WPARAM wp, LPARAM lp ){

	int itemid;
	SObject2Mesh *mdef;
	
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

	case IDC_EDIT_ELEMENT:
		itemid = SendDlgItemMessage(
			m_DialogWindow, IDC_ELEMENT_LIST,
			LB_GETCURSEL, 0, 0 );
		if( itemid != LB_ERR ){
			mdef = (SObject2Mesh*)SendDlgItemMessage(
				m_DialogWindow, IDC_ELEMENT_LIST,
				LB_GETITEMDATA, itemid, 0 );
			mdef->m_Ops.RunEditor( m_Instance, 
				m_DialogWindow );
		}
		break;
	}
}

//----------------------------------------------------
