//----------------------------------------------------
// file: SceneClassList.cpp
//----------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "SceneClassList.h"
#include "Scene.h"
#include "ui_tools.h"
#include "EditObject.h"
#include "ELight.h"
#include "Sound.h"
#include "Occluder.h"
#include "glow.h"
#include "dpatch.h"
#include "rpoint.h"
#include "AITraffic.h"
#include "sector.h"
#include "portal.h"
#include "event.h"
#include "PSObject.h"
#include "DetailObjects.h"
//----------------------------------------------------

SceneObject *NewObjectFromClassID( int _ClassID ){
	switch( _ClassID ){
		case OBJCLASS_EDITOBJECT: return new CEditObject();
		case OBJCLASS_LIGHT:    return new CLight();
		case OBJCLASS_SOUND:    return new CSound();
		case OBJCLASS_OCCLUDER: return new COccluder();
		case OBJCLASS_GLOW:     return new CGlow();
        case OBJCLASS_RPOINT:   return new CRPoint();
        case OBJCLASS_AITPOINT:	return new CAITPoint();
        case OBJCLASS_SECTOR:	return new CSector();
        case OBJCLASS_PORTAL:	return new CPortal();
        case OBJCLASS_EVENT:	return new CEvent();
        case OBJCLASS_PS:		return new CPSObject();
        default: throw -1;
	}
}
//----------------------------------------------------
LPSTR GetNameByClassID(EObjClass cls_id){
    switch(cls_id){
    case OBJCLASS_EDITOBJECT: return "Object";
    case OBJCLASS_LIGHT:    return "Light";
    case OBJCLASS_SOUND:    return "Sound";
    case OBJCLASS_OCCLUDER: return "Occluder";
    case OBJCLASS_GLOW:     return "Glow";
    case OBJCLASS_RPOINT:   return "RPoint";
    case OBJCLASS_AITPOINT:	return "AITPoint";
    case OBJCLASS_SECTOR: 	return "Sector";
    case OBJCLASS_PORTAL: 	return "Portal";
    case OBJCLASS_EVENT: 	return "Event";
    case OBJCLASS_PS:		return "PS";
    case OBJCLASS_DO:		return "DetailObject";
    default: return 0;
    }
}
bool IsClassID(EObjClass cls_id){
    switch(cls_id){
    case OBJCLASS_EDITOBJECT: return true;
    case OBJCLASS_LIGHT:    return true;
    case OBJCLASS_SOUND:    return true;
    case OBJCLASS_OCCLUDER: return true;
    case OBJCLASS_GLOW:     return true;
    case OBJCLASS_RPOINT:   return true;
    case OBJCLASS_AITPOINT:	return true;
    case OBJCLASS_SECTOR: 	return true;
    case OBJCLASS_PORTAL: 	return true;
    case OBJCLASS_EVENT: 	return true;
    case OBJCLASS_PS:		return true;
    case OBJCLASS_DO:		return true;
    default: return 0;
    }
}
EObjClass ClassIDFromTargetID( int cls_id ){
	switch( cls_id ){
	case etObject:  return OBJCLASS_EDITOBJECT;
	case etSound:   return OBJCLASS_SOUND;
	case etLight:   return OBJCLASS_LIGHT;
    case etOccluder:return OBJCLASS_OCCLUDER;
	case etGlow:    return OBJCLASS_GLOW;
	case etRPoint:  return OBJCLASS_RPOINT;
	case etAITPoint:return OBJCLASS_AITPOINT;
    case etDPatch:	return OBJCLASS_DPATCH;
    case etSector:	return OBJCLASS_SECTOR;
    case etPortal:	return OBJCLASS_PORTAL;
    case etEvent:	return OBJCLASS_EVENT;
    case etPS:		return OBJCLASS_PS;
	case etDO: 		return OBJCLASS_DO;
    default: throw -1;
	}
}
bool IsObjectListClassID(EObjClass cls_id){
    switch(cls_id){
    case OBJCLASS_EDITOBJECT: return true;
    case OBJCLASS_LIGHT:    return true;
    case OBJCLASS_SOUND:    return true;
    case OBJCLASS_OCCLUDER: return true;
    case OBJCLASS_GLOW:     return true;
    case OBJCLASS_RPOINT:   return true;
    case OBJCLASS_AITPOINT:	return true;
    case OBJCLASS_SECTOR:	return true;
    case OBJCLASS_PORTAL:	return true;
    case OBJCLASS_EVENT:	return true;
    case OBJCLASS_PS:		return true;
    case OBJCLASS_DO:		return true;
    default: return false;
    }
}

