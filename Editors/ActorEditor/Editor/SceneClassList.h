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
		CDB::RESULT 		inf;
		CSceneObject*		s_obj;
		CEditableObject*	e_obj;
		CEditableMesh*		e_mesh;
		Fvector     		pt;
		SRayPickInfo		(){Reset();}
		IC void Reset		(){ ZeroMemory(this,sizeof(SRayPickInfo));inf.range = 5000;}
		IC void SetRESULT	(CDB::MODEL* M, CDB::RESULT* R){inf=*R;inf.id=(M->get_tris()+inf.id)->dummy;}
	};
    DEFINE_VECTOR(CDB::RESULT,BPInfVec,BPInfIt);
	struct SBoxPickInfo{
    	BPInfVec			inf;
		CSceneObject*		s_obj;
		CEditableObject*	e_obj;
		CEditableMesh*		e_mesh;
		SBoxPickInfo		(){Reset();}
		IC void Reset		(){ZeroMemory(this,sizeof(SBoxPickInfo));}
		IC void AddRESULT	(CDB::MODEL* M, CDB::RESULT* R){inf.push_back(*R); inf.back().id=(M->get_tris()+inf.back().id)->dummy;}
	};
	DEFINE_VECTOR(SBoxPickInfo,SBoxPickInfoVec,SBoxPickInfoIt);
#endif

//----------------------------------------------------
class CCustomObject;

enum EObjClass{
    OBJCLASS_DUMMY     	= -1,
    OBJCLASS_GROUP		= 0,
    OBJCLASS_GLOW	   	= 1,
    OBJCLASS_SCENEOBJECT= 2,
    OBJCLASS_LIGHT	   	= 3,
    OBJCLASS_F_R_E_E  	= 4,
    OBJCLASS_SOUND	   	= 5,
    OBJCLASS_SPAWNPOINT	= 6,
    OBJCLASS_WAY	  	= 7,
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
bool 			IsGroupClassID			(EObjClass cls_id);
EObjClass 		ClassIDFromTargetID		(int target);
//----------------------------------------------------
#endif /*_INCDEF_SceneClassList_H_*/


