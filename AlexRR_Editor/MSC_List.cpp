//----------------------------------------------------
// file: MSC_List.cpp
//----------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "NetDeviceLog.h"
#include "MSC_List.h"
#include "MSC_AddLandscape.h"
#include "MSC_SelLandscape.h"
#include "MSC_MoveLandscape.h"
#include "MSC_RotateLandscape.h"
#include "MSC_ScaleLandscape.h"
#include "MSC_AddObject.h"
#include "MSC_SelObject.h"
#include "MSC_MoveObject.h"
#include "MSC_RotateObject.h"
#include "MSC_ScaleObject.h"
#include "MSC_MovePivot.h"
#include "MSC_AddLight.h"
#include "MSC_SelLight.h"
#include "MSC_MoveLight.h"
#include "MSC_AddSndPlane.h"
#include "MSC_SelSndPlane.h"
#include "MSC_MoveSndPlane.h"
#include "MSC_RotateSndPlane.h"
#include "MSC_ScaleSndPlane.h"
#include "MSC_AddSound.h"
#include "MSC_SelSound.h"
#include "MSC_MoveSound.h"

//----------------------------------------------------


MouseCallback_AddLandscape			MSC_AddLandscape;
MouseCallback_SelLandscape			MSC_SelLandscape;
MouseCallback_MoveLandscape			MSC_MoveLandscape;
MouseCallback_RotateLandscape		MSC_RotateLandscape;
MouseCallback_ScaleLandscape		MSC_ScaleLandscape;

MouseCallback_AddObject				MSC_AddObject;
MouseCallback_SelObject				MSC_SelObject;
MouseCallback_MoveObject			MSC_MoveObject;
MouseCallback_RotateObject			MSC_RotateObject;
MouseCallback_ScaleObject			MSC_ScaleObject;

MouseCallback_MovePivot             MSC_MovePivot;

MouseCallback_AddLight				MSC_AddLight;
MouseCallback_SelLight				MSC_SelLight;
MouseCallback_MoveLight				MSC_MoveLight;

MouseCallback_AddSndPlane			MSC_AddSndPlane;
MouseCallback_SelSndPlane			MSC_SelSndPlane;
MouseCallback_MoveSndPlane			MSC_MoveSndPlane;
MouseCallback_RotateSndPlane		MSC_RotateSndPlane;
MouseCallback_ScaleSndPlane			MSC_ScaleSndPlane;

MouseCallback_AddSound				MSC_AddSound;
MouseCallback_SelSound				MSC_SelSound;
MouseCallback_MoveSound				MSC_MoveSound;

//----------------------------------------------------

MSCList g_MouseCB;

void MSC_Init(){
	g_MouseCB.push_back( &MSC_AddLandscape );
	g_MouseCB.push_back( &MSC_SelLandscape );
	g_MouseCB.push_back( &MSC_MoveLandscape );
	g_MouseCB.push_back( &MSC_RotateLandscape );
	g_MouseCB.push_back( &MSC_ScaleLandscape );

	g_MouseCB.push_back( &MSC_AddObject );
	g_MouseCB.push_back( &MSC_SelObject );
	g_MouseCB.push_back( &MSC_MoveObject );
	g_MouseCB.push_back( &MSC_RotateObject );
	g_MouseCB.push_back( &MSC_ScaleObject );

	g_MouseCB.push_back( &MSC_MovePivot );

	g_MouseCB.push_back( &MSC_AddLight );
	g_MouseCB.push_back( &MSC_SelLight );
	g_MouseCB.push_back( &MSC_MoveLight );

	g_MouseCB.push_back( &MSC_AddSndPlane );
	g_MouseCB.push_back( &MSC_SelSndPlane );
	g_MouseCB.push_back( &MSC_MoveSndPlane );
	g_MouseCB.push_back( &MSC_RotateSndPlane );
	g_MouseCB.push_back( &MSC_ScaleSndPlane );

	g_MouseCB.push_back( &MSC_AddSound );
	g_MouseCB.push_back( &MSC_SelSound );
	g_MouseCB.push_back( &MSC_MoveSound );
}

void MSC_Uninit(){
	g_MouseCB.clear();
}

MouseCallback *MSC_Select( int target, int action ){
	MSCListIt _M = g_MouseCB.begin();
	MSCListIt _E = g_MouseCB.end();
	for( ;_M != _E; _M++ )
		if( (*_M)->Compare(target,action) )
			return (*_M);
	return 0;
}

//----------------------------------------------------
