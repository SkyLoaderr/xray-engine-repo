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
#include "glow.h"
#include "particle.h"
#include "rpoint.h"

//----------------------------------------------------

SceneObject *NewObjectFromClassID( int _ClassID ){
	switch( _ClassID ){
		case OBJCLASS_SOBJECT2: return new SObject2();
		case OBJCLASS_LIGHT:    return new Light();
		case OBJCLASS_SOUND:    return new CSound();
		case OBJCLASS_PCLIPPER: return new PClipper();
		case OBJCLASS_GLOW:     return new CGlow();
		case OBJCLASS_PARTICLE: return new CParticle();
        case OBJCLASS_RPOINT:   return new CRPoint();

        default: throw -1;
	}
}
//----------------------------------------------------

