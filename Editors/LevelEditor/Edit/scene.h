//----------------------------------------------------
// file: Scene.h
//----------------------------------------------------
#ifndef SceneH
#define SceneH

#include "SceneGraph.h"
#include "Communicate.h"
#include "pure.h"
#include "ElTree.hpp"
//refs
struct FSChunkDef;
class PropValue;
//----------------------------------------------------

#pragma pack( push,1 )
struct UndoItem {
	char m_FileName[MAX_PATH];
};
#pragma pack( pop )

class CLight;
class CSceneObject;
class EDetailManager;
class CInifile;
class TProperties;
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
	void 		Save			(IWriter&);
	void 		Read			(IReader&);
};

DEFINE_VECTOR(st_Environment,EnvList,EnvIt);

struct st_LevelOptions{
	AnsiString	m_FNLevelPath;
	AnsiString	m_LevelName;   // obsolette
	AnsiString 	m_BOPText;
	AnsiString 	m_SkydomeObjName;
    EnvList		m_Envs;
    int 		m_CurEnv;
    float		m_DOClusterSize;

    b_params	m_BuildParams;

    st_LevelOptions();
    // routine
	void 		Save			(IWriter&);
	void 		Read			(IReader&);
    void		Reset			();
	void 		InitDefaultText	();
    void		WriteToLTX		(CInifile* pIni);
};

struct st_GroupItem{
	u32			dwUpdateFrame;
	Fbox		box;
	ObjectList	objects;
    st_GroupItem(){box.set(0,0,0,0,0,0);}
};

DEFINE_MAP(int,st_GroupItem,GroupMap,GroupPairIt);

class EScene:
	public pureDeviceCreate,
	public pureDeviceDestroy
{
public:
	// addition objects
    EDetailManager*	m_DetailObjects;
    CSceneObject*	m_SkyDome;

    ObjectList		m_SnapObjects;

	typedef	FixedMAP<float,CSceneObject*>   mapObject_D;
	typedef mapObject_D::TNode	 	    mapObject_Node;
	mapObject_D						    mapRenderObjects;
public:
	st_LevelOptions	m_LevelOp;

    vector<CLight*> frame_light;
protected:
	bool m_Valid;
	int m_Locked;

	int m_LastAvailObject;

	ObjectMap  m_Objects;

	deque<UndoItem> m_UndoStack;
	deque<UndoItem> m_RedoStack;

	TProperties* m_SummaryInfo;
public:
    struct ERR{
	    struct Face{
    		Fvector		p[3];
	    };
	    struct Edge{
    		Fvector		p[2];
	    };
	    struct Vert{
    		Fvector		p[1];
	    };
	    DEFINE_VECTOR(Face,FaceVec,FaceIt);
	    DEFINE_VECTOR(Edge,EdgeVec,EdgeIt);
	    DEFINE_VECTOR(Vert,VertVec,VertIt);
    	FaceVec			m_InvalidFaces;
	    EdgeVec			m_MultiEdges;
	    VertVec			m_TJVerts;
        void			Clear()
        {
        	m_InvalidFaces.clear();
	    	m_MultiEdges.clear	();
	    	m_TJVerts.clear		();
        }
        void AppendPoint(const Fvector& p0)
        {
        	m_TJVerts.push_back(Vert());
            m_TJVerts.back().p[0].set(p0);
        }
        void AppendEdge	(const Fvector& p0, const Fvector& p1)
        {
        	m_MultiEdges.push_back(Edge());
        	m_MultiEdges.back().p[0].set(p0);
        	m_MultiEdges.back().p[1].set(p1);
        }
        void AppendFace	(const Fvector& p0, const Fvector& p1, const Fvector& p2)
        {
        	m_InvalidFaces.push_back(Face());
        	m_InvalidFaces.back().p[0].set(p0);
        	m_InvalidFaces.back().p[1].set(p1);
        	m_InvalidFaces.back().p[2].set(p2);
        }
    };
    ERR				m_CompilerErrors;
protected:
    void OnLoadAppendObject			(CCustomObject* O);
    void OnLoadSelectionAppendObject(CCustomObject* O);
public:
    bool m_Modified;

	typedef void (__closure *TAppendObject)(CCustomObject* object);

	bool ReadObject					(IReader& F, CCustomObject*& O);
	bool ReadObjects				(IReader& F, u32 chunk_id, TAppendObject on_append);
	void SaveObject					(CCustomObject* O,IWriter& F);
	void SaveObjects				(ObjectList& lst, u32 chunk_id, IWriter& F);
public:
	bool Load						(char *_FileName);
	void Save						(char *_FileName, bool bUndo);
	bool LoadSelection				(const char *_FileName);
	void SaveSelection				(int classfilter, char *_FileName);
	void Unload						();
	void ClearObjects				(bool bDestroy);
	void LoadCompilerError			(LPCSTR fn);
    void ClearCompilerErrors		(){m_CompilerErrors.Clear();}

	IC bool valid					()           	{ return m_Valid; }

	IC bool locked					()          	{ return m_Locked!=0; }
	IC void lock					()            	{ m_Locked++; }
	IC void unlock					()          	{ m_Locked--; }
	IC void waitlock				()        		{ while( locked() ) Sleep(0); }

	IC ObjectList& ListObj    		(EObjClass cat)	{ VERIFY((cat>=0)&&(cat<OBJCLASS_COUNT));
                                        			return m_Objects[cat]; }
	IC ObjectIt FirstObj      		(EObjClass cat)	{ return ListObj(cat).begin(); }
	IC ObjectIt LastObj       		(EObjClass cat)	{ return ListObj(cat).end(); }
	IC ObjectPairIt FirstClass		()				{ return m_Objects.begin(); }
	IC ObjectPairIt LastClass 		()				{ return m_Objects.end(); }
	IC int ObjCount           		(EObjClass cat)	{ return ListObj(cat).size(); }
	int ObjCount 			        ();

	void RenderSky					(const Fmatrix& camera);
	void Render                     (const Fmatrix& camera);
	void OnFrame					(float dT);

	int AddToSnapList				();
    int SetSnapList					();
    void ClearSnapList				();
    void UpdateSnapList 			();

	void AddObject                  (CCustomObject* object, bool bExecUndo=true);
	void RemoveObject               (CCustomObject* object, bool bExecUndo=true);
    bool ContainsObject             (CCustomObject* object, EObjClass classfilter);

	ObjectList* GetSnapList			();
	CCustomObject *RayPick   		(const Fvector& start, const Fvector& dir, EObjClass classfilter, SRayPickInfo* pinf, bool bDynamicTest, ObjectList* snap_list);
	int BoxPick						(const Fbox& box, SBoxPickInfoVec& pinf, ObjectList* snap_list=0);

	int RaySelect               	(int flag, EObjClass classfilter=OBJCLASS_DUMMY, bool bOnlyNearest=true); // flag=0,1,-1 (-1 invert)
	int FrustumSelect               (int flag, EObjClass classfilter=OBJCLASS_DUMMY);
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
    void ZoomExtents				(BOOL bSelectedOnly);

	int FrustumPick					(const CFrustum& frustum, EObjClass classfilter, ObjectList& ol);
	int SpherePick					(const Fvector& center, float radius, EObjClass classfilter, ObjectList& ol);

	void GenObjectName				(EObjClass cls_id, char *buffer, const char* prefix=NULL);
	CCustomObject* FindObjectByName	(LPCSTR name, EObjClass classfilter);
    CCustomObject* FindObjectByName	(LPCSTR name, CCustomObject* pass_object);
    bool FindDuplicateName          ();

	void UndoClear					();
	void UndoSave					();
	bool Undo						();
	bool Redo						();

	void SetLights					();
	void ClearLights				();
    void TurnLightsForObject		(CSceneObject* obj);

    bool GetBox						(Fbox& box, EObjClass classfilter);
    bool GetBox						(Fbox& box, ObjectList& lst);
	void UpdateSkydome				();
    void WriteToLTX					(CInifile* pIni);

public:
	int  GetQueryObjects			(ObjectList& objset, EObjClass classfilter, int iSel=1, int iVis=1, int iLock=0);
    template <class Predicate>
    int  GetQueryObjects_if			(ObjectList& dest, EObjClass classfilter, Predicate cmp){
        for(ObjectPairIt it=FirstClass(); it!=LastClass(); it++){
            ObjectList& lst = it->second;
            if ((classfilter==OBJCLASS_DUMMY)||(classfilter==it->first)){
                for(ObjectIt _F = lst.begin();_F!=lst.end();_F++){
        			if (cmp(_F)) dest.push_back(*_F);
                }
            }
        }
        return dest.size();
    }
public:

	void 			OnCreate		();
	void 			OnDestroy		();
    void 			Modified		();
    bool 			IfModified		();
	bool 			IsModified		();

    int 			GetUndoCount	(){return m_UndoStack.size();}
    int 			GetRedoCount	(){return m_RedoStack.size();}

    bool 			Validate		(bool bNeedOkMsg, bool bTestPortal, bool bTestHOM, bool bTestGlow, bool bTestShaderCompatible);
    void 			OnObjectsUpdate	();

					EScene			();
	virtual 		~EScene			();

	virtual	void	OnDeviceCreate	();
	virtual	void	OnDeviceDestroy	();

	void 			OnShowHint		(AStringVec& dest);

    void			SynchronizeObjects();

    void			ShowSummaryInfo	();
};

//----------------------------------------------------
extern EScene Scene;
//----------------------------------------------------

#endif /*_INCDEF_Scene_H_*/

