//----------------------------------------------------
// file: MSC_Main.cpp
//----------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "NetDeviceLog.h"
#include "MSC_Main.h"

//----------------------------------------------------

float g_MouseMoveSpeed = 0.01f;
float g_MouseRotateSpeed = 0.005f;
float g_MouseScaleSpeed = 0.01f;

//----------------------------------------------------

MouseCallback::MouseCallback( int _Target, int _Action ){
	m_Action = _Action;
	m_Target = _Target;
};

MouseCallback::~MouseCallback(){
}

//----------------------------------------------------

