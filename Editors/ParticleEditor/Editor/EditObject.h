//----------------------------------------------------
// file: CEditObject.h
//----------------------------------------------------
#ifndef _INCDEF_Object_H_
#define _INCDEF_Object_H_
                                      
#include "Bone.h"
#include "SceneClassList.h"
//----------------------------------------------------
struct 	SRayPickInfo;
class 	CEditableMesh;
class 	CFrustum;
class 	CSMotion;
class 	COMotion;
class 	CCustomMotion;
class	CBone;
class	Shader;
class	Mtl;
class	CExporter;
struct	FSChunkDef;
struct	st_ObjectDB;

#ifdef _LW_IMPORT
#include <lwobjimp.h>
#include <lwsurf.h>
#endif

#define LOD_SHADER_NAME 	"def_shaders\\lod"
#define LOD_SAMPLE_COUNT 	8

// refs
class XRayMaterial;

class CSurface
{
public:
	AnsiString		m_Name;
    AnsiString		m_Texture;	//
    AnsiString		m_VMap;		//
    AnsiString		m_ShaderName;
    AnsiString		m_ShaderXRLCName;
    Shader*			m_Shader;
    BYTE			m_Sideflag;
    DWORD			m_dwFVF;
#ifdef _MAX_EXPORT
	DWORD			mid;
	Mtl*			mtl;
#endif
#ifdef _LW_IMPORT
	LWSurfaceID		surf_id;
#endif 
	DWORD			tag;
public:
	CSurface		()
	{
		m_Shader	= 0;
		m_Sideflag	= 0;
		m_dwFVF		= 0;
#ifdef _MAX_EXPORT
		mtl			= 0;
		mid			= 0;
#endif
#ifdef _LW_IMPORT
		surf_id		= 0;
#endif 
		tag			= 0;
	}
#ifdef _EDITOR
					~CSurface		(){DeleteShader();}
	IC void			CopyFrom		(CSurface* surf){*this = *surf; m_Shader=0;}
    IC int			_Priority		()const {return m_Shader?m_Shader->lod0->Flags.iPriority:1;}
    IC bool			_StrictB2F		()const {return m_Shader?m_Shader->lod0->Flags.bStrictB2F:false;}
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
    IC void 		SetShaderXRLC	(LPCSTR name){m_ShaderXRLCName=name;}
    IC void			Set2Sided		(BOOL fl){m_Sideflag=fl;}
    IC void			SetFVF			(DWORD fvf){m_dwFVF=fvf;}
    IC void			SetTexture		(LPCSTR name){m_Texture=name;}
    IC void			SetVMap			(LPCSTR name){m_VMap=name;}
#ifdef _EDITOR
    IC void			CreateShader	(){m_Shader=Device.Shader.Create(m_ShaderName.c_str(),m_Texture.c_str());}
    IC void			DeleteShader	(){ if (m_Shader) Device.Shader.Delete(m_Shader); m_Shader=0; }
#endif
};

DEFINE_VECTOR	(CSurface*,SurfaceVec,SurfaceIt);
DEFINE_VECTOR	(CSurface,SurfInstVec,SurfInstIt);
DEFINE_VECTOR	(CEditableMesh*,EditMeshVec,EditMeshIt);
DEFINE_VECTOR	(COMotion*,OMotionVec,OMotionIt);
DEFINE_VECTOR	(CSMotion*,SMotionVec,SMotionIt);

#define EOBJECT_LS_RENDERBUFFER		0x0001

struct st_AnimParam{
    float			t;
    float			min_t;
    float			max_t;
    bool			bPlay;
    bool 			bLooped;
    void			Set		(CCustomMotion* M, bool loop);
    float			Frame	()			{ return t;}
    void			Update	(float dt);
    void			Play	(){bPlay=true; t=min_t;}
    void			Stop	(){bPlay=false; t=min_t;}
    void			Pause	(){bPlay=!bPlay;}
};

struct SBonePart{
	AnsiString 		alias;
    IntVec 			bones;
};
DEFINE_VECTOR(SBonePart,BPVec,BPIt);

class CEditableObject{
	friend class CSceneObject;
	friend class CEditableMesh;
    friend class TfrmPropertiesEObject;
    friend class CSector;
    friend class TUI_ControlSectorAdd;
	friend class ELibrary;
	friend class TfrmEditLibrary;
	friend class MeshExpUtility;

// general
	AnsiString		m_ClassScript;

	SurfaceVec		m_Surfaces;
	EditMeshVec		m_Meshes;

    Shader*			m_LODShader;

	// skeleton
	BoneVec			m_Bones;
	SMotionVec		m_SMotions;
    BPVec			m_BoneParts;
    CSMotion*		m_ActiveSMotion;
public:
    st_AnimParam	m_SMParam;
public:
	enum{
		eoDynamic 	 	= (1<<0),
		eoProgressive 	= (1<<1),
        eoUsingLOD		= (1<<2),	
        eoHOM			= (1<<3),	
		eoFORCE32		= DWORD(-1)
    };
private:
	// options
	DWORD			m_dwFlags;
	// bounding volume
	Fbox 			m_Box;
public:
    // temp variable for actor
	Fvector 		a_vPosition;
    Fvector			a_vRotate;

    // temp variables for transformation
	Fvector 		t_vPosition;
    Fvector			t_vScale;
    Fvector			t_vRotate;
 
    bool			bOnModified;
    IC bool			IsModified				(){return bOnModified;}
    IC void 		Modified				(){bOnModified=true;}

    AnsiString		m_LoadName;
    int				m_RefCount;
protected:
    st_Version		m_ObjVer;

    void 			ClearGeometry			();

    void			ClearRenderBuffers		();

    void			UpdateRenderBuffers		();

	void 			UpdateBoneParenting		();

    void			UpdateLODShader			();
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

    // LOD
	LPCSTR			GetLODTextureName		(AnsiString& name);
    LPCSTR			GetLODShaderName		(){return LOD_SHADER_NAME;}
    void			GetLODFrame				(int frame, Fvector p[4], Fvector2 t[4], const Fmatrix* parent=0);

    // skeleton
    IC BPIt			FirstBonePart			()	{return m_BoneParts.begin();}
    IC BPIt			LastBonePart			()	{return m_BoneParts.end();}
	IC BPVec&		BoneParts				()	{return m_BoneParts;}
    IC int			BonePartCount			()	{return m_BoneParts.size();}
    IC BoneIt		FirstBone				()	{return m_Bones.begin();}
    IC BoneIt		LastBone				()	{return m_Bones.end();}
	IC BoneVec&		Bones					()	{return m_Bones;}
    IC int			BoneCount				()	{return m_Bones.size();}
    LPCSTR			BoneNameByID			(int id);
    int				GetRootBoneID			();
    int				BoneIDByName			(LPCSTR name);
    int				PartIDByName			(LPCSTR name);
    IC CBone*		GetBone					(DWORD idx){VERIFY(idx<m_Bones.size()); return m_Bones[idx];}
    void			GetBoneWorldTransform	(DWORD bone_idx, float t, CSMotion* motion, Fmatrix& matrix);
    IC SMotionIt	FirstSMotion			()	{return m_SMotions.begin();}
    IC SMotionIt	LastSMotion				()	{return m_SMotions.end();}
	SMotionVec&		SMotions				()	{return m_SMotions;}
    IC int			SMotionCount 			()	{return m_SMotions.size();}
    IC void			SkeletonPlay 			()	{m_SMParam.Play();}
    IC void			SkeletonStop 			()	{m_SMParam.Stop();}
    IC void			SkeletonPause 			()	{m_SMParam.Pause();}

///    IC bool			CheckVersion			()  {if(m_LibRef) return (m_ObjVer==m_LibRef->m_ObjVer); return true;}
    // get object properties methods
	IC DWORD& 		GetFlags	   			(){return m_dwFlags; }
	IC bool 		IsFlag	     			(DWORD flag){return !!(m_dwFlags&flag); }
    IC void			SetFlag					(DWORD flag){m_dwFlags|=flag;}
    IC void			ResetFlag				(DWORD flag){m_dwFlags&=~flag;}
    IC bool			IsDynamic				(){return IsFlag(eoDynamic);}

	IC AnsiString&	GetClassScript			()	{return m_ClassScript;}
    IC const Fbox&	GetBox					() 	{return m_Box;}

    // animation
    IC bool			IsSkeleton				()	{return !!m_Bones.size();}
    IC bool			IsSMotionActive			()	{return IsSkeleton()&&m_ActiveSMotion; }
    CSMotion*		GetActiveSMotion		()	{return m_ActiveSMotion; }
	void			SetActiveSMotion		(CSMotion* mot);
	bool 			CheckBoneCompliance		(CSMotion* M);

	void 			ResetAnimation			();
	CSMotion*		ResetSAnimation			();
    void			CalculateAnimation		(bool bGenInvMat=false);

    // statistics methods
    int 			GetFaceCount			();
	int 			GetVertexCount			();
    int 			GetSurfFaceCount		(LPCSTR surf_name);

    // render methods
	void 			Render					(const Fmatrix& parent, int priority, bool strictB2F);
	void 			RenderSelection			(const Fmatrix& parent, CEditableMesh* m=0, DWORD c=0x40E64646);
	void 			RenderEdge				(const Fmatrix& parent, CEditableMesh* m=0, DWORD c=0xFFC0C0C0);
	void 			RenderBones				(const Fmatrix& parent);
	void 			RenderAnimation			(const Fmatrix& parent);
	void 			RenderSingle			(const Fmatrix& parent);
	void 			RenderSkeletonSingle	(const Fmatrix& parent);
	void 			RenderLOD				(const Fmatrix& parent);

    // update methods
	void 			OnFrame					();
	void 			UpdateBox				();
	void		    EvictObject				();

    // pick methods
	bool 			RayPick					(float& dist, Fvector& S, Fvector& D, Fmatrix& inv_parent, SRayPickInfo* pinf=0);
#ifdef _LEVEL_EDITOR
    bool 			BoxPick					(CSceneObject* obj, const Fbox& box, Fmatrix& parent, SBoxPickInfoVec& pinf);
	bool 			FrustumPick				(const CFrustum& frustum, const Fmatrix& parent);
    bool 			SpherePick				(const Fvector& center, float radius, const Fmatrix& parent);
#endif
    // change position/orientation methods
	void 			TranslateToWorld		(const Fmatrix& parent);

    // clone/copy methods
    void			RemoveMesh				(CEditableMesh* mesh);

    bool			RemoveSMotion			(LPCSTR name);
    bool			RenameSMotion			(LPCSTR old_name, LPCSTR new_name);
    CSMotion*		AppendSMotion			(LPCSTR name, LPCSTR fname);
    bool			ReloadSMotion			(CSMotion* M, LPCSTR fname);
    void			ClearSMotions			();
    bool			LoadSMotions			(LPCSTR fname);
    bool			SaveSMotions			(LPCSTR fname);

    // load/save methods
	void 			LoadMeshDef				(FSChunkDef *chunk);
	bool 			Reload					();
	bool 			Load					(LPCSTR fname);
	bool 			LoadObject				(LPCSTR fname);
	void 			SaveObject				(LPCSTR fname);
    CSMotion*		LoadSMotion				(LPCSTR fname);
  	bool 			Load					(CStream&);
	void 			Save					(CFS_Base&);
	bool			Import_LWO				(LPCSTR fname, bool bNeedOptimize);

    // contains methods
    CEditableMesh* 	FindMeshByName			(LPCSTR name, CEditableMesh* Ignore=0);
    void			GenerateMeshNames		();
    void			VerifyMeshNames			();
    bool 			ContainsMesh			(const CEditableMesh* m);
	CSurface*		FindSurfaceByName		(LPCSTR surf_name, int* s_id=0);
    CBone*			FindBoneByName			(LPCSTR name);
    int				GetBoneIndexByWMap		(LPCSTR wm_name);
    CSMotion* 		FindSMotionByName		(LPCSTR name, const CSMotion* Ignore=0);
    void			GenerateSMotionName		(char* buffer, LPCSTR start_name, const CSMotion* M);

    // device dependent routine
	void 			OnDeviceCreate 			();
	void 			OnDeviceDestroy			();

    // export routine
    // skeleton
	bool			ExportSkeletonOGF		(LPCSTR fname);
    bool			PrepareSVGeometry		(CFS_Base& F);
    bool			PrepareSVMotions		(CFS_Base& F);
    bool			PrepareSV				(CFS_Base& F);
    // ogf
    bool			ExportObjectOGF			(LPCSTR fname);
    bool			PrepareOGF				(CFS_Base& F);
    // HOM
    bool			ExportHOMPart			(CFS_Base& F);
    bool			ExportHOM				(LPCSTR fname);
#ifdef _MAX_EXPORT
	BOOL			ExtractTexName			(Texmap *src, LPSTR dest);
	BOOL			ParseStdMaterial		(StdMat* src, CSurface* dest);
	BOOL			ParseMultiMaterial		(MultiMtl* src, DWORD mid, CSurface* dest);
	BOOL			ParseXRayMaterial		(XRayMaterial* src, DWORD mid, CSurface* dest);
	CSurface*		CreateSurface			(Mtl* M, DWORD mat_id);
	LPCSTR			GenerateSurfaceName		(LPCSTR base_name); 
	bool			ImportMAXSkeleton		(CExporter* exporter);
#endif
#ifdef _LW_EXPORT
	bool			Import_LWO				(st_ObjectDB *I);
#endif

#ifdef _LW_IMPORT
	bool			Export_LW				(LWObjectImport *local);
#endif
	bool			ExportLWO				(LPCSTR fname);
};
//----------------------------------------------------
//----------------------------------------------------
#define EOBJ_CURRENT_VERSION		0x0010
//----------------------------------------------------
#define EOBJ_CHUNK_OBJECT_BODY		0x7777
#define EOBJ_CHUNK_VERSION		  	0x0900
#define EOBJ_CHUNK_REFERENCE     	0x0902
#define EOBJ_CHUNK_FLAGS           	0x0903
#define EOBJ_CHUNK_SURFACES			0x0905
#define EOBJ_CHUNK_EDITMESHES      	0x0910
#define EOBJ_CHUNK_LIB_VERSION     	0x0911
#define EOBJ_CHUNK_CLASSSCRIPT     	0x0912
#define EOBJ_CHUNK_BONES			0x0913
//#define EOBJ_CHUNK_OMOTIONS			0x0914
#define EOBJ_CHUNK_SMOTIONS			0x0916
#define EOBJ_CHUNK_SURFACES_XRLC	0x0918
#define EOBJ_CHUNK_BONEPARTS		0x0919
#define EOBJ_CHUNK_ACTORTRANSFORM	0x0920
//----------------------------------------------------


#endif /*_INCDEF_EditObject_H_*/


