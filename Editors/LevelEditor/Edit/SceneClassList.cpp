//----------------------------------------------------
// file: SceneClassList.cpp
//----------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "Scene.h"
#include "ui_tools.h"
#include "SceneObject.h"
#include "ELight.h"
#include "EShape.h"
#include "Sound.h"
#include "glow.h"
#include "spawnpoint.h"
#include "WayPoint.h"
#include "sector.h"
#include "portal.h"
#include "event.h"
#include "PSObject.h"
#include "DetailObjects.h"
#include "GroupObject.h"

#include "UI_LightTools.h"
#include "UI_ShapeTools.h"
#include "UI_ObjectTools.h"
#include "UI_SoundTools.h"
#include "UI_GlowTools.h"
#include "UI_RPointTools.h"
#include "UI_WayPointTools.h"
#include "UI_SectorTools.h"
#include "UI_PortalTools.h"
#include "UI_EventTools.h"
#include "UI_PSTools.h"
#include "UI_DOTools.h"
#include "UI_GroupTools.h"
//----------------------------------------------------

TUI_CustomTools* NewToolFromTarget(int _tgt)
{
	ETarget tgt			= ETarget(_tgt);
    switch(tgt){
	case etGroup:		return new TUI_GroupTools		();
    case etLight:		return new TUI_LightTools		();
    case etShape:		return new TUI_ShapeTools		();
    case etObject:		return new TUI_ObjectTools		();
    case etSound:		return new TUI_SoundTools		();
    case etGlow:		return new TUI_GlowTools		();
    case etSpawnPoint:	return new TUI_SpawnPointTools	();
    case etSector:		return new TUI_SectorTools		();
    case etPortal:		return new TUI_PortalTools		();
    case etEvent:		return new TUI_EventTools		();
    case etWay:			return new TUI_WayPointTools	();
    case etPS:			return new TUI_PSTools			();
    case etDO:			return new TUI_DOTools			();
    default: THROW2("Can't find specify tools.");
    }
    return 0;
}
CCustomObject *NewObjectFromClassID( int _ClassID, LPVOID data, LPCSTR name ){
	switch( _ClassID ){
    	case OBJCLASS_GROUP:		return new CGroupObject(data,name);
		case OBJCLASS_SCENEOBJECT: 	return new CSceneObject(data,name);
		case OBJCLASS_LIGHT:    	return new CLight(data,name);
        case OBJCLASS_SHAPE:		return new CEditShape(data,name);
		case OBJCLASS_SOUND:    	return new CSound(data,name);
		case OBJCLASS_GLOW:     	return new CGlow(data,name);
        case OBJCLASS_SPAWNPOINT:	return new CSpawnPoint(data,name);
        case OBJCLASS_WAY:			return new CWayObject(data,name);
        case OBJCLASS_SECTOR:		return new CSector(data,name);
        case OBJCLASS_PORTAL:		return new CPortal(data,name);
        case OBJCLASS_EVENT:		return new CEvent(data,name);
        case OBJCLASS_PS:			return new CPSObject(data,name);
        default: throw -1;
	}
}
//----------------------------------------------------
LPSTR GetNameByClassID(EObjClass cls_id){
    switch(cls_id){
	case OBJCLASS_GROUP:		return "group";
    case OBJCLASS_SCENEOBJECT: 	return "object";
    case OBJCLASS_LIGHT:    	return "light";
    case OBJCLASS_SHAPE:		return "shape";
    case OBJCLASS_SOUND:    	return "sound";
    case OBJCLASS_GLOW:     	return "glow";
    case OBJCLASS_SPAWNPOINT:   return "spawnpoint";
    case OBJCLASS_WAY:			return "way";
    case OBJCLASS_SECTOR: 		return "sector";
    case OBJCLASS_PORTAL: 		return "portal";
    case OBJCLASS_EVENT: 		return "event";
    case OBJCLASS_PS:			return "ps";
    case OBJCLASS_DO:			return "detailobject";
    default: THROW2("Gen empty name"); return 0;
    }
}
//----------------------------------------------------
LPSTR GetClassNameByClassID(EObjClass cls_id)
{
    switch(cls_id){
	case OBJCLASS_GROUP:		return "Group";
    case OBJCLASS_SCENEOBJECT: 	return "Object";
    case OBJCLASS_LIGHT:    	return "Light";
    case OBJCLASS_SHAPE:		return "Shape";
    case OBJCLASS_SOUND:    	return "Sound";
    case OBJCLASS_GLOW:     	return "Glow";
    case OBJCLASS_SPAWNPOINT:   return "Spawn Point";
    case OBJCLASS_WAY:			return "Way";
    case OBJCLASS_SECTOR: 		return "Sector";
    case OBJCLASS_PORTAL: 		return "Portal";
    case OBJCLASS_EVENT: 		return "Event";
    case OBJCLASS_PS:			return "Particle System";
    default: THROW2("Gen empty name"); return 0;
    }
}
bool IsClassID(EObjClass cls_id){
    switch(cls_id){
	case OBJCLASS_GROUP:		return true;
    case OBJCLASS_SCENEOBJECT: 	return true;
    case OBJCLASS_LIGHT:    	return true;
    case OBJCLASS_SHAPE:		return true;
    case OBJCLASS_SOUND:    	return true;
    case OBJCLASS_GLOW:     	return true;
    case OBJCLASS_SPAWNPOINT:   return true;
    case OBJCLASS_WAY:			return true;
    case OBJCLASS_SECTOR: 		return true;
    case OBJCLASS_PORTAL: 		return true;
    case OBJCLASS_EVENT: 		return true;
    case OBJCLASS_PS:			return true;
    case OBJCLASS_DO:			return true;
    default: return 0;
    }
}
EObjClass ClassIDFromTargetID( int cls_id ){
	switch( cls_id ){
	case etGroup:		return OBJCLASS_GROUP;
	case etObject:  	return OBJCLASS_SCENEOBJECT;
	case etSound:   	return OBJCLASS_SOUND;
	case etLight:   	return OBJCLASS_LIGHT;
    case etShape:		return OBJCLASS_SHAPE;
	case etGlow:    	return OBJCLASS_GLOW;
	case etSpawnPoint:  return OBJCLASS_SPAWNPOINT;
	case etWay:			return OBJCLASS_WAY;
    case etSector:		return OBJCLASS_SECTOR;
    case etPortal:		return OBJCLASS_PORTAL;
    case etEvent:		return OBJCLASS_EVENT;
    case etPS:			return OBJCLASS_PS;
	case etDO: 			return OBJCLASS_DO;
    default: throw -1;
	}
}
bool IsObjectListClassID(EObjClass cls_id){
    switch(cls_id){
	case OBJCLASS_GROUP:		return true;
    case OBJCLASS_SCENEOBJECT: 	return true;
    case OBJCLASS_LIGHT:    	return true;
    case OBJCLASS_SHAPE:		return true;
    case OBJCLASS_SOUND:    	return true;
    case OBJCLASS_GLOW:     	return true;
    case OBJCLASS_SPAWNPOINT:   return true;
    case OBJCLASS_WAY:			return true;
    case OBJCLASS_SECTOR:		return true;
    case OBJCLASS_PORTAL:		return true;
    case OBJCLASS_EVENT:		return true;
    case OBJCLASS_PS:			return true;
    case OBJCLASS_DO:			return true;
    default: return false;
    }
}
bool IsGroupClassID(EObjClass cls_id)
{
    switch(cls_id){
    case OBJCLASS_SCENEOBJECT: 	return true;
    case OBJCLASS_LIGHT:    	return true;
    case OBJCLASS_SOUND:    	return true;
    case OBJCLASS_GLOW:     	return true;
    case OBJCLASS_SPAWNPOINT:   return true;
    case OBJCLASS_EVENT:		return true;
    case OBJCLASS_PS:			return true;
    default: return false;
    }
}

