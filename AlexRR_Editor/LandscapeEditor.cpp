//----------------------------------------------------
// file: LandscapeEditor.cpp
//----------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "NetDeviceLog.h"
#include "LandscapeEditor.h"
#include "Landscape.h"
#include "SceneClassList.h"
#include "MenuTemplate.rh"

//----------------------------------------------------

SLandscapeEditor::SLandscapeEditor()
	:SceneClassEditor( IDD_EDITOR_LANDSCAPE )
{
	for(int i=0;i<16;i++)
		m_CustColors[i] = RGB(i*16,i*16,i*16);
}

SLandscapeEditor::~SLandscapeEditor(){
}

//----------------------------------------------------

void SLandscapeEditor::GetObjectsInfo(){
	_ASSERTE( !m_Objects.empty() );
	
	SLandscape *_L = 0;
	ObjectIt _F = m_Objects.begin();

	_ASSERTE( (*_F)->ClassID()==OBJCLASS_LANDSCAPE );
	_L = (SLandscape *)(*_F);

	m_Ambient.ObjFirsInit( _L->m_Ambient );
	m_Diffuse.ObjFirsInit( _L->m_Diffuse );
	m_Specular.ObjFirsInit( _L->m_Specular );
	m_Emission.ObjFirsInit( _L->m_Emission );
	m_Power.ObjFirsInit( _L->m_Power );
	m_Ops.Copy( _L->m_Ops );
	_F++;
	
	for(;_F!=m_Objects.end();_F++){

		_ASSERTE( (*_F)->ClassID()==OBJCLASS_LANDSCAPE );
		_L = (SLandscape *)(*_F);

		m_Ambient.ObjNextInit( _L->m_Ambient );
		m_Diffuse.ObjNextInit( _L->m_Diffuse );
		m_Specular.ObjNextInit( _L->m_Specular );
		m_Emission.ObjNextInit( _L->m_Emission );
		m_Power.ObjNextInit( _L->m_Power );
	}
}

void SLandscapeEditor::ApplyObjectsInfo(){
	_ASSERTE( !m_Objects.empty() );
	
	SLandscape *_L = 0;
	ObjectIt _F = m_Objects.begin();

	for(;_F!=m_Objects.end();_F++){

		_ASSERTE( (*_F)->ClassID()==OBJCLASS_LANDSCAPE );
		_L = (SLandscape *)(*_F);

		m_Ambient.ObjApply( _L->m_Ambient );
		m_Diffuse.ObjApply( _L->m_Diffuse );
		m_Specular.ObjApply( _L->m_Specular );
		m_Emission.ObjApply( _L->m_Emission );
		m_Power.ObjApply( _L->m_Power );

		_L->m_Ops.Copy( m_Ops );
	}
}

//----------------------------------------------------

void SLandscapeEditor::DlgExtract(){
	m_Power.DlgGet( m_DialogWindow, IDC_LAND_POWER );
}

void SLandscapeEditor::DlgSet(){
	m_Power.DlgSet( m_DialogWindow, IDC_LAND_POWER );
}

//----------------------------------------------------

void SLandscapeEditor::DlgInit( HWND hw ){
	GetObjectsInfo();
	DlgSet();
}

void SLandscapeEditor::Command( WPARAM wp, LPARAM lp ){
	
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

	case IDC_LAND_AMBIENT:
		ChangeColor( m_Ambient );
		InvalidateRect( GetDlgItem(m_DialogWindow,LOWORD(wp)), 0, 0 );
		break;

	case IDC_LAND_DIFFUSE:
		ChangeColor( m_Diffuse );
		InvalidateRect( GetDlgItem(m_DialogWindow,LOWORD(wp)), 0, 0 );
		break;

	case IDC_LAND_SPECULAR:
		ChangeColor( m_Specular );
		InvalidateRect( GetDlgItem(m_DialogWindow,LOWORD(wp)), 0, 0 );
		break;

	case IDC_LAND_EMISSION:
		ChangeColor( m_Emission );
		InvalidateRect( GetDlgItem(m_DialogWindow,LOWORD(wp)), 0, 0 );
		break;

	case IDC_OBJ_OPS:
		m_Ops.RunEditor( m_Instance, m_DialogWindow );
		break;

	}
}

void SLandscapeEditor::Notify( WPARAM wp, LPARAM lp ){
}

void SLandscapeEditor::DrawItem( WPARAM wp, LPARAM lp ){

	LPDRAWITEMSTRUCT ds = (LPDRAWITEMSTRUCT)lp;

	MultiObjColor *color = 0;
	switch( (UINT)wp ){
		case IDC_LAND_AMBIENT: color = &m_Ambient; break;
		case IDC_LAND_DIFFUSE: color = &m_Diffuse; break;
		case IDC_LAND_SPECULAR: color = &m_Specular; break;
		case IDC_LAND_EMISSION: color = &m_Emission; break;
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

//----------------------------------------------------

void SLandscapeEditor::ChangeColor( MultiObjColor& color ){

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
