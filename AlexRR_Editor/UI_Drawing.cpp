//----------------------------------------------------
// file: UI_Drawing.cpp
//----------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "NetDeviceLog.h"
#include "UI_Main.h"
#include "CustomControls.h"
#include "MenuTemplate.rh"

//----------------------------------------------------

void XRayEditorUI::ResetToDefaultColors(){
	m_ColorFore_B.set( 191, 255, 191 );
	m_ColorBack_S.set( 140, 116, 175 );
	m_ColorFore_S.set( 191, 255, 191 );
	m_BackgroundColor.set( 127, 127, 127 );
	m_ColorGrid.set( 150, 150, 150 );
	m_ColorGridTh.set( 180, 180, 180 );
}

void XRayEditorUI::ResetToDefaultGrid(){
	m_GridCenter.set( 0, 0 );
	m_GridStep.set( 2.f, 2.f );
	m_GridSubDiv[0] = 8;
	m_GridSubDiv[1] = 8;
	m_GridCounts[0] = 127;
	m_GridCounts[1] = 127;
	D3D_UpdateGrid();
}

//----------------------------------------------------

void XRayEditorUI::DrawButton( LPDRAWITEMSTRUCT _DS ){

	char _Text[256] = "";
	GetWindowText( _DS->hwndItem, _Text, 255 );

	IColor topcolor,botcolor;
	IColor defcolor,textcolor;

	if( (_DS->itemState&ODS_SELECTED) ){
		textcolor.set( g_HButtonTextColor );
		defcolor.set( g_HButtonForeColor );
	} else {
		textcolor.set( g_ButtonTextColor );
		defcolor.set( g_ButtonForeColor );
	}

	topcolor.set( defcolor );
	botcolor.set( defcolor );

	if( (_DS->itemState&ODS_SELECTED) ){
		topcolor.scale( 0.75f );
		botcolor.scale( 1.25f );
	} else {
		topcolor.scale( 1.25f );
		botcolor.scale( 0.75f );
	}

	HBRUSH hbr = CreateSolidBrush(defcolor.colorref());
	FillRect( _DS->hDC, &_DS->rcItem, hbr );
	DeleteObject( hbr );

	HPEN hp0_up = CreatePen( PS_SOLID, 1, topcolor.colorref() );
	HPEN hp0_down = CreatePen( PS_SOLID, 1, botcolor.colorref() );

	MoveToEx( _DS->hDC,_DS->rcItem.left,_DS->rcItem.bottom-1,0);

	hp0_up = (HPEN)SelectObject( _DS->hDC, hp0_up );
	LineTo( _DS->hDC,_DS->rcItem.left,_DS->rcItem.top);
	LineTo( _DS->hDC,_DS->rcItem.right-1,_DS->rcItem.top);
	hp0_up = (HPEN)SelectObject( _DS->hDC, hp0_up );

	hp0_down = (HPEN)SelectObject( _DS->hDC, hp0_down );
	LineTo( _DS->hDC,_DS->rcItem.right-1,_DS->rcItem.bottom-1);
	LineTo( _DS->hDC,_DS->rcItem.left,_DS->rcItem.bottom-1);
	hp0_down = (HPEN)SelectObject( _DS->hDC, hp0_down );

	DeleteObject( hp0_up );
	DeleteObject( hp0_down );

	if( _Text[0] ){
		HFONT oldfont = (HFONT)SelectObject( _DS->hDC, g_ButtonFont );
		SetTextColor(_DS->hDC,textcolor.colorref());
		SetBkMode(_DS->hDC, TRANSPARENT );
		SetTextAlign(_DS->hDC, TA_CENTER | TA_BOTTOM );
		TextOut(_DS->hDC,_DS->rcItem.right/2,_DS->rcItem.bottom,_Text,strlen(_Text));
		SelectObject(_DS->hDC,oldfont);
	}
}

//----------------------------------------------------


void XRayEditorUI::GoldFill( HDC hdc, RECT *rc ){
	HBRUSH hbr = CreateSolidBrush(g_BackColor.colorref());
	FillRect( hdc, rc, hbr );
	DeleteObject( hbr );
}

//----------------------------------------------------

void XRayEditorUI::DrawGoldText( LPDRAWITEMSTRUCT _DS ){

	char _Text[256] = "";
	GetWindowText( _DS->hwndItem, _Text, 255 );

	HBRUSH hbr = CreateSolidBrush(m_ColorBack_S.colorref());
	FillRect( _DS->hDC, &_DS->rcItem, hbr );
	DeleteObject( hbr );

	if( _Text[0] ){
		SetTextColor(_DS->hDC,m_ColorFore_S.colorref());
		SetBkMode(_DS->hDC, TRANSPARENT );
		SetTextAlign(_DS->hDC, TA_CENTER | TA_BOTTOM );
		TextOut(_DS->hDC,_DS->rcItem.right/2,_DS->rcItem.bottom,_Text,strlen(_Text));
	}
}

//----------------------------------------------------

void XRayEditorUI::DrawCheck( LPDRAWITEMSTRUCT _DS, int _Selected ){
	DrawButton( _DS );

	BYTE _R = _Selected?255:127;
	BYTE _G = _Selected?127:255;
	BYTE _B = _Selected?127:127;

	COLORREF maincolor = RGB(_R,_G,_B);
	COLORREF downcolor0 = RGB(_R*2,_G*2,_B*2);
	COLORREF upcolor0 = RGB(_R/2,_G/2,_B/2);

	RECT rc1/*,rc2*/;

	rc1.left = _DS->rcItem.left + 2;
	rc1.right = _DS->rcItem.bottom - 2;
	rc1.top = _DS->rcItem.top + 2;
	rc1.bottom = _DS->rcItem.bottom - 2;

	/*rc2.left = _DS->rcItem.right - (_DS->rcItem.bottom - 2);
	rc2.right = _DS->rcItem.right - 2;
	rc2.top = rc1.top;
	rc2.bottom = rc1.bottom;*/

	HBRUSH hbr = CreateSolidBrush(maincolor);
	FillRect( _DS->hDC, &rc1, hbr );
	//FillRect( _DS->hDC, &rc2, hbr );
	DeleteObject( hbr );

	HPEN hp0_up = CreatePen( PS_SOLID, 1, upcolor0 );
	HPEN hp0_down = CreatePen( PS_SOLID, 1, downcolor0 );


	MoveToEx( _DS->hDC,rc1.left,rc1.bottom-1,0);

	hp0_up = (HPEN)SelectObject( _DS->hDC, hp0_up );
	LineTo( _DS->hDC,rc1.left,rc1.top);
	LineTo( _DS->hDC,rc1.right-1,rc1.top);
	hp0_up = (HPEN)SelectObject( _DS->hDC, hp0_up );

	hp0_down = (HPEN)SelectObject( _DS->hDC, hp0_down );
	LineTo( _DS->hDC,rc1.right-1,rc1.bottom-1);
	LineTo( _DS->hDC,rc1.left,rc1.bottom-1);
	hp0_down = (HPEN)SelectObject( _DS->hDC, hp0_down );

/*	
	MoveToEx( _DS->hDC,rc2.left,rc2.bottom-1,0);
	
	hp0_up = (HPEN)SelectObject( _DS->hDC, hp0_up );
	LineTo( _DS->hDC,rc2.left,rc2.top);
	LineTo( _DS->hDC,rc2.right-1,rc2.top);
	hp0_up = (HPEN)SelectObject( _DS->hDC, hp0_up );

	hp0_down = (HPEN)SelectObject( _DS->hDC, hp0_down );
	LineTo( _DS->hDC,rc2.right-1,rc2.bottom-1);
	LineTo( _DS->hDC,rc2.left,rc2.bottom-1);
	hp0_down = (HPEN)SelectObject( _DS->hDC, hp0_down );
*/

	DeleteObject( hp0_up );
	DeleteObject( hp0_down );
}

//----------------------------------------------------

