//----------------------------------------------------
// file: SceneClassList.h
//----------------------------------------------------

#ifndef _INCDEF_SceneClassList_H_
#define _INCDEF_SceneClassList_H_
//----------------------------------------------------

class CEditableObject;
class CEditableMesh;
class CSceneObject;

#ifdef _EDITOR
	struct SRayPickInfo{
		CSceneObject*		s_obj;
		CEditableObject*	e_obj;
		CEditableMesh*		e_mesh;
		Fvector     		pt;
		CDB::RESULT 		rp_inf;
		SRayPickInfo		(){Reset();}
		IC void Reset		(){ ZeroMemory(this,sizeof(SRayPickInfo));rp_inf.range = 5000;}
	};
    DEFINE_VECTOR(CDB::RESULT,BPInfVec,BPInfIt);
	struct SBoxPickInfo{
		CSceneObject*		s_obj;
		CEditableObject*	e_obj;
		CEditableMesh*		e_mesh;
    	BPInfVec			bp_inf;
		SBoxPickInfo		(){Reset();}
		IC void Reset		(){ZeroMemory(this,sizeof(SBoxPickInfo));}
	};
	DEFINE_VECTOR(SBoxPickInfo,SBoxPickInfoVec,SBoxPickInfoIt);
#endif

//----------------------------------------------------
class CCustomObject;

enum EObjClass{
    OBJCLASS_DUMMY     	= -1,
    OBJCLASS_GLOW	   	= 1,
    OBJCLASS_SCENEOBJECT= 2,
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
CCustomObject*	NewObjectFromClassID	(int _ClassID);
LPSTR 			GetNameByClassID		(EObjClass cls_id);
bool 			IsClassID				(EObjClass cls_id);
bool 			IsObjectListClassID		(EObjClass cls_id);
EObjClass 		ClassIDFromTargetID		(int target);
//----------------------------------------------------
#endif /*_INCDEF_SceneClassList_H_*/


