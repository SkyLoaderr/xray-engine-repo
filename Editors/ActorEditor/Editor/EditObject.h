#ifndef EditObjectH
#define EditObjectH

#include "Bone.h"
#include "SceneClassList.h"
#ifdef _EDITOR
	#include "PropertiesListTypes.h"
//	#include "PropertiesListHelper.h"
	#include "GameMtlLib.h"
#endif
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
class	CMayaTranslator;
struct	st_ObjectDB;
struct	SXRShaderData;

#ifndef _EDITOR
	class PropValue;
	class SSceneSummary;
#endif

#ifdef _LW_IMPORT
#include <lwobjimp.h>
#include <lwsurf.h>
#endif

#define LOD_SHADER_NAME 	"def_shaders\\lod"
#define LOD_SAMPLE_COUNT 	8.f
#define LOD_IMAGE_SIZE 		64.f

// refs
class XRayMtl;

class CSurface
{
    u32				m_GameMtlID;
    Shader*			m_Shader;
	enum ERTFlags{
         rtValidShader	= (1<<0),
	};
public:
	enum EFlags{
    	sf2Sided		= (1<<0),
        sfValidGameMtlID= (1<<1),
    };
	AnsiString		m_Name;
    AnsiString		m_Texture;	//
    AnsiString		m_VMap;		//
    AnsiString		m_ShaderName;
    AnsiString		m_ShaderXRLCName;
    AnsiString		m_GameMtlName;
    Flags32			m_Flags;
    u32				m_dwFVF;
#ifdef _MAX_EXPORT
	u32				mid;
	Mtl*			mtl;
#endif
#ifdef _LW_IMPORT
	LWSurfaceID		surf_id;
#endif
    Flags32			m_RTFlags;
	u32				tag;
public:
	CSurface		()
	{
    	m_GameMtlName="default";
        m_GameMtlID	= GAMEMTL_NONE;
		m_Shader	= 0;
        m_RTFlags.zero	();
		m_Flags.zero	();
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
					~CSurface		(){R_ASSERT(!m_Shader);}
	IC void			CopyFrom		(CSurface* surf){*this = *surf; m_Shader=0;}
    IC int			_Priority		()	{return _Shader()?_Shader()->E[0]->Flags.iPriority:1;}
    IC bool			_StrictB2F		()	{return _Shader()?_Shader()->E[0]->Flags.bStrictB2F:false;}
	IC Shader*		_Shader			()	{if (!m_RTFlags.is(rtValidShader)) OnDeviceCreate(); return m_Shader;}
#endif
    IC LPCSTR		_Name			()const {return m_Name.c_str();}
    IC LPCSTR		_ShaderName		()const {return m_ShaderName.c_str();}
    IC LPCSTR		_GameMtlName	()const {return m_GameMtlName.c_str();}
    IC LPCSTR		_ShaderXRLCName	()const {return m_ShaderXRLCName.c_str();}
    IC u32			_FVF			()const {return m_dwFVF;}
    IC LPCSTR		_Texture		(){return m_Texture.c_str();}
    IC LPCSTR		_VMap			(){return m_VMap.c_str();}
    IC void			SetName			(LPCSTR name){m_Name=name;}
	IC void			SetShader		(LPCSTR name)
	{
		R_ASSERT(name&&name[0]); 
		m_ShaderName=name; 
#ifdef _EDITOR 
		OnDeviceDestroy(); 
#endif
	}
    IC void 		SetShaderXRLC	(LPCSTR name){m_ShaderXRLCName=name;}
    IC void			SetGameMtl		(LPCSTR name){m_GameMtlName=name;m_Flags.set(sfValidGameMtlID,FALSE);}
    IC void			SetFVF			(u32 fvf){m_dwFVF=fvf;}
    IC void			SetTexture		(LPCSTR name){string256 buf; strcpy(buf,name); if(strext(buf)) *strext(buf)=0; m_Texture=buf;}
    IC void			SetVMap			(LPCSTR name){m_VMap=name;}
#ifdef _EDITOR
    IC u32			_GameMtl		()
    {
    	if (!m_Flags.is(sfValidGameMtlID)){
        	m_GameMtlID = GMLib.GetMaterialID(m_GameMtlName.c_str()); R_ASSERT(m_GameMtlID!=GAMEMTL_NONE);
        	m_Flags.set(sfValidGameMtlID,TRUE);
        }
        return m_GameMtlID;
    }
    IC void			OnDeviceCreate	()
    { 
        R_ASSERT(!m_RTFlags.is(rtValidShader));
    	if (m_ShaderName.Length()&&m_Texture.Length()) 	m_Shader = Device.Shader.Create(m_ShaderName.c_str(),m_Texture.c_str()); 
        else                                       		m_Shader = Device.Shader.Create("editor\\wire");
        m_RTFlags.set(rtValidShader,TRUE);
    }
    IC void			OnDeviceDestroy	()
    {
    	Device.Shader.Delete(m_Shader);
        m_RTFlags.set(rtValidShader,FALSE);
    }
	// properties
	void __fastcall OnChangeGameMtl	(PropValue* sender){ m_Flags.set(sfValidGameMtlID,FALSE); }
#endif
};

DEFINE_VECTOR	(CSurface*,SurfaceVec,SurfaceIt);
DEFINE_VECTOR	(CSurface,SurfInstVec,SurfInstIt);
DEFINE_VECTOR	(CEditableMesh*,EditMeshVec,EditMeshIt);
DEFINE_VECTOR	(COMotion*,OMotionVec,OMotionIt);
DEFINE_VECTOR	(CSMotion*,SMotionVec,SMotionIt);

struct st_AnimParam{
    float			t;
    float			min_t;
    float			max_t;
    bool			bPlay;
    void			Set		(CCustomMotion* M);
    float			Frame	()			{ return t;}
    void			Update	(float dt, float speed, bool loop);
    void			Play	(){bPlay=true; t=min_t;}
    void			Stop	(){bPlay=false; t=min_t;}
    void			Pause	(){bPlay=!bPlay;}
};

struct SBonePart{
	AnsiString 		alias;
    IntVec 			bones;
};
DEFINE_VECTOR(SBonePart,BPVec,BPIt);

const u32 FVF_SV	= D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_NORMAL;

class CEditableObject{
	friend class CSceneObject;
	friend class CEditableMesh;
    friend class TfrmPropertiesEObject;
    friend class CSector;
    friend class TUI_ControlSectorAdd;
	friend class ELibrary;
	friend class TfrmEditLibrary;
	friend class MeshExpUtility;

#ifdef _EDITOR
    IRender_Visual*	m_Visual;
	SGeometry* 		vs_SkeletonGeom;
#endif
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
	// options
	Flags32			m_Flags;
	enum{
		eoDynamic 	 	= (1<<0),
		eoProgressive 	= (1<<1),
        eoUsingLOD		= (1<<2),
        eoHOM			= (1<<3),
        eoMultipleUsage	= (1<<4),
		eoFORCE32		= u32(-1)
    };
    IC BOOL			IsDynamic				(){return m_Flags.is(eoDynamic);}
    IC BOOL			IsStatic				(){return !m_Flags.is(eoDynamic)&&!m_Flags.is(eoHOM)&&!m_Flags.is(eoMultipleUsage);}
    IC BOOL			IsMUStatic				(){return !m_Flags.is(eoDynamic)&&!m_Flags.is(eoHOM)&&m_Flags.is(eoMultipleUsage);}
private:
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
    int				m_Version;

    void 			ClearGeometry			();

	void 			PrepareBones			();
    void			DefferedLoadRP			();
    void			DefferedUnloadRP		();

	void __fastcall OnChangeTransform		(PropValue* prop);
    void __fastcall	OnChangeShader			(PropValue* prop);
public:
	enum{
	    LS_RBUFFERS	= (1<<0),
//	    LS_GEOMETRY	= (1<<1),
    };
    Flags32			m_LoadState;

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
	IC void			AppendMesh				(CEditableMesh* M){m_Meshes.push_back(M);}
    IC SurfaceVec&	Surfaces				()	{return m_Surfaces;}
    IC SurfaceIt	FirstSurface			()	{return m_Surfaces.begin();}
    IC SurfaceIt	LastSurface				()	{return m_Surfaces.end();}
    IC int			SurfaceCount			()	{return m_Surfaces.size();}

    // LOD
	AnsiString		GetLODTextureName		();
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
    IC CBone*		GetBone					(u32 idx){VERIFY(idx<m_Bones.size()); return m_Bones[idx];}
    void			GetBoneWorldTransform	(u32 bone_idx, float t, CSMotion* motion, Fmatrix& matrix);
    IC SMotionIt	FirstSMotion			()	{return m_SMotions.begin();}
    IC SMotionIt	LastSMotion				()	{return m_SMotions.end();}
	SMotionVec&		SMotions				()	{return m_SMotions;}
    IC int			SMotionCount 			()	{return m_SMotions.size();}
    IC void			SkeletonPlay 			()	{m_SMParam.Play();}
    IC void			SkeletonStop 			()	{m_SMParam.Stop();}
    IC void			SkeletonPause 			()	{m_SMParam.Pause();}

///    IC bool			CheckVersion			()  {if(m_LibRef) return (m_ObjVer==m_LibRef->m_ObjVer); return true;}
    // get object properties methods

	IC AnsiString&	GetClassScript			()	{return m_ClassScript;}
    IC const Fbox&	GetBox					() 	{return m_Box;}

    // animation
    IC bool			IsSkeleton				()	{return !!m_Bones.size();}
    IC bool			IsSMotionActive			()	{return IsSkeleton()&&m_ActiveSMotion; }
    CSMotion*		GetActiveSMotion		()	{return m_ActiveSMotion; }
	void			SetActiveSMotion		(CSMotion* mot);
	bool 			CheckBoneCompliance		(CSMotion* M);

	bool 			LoadBoneData			(IReader& F);
	void 			SaveBoneData			(IWriter& F);
    void			ResetBones				();
	CSMotion*		ResetSAnimation			(bool bGotoBindPose=true);
	void 			CalculateAnimation		(CBone* bone, CSMotion* motion, bool bGenInvMat=false, bool bCalcRest=false);
    void			CalculateAnimation		(CSMotion* motion, bool bGenInvMat=false, bool bCalcRest=false);
	void			GotoBindPose			();

    // statistics methods
    int 			GetFaceCount			();
	int 			GetVertexCount			();
    int 			GetSurfFaceCount		(LPCSTR surf_name);
	bool 			GetSummaryInfo			(SSceneSummary* inf);

    // render methods
	void 			Render					(const Fmatrix& parent, int priority, bool strictB2F);
	void 			RenderSelection			(const Fmatrix& parent, CEditableMesh* m=0, u32 c=0x40E64646);
	void 			RenderEdge				(const Fmatrix& parent, CEditableMesh* m=0, u32 c=0xFFC0C0C0);
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
	bool 			RayPick					(float& dist, const Fvector& S, const Fvector& D, const Fmatrix& inv_parent, SRayPickInfo* pinf=0);
#ifdef _EDITOR
	void 			RayQuery				(const Fmatrix& parent, const Fmatrix& inv_parent, SPickQuery& pinf);
	void 			BoxQuery				(const Fmatrix& parent, const Fmatrix& inv_parent, SPickQuery& pinf);
    bool 			BoxPick					(CSceneObject* obj, const Fbox& box, const Fmatrix& inv_parent, SBoxPickInfoVec& pinf);
	bool 			FrustumPick				(const CFrustum& frustum, const Fmatrix& parent);
    bool 			SpherePick				(const Fvector& center, float radius, const Fmatrix& parent);

    // bone
	CBone* 			PickBone				(const Fvector& S, const Fvector& D, const Fmatrix& parent);
	void 			SelectBones				(bool bVal);
	void 			SelectBone				(CBone* b, bool bVal);
    void			ClampByLimits			(bool bSelOnly);
#endif
    // change position/orientation methods
	void 			TranslateToWorld		(const Fmatrix& parent);

    // clone/copy methods
    void			RemoveMesh				(CEditableMesh* mesh);

    bool			RemoveSMotion			(LPCSTR name);
    bool			RenameSMotion			(LPCSTR old_name, LPCSTR new_name);
    bool			AppendSMotion			(LPCSTR fname);
    void			ClearSMotions			();
    bool			SaveSMotions			(LPCSTR fname);

    // load/save methods
	//void 			LoadMeshDef				(FSChunkDef *chunk);
	bool 			Reload					();
	bool 			Load					(LPCSTR fname);
	bool 			LoadObject				(LPCSTR fname);
	void 			SaveObject				(LPCSTR fname);
  	bool 			Load					(IReader&);
	void 			Save					(IWriter&);
#ifdef _EDITOR
	void 			FillMotionList			(LPCSTR pref, ListItemsVec& items, int modeID);
	void 			FillBoneList			(LPCSTR pref, ListItemsVec& items, int modeID);
    void			FillSurfaceList			(LPCSTR pref, ListItemsVec& items, int modeID);
    void			FillSurfaceProps		(CSurface* surf, LPCSTR pref, PropItemVec& items);
	void 			FillBasicProps			(LPCSTR pref, PropItemVec& items);
	void 			FillSummaryProps		(LPCSTR pref, PropItemVec& items);
	bool			CheckShaderCompatible	();
#endif
	bool			Import_LWO				(LPCSTR fname, bool bNeedOptimize);

    // contains methods
    CEditableMesh* 	FindMeshByName			(LPCSTR name, CEditableMesh* Ignore=0);
    void			GenerateMeshNames		();
    void			VerifyMeshNames			();
    bool 			ContainsMesh			(const CEditableMesh* m);
	CSurface*		FindSurfaceByName		(LPCSTR surf_name, int* s_id=0);
    CBone*			FindBoneByName			(LPCSTR name);
    int				GetSelectedBones		(BoneVec& sel_bones);
    int				GetBoneIndexByWMap		(LPCSTR wm_name);
    CSMotion* 		FindSMotionByName		(LPCSTR name, const CSMotion* Ignore=0);
    void			GenerateSMotionName		(char* buffer, LPCSTR start_name, const CSMotion* M);
    bool			GenerateBoneShape		(bool bSelOnly);

    // device dependent routine
	void 			OnDeviceCreate 			();
	void 			OnDeviceDestroy			();

    // export routine
    // skeleton
    bool			PrepareSVGeometry		(IWriter& F);
    bool			PrepareSVKeys			(IWriter& F);
    bool			PrepareSVDefs			(IWriter& F);
    bool			PrepareSkeletonOGF		(IWriter& F);
    // rigid
    bool			PrepareRigidOGF			(IWriter& F);
	// ogf
    bool			PrepareOGF				(IWriter& F);
	bool			ExportOGF				(LPCSTR fname);
    // HOM
    bool			ExportHOMPart			(const Fmatrix& parent, IWriter& F);
    bool			ExportHOM				(LPCSTR fname);

	LPCSTR			GenerateSurfaceName		(LPCSTR base_name);
#ifdef _MAX_EXPORT
	BOOL			ExtractTexName			(Texmap *src, LPSTR dest);
	BOOL			ParseStdMaterial		(StdMat* src, CSurface* dest);
	BOOL			ParseMultiMaterial		(MultiMtl* src, u32 mid, CSurface* dest);
	BOOL			ParseXRayMaterial		(XRayMtl* src, u32 mid, CSurface* dest);
	CSurface*		CreateSurface			(Mtl* M, u32 mat_id);
	bool			ImportMAXSkeleton		(CExporter* exporter);
#endif
#ifdef _LW_EXPORT
	bool			Import_LWO				(st_ObjectDB *I);
#endif
#ifdef _LW_IMPORT
	bool			Export_LW				(LWObjectImport *local);
#endif
#ifdef _MAYA_EXPORT
	BOOL			ParseMAMaterial			(CSurface* dest, SXRShaderData& d);
	CSurface*		CreateSurface			(LPCSTR m_name, SXRShaderData& d);
	CSurface*		CreateSurface			(MObject shader);
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
#define EOBJ_CHUNK_SURFACES2		0x0906
#define EOBJ_CHUNK_SURFACES3		0x0907
#define EOBJ_CHUNK_EDITMESHES      	0x0910
#define EOBJ_CHUNK_LIB_VERSION     	0x0911
#define EOBJ_CHUNK_CLASSSCRIPT     	0x0912
#define EOBJ_CHUNK_BONES			0x0913
//#define EOBJ_CHUNK_OMOTIONS			0x0914
#define EOBJ_CHUNK_SMOTIONS			0x0916
#define EOBJ_CHUNK_SURFACES_XRLC	0x0918
#define EOBJ_CHUNK_BONEPARTS		0x0919
#define EOBJ_CHUNK_ACTORTRANSFORM	0x0920
#define EOBJ_CHUNK_BONES2			0x0921
//----------------------------------------------------


#endif /*_INCDEF_EditObject_H_*/


