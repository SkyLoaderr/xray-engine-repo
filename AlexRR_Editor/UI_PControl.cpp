//----------------------------------------------------
// file: UI_PControl.cpp
//----------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "NetDeviceLog.h"
#include "UI_Main.h"

//----------------------------------------------------

UI_PControl::UI_PControl()
	:UI_ViewControl()
{
	m_SaveTransform.identity();
	m_Up.set( 0, 1, 0 );
	m_Side.set( 0, 0, 0 );
}

UI_PControl::~UI_PControl(){
}

//----------------------------------------------------

void UI_PControl::MouseStartMove(){
	
	m_SaveTransform.set( UI.m_Camera );

	if( m_Alternate ){

		m_Up.set( 0, -1, 0 );
		m_Side.set( 0, 0, 0 );

	} else {

		m_Up.inverse( UI.m_Camera.k );
		m_Side.set( UI.m_Camera.i );

		m_Up.y = 0;
		m_Side.y = 0;

		m_Up.safe_normalize();
		m_Side.safe_normalize();
	}

	if( m_Accelerated ){
		m_Up.mul( 10.f );
		m_Side.mul( 10.f ); }
}

void UI_PControl::MouseEndMove(){
}

void UI_PControl::MouseMove(){
	
	IVector vmove;

	if( m_RelMove.x ){
		vmove.mul( m_Side, m_RelMove.x * 0.01f );
		UI.m_Camera.c.add( vmove );
	}

	if( m_RelMove.y ){
		vmove.mul( m_Up, m_RelMove.y * 0.01f );
		UI.m_Camera.c.add( vmove );
	}

	UI.Redraw();
}

//----------------------------------------------------
