//----------------------------------------------------
// file: SceneClassList.h
//----------------------------------------------------

#ifndef _INCDEF_SceneClassList_H_
#define _INCDEF_SceneClassList_H_
//----------------------------------------------------

#include "cl_defs.h";
#include "cl_rapid.h";

class CEditObject;
class CEditMesh;

struct SRayPickInfo{
    CEditObject*	obj;
    CEditMesh*		mesh;
    Fvector     	pt;
    RAPID::raypick_info rp_inf;
    SRayPickInfo	(){Reset();}
    IC void Reset	(){ ZeroMemory(this,sizeof(SRayPickInfo));rp_inf.range = 5000;}
};
struct SBoxPickInfo{
    CEditObject*	obj;
    CEditMesh*		mesh;
    RAPID::bboxpick_info bp_inf;
    SBoxPickInfo	(){Reset();}
    IC void Reset	(){ZeroMemory(this,sizeof(SBoxPickInfo));}
};
DEFINE_VECTOR(SBoxPickInfo,SBoxPickInfoVec,SBoxPickInfoIt);

//----------------------------------------------------
class SceneObject;

enum EObjClass{
    OBJCLASS_DUMMY     	= -1,
    OBJCLASS_DPATCH    	= 0,
    OBJCLASS_GLOW	   	= 1,
    OBJCLASS_EDITOBJECT	= 2,
    OBJCLASS_LIGHT	   	= 3,
    OBJCLASS_OCCLUDER  	= 4,
    OBJCLASS_SOUND	   	= 5,
    OBJCLASS_RPOINT	   	= 6,
    OBJCLASS_AITPOINT  	= 7,
    OBJCLASS_SECTOR    	= 8,
    OBJCLASS_PORTAL    	= 9,
    OBJCLASS_EVENT	   	= 10,
    OBJCLASS_PS		   	= 11,
    OBJCLASS_DO			= 12,
    OBJCLASS_COUNT,
    OBJCLASS_force_dword = -1
};

//----------------------------------------------------
SceneObject*	NewObjectFromClassID	(int _ClassID);
LPSTR 			GetNameByClassID		(EObjClass cls_id);
bool 			IsClassID				(EObjClass cls_id);
bool 			IsObjectListClassID		(EObjClass cls_id);
EObjClass 		ClassIDFromTargetID		(int target);
//----------------------------------------------------
#endif /*_INCDEF_SceneClassList_H_*/


