//----------------------------------------------------
// file: UI_SControl.cpp
//----------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "NetDeviceLog.h"
#include "UI_Main.h"

//----------------------------------------------------

UI_SControl::UI_SControl()
	:UI_ViewControl()
{
	m_SaveTransform.identity();
	m_SCenter.set( 0, 0, 0 );
}

UI_SControl::~UI_SControl(){
}

//----------------------------------------------------

void UI_SControl::MouseStartMove(){
	m_SaveTransform.set( UI.m_Camera );
	m_SCenter.set( 0, 0, 0 );
}

void UI_SControl::MouseEndMove(){
}

void UI_SControl::MouseMove(){
	UI.Redraw();
}

//----------------------------------------------------
