//----------------------------------------------------
// file: SceneClassList.cpp
//----------------------------------------------------
#include "Pch.h"
#pragma hdrstop

#include "SceneClassList.h"
#include "Scene.h"
#include "SObject2.h"
#include "Light.h"
#include "Sound.h"
#include "PClipper.h"

//----------------------------------------------------

SceneObject *NewObjectFromClassID( int _ClassID ){
	switch( _ClassID ){
		case OBJCLASS_DUMMY: return new SceneObject();
		case OBJCLASS_SOBJECT2: return new SObject2();
		case OBJCLASS_LIGHT: return new Light();
		case OBJCLASS_SOUND: return new CSound();
		case OBJCLASS_PCLIPPER: return new PClipper();
        default: throw -1;
	}
}
//----------------------------------------------------

