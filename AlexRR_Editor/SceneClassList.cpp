//----------------------------------------------------
// file: SceneClassList.cpp
//----------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "NetDeviceLog.h"
#include "SceneClassList.h"

#include "Scene.h"
#include "Landscape.h"
#include "SObject2.h"
#include "Light.h"
#include "SndPlane.h"
#include "Sound.h"

#include "SceneClassEditor.h"
#include "LandscapeEditor.h"
#include "SObject2Editor.h"
#include "LightEditor.h"
#include "SndPlaneEditor.h"
#include "SoundEditor.h"

//----------------------------------------------------

SceneObject *NewObjectFromClassID( int _ClassID ){
	switch( _ClassID ){
		case OBJCLASS_DUMMY: return new SceneObject();
		case OBJCLASS_LANDSCAPE: return new SLandscape();
		case OBJCLASS_SOBJECT2: return new SObject2();
		case OBJCLASS_LIGHT: return new Light();
		case OBJCLASS_SOUND: return new CSound();
		case OBJCLASS_SNDPLANE: return new SndPlane();
	}
	return false;
}

//----------------------------------------------------

SLandscapeEditor E_Landscape;
SObject2Editor E_Object2;
LightEditor E_Light;
CSoundEditor E_Sound;
SndPlaneEditor E_SndPlane;

//----------------------------------------------------
