//----------------------------------------------------
// file: CEditObject.h
//----------------------------------------------------
#ifndef _INCDEF_Object_H_
#define _INCDEF_Object_H_

#include "SceneObject.h"
//----------------------------------------------------
struct 	SRayPickInfo;
class 	CEditMesh;
class 	CFrustum;
class 	Shader;
class 	ETextureCore;
class 	CSMotion;
class 	COMotion;
class 	CCustomMotion;
class	CBone;

struct st_Surface{
	char			name[MAX_OBJ_NAME];
    AStringVec		textures;       //
    AStringVec		vmaps;			// одинаковый размер массивов!!!
    Shader*			shader;
    BYTE			sideflag;
    DWORD			dwFVF;
    bool			has_alpha; 		// use internal (only for render)
    				st_Surface		(){ZeroMemory(this,sizeof(st_Surface));}
				    ~st_Surface		();
    ERenderPriority	RenderPriority	();
};
DEFINE_VECTOR	(st_Surface*,SurfaceVec,SurfaceIt);
DEFINE_VECTOR	(CEditMesh*,EditMeshVec,EditMeshIt);
DEFINE_VECTOR	(CBone*,BoneVec,BoneIt);
DEFINE_VECTOR	(COMotion*,OMotionVec,OMotionIt);
DEFINE_VECTOR	(CSMotion*,SMotionVec,SMotionIt);

#define EOBJECT_LS_RENDERBUFFER		0x0001

struct st_AnimParam{
    float			t;
    float			min_t;
    float			max_t;
    void			Set		(CCustomMotion* M);
    float			Frame	()			{ return t;}
    void			Update	(float dt);
};

class CEditObject : public SceneObject {
	friend class CEditMesh;
    friend class TfrmPropertiesObject;
    friend class CSector;
    friend class TUI_ControlSectorAdd;
    friend class CLibObject;

// general
	AnsiString		m_ClassScript;

	SurfaceVec		m_Surfaces;
	EditMeshVec		m_Meshes;
	BoneVec			m_Bones;
	SMotionVec		m_SMotions;
	OMotionVec		m_OMotions;

    COMotion*		m_ActiveOMotion;
    st_AnimParam	m_OMParam;

    CSMotion*		m_ActiveSMotion;
    st_AnimParam	m_SMParam;
protected:
	// options
	int 			m_DynamicObject;

    // orientation
    Fvector 		vScale;
    Fvector 		vRotate;
    Fvector 		vPosition;
	Fmatrix 		mTransform;

    Fvector 		vMotRotate;
    Fvector 		vMotPosition;

	// bounding volume
	Fbox 			m_Box;
    // internal use
    float 			m_fRadius;
    Fvector 		m_Center; 			// центр в мировых координатах

public:
    struct st_ObjVer{
        union{
            struct{
                int f_age;
                int res0;
            };
            __int64 ver;
        };
        st_ObjVer   (){reset();}
        int size	(){return sizeof(st_ObjVer);}
        bool operator == (st_ObjVer& v)	{return v.f_age==f_age;}
        void reset	(){ver=0;}
    };
protected:    
    st_ObjVer		m_ObjVer;
	CEditObject 	*m_LibRef;
    bool 			bLibItem;

    void 			CopyGeometry			(CEditObject* source);
    void 			ClearGeometry			();

    void			ClearRenderBuffers		();

    bool			Import_LWO				(const char* fname, bool bNeedOptimize);
    void			UpdateRenderBuffers		();

	void 			UpdateBoneParenting		();

    void			UpdateTransform			(const Fvector& T, const Fvector& R, const Fvector& S);
public:
    DWORD			m_LoadState;
public:
    // constructor/destructor methods
					CEditObject				(bool bLib=false);
					CEditObject				(char *name, bool bLib=false);
    				CEditObject				(CEditObject* source);
	virtual 		~CEditObject			();

	void 			Construct				();
    void 			CloneFrom				(CEditObject* source, bool bSetPlacement);

    IC EditMeshIt	FirstMesh				()	{return m_Meshes.begin();}
    IC EditMeshIt	LastMesh				()	{return m_Meshes.end();}
    IC int			MeshCount				()	{return m_Meshes.size();}
    IC SurfaceIt	FirstSurface			()	{return m_Surfaces.begin();}
    IC SurfaceIt	LastSurface				()	{return m_Surfaces.end();}
    IC int			SurfaceCount			()	{return m_Surfaces.size();}
    IC BoneIt		FirstBone				()	{return m_Bones.begin();}
    IC BoneIt		LastBone				()	{return m_Bones.end();}
    IC int			BoneCount				()	{return m_Bones.size();}
    IC CBone*		GetBone					(DWORD idx){VERIFY(idx<m_Bones.size()); return m_Bones[idx];}
    void			GetBoneWorldTransform	(DWORD bone_idx, float t, CSMotion* motion, Fmatrix& matrix);
    IC SMotionIt	FirstSMotion			()	{return m_SMotions.begin();}
    IC SMotionIt	LastSMotion				()	{return m_SMotions.end();}
    IC int			SMotionCount 			()	{return m_SMotions.size();}
    IC OMotionIt	FirstOMotion			()	{return m_OMotions.begin();}
    IC OMotionIt	LastOMotion				()	{return m_OMotions.end();}
    IC int			OMotionCount 			()	{return m_OMotions.size();}
    
    IC bool			CheckVersion			()  {if(m_LibRef) return (m_ObjVer==m_LibRef->m_ObjVer); return true;}
    // get object properties methods
	IC bool 		IsLibItem     			()	{return bLibItem; }
	IC bool 		IsReference   			()	{return (m_LibRef!=0); }
	IC bool 		IsSceneItem   			()	{return ((m_LibRef==0)&&!bLibItem); }
	IC bool 		IsDynamic     			()	{return (m_DynamicObject!=0); }
    IC void			SetDynamic				(bool bDynamic){m_DynamicObject=bDynamic;}
	IC char 		*GetRefName   			()	{return m_LibRef?m_LibRef->m_Name:m_Name; }
	IC bool 		RefCompare				(CEditObject *to){return (m_LibRef==to); }
	IC CEditObject*	GetRef					()	{return m_LibRef; }
	IC void			SetRef					(CEditObject* ref)	{m_LibRef = ref;}
	IC AnsiString&	GetClassScript			()	{return m_ClassScript; }
    IC bool			IsOMotionable			()	{return (m_LibRef)?!!m_LibRef->IsOMotionable():!!m_OMotions.size();}
    IC bool			IsOMotionActive			()	{return IsOMotionable()&&m_ActiveOMotion; }
	void			SetActiveOMotion		(COMotion* mot, bool upd_t=true);
    IC bool			IsSkeleton				()	{return (m_LibRef)?m_LibRef->IsSkeleton():!!m_Bones.size();}
    IC bool			IsSMotionActive			()	{return IsSkeleton()&&m_ActiveSMotion; }
	void			SetActiveSMotion		(CSMotion* mot);
	bool 			CheckBoneCompliance		(CSMotion* M);

    virtual void 	ResetAnimation			(bool upd_t=true);
    void			CalculateAnimation		(bool bGenInvMat=false);

    // statistics methods
    int 			GetFaceCount			();
	int 			GetVertexCount			();
    int 			GetSurfFaceCount		(const char* surf_name);

    // render methods
	virtual bool 	IsRender				(Fmatrix& parent);
	virtual void 	Render					(Fmatrix& parent, ERenderPriority flag);
	void 			RenderSelection			(Fmatrix& parent);
	void 			RenderEdge				(Fmatrix& parent, CEditMesh* m=0);
	void 			RenderBones				(const Fmatrix& parent);
	void 			RenderAnimation			(const Fmatrix& parent);
	void 			RenderSingle			(Fmatrix& parent);

    // update methods
	virtual void 	RTL_Update				(float dT);
	void 			UpdateBox				();
    virtual void	UpdateTransform			();
	void		    LightenObject			();

    // pick methods
    void 			BoxPick					(const Fbox& box, Fmatrix& parent, SBoxPickInfoVec& pinf);
	virtual bool 	RayPick					(float& dist, Fvector& S, Fvector& D, Fmatrix& parent, SRayPickInfo* pinf=0);
	virtual bool 	FrustumPick				(const CFrustum& frustum, const Fmatrix& parent);
    virtual bool 	SpherePick				(const Fvector& center, float radius, const Fmatrix& parent);

    // change position/orientation methods
	virtual void 	Move					(Fvector& amount);
	virtual void 	Rotate					(Fvector& center, Fvector& axis, float angle);
	virtual void 	Scale					(Fvector& center, Fvector& amount);
	virtual void 	LocalRotate				(Fvector& axis, float angle);
	virtual void 	LocalScale				(Fvector& amount);
	virtual void 	TranslateToWorld		();

    // get orintation/bounding volume methods
	virtual bool 	GetBox					(Fbox& box);
    void 			GetFullTransformToWorld	(Fmatrix& m);
    void 			GetFullTransformToLocal	(Fmatrix& m);
    IC const Fvector& GetCenter				(){return m_Center;}
    IC float		GetRadius				(){return m_fRadius;}
    IC const Fmatrix& GetTransform			(){return mTransform;}
    IC const Fvector& GetPosition			(){return vPosition;}
    IC const Fvector& GetRotate				(){return vRotate;}
    IC const Fvector& GetScale				(){return vScale;}
    IC Fvector& 	Position				(){return vPosition;}
    IC Fvector& 	Rotate					(){return vRotate;}
    IC Fvector& 	Scale					(){return vScale;}

    virtual bool 	GetPosition				(Fvector& pos){pos.set(vPosition); return true; }
    virtual bool 	GetRotate				(Fvector& rot){rot.set(vRotate); return true; }
    virtual bool 	GetScale				(Fvector& scale){scale.set(vScale); return true; }

    virtual void 	SetPosition				(Fvector& pos){vPosition.set(pos);}
    virtual void 	SetRotate				(Fvector& rot){vRotate.set(rot);}
    virtual void 	SetScale				(Fvector& scale){vScale.set(scale);}

    // clone/copy methods
	virtual void 	CloneFromLib			(SceneObject *source);
	virtual void 	LibReference			(SceneObject *source);
	virtual void 	ResolveReference		();
    void			RemoveMesh				(CEditMesh* mesh);
    void			RemoveOMotion			(const char* name);
    bool			RenameOMotion			(const char* old_name, const char* new_name);
    COMotion*		AppendOMotion			(const char* fname);
    void			ClearOMotions			();
    void			LoadOMotions			(const char* fname);
    void			SaveOMotions			(const char* fname);

    void			RemoveSMotion			(const char* name);
    bool			RenameSMotion			(const char* old_name, const char* new_name);
    CSMotion*		AppendSMotion			(const char* fname);
    bool			ReloadSMotion			(CSMotion* M, const char* fname);
    void			ClearSMotions			();
    void			LoadSMotions			(const char* fname);
    void			SaveSMotions			(const char* fname);

    // load/save methods
	void 			LoadMeshDef				(FSChunkDef *chunk);
	bool 			Load					(const char* fname);
	bool 			LoadObject				(const char* fname);
	void 			SaveObject				(const char* fname);
    CSMotion*		LoadSMotion				(const char* fname);
    COMotion*		LoadOMotion				(const char* fname);
  	virtual bool 	Load					(CStream&);
	virtual void 	Save					(CFS_Base&);

    // contains methods
    CEditMesh* 		FindMeshByName			(const char* name, CEditMesh* Ignore=0);
    void			GenerateMeshNames		();
    void			VerifyMeshNames			();
    bool 			ContainsMesh			(const CEditMesh* m);
	st_Surface*		FindSurfaceByName		(const char* surf_name, int* s_id=0);
    CBone*			FindBoneByName			(const char* name);
    int				GetBoneIndexByWMap		(const char* wm_name);
    COMotion* 		FindOMotionByName		(const char* name, const COMotion* Ignore=0);
    CSMotion* 		FindSMotionByName		(const char* name, const CSMotion* Ignore=0);
    void			GenerateOMotionName		(char* buffer, const char* start_name, const COMotion* M);
    void			GenerateSMotionName		(char* buffer, const char* start_name, const CSMotion* M);

    // device dependent routine
	virtual void 	OnDeviceCreate 			();
	virtual void 	OnDeviceDestroy			();
};
//----------------------------------------------------
#endif /*_INCDEF_EditObject_H_*/


