//----------------------------------------------------
// file: Scene.h
//----------------------------------------------------
#ifndef _INCDEF_Scene_H_
#define _INCDEF_Scene_H_

#include "SceneObject.h";
#include "SceneClassList.h"
#include "SceneGraph.h"
#include "Communicate.h"

//refs
struct FSChunkDef;
//----------------------------------------------------

#pragma pack( push,1 )
struct UndoItem {
	char m_FileName[MAX_PATH];
};
#pragma pack( pop )

class CLight;
class CEditObject;
class CEditMesh;
class CDPatchSystem;
class CDetailManager;
class CInifile;
class CStream;
class CFS_Base;
/*
int __cdecl _CompareByDist( const void *a, const void *b)
{
    return ((SRayPickInfo*)a)->rp_inf.range - ((SRayPickInfo*)b)->rp_inf.range;
}
*/

struct st_Environment{
    float 		m_ViewDist;
    float 		m_Fogness;
    Fcolor 		m_FogColor;
    Fcolor 		m_AmbColor;
    Fcolor 		m_SkyColor;

    st_Environment();
    void		Reset();
	void 		Save			(CFS_Base&);
	void 		Read			(CStream&);
};

DEFINE_VECTOR(st_Environment,EnvList,EnvIt);

struct st_LevelOptions{
	AnsiString	m_FNLevelPath;
	AnsiString	m_LevelName;
	AnsiString 	m_BOPText;
	AnsiString 	m_SkydomeObjName;
    EnvList		m_Envs;
    int 		m_CurEnv;
    float		m_DOClusterSize;

    st_LevelOptions();
    // routine
	void 		Save			(CFS_Base&);
	void 		Read			(CStream&);
    void		Reset			();
	void 		InitDefaultText	();
    void		WriteToLTX		(CInifile* pIni);
};

struct st_GroupItem{
	DWORD		dwUpdateFrame;
	Fbox		box;
	ObjectList	objects;
    st_GroupItem(){box.set(0,0,0,0,0,0);}
};

DEFINE_MAP(int,st_GroupItem,GroupMap,GroupPairIt);

class EScene {
public:
	// addition objects
    CDPatchSystem* 	m_DetailPatches;
    CDetailManager*	m_DetailObjects;
    CEditObject*	m_SkyDome;

    ObjectList		m_SnapObjects;
    
	typedef	FixedMAP<float,CEditObject*>   mapObject_D;
	typedef mapObject_D::TNode	 	    mapObject_Node;
	mapObject_D						    mapRenderObjects;
public:
	st_LevelOptions	m_LevelOp;

// Build options
    b_params    m_BuildParams;

    vector<CLight*> frame_light;
//    CList<SRayPickInfo> pickFaces;
protected:
	bool m_Valid;
	int m_Locked;

	int m_LastAvailObject;

	ObjectMap  m_Objects;

	deque<UndoItem> m_UndoStack;
	deque<UndoItem> m_RedoStack;

    GroupMap	m_Groups;
    
protected:
	SceneObject* ReadObject(CStream*);

public:
    bool m_Modified;

public:
	bool Load(char *_FileName);
	void Save(char *_FileName, bool bUndo);
	bool LoadSelection(const char *_FileName);
	bool LoadSelection(const char *_FileName,ObjectList& lst);
	void SaveSelection(int classfilter, char *_FileName);
	void Unload();
	void ClearObjects(bool bDestroy);

	IC bool valid()           { return m_Valid; }

	IC bool locked()          { return m_Locked!=0; }
	IC void lock()            { m_Locked++; }
	IC void unlock()          { m_Locked--; }
	IC void waitlock()        { while( locked() ) Sleep(0); }

	IC ObjectList& ListObj    (EObjClass cat){ VERIFY((cat>=0)&&(cat<OBJCLASS_COUNT));
                                        return m_Objects[cat]; }
	IC ObjectIt FirstObj      (EObjClass cat){ return ListObj(cat).begin(); }
	IC ObjectIt LastObj       (EObjClass cat){ return ListObj(cat).end(); }
	IC ObjectPairIt FirstClass(){ return m_Objects.begin(); }
	IC ObjectPairIt LastClass (){ return m_Objects.end(); }
	IC int ObjCount           (EObjClass cat){ return ListObj(cat).size(); }
	int ObjCount 			        ();

	void Render                     (Fmatrix *_Camera);
	void Update                     (float dT);

	int AddToSnapList				();
    int SetSnapList					();
    void ClearSnapList				();
    void UpdateSnapList 			();

	void AddObject                  (SceneObject* object, bool bManual=true);
	void RemoveObject               (SceneObject* object, bool bManual=true);
    bool ContainsObject             (SceneObject* object, EObjClass classfilter);

	SceneObject *RayPick           	(const Fvector& start, const Fvector& dir, EObjClass classfilter, SRayPickInfo* pinf, bool bDynamicTest, bool bUseSnapList);
	int BoxPick						(const Fbox& box, SBoxPickInfoVec& pinf, ObjectList* snap_list=0);

	int FrustumSelect               (bool flag, EObjClass classfilter=OBJCLASS_DUMMY);
	int SelectObjects               (bool flag, EObjClass classfilter=OBJCLASS_DUMMY);
	int LockObjects               	(bool flag, EObjClass classfilter=OBJCLASS_DUMMY, bool bAllowSelectionFlag=false, bool bSelFlag=true);
	int ShowObjects                 (bool flag, EObjClass classfilter=OBJCLASS_DUMMY, bool bAllowSelectionFlag=false, bool bSelFlag=true);
	int InvertSelection             (EObjClass classfilter);
	int SelectionCount              (bool testflag, EObjClass classfilter);
	int RemoveSelection             (EObjClass classfilter);
	int CutSelection                (EObjClass classfilter);
	int CopySelection               (EObjClass classfilter);
	int PasteSelection              ();

    void ResetAnimation				();
    void UpdateGroups				();
	int  GroupGetEmptyIndex			();
	int  GroupSelect				(int idx,bool bSelect,bool bClearPrevSel);
	void GroupCreate				(bool bDisplayErrorMsg);
	void GroupDestroy				();
	void GroupSave					();
	bool GroupAddItems				(int idx, ObjectList& lst);
	bool GroupAddItem				(int idx, SceneObject* O, bool bLoadMode=false);
	void GroupRemove				(int idx);
	void GroupUpdateBox				(int idx);
    void UngroupAll					();
    st_GroupItem& GetGroupItem		(int idx);

    void ZoomExtents				(BOOL bSelectedOnly);

	int FrustumPick					(const CFrustum& frustum, EObjClass classfilter, ObjectList& ol);
	int SpherePick					(const Fvector& center, float radius, EObjClass classfilter, ObjectList& ol);

	SceneObject* FindObjectByName	(char *name, EObjClass classfilter);
    SceneObject* FindObjectByName   (char *name, SceneObject* pass_object);
    bool FindDuplicateName          ();

	void UndoClear					();
	void UndoSave					();
	bool Undo						();
	bool Redo						();

	void SetLights					();
	void ClearLights				();
    void TurnLightsForObject		(CEditObject* obj);

	void GenObjectName				(EObjClass cls_id, char *buffer, const char* prefix=NULL);
	bool SearchName					(char *name);

    bool GetBox						(Fbox& box, EObjClass classfilter);
    bool GetBox						(Fbox& box, ObjectList& lst);
	void UpdateSkydome				();
    void WriteToLTX					(CInifile* pIni);
    
public:
	SceneObject* GetQueryObject		(EObjClass classfilter, int iSel=1, int iVis=1, int iLock=0);
	int  GetQueryObjects			(ObjectList& objset, EObjClass classfilter, int iSel=1, int iVis=1, int iLock=0);
    template <class Predicate>
    int  GetQueryObjects_if			(ObjectList& dest, EObjClass classfilter, Predicate cmp){
        VERIFY(classfilter!=OBJCLASS_DUMMY);
    	ObjectIt _F = FirstObj(classfilter);
        ObjectIt _E = LastObj(classfilter);
        for(;_F!=_E;_F++) if (cmp(_F)) dest.push_back(*_F);
        return dest.size();
    }
public:

	void Init();
	void Clear();
    void Modified();
    bool IfModified();
	bool IsModified();

    int GetUndoCount(){return m_UndoStack.size();}
    int GetRedoCount(){return m_RedoStack.size();}

    bool Validate(bool bNeedMsg=false, bool bTestPortal=true);
    void OnObjectsUpdate();

	EScene();
	virtual ~EScene();
    
	void 	OnDeviceCreate();
	void 	OnDeviceDestroy();

	void 	OnShowHint(AStringVec& dest);

    void	SynchronizeObjects();
};

//----------------------------------------------------
extern EScene* Scene;
//----------------------------------------------------

#endif /*_INCDEF_Scene_H_*/

