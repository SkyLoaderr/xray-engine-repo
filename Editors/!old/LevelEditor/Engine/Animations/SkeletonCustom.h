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
};
#pragma pack(pop)

//*** Shared Bone Data ****************************************************************************
class ENGINE_API		CBoneData
{
public:
	u16					SelfID;
	u16					ParentID;
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
						CBoneData		(u16 ID):SelfID(ID)	{}
	virtual				~CBoneData		()					{}
#ifdef DEBUG
	typedef svector<int,128>	BoneDebug;
	void						DebugQuery		(BoneDebug& L);
#endif
	// assign face
	void				AppendFace		(u16 child_idx, u16 idx)
	{
		child_faces[child_idx].push_back(idx);
	}
	// Calculation
	void				CalculateM2B	(const Fmatrix& Parent);
	virtual void		Calculate		(CKinematics* K, Fmatrix *Parent)=0;
};

class ENGINE_API CSkeletonWallmark
{
public:
	const Fmatrix*		xform;
	ref_shader			shader;
	Fvector3			contact_point;	// on model space
	Fsphere				bounds;
public:
	struct WMFace{
		Fvector3		vert	[3];
		Fvector2		uv		[3];
		u16				bone_id	[3][2];
		float			weight	[3];
	};
	float				fTimeStart;
	DEFINE_VECTOR		(WMFace,WMFacesVec,WMFacesVecIt);
	WMFacesVec			s_faces;
	xr_vector<FVF::LIT>	r_verts;
public:
	CSkeletonWallmark():xform(0),fTimeStart(0.f){}
	void				Clear	()
	{
		xform			= 0;
		s_faces.clear	();
		r_verts.clear	();
	}
};
DEFINE_VECTOR(CSkeletonWallmark*,SkeletonWMVec,SkeletonWMVecIt);

class ENGINE_API	CKinematics: public FHierrarhyVisual
{
	typedef FHierrarhyVisual	inherited;
	friend class				CBoneData;
	friend class				CSkeletonX;
protected:
	struct str_pred : public std::binary_function<shared_str, shared_str, bool>	{	
		IC bool operator()(const shared_str& x, const shared_str& y) const	{	return xr_strcmp(x,y)<0;	}
	};
public:
	typedef xr_vector<std::pair<shared_str,u32> >	accel;
	typedef xr_map<shared_str,u16,str_pred>		accel_map;
protected:
	SkeletonWMVec				wallmarks;

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

	// wallmarks
	void						AddWallmark			(const Fmatrix* parent, const Fvector3& start, const Fvector3& dir, u16 bone_id, ref_shader shader, float size);
	void						CalculateWallmarks	();

	// General "Visual" stuff
    virtual void				Copy				(IRender_Visual *pFrom);
	virtual void				Load				(const char* N, IReader *data, u32 dwFlags);
	virtual void 				Spawn				();
    virtual void 				Release				();

	virtual	CKinematics*		dcast_PKinematics	()				{ return this;	}
};
IC CKinematics* PKinematics		(IRender_Visual* V)		{ return V?V->dcast_PKinematics():0; }
//---------------------------------------------------------------------------
#endif
