//----------------------------------------------------
// file: UI_RControl.cpp
//----------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "NetDeviceLog.h"
#include "UI_Main.h"

//----------------------------------------------------

UI_RControl::UI_RControl()
	:UI_ViewControl()
{
	m_SaveTransform.identity();
	m_RCenter.set( 0, 0, 0 );
}

UI_RControl::~UI_RControl(){
}

//----------------------------------------------------

void UI_RControl::MouseStartMove(){
	m_SaveTransform.set( UI.m_Camera );
	m_RCenter.set( UI.pivot() );
}

void UI_RControl::MouseEndMove(){
}

void UI_RControl::MouseMove(){

	float kr = 0.0005f;
	if( m_Accelerated )
		kr *= 5.f;

	if( m_Alternate ){

		IMatrix rmatrix;
		if( m_RelMove.x ){

			IVector vr;
			vr.set( 0, 1, 0 );
			rmatrix.r( m_RelMove.x * kr, vr );

			//rmatrix.r( m_RelMove.x * kr, UI.m_Camera.j );
			
			UI.m_Camera.c.sub( m_RCenter );
			rmatrix.transform( UI.m_Camera.c );
			UI.m_Camera.c.add( m_RCenter );
			
			rmatrix.transform( UI.m_Camera.i );
			rmatrix.transform( UI.m_Camera.j );
			rmatrix.transform( UI.m_Camera.k ); }

		if( m_RelMove.y ){

			rmatrix.r( m_RelMove.y * kr, UI.m_Camera.i );
			
			UI.m_Camera.c.sub( m_RCenter );
			rmatrix.transform( UI.m_Camera.c );
			UI.m_Camera.c.add( m_RCenter );
			
			rmatrix.transform( UI.m_Camera.i );
			rmatrix.transform( UI.m_Camera.j );
			rmatrix.transform( UI.m_Camera.k ); }
	
	} else {

		IMatrix rmatrix;
		if( m_RelMove.x ){

			IVector vr;
			vr.set( 0, 1, 0 );
			rmatrix.r( m_RelMove.x * kr, vr );

			//rmatrix.r( m_RelMove.x * kr, UI.m_Camera.j );
			rmatrix.transform( UI.m_Camera.i );
			rmatrix.transform( UI.m_Camera.j );
			rmatrix.transform( UI.m_Camera.k ); }

		if( m_RelMove.y ){
			rmatrix.r( m_RelMove.y * kr, UI.m_Camera.i );
			rmatrix.transform( UI.m_Camera.i );
			rmatrix.transform( UI.m_Camera.j );
			rmatrix.transform( UI.m_Camera.k ); }
	}

	UI.Redraw();
}

//----------------------------------------------------
