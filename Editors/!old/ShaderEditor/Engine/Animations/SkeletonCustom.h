//---------------------------------------------------------------------------
#ifndef SkeletonCustomH
#define SkeletonCustomH

#include		"fhierrarhyvisual.h"
#include		"bone.h"

// consts
const	u32		MAX_BONE_PARAMS		=	5;
const	u32		UCalc_Interval		=	33;	// 30 fps

// refs
class	ENGINE_API CKinematics;
class	ENGINE_API CInifile;
class	ENGINE_API CBoneData;
class   ENGINE_API CBoneInstance;

// t-defs
typedef xr_vector<CBoneData*>		vecBones;
typedef vecBones::iterator			vecBonesIt;

// callback
typedef void (__stdcall * BoneCallback)		(CBoneInstance* P);
typedef void (__stdcall * UpdateCallback)	(CKinematics*	P);

//*** Bone Instance *******************************************************************************
#pragma pack(push,8)
class ENGINE_API		CBoneInstance
{
public:
	// data
	Fmatrix				mTransform;							// final x-form matrix (local to model)
	Fmatrix				mRenderTransform;					// final x-form matrix (model_base -> bone -> model)
	BoneCallback		Callback;
	void*				Callback_Param;
	BOOL				Callback_overwrite;					// performance hint - don't calc anims
	float				param			[MAX_BONE_PARAMS];	// 

	// methods
	void				construct		();
	void				set_callback	(BoneCallback C, void* Param, BOOL overwrite=FALSE);
	void				set_param		(u32 idx, float data);
	float				get_param		(u32 idx);

	u32					mem_usage		(){return sizeof(*this);}
};
#pragma pack(pop)

//*** Shared Bone Data ****************************************************************************
class ENGINE_API		CBoneData
{
protected:
	u16					SelfID;
	u16					ParentID;
public:
	shared_str			name;

	vecBones			children;		// bones which are slaves to this
	Fobb				obb;			

	Fmatrix				bind_transform;
    Fmatrix				m2b_transform;	// model to bone conversion transform
    SBoneShape			shape;
    shared_str			game_mtl_name;
	u16					game_mtl_idx;
    SJointIKData		IK_data;
    float				mass;
    Fvector				center_of_mass;

	DEFINE_VECTOR		(u16,FacesVec,FacesVecIt);
	DEFINE_VECTOR		(FacesVec,ChildFacesVec,ChildFacesVecIt);
	ChildFacesVec		child_faces;	// shared
public:    
						CBoneData		(u16 ID):SelfID(ID)	{VERIFY(SelfID!=BI_NONE);}
	virtual				~CBoneData		()					{}
#ifdef DEBUG
	typedef svector<int,128>	BoneDebug;
	void						DebugQuery		(BoneDebug& L);
#endif
	IC void				SetParentID		(u16 id){ParentID=id;}
	
	IC u16				GetSelfID		(){return SelfID;}
	IC u16				GetParentID		(){return ParentID;}

	// assign face
	void				AppendFace		(u16 child_idx, u16 idx)
	{
		child_faces[child_idx].push_back(idx);
	}
	// Calculation
	void				CalculateM2B	(const Fmatrix& Parent);
	virtual void		Calculate		(CKinematics* K, Fmatrix *Parent)=0;

	virtual u32			mem_usage		()
	{
		u32 sz			= sizeof(*this)+sizeof(vecBones::value_type)*children.size();
		for (ChildFacesVecIt c_it=child_faces.begin(); c_it!=child_faces.end(); c_it++)
			sz			+= c_it->size()*sizeof(FacesVec::value_type)+sizeof(*c_it);
		return			sz;
	}
};

class ENGINE_API CSkeletonWallmark		// 4+4+4+12+4+16+16 = 60
{
	CKinematics*		m_Parent;		// 4
	const Fmatrix*		m_XForm;		// 4
	ref_shader			m_Shader;		// 4
	Fvector3			m_ContactPoint;	// 12		model space
	float				m_fTimeStart;	// 4
public:
	Fsphere				m_LocalBounds;	// 16		model space
	struct WMFace{
		Fvector3		vert	[3];
		Fvector2		uv		[3];
		u16				bone_id	[3][2];
		float			weight	[3];
	};
	DEFINE_VECTOR		(WMFace,WMFacesVec,WMFacesVecIt);
	WMFacesVec			m_Faces;		// 16 
public:
	Fsphere				m_Bounds;		// 16		world space
public:									
						CSkeletonWallmark	(CKinematics* p,const Fmatrix* m, ref_shader s, const Fvector& cp, float ts):
						m_Parent(p),m_XForm(m),m_Shader(s),m_fTimeStart(ts),
						m_ContactPoint(cp){}
	IC CKinematics*		Parent				(){return m_Parent;}
	IC u32				VCount				(){return m_Faces.size()*3;}
	IC bool				Similar				(ref_shader& sh, const Fvector& cp, float eps){return (m_Shader==sh)&&m_ContactPoint.similar(cp,eps);}
	IC float			TimeStart			(){return m_fTimeStart;}
	IC const Fmatrix*	XFORM				(){return m_XForm;}
	IC const Fvector3&	ContactPoint		(){return m_ContactPoint;}
	IC ref_shader		Shader				(){return m_Shader;}
};
DEFINE_VECTOR(CSkeletonWallmark*,SkeletonWMVec,SkeletonWMVecIt);

class ENGINE_API	CKinematics: public FHierrarhyVisual
{
	typedef FHierrarhyVisual	inherited;
	friend class				CBoneData;
	friend class				CSkeletonX;
public:
	typedef xr_vector<std::pair<shared_str,u32> >	accel;
protected:
	SkeletonWMVec				wallmarks;
	u32							wm_frame;

	// Globals
    CInifile*					pUserData;
	CBoneInstance*				bone_instances;	// bone instances
	vecBones*					bones;			// all bones	(shared)
	u16							iRoot;			// Root bone index

	// Fast search
	accel*						bone_map_N;		// bones  assotiations	(shared)	- sorted by name
	accel*						bone_map_P;		// bones  assotiations	(shared)	- sorted by name-pointer

	u32							Update_LastTime;

	u32							UCalc_Time;
	s32							UCalc_Visibox;

    Flags64						visimask;
    
	CSkeletonX*					LL_GetChild				(u32 idx);

	// internal functions
    virtual CBoneData*			CreateBoneData			(u16 ID)=0;
	virtual void				IBoneInstances_Create	();
	virtual void				IBoneInstances_Destroy	();
public:
	UpdateCallback				Update_Callback;
	void*						Update_Callback_Param;
public:
	// wallmarks
	void						AddWallmark			(const Fmatrix* parent, const Fvector3& start, const Fvector3& dir, ref_shader shader, float size);
	void						CalculateWallmarks	();
	void						RenderWallmark		(CSkeletonWallmark* wm, FVF::LIT* &verts);
public:
	virtual						~CKinematics		();

	// Low level interface
	u16							LL_BoneID		(LPCSTR  B);
	u16							LL_BoneID		(const shared_str& B);
	LPCSTR						LL_BoneName_dbg	(u16 ID);

    CInifile*					LL_UserData			(){return pUserData;}
	accel*						LL_Bones			(){return bone_map_N;}
	CBoneInstance&				LL_GetBoneInstance	(u16 bone_id)		{	VERIFY(bone_id<LL_BoneCount()); return bone_instances[bone_id];	}
	CBoneData&					LL_GetData			(u16 bone_id)		{	VERIFY(bone_id<LL_BoneCount()); return *((*bones)[bone_id]);	}
	u16							LL_BoneCount		()					{	return u16(bones->size());										}
	u16							LL_VisibleBoneCount	()					{	u64 F=visimask.flags&((u64(1)<<u64(LL_BoneCount()))-1); return u16(btwCount1(F)); }
	Fmatrix&					LL_GetTransform		(u16 bone_id)		{	return LL_GetBoneInstance(bone_id).mTransform;					}
	Fmatrix&					LL_GetTransform_R	(u16 bone_id)		{	return LL_GetBoneInstance(bone_id).mRenderTransform;			}	// rendering only
	Fobb&						LL_GetBox			(u16 bone_id)		{	VERIFY(bone_id<LL_BoneCount());	return (*bones)[bone_id]->obb;	}
	void						LL_GetBindTransform (xr_vector<Fmatrix>& matrices);

	u16							LL_GetBoneRoot		()					{	return iRoot;													}
	void						LL_SetBoneRoot		(u16 bone_id)		{	VERIFY(bone_id<LL_BoneCount());	iRoot=bone_id;					}

    BOOL						LL_GetBoneVisible	(u16 bone_id)	{VERIFY(bone_id<LL_BoneCount()); return visimask.is(u64(1)<<bone_id);	}
	void						LL_SetBoneVisible	(u16 bone_id, BOOL val, BOOL bRecursive);
	u64							LL_GetBonesVisible	()				{return visimask.get();	}
	void						LL_SetBonesVisible	(u64 mask);

	// Main functionality
	virtual void				CalculateBones				(BOOL bForceExact	=	FALSE)		=	0;		// Recalculate skeleton
	void						CalculateBones_Invalidate	();
	void						Callback					(UpdateCallback C, void* Param)		{	Update_Callback	= C; Update_Callback_Param	= Param;	}

	// debug
#ifdef DEBUG
	void						DebugRender			(Fmatrix& XFORM);
#endif

	// General "Visual" stuff
    virtual void				Copy				(IRender_Visual *pFrom);
	virtual void				Load				(const char* N, IReader *data, u32 dwFlags);
	virtual void 				Spawn				();
    virtual void 				Release				();

	virtual	CKinematics*		dcast_PKinematics	()				{ return this;	}

	virtual u32					mem_usage			(bool bInstance)
	{
		u32 sz					= sizeof(*this);
		sz						+= bone_instances?bone_instances->mem_usage():0;
		if (!bInstance){
//			sz					+= pUserData?pUserData->mem_usage():0;
			for (vecBonesIt b_it=bones->begin(); b_it!=bones->end(); b_it++)
				sz				+= sizeof(vecBones::value_type)+(*b_it)->mem_usage();
		}
		return sz;
	}
};
IC CKinematics* PKinematics		(IRender_Visual* V)		{ return V?V->dcast_PKinematics():0; }
//---------------------------------------------------------------------------
#endif
