//----------------------------------------------------
// file: LightEditor.cpp
//----------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "NetDeviceLog.h"
#include "LightEditor.h"
#include "Light.h"
#include "SceneClassList.h"
#include "MenuTemplate.rh"

//----------------------------------------------------

LightEditor::LightEditor()
	:SceneClassEditor( IDD_EDITOR_LIGHT )
{
}

LightEditor::~LightEditor(){
}

//----------------------------------------------------

void LightEditor::GetObjectsInfo(){
	_ASSERTE( !m_Objects.empty() );
	
	Light *_L = 0;
	ObjectIt _F = m_Objects.begin();

	_ASSERTE( (*_F)->ClassID()==OBJCLASS_LIGHT );
	_L = (Light *)(*_F);

	m_CtlCastShadows.ObjFirsInit( _L->m_CastShadows );
	m_CtlDirectional.ObjFirsInit( _L->m_Directional );

	m_CtlAmbient.ObjFirsInit( _L->m_Ambient );
	m_CtlDiffuse.ObjFirsInit( _L->m_Diffuse );
	m_CtlSpecular.ObjFirsInit( _L->m_Specular );

	m_CtlRange.ObjFirsInit( _L->m_Range );
	m_CtlAttenuation0.ObjFirsInit( _L->m_Attenuation0 );
	m_CtlAttenuation1.ObjFirsInit( _L->m_Attenuation1 );
	m_CtlAttenuation2.ObjFirsInit( _L->m_Attenuation2 );

	_F++;
	
	for(;_F!=m_Objects.end();_F++){

		_ASSERTE( (*_F)->ClassID()==OBJCLASS_LIGHT );
		_L = (Light *)(*_F);

		m_CtlCastShadows.ObjNextInit( _L->m_CastShadows );
		m_CtlDirectional.ObjNextInit( _L->m_Directional );

		m_CtlAmbient.ObjNextInit( _L->m_Ambient );
		m_CtlDiffuse.ObjNextInit( _L->m_Diffuse );
		m_CtlSpecular.ObjNextInit( _L->m_Specular );

		m_CtlRange.ObjNextInit( _L->m_Range );
		m_CtlAttenuation0.ObjNextInit( _L->m_Attenuation0 );
		m_CtlAttenuation1.ObjNextInit( _L->m_Attenuation1 );
		m_CtlAttenuation2.ObjNextInit( _L->m_Attenuation2 );
	}
}

void LightEditor::ApplyObjectsInfo(){
	_ASSERTE( !m_Objects.empty() );
	
	Light *_L = 0;
	ObjectIt _F = m_Objects.begin();

	for(;_F!=m_Objects.end();_F++){

		_ASSERTE( (*_F)->ClassID()==OBJCLASS_LIGHT );
		_L = (Light *)(*_F);

		m_CtlCastShadows.ObjApply( _L->m_CastShadows );
		m_CtlDirectional.ObjApply( _L->m_Directional );

		m_CtlAmbient.ObjApply( _L->m_Ambient );
		m_CtlDiffuse.ObjApply( _L->m_Diffuse );
		m_CtlSpecular.ObjApply( _L->m_Specular );

		m_CtlRange.ObjApply( _L->m_Range );
		m_CtlAttenuation0.ObjApply( _L->m_Attenuation0 );
		m_CtlAttenuation1.ObjApply( _L->m_Attenuation1 );
		m_CtlAttenuation2.ObjApply( _L->m_Attenuation2 );

		_L->FillD3DParams();
	}
}

//----------------------------------------------------

void LightEditor::DlgExtract(){
	m_CtlCastShadows.DlgGet( m_DialogWindow, IDC_LIGHT_CAST_SHADOWS );
	m_CtlDirectional.DlgGet( m_DialogWindow, IDC_LIGHT_DIRECTIONAL );
	m_CtlRange.DlgGet( m_DialogWindow, IDC_LIGHT_RANGE );
	m_CtlAttenuation0.DlgGet( m_DialogWindow, IDC_LIGHT_ATT0 );
	m_CtlAttenuation1.DlgGet( m_DialogWindow, IDC_LIGHT_ATT1 );
	m_CtlAttenuation2.DlgGet( m_DialogWindow, IDC_LIGHT_ATT2 );
}

void LightEditor::DlgSet(){
	m_CtlCastShadows.DlgSet( m_DialogWindow, IDC_LIGHT_CAST_SHADOWS );
	m_CtlDirectional.DlgSet( m_DialogWindow, IDC_LIGHT_DIRECTIONAL );
	m_CtlRange.DlgSet( m_DialogWindow, IDC_LIGHT_RANGE );
	m_CtlAttenuation0.DlgSet( m_DialogWindow, IDC_LIGHT_ATT0 );
	m_CtlAttenuation1.DlgSet( m_DialogWindow, IDC_LIGHT_ATT1 );
	m_CtlAttenuation2.DlgSet( m_DialogWindow, IDC_LIGHT_ATT2 );
}

//----------------------------------------------------

void LightEditor::DlgInit( HWND hw ){
	GetObjectsInfo();
	DlgSet();
}

void LightEditor::Command( WPARAM wp, LPARAM lp ){
	
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

	case IDC_LIGHT_AMBIENT:
		ChangeColor( m_CtlAmbient );
		InvalidateRect( GetDlgItem(m_DialogWindow,LOWORD(wp)), 0, 0 );
		break;

	case IDC_LIGHT_DIFFUSE:
		ChangeColor( m_CtlDiffuse );
		InvalidateRect( GetDlgItem(m_DialogWindow,LOWORD(wp)), 0, 0 );
		break;

	case IDC_LIGHT_SPECULAR:
		ChangeColor( m_CtlSpecular );
		InvalidateRect( GetDlgItem(m_DialogWindow,LOWORD(wp)), 0, 0 );
		break;
	}
}

void LightEditor::DrawItem( WPARAM wp, LPARAM lp ){

	LPDRAWITEMSTRUCT ds = (LPDRAWITEMSTRUCT)lp;

	MultiObjColor *color = 0;
	switch( (UINT)wp ){
		case IDC_LIGHT_AMBIENT: color = &m_CtlAmbient; break;
		case IDC_LIGHT_DIFFUSE: color = &m_CtlDiffuse; break;
		case IDC_LIGHT_SPECULAR: color = &m_CtlSpecular; break;
	}

	if( color ){
		if( color->diffs() ){
			MoveToEx(ds->hDC,0,0,0);
			LineTo(ds->hDC,ds->rcItem.right-1,0);
			LineTo(ds->hDC,ds->rcItem.right-1,ds->rcItem.bottom-1);
			LineTo(ds->hDC,0,ds->rcItem.bottom-1);
			LineTo(ds->hDC,0,0);

		} else {
			HBRUSH hbr = CreateSolidBrush( color->Get() );
			FillRect( ds->hDC, &ds->rcItem, hbr );
			DeleteObject( hbr );
		}
	}
}

void LightEditor::ChangeColor( MultiObjColor& color ){

	CHOOSECOLOR ccr;
	memset( &ccr, 0, sizeof(ccr) );

	ccr.lStructSize = sizeof(ccr);
	ccr.hwndOwner = m_DialogWindow;
	ccr.rgbResult = color.Get();
	ccr.lpCustColors = m_CustColors;
	ccr.Flags = CC_ANYCOLOR|CC_FULLOPEN|CC_RGBINIT;

	if( ChooseColor( &ccr ) )
		color.Set( ccr.rgbResult );
}

//----------------------------------------------------
