//----------------------------------------------------
// file: SceneObject.h
//----------------------------------------------------
#ifndef _INCDEF_SceneObject_H_
#define _INCDEF_SceneObject_H_

#include "EditObject.h"
#include "customobject.h"
//----------------------------------------------------

class CSceneObject : public CCustomObject {
	CEditableObject*m_pRefs;
    st_Version		m_ObjVer;
public:
	enum{
		eDummy 	 	= (1<<0),
		eFORCE32	= DWORD(-1)
    };
private:
	Fbox			m_TBBox;
	// options
	DWORD			m_dwFlags;
public:
    // get object properties methods
	IC DWORD& 		GetFlags	   			(){return m_dwFlags; }
	IC bool 		IsFlag	     			(DWORD flag){return !!(m_dwFlags&flag); }
    IC void			SetFlag					(DWORD flag){m_dwFlags|=flag;}
    IC void			ResetFlag				(DWORD flag){m_dwFlags&=~flag;}
public:
	// sounds
    AStringVec		m_Sounds;
    AnsiString		m_ActiveSound;
    IC bool			IsSoundable				()	{return !m_Sounds.empty();}
    IC bool			IsSoundActive			()	{return IsSoundable()&&!m_ActiveSound.IsEmpty(); }
    void			RemoveSound				(LPCSTR name);
    void			ClearSounds				();
    bool			AppendSound				(const char* fname);

	void			SetActiveSound			(LPCSTR snd);
	void			ResetActiveSound		(){SetActiveSound("");}

	// motions
    st_AnimParam	m_OMParam;
	OMotionVec		m_OMotions;
    COMotion*		m_ActiveOMotion;
    Fvector			m_vMotionPosition;
    Fvector			m_vMotionRotation;

    COMotion* 		FindOMotionByName		(const char* name, const COMotion* Ignore=0);
    void			GenerateOMotionName		(char* buffer, const char* start_name, const COMotion* M);
    void			RemoveOMotion			(const char* name);
    bool			RenameOMotion			(const char* old_name, const char* new_name);
    COMotion*		AppendOMotion			(const char* fname);
    void			ClearOMotions			();
    void			LoadOMotions			(const char* fname);
    void			SaveOMotions			(const char* fname);
    COMotion*		LoadOMotion				(const char* fname);

    // object motions
    IC OMotionIt	FirstOMotion			()	{return m_OMotions.begin();}
    IC OMotionIt	LastOMotion				()	{return m_OMotions.end();}
    IC int			OMotionCount 			()	{return m_OMotions.size();}
    IC bool			IsOMotionable			()	{return !m_OMotions.empty();}
    IC bool			IsOMotionActive			()	{return IsOMotionable()&&m_ActiveOMotion; }

	void			SetActiveOMotion		(COMotion* mot);
	void			ResetActiveOMotion		(){SetActiveOMotion(0);}

    virtual Fvector& GetPosition			()	{return m_ActiveOMotion?m_vMotionPosition:FPosition; }
    virtual Fvector& GetRotation			()	{return m_ActiveOMotion?m_vMotionRotation:FRotation; }
    virtual void 	SetPosition				(Fvector& pos)	{ if (m_ActiveOMotion) m_vMotionPosition.set(pos); else FPosition.set(pos);	UpdateTransform();}
	virtual void 	SetRotation				(Fvector& rot)	{ if (m_ActiveOMotion) m_vMotionRotation.set(rot); else FRotation.set(rot);	UpdateTransform();}
protected:
	typedef CCustomObject inherited;
    int				m_iBlinkTime;           
public:
    // constructor/destructor methods
					CSceneObject			(LPVOID data, LPCSTR name);
	virtual 		~CSceneObject			();

	virtual void 	Select					(BOOL flag);
	void 			Construct				(LPVOID data);

    IC bool			CheckVersion			()  {return m_pRefs?(m_ObjVer==m_pRefs->m_ObjVer):false;}
    // get object properties methods
	IC bool 		RefCompare				(CEditableObject *to){return m_pRefs?!!(m_pRefs==to):false; }
	IC bool 		RefCompare				(LPCSTR ref){return m_pRefs?(strcmp(ref,m_pRefs->GetName())==0):false; }
	IC CEditableObject*	GetReference		()	{return m_pRefs; }
	CEditableObject*SetReference			(LPCSTR ref_name);
	IC EditMeshVec* Meshes					() {return m_pRefs?&m_pRefs->Meshes():0;}
    IC LPCSTR		GetRefName				() {return m_pRefs?m_pRefs->GetName():0;}

    // statistics methods
	IC bool 		IsDynamic     			()	{return (m_pRefs?m_pRefs->IsDynamic():false); }
	IC bool 		IsStatic     			()	{return (m_pRefs?m_pRefs->IsStatic():false); }
    int 			GetFaceCount			();
    void			GetFaceWorld			(CEditableMesh* M, int idx, Fvector* verts);
	int 			GetVertexCount			();
    int 			GetSurfFaceCount		(const char* surf_name);

    // render methods
	virtual bool 	IsRender				();
	virtual void 	Render					(int priority, bool strictB2F);
	void 			RenderSelection			(DWORD color=0x40E64646);
	void 			RenderEdge				(CEditableMesh* m=0, DWORD color=0xFFC0C0C0);
	void 			RenderBones				();
	void 			RenderAnimation			();
	void 			RenderSingle			();

    // update methods           
	virtual void 	OnFrame					();
    virtual void	OnUpdateTransform		();

    // misc
	void		    EvictObject				();
    virtual void 	ResetAnimation			(bool upd_t=true);

    // pick methods
    bool 			BoxPick					(const Fbox& box, SBoxPickInfoVec& pinf);
	virtual bool 	RayPick					(float& dist, Fvector& S, Fvector& D, SRayPickInfo* pinf=0);
	virtual bool 	FrustumPick				(const CFrustum& frustum);
    virtual bool 	SpherePick				(const Fvector& center, float radius);

    // change position/orientation methods
	virtual void 	PivotScale				(const Fmatrix& prev_inv, const Fmatrix& current, Fvector& amount);
	virtual void 	Scale					(Fvector& amount);

    // get orintation/bounding volume methods
	virtual bool 	GetBox					(Fbox& box);
	virtual bool 	GetUTBox				(Fbox& box);
    void 			GetFullTransformToWorld	(Fmatrix& m);
    void 			GetFullTransformToLocal	(Fmatrix& m);

    // load/save methods
  	virtual bool 	Load					(CStream&);
	virtual void 	Save					(CFS_Base&);
    virtual bool	ExportGame				(SExportStreams& data);
};
//----------------------------------------------------
#endif /*_INCDEF_EditObject_H_*/


