#include "stdafx.h"
#pragma hdrstop

#include "ESceneCustomOTools.h"

#include "SceneObject.h"
#include "ELight.h"
#include "EShape.h"
#include "ESound_Source.h"
#include "ESound_Environment.h"
#include "glow.h"
#include "spawnpoint.h"
#include "WayPoint.h"
#include "sector.h"
#include "portal.h"
#include "EParticlesObject.h"
#include "DetailObjects.h"
#include "GroupObject.h"

CCustomObject* ESceneCustomOTools::CreateObject(LPVOID data, LPCSTR name)
{
	CCustomObject* O = 0;
	switch( ClassID ){
    case OBJCLASS_GROUP:		O = xr_new<CGroupObject>(data,name);  			break;
    case OBJCLASS_SCENEOBJECT: 	O = xr_new<CSceneObject>(data,name);            break;
    case OBJCLASS_LIGHT:    	O = xr_new<CLight>(data,name);					break;
    case OBJCLASS_SHAPE:		O = xr_new<CEditShape>(data,name);              break;
    case OBJCLASS_SOUND_SRC:   	O = xr_new<ESoundSource>(data,name);            break;
    case OBJCLASS_SOUND_ENV:   	O = xr_new<ESoundEnvironment>(data,name);       break;
    case OBJCLASS_GLOW:     	O = xr_new<CGlow>(data,name);                   break;
    case OBJCLASS_SPAWNPOINT:	O = xr_new<CSpawnPoint>(data,name);             break;
    case OBJCLASS_WAY:			O = xr_new<CWayObject>(data,name);              break;
    case OBJCLASS_SECTOR:		O = xr_new<CSector>(data,name);                 break;
    case OBJCLASS_PORTAL:		O = xr_new<CPortal>(data,name);                 break;
    case OBJCLASS_PS:			O = xr_new<EParticlesObject>(data,name);        break;
    default: throw -1;
	}
    O->ParentTools				= this;
    return O;
}

 