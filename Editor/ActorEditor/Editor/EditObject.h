//----------------------------------------------------
// file: CEditObject.h
//----------------------------------------------------
#ifndef _INCDEF_Object_H_
#define _INCDEF_Object_H_

#include "SceneClassList.h"
//----------------------------------------------------
struct 	SRayPickInfo;
class 	CEditableMesh;
class 	CFrustum;
class 	Shader;
class 	CSMotion;
class 	COMotion;
class 	CCustomMotion;
class	CBone;
class	Mtl;
struct	FSChunkDef;
struct	st_ObjectDB;

class CSurface
{
	AnsiString		m_Name;
    AnsiString		m_Texture;	//
    AnsiString		m_VMap;		//
    AnsiString		m_ShaderName;
    AnsiString		m_ShaderXRLCName;
    Shader*			m_Shader;
    BYTE			m_Sideflag;
    DWORD			m_dwFVF;
public:
    				CSurface		(){ZeroMemory(this,sizeof(CSurface));}
#ifdef _EDITOR
					~CSurface		(){DeleteShader();}
	IC void			CopyFrom		(CSurface* surf){*this = *surf; m_Shader=0;}
    IC int			_Priority		()const {return m_Shader?m_Shader->Flags.iPriority:1;}
    IC bool			_StrictB2F		()const {return m_Shader?m_Shader->Flags.bStrictB2F:false;}
#endif
    IC LPCSTR		_Name			()const {return m_Name.c_str();}
    IC Shader*		_Shader			()const {return m_Shader;}
    IC LPCSTR		_ShaderName		()const {return m_ShaderName.c_str();}
    IC LPCSTR		_ShaderXRLCName	()const {return m_ShaderXRLCName.c_str();}
    IC DWORD		_FVF			()const {return m_dwFVF;}
    IC BOOL			_2Sided			()const {return !!m_Sideflag;}
    IC LPCSTR		_Texture		(){return m_Texture.c_str();}
    IC LPCSTR		_VMap			(){return m_VMap.c_str();}
    IC void			SetName			(LPCSTR name){m_Name=name;}
    IC void			SetShader		(LPCSTR name, Shader* sh){R_ASSERT(name&&name[0]); m_ShaderName=name; m_Shader=sh;}
    IC void			ED_SetShader	(LPCSTR name){R_ASSERT(name&&name[0]); m_ShaderName=name;}
    IC void 		SetShaderXRLC	(LPCSTR name){R_ASSERT(name&&name[0]); m_ShaderXRLCName=name;}
    IC void			Set2Sided		(BOOL fl){m_Sideflag=fl;}
    IC void			SetFVF			(DWORD fvf){m_dwFVF=fvf;}
    IC void			SetTexture		(LPCSTR name){m_Texture=name;}
    IC void			SetVMap			(LPCSTR name){m_VMap=name;}
#ifdef _EDITOR
    IC void			CreateShader	(){m_Shader=Device.Shader.Create(m_ShaderName.c_str(),m_Texture.c_str());}
    IC void			DeleteShader	(){ if (m_Shader) Device.Shader.Delete(m_Shader); m_Shader=0; }
#endif
#ifdef _MAX_EXPORT
	DWORD			mat_id;
	Mtl*			pMtlMain;
#endif
};

DEFINE_VECTOR	(CSurface*,SurfaceVec,SurfaceIt);
DEFINE_VECTOR	(CSurface,SurfInstVec,SurfInstIt);
DEFINE_VECTOR	(CEditableMesh*,EditMeshVec,EditMeshIt);
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

class CEditableObject{
	friend class CSceneObject;
	friend class CEditableMesh;
    friend class TfrmPropertiesObject;
    friend class CSector;
    friend class TUI_ControlSectorAdd;
	friend class ELibrary;
	friend class TfrmEditLibrary;
	friend class MeshExpUtility;

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

	// bounding volume
	Fbox 			m_Box;
public:
    // temp variables for transformation
	Fvector 		t_vPosition;
    Fvector			t_vScale;
    Fvector			t_vRotate;

    bool			t_bOnModified;
    IC bool			IsModified				(){return t_bOnModified;}
    IC void			Modified				(){t_bOnModified=true;}

    AnsiString		m_LoadName;
    int				m_RefCount;
protected:
    st_Version		m_ObjVer;

    void 			ClearGeometry			();

    void			ClearRenderBuffers		();

    bool			Import_LWO				(const char* fname, bool bNeedOptimize);
    void			UpdateRenderBuffers		();

	void 			UpdateBoneParenting		();
public:
    DWORD			m_LoadState;

	AnsiString		m_LibName;
public:
    // constructor/destructor methods
					CEditableObject			(LPCSTR name);
	virtual 		~CEditableObject		();

    LPCSTR			GetName					(){ return m_LibName.c_str();}

    IC EditMeshIt	FirstMesh				()	{return m_Meshes.begin();}
    IC EditMeshIt	LastMesh				()	{return m_Meshes.end();}
    IC EditMeshVec& Meshes					()	{return m_Meshes; }
    IC int			MeshCount				()	{return m_Meshes.size();}
    IC SurfaceIt	FirstSurface			()	{return m_Surfaces.begin();}
    IC SurfaceIt	LastSurface				()	{return m_Surfaces.end();}
    IC int			SurfaceCount			()	{return m_Surfaces.size();}
    IC BoneIt		FirstBone				()	{return m_Bones.begin();}
    IC BoneIt		LastBone				()	{return m_Bones.end();}
	IC BoneVec&		Bones					()	{return m_Bones;}
    IC int			BoneCount				()	{return m_Bones.size();}
    IC CBone*		GetBone					(DWORD idx){VERIFY(idx<m_Bones.size()); return m_Bones[idx];}
    void			GetBoneWorldTransform	(DWORD bone_idx, float t, CSMotion* motion, Fmatrix& matrix);
    IC SMotionIt	FirstSMotion			()	{return m_SMotions.begin();}
    IC SMotionIt	LastSMotion				()	{return m_SMotions.end();}
    IC int			SMotionCount 			()	{return m_SMotions.size();}
    IC OMotionIt	FirstOMotion			()	{return m_OMotions.begin();}
    IC OMotionIt	LastOMotion				()	{return m_OMotions.end();}
    IC int			OMotionCount 			()	{return m_OMotions.size();}

///    IC bool			CheckVersion			()  {if(m_LibRef) return (m_ObjVer==m_LibRef->m_ObjVer); return true;}
    // get object properties methods
	IC bool 		IsDynamic     			()	{return (m_DynamicObject!=0); }
    IC void			SetDynamic				(bool bDynamic){m_DynamicObject=bDynamic;}
	IC AnsiString&	GetClassScript			()	{return m_ClassScript;}
    IC const Fbox&	GetBox					() 	{return m_Box;}

    // animation
    IC bool			IsOMotionable			()	{return !!m_OMotions.size();}
    IC bool			IsOMotionActive			()	{return IsOMotionable()&&m_ActiveOMotion; }
	void			SetActiveOMotion		(COMotion* mot, bool upd_t=true);
    IC bool			IsSkeleton				()	{return !!m_Bones.size();}
    IC bool			IsSMotionActive			()	{return IsSkeleton()&&m_ActiveSMotion; }
	void			SetActiveSMotion		(CSMotion* mot);
	bool 			CheckBoneCompliance		(CSMotion* M);

	void 			ResetAnimation			(bool upd_t=true);
    void			CalculateAnimation		(bool bGenInvMat=false);

    // statistics methods
    int 			GetFaceCount			();
	int 			GetVertexCount			();
    int 			GetSurfFaceCount		(const char* surf_name);

    // render methods
	void 			Render					(Fmatrix& parent, int priority, bool strictB2F);
	void 			RenderSelection			(Fmatrix& parent);
	void 			RenderEdge				(Fmatrix& parent, CEditableMesh* m=0);
	void 			RenderBones				(const Fmatrix& parent);
	void 			RenderAnimation			(const Fmatrix& parent);
	void 			RenderSingle			(Fmatrix& parent);

    // update methods
	void 			RTL_Update				(float dT);
	void 			UpdateBox				();
	void		    LightenObject			();

    // pick methods
	bool 			RayPick					(float& dist, Fvector& S, Fvector& D, Fmatrix& parent, SRayPickInfo* pinf=0);
#ifdef _LEVEL_EDITOR
    void 			BoxPick					(const Fbox& box, Fmatrix& parent, SBoxPickInfoVec& pinf);
	bool 			FrustumPick				(const CFrustum& frustum, const Fmatrix& parent);
    bool 			SpherePick				(const Fvector& center, float radius, const Fmatrix& parent);
#endif
    // change position/orientation methods
	void 			TranslateToWorld		(const Fmatrix& parent);

    // clone/copy methods
    void			RemoveMesh				(CEditableMesh* mesh);
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
	bool 			Reload					();
	bool 			Load					(const char* fname);
	bool 			LoadObject				(const char* fname);
	void 			SaveObject				(const char* fname);
    CSMotion*		LoadSMotion				(const char* fname);
    COMotion*		LoadOMotion				(const char* fname);
  	bool 			Load					(CStream&);
	void 			Save					(CFS_Base&);

    // contains methods
    CEditableMesh* 	FindMeshByName			(const char* name, CEditableMesh* Ignore=0);
    void			GenerateMeshNames		();
    void			VerifyMeshNames			();
    bool 			ContainsMesh			(const CEditableMesh* m);
	CSurface*		FindSurfaceByName		(const char* surf_name, int* s_id=0);
    CBone*			FindBoneByName			(const char* name);
    int				GetBoneIndexByWMap		(const char* wm_name);
    COMotion* 		FindOMotionByName		(const char* name, const COMotion* Ignore=0);
    CSMotion* 		FindSMotionByName		(const char* name, const CSMotion* Ignore=0);
    void			GenerateOMotionName		(char* buffer, const char* start_name, const COMotion* M);
    void			GenerateSMotionName		(char* buffer, const char* start_name, const CSMotion* M);

    // device dependent routine
	void 			OnDeviceCreate 			();
	void 			OnDeviceDestroy			();

#ifdef _MAX_EXPORT
	CSurface*		CreateSurface			(Mtl* M, DWORD m_id);
	LPCSTR			GenerateSurfaceName		(const char* base_name);
#endif
	
#ifdef _LWO_EXPORT
		// import routines
	bool			Import_LWO				(st_ObjectDB *I);
#endif
};
//----------------------------------------------------
#define EOBJ_CURRENT_VERSION		0x0010
//----------------------------------------------------
#define EOBJ_CHUNK_OBJECT_BODY		0x7777
#define EOBJ_CHUNK_VERSION		  	0x0900
#define EOBJ_CHUNK_REFERENCE     	0x0902
#define EOBJ_CHUNK_FLAG           	0x0903
#define EOBJ_CHUNK_SURFACES			0x0905
#define EOBJ_CHUNK_EDITMESHES      	0x0910
#define EOBJ_CHUNK_LIB_VERSION     	0x0911
#define EOBJ_CHUNK_CLASSSCRIPT     	0x0912
#define EOBJ_CHUNK_BONES			0x0913
#define EOBJ_CHUNK_OMOTIONS			0x0914
#define EOBJ_CHUNK_ACTIVE_OMOTION	0x0915
#define EOBJ_CHUNK_SMOTIONS			0x0916
#define EOBJ_CHUNK_ACTIVE_SMOTION	0x0917
#define EOBJ_CHUNK_SURFACES_XRLC	0x0918
//----------------------------------------------------

#endif /*_INCDEF_EditObject_H_*/


