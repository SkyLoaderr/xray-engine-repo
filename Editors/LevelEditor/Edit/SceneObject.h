//----------------------------------------------------
#ifndef SceneObjectH
#define SceneObjectH

#include "EditObject.h"
#include "customobject.h"
//----------------------------------------------------

class CSceneObject : public CCustomObject {
	AnsiString		m_ReferenceName;
	CEditableObject*m_pReference;
    st_Version		m_ObjVer;
	void __fastcall ReferenceChange			(PropValue* sender);
public:
	enum{
		flDummy 	= (1<<0),
		flFORCE32	= u32(-1)
    };
private:
	Fbox			m_TBBox;
	// options
    Flags32			m_Flags;
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
    virtual void 	SetScale				(Fvector& scale)
    { 
    	if (m_pReference&&m_pReference->IsDynamic()){
        	ELog.Msg(mtError,"Dynamic object %s - can't scale.", Name);
        }else{
			FScale.set(scale); 
            UpdateTransform();
        }
    }
protected:
	typedef CCustomObject inherited;
    int				m_iBlinkTime;           
public:
    // constructor/destructor methods
					CSceneObject			(LPVOID data, LPCSTR name);
	virtual 		~CSceneObject			();

	virtual void 	Select					(BOOL flag);
	void 			Construct				(LPVOID data);

    IC bool			CheckVersion			()  {return m_pReference?(m_ObjVer==m_pReference->m_ObjVer):false;}
    // get object properties methods
	IC bool 		RefCompare				(CEditableObject *to){return m_pReference?!!(m_pReference==to):false; }
	IC bool 		RefCompare				(LPCSTR ref){return m_pReference?(strcmp(ref,m_pReference->GetName())==0):false; }
	IC CEditableObject*	GetReference		()	{return m_pReference; }
	CEditableObject*SetReference			(LPCSTR ref_name);
	CEditableObject*UpdateReference			();
	IC EditMeshVec* Meshes					() {return m_pReference?&m_pReference->Meshes():0;}
    IC LPCSTR		GetRefName				() {return m_pReference?m_pReference->GetName():0;}

    // statistics methods
	IC bool 		IsDynamic     			()	{return (m_pReference?m_pReference->IsDynamic():false); }
	IC bool 		IsStatic     			()	{return (m_pReference?m_pReference->IsStatic():false); }
	IC bool 		IsMUStatic     			()	{return (m_pReference?m_pReference->IsMUStatic():false); }
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

    // get orintation/bounding volume methods
	virtual bool 	GetBox					(Fbox& box);
	virtual bool 	GetUTBox				(Fbox& box);
    void 			GetFullTransformToWorld	(Fmatrix& m);
    void 			GetFullTransformToLocal	(Fmatrix& m);

	// editor integration
	virtual void	FillProp				(LPCSTR pref, PropItemVec& values);

    // load/save methods
  	virtual bool 	Load					(CStream&);
	virtual void 	Save					(CFS_Base&);
    virtual bool	ExportGame				(SExportStreams& data);
};
//----------------------------------------------------
#endif /*_INCDEF_EditObject_H_*/


