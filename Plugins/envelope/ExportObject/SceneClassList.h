//----------------------------------------------------
// file: SceneClassList.h
//----------------------------------------------------

#ifndef _INCDEF_SceneClassList_H_
#define _INCDEF_SceneClassList_H_
//----------------------------------------------------

class SceneObject;

enum EObjClass{
    OBJCLASS_DUMMY     = -1,
    OBJCLASS_DPATCH    = 0,
    OBJCLASS_GLOW	   = 1,
    OBJCLASS_EDITOBJECT= 2,
    OBJCLASS_LIGHT	   = 3,
    OBJCLASS_PCLIPPER  = 4,
    OBJCLASS_SOUND	   = 5,
    OBJCLASS_RPOINT	   = 6,
    OBJCLASS_NAVPOINT  = 7,
    OBJCLASS_SECTOR    = 8,
    OBJCLASS_PORTAL    = 9,
    OBJCLASS_COUNT,
    OBJCLASS_force_dword = -1
};

//----------------------------------------------------
SceneObject*	NewObjectFromClassID	(int _ClassID);
LPSTR 			GetNameByClassID		(EObjClass cls_id);
bool 			IsClassID				(EObjClass cls_id);
bool 			IsObjectListClassID		(EObjClass cls_id);
EObjClass 		ClassIDFromTargetID		(int target);
int		 		TargetIDFromClassID		(EObjClass cls_id);
//----------------------------------------------------
#endif /*_INCDEF_SceneClassList_H_*/


