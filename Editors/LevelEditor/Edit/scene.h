//----------------------------------------------------
// file: Scene.h
//----------------------------------------------------
#ifndef SceneH
#define SceneH

#include "SceneGraph.h"
#include "Communicate.h"
#include "pure.h"
#include "ElTree.hpp"

#include "SceneCustom.h"
#include "ESceneCustomMTools.h"
#include "ESceneCustomOTools.h"
//refs
struct FSChunkDef;
class PropValue;
struct SPBItem;
//----------------------------------------------------

#pragma pack( push,1 )
struct UndoItem {
	char m_FileName[MAX_PATH];
};
#pragma pack( pop )

class TProperties;
/*
int __cdecl _CompareByDist( const void *a, const void *b)
{
    return ((SRayPickInfo*)a)->rp_inf.range - ((SRayPickInfo*)b)->rp_inf.range;
}
*/

struct st_LevelOptions{
	ref_str		m_FNLevelPath;
	ref_str 	m_BOPText;

    b_params	m_BuildParams;

    st_LevelOptions();
    // routine
	void 		Save			(IWriter&);
	void 		Read			(IReader&);
    void		Reset			();
	void 		InitDefaultText	();
};

class EScene: public ISceneCustom,
	public pureDeviceCreate,
	public pureDeviceDestroy
{
public:
	typedef	FixedMAP<float,CCustomObject*>	mapObject_D;
	typedef mapObject_D::TNode	 	    	mapObject_Node;
	mapObject_D						    	mapRenderObjects;
public:
	st_LevelOptions	m_LevelOp;
protected:
	bool m_Valid;
	int m_Locked;

	int m_LastAvailObject;

    SceneToolsMap   m_SceneTools;

	xr_deque<UndoItem> m_UndoStack;
	xr_deque<UndoItem> m_RedoStack;

	TProperties* m_SummaryInfo;

    ObjectList			m_ESO_SnapObjects; // временно здесь а вообще нужно перенести в ESceneTools
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
    bool 			OnLoadAppendObject			(CCustomObject* O);
    bool 			OnLoadSelectionAppendObject(CCustomObject* O);
protected:
	void			RegisterSceneTools			(ESceneCustomMTools* mt);
	void			CreateSceneTools			();
	void			DestroySceneTools			();

    void 			FindObjectByNameCB			(LPCSTR new_name, bool& res){res=!!FindObjectByName(new_name,0);}
public:
	enum{
    	flRT_Unsaved 	= (1<<0),
    	flRT_Modified 	= (1<<1),
    };
    Flags32			m_RTFlags;
public:

	typedef bool (__closure *TAppendObject)(CCustomObject* object);

	bool 			ReadObject			(IReader& F, CCustomObject*& O);
	bool 			ReadObjects			(IReader& F, u32 chunk_id, TAppendObject on_append, SPBItem* pb);
	void 			SaveObject			(CCustomObject* O,IWriter& F);
	void 			SaveObjects			(ObjectList& lst, u32 chunk_id, IWriter& F);
public:
	void			ExportGame			(SExportStreams& F);
	bool 			Load				(LPCSTR initial, LPCSTR map_name, bool bUndo);
	void 			Save				(LPCSTR initial, LPCSTR map_name, bool bUndo);
	bool 			LoadSelection		(LPCSTR initial, LPCSTR fname);
	void 			SaveSelection		(EObjClass classfilter, LPCSTR initial, LPCSTR fname);
	void 			Unload				();
	void 			Clear				();
	void 			LoadCompilerError	(LPCSTR fn);
    void 			ClearCompilerErrors	(){m_CompilerErrors.Clear();}

	IC bool 		valid				()           	{ return m_Valid; }

	IC bool 		locked				()          	{ return m_Locked!=0; }
	IC void 		lock				()            	{ m_Locked++; }
	IC void 		unlock				()          	{ m_Locked--; }
	IC void 		waitlock			()        		{ while( locked() ) Sleep(0); }

	IC ESceneCustomMTools* GetMTools	(EObjClass cat)	{ return m_SceneTools[cat]; }
	IC ESceneCustomOTools* GetOTools	(EObjClass cat)	{ return dynamic_cast<ESceneCustomOTools*>(GetMTools(cat)); }
	IC SceneToolsMapPairIt FirstTools	()				{ return m_SceneTools.begin(); }
	IC SceneToolsMapPairIt LastTools	()				{ return m_SceneTools.end(); }

	IC ObjectList&	ListObj    			(EObjClass cat)	{ VERIFY(GetOTools(cat)); return GetOTools(cat)->GetObjects(); }
	IC ObjectIt 	FirstObj      		(EObjClass cat)	{ VERIFY(GetOTools(cat)); return ListObj(cat).begin(); }
	IC ObjectIt 	LastObj       		(EObjClass cat)	{ VERIFY(GetOTools(cat)); return ListObj(cat).end(); }
	IC int 			ObjCount           	(EObjClass cat)	{ return ListObj(cat).size(); }
	int 			ObjCount 			();

	void 			RenderSky			(const Fmatrix& camera);
	void 			Render              (const Fmatrix& camera);
	void 			OnFrame				(float dT);

	virtual void 	AppendObject		(CCustomObject* object, bool bExecUndo=true);
	virtual bool 	RemoveObject		(CCustomObject* object, bool bExecUndo=true);
    bool 			ContainsObject		(CCustomObject* object, EObjClass classfilter);

    // Snap List Part
	bool 			FindObjectInSnapList(CCustomObject* O);
	bool 			AddToSnapList		(CCustomObject* O, bool bUpdateScene=true);
    bool			DelFromSnapList		(CCustomObject* O, bool bUpdateScene=true);
	int 			AddSelToSnapList	();
    int 			DelSelFromSnapList	();
    int 			SetSnapList			();
    void 			RenderSnapList		();
    void 			RenderCompilerErrors();
    void 			ClearSnapList		(bool bCurrentOnly);
    void 			SelectSnapList		();
    void 			UpdateSnapList 	   	();
	virtual ObjectList* 	GetSnapList			(bool bIgnoreUse);

	virtual CCustomObject*	RayPickObject 		(float dist, const Fvector& start, const Fvector& dir, EObjClass classfilter, SRayPickInfo* pinf, ObjectList* from_list);
	int 			BoxPickObjects		(const Fbox& box, SBoxPickInfoVec& pinf, ObjectList* from_list);
    int				RayQuery			(SPickQuery& RQ, const Fvector& start, const Fvector& dir, float dist, u32 flags, ObjectList* snap_list);
    int 			BoxQuery			(SPickQuery& RQ, const Fbox& bb, u32 flags, ObjectList* snap_list);
    int				RayQuery			(SPickQuery& RQ, const Fvector& start, const Fvector& dir, float dist, u32 flags, CDB::MODEL* model);
    int 			BoxQuery			(SPickQuery& RQ, const Fbox& bb, u32 flags, CDB::MODEL* model);

	int 			RaySelect           (int flag, EObjClass classfilter=OBJCLASS_DUMMY); // flag=0,1,-1 (-1 invert)
	int 			FrustumSelect       (int flag, EObjClass classfilter=OBJCLASS_DUMMY);
	void			SelectObjects       (bool flag, EObjClass classfilter=OBJCLASS_DUMMY);
	int 			LockObjects         (bool flag, EObjClass classfilter=OBJCLASS_DUMMY, bool bAllowSelectionFlag=false, bool bSelFlag=true);
	void 			ShowObjects         (bool flag, EObjClass classfilter=OBJCLASS_DUMMY, bool bAllowSelectionFlag=false, bool bSelFlag=true);
	void			InvertSelection     (EObjClass classfilter);
	int 			SelectionCount      (bool testflag, EObjClass classfilter);
	void			RemoveSelection     (EObjClass classfilter);
	void 			CutSelection        (EObjClass classfilter);
	void			CopySelection       (EObjClass classfilter);
	void			PasteSelection      ();

	void 			SelectLightsForObject(CCustomObject* obj);

    void 			ZoomExtents			(EObjClass cls, BOOL bSelectedOnly);

	int 			FrustumPick			(const CFrustum& frustum, EObjClass classfilter, ObjectList& ol);
	int 			SpherePick			(const Fvector& center, float radius, EObjClass classfilter, ObjectList& ol);

	virtual void			GenObjectName		(EObjClass cls_id, char *buffer, const char* prefix=NULL);
	virtual CCustomObject* 	FindObjectByName	(LPCSTR name, EObjClass classfilter);
    virtual CCustomObject* 	FindObjectByName	(LPCSTR name, CCustomObject* pass_object);
    bool 			FindDuplicateName   ();

	void 			UndoClear			();
	void 			UndoSave			();
	bool 			Undo				();
	bool 			Redo				();

    bool 			GetBox				(Fbox& box, EObjClass classfilter);
    bool 			GetBox				(Fbox& box, ObjectList& lst);

public:
	int  			GetQueryObjects		(ObjectList& objset, EObjClass classfilter, int iSel=1, int iVis=1, int iLock=0);
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
	bool 			IsUnsaved		();
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

    std::string		LevelPath		();
};

//----------------------------------------------------
extern EScene*& Scene;
//----------------------------------------------------

#endif /*_INCDEF_Scene_H_*/

