//---------------------------------------------------------------------------
#ifndef SkeletonCustomH
#define SkeletonCustomH

#include		"fhierrarhyvisual.h"
#include		"bone.h"

// consts
const	u32		MAX_BONE_PARAMS		=	5;

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
	Fmatrix				mTransform;							// final x-form matrix
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
    SBoneShape			shape;
    ref_str				game_mtl_name;
	u16					game_mtl_idx;
    SJointIKData		IK_data;
    float				mass;
    Fvector				center_of_mass;
public:    
						CBoneData		(u16 ID):SelfID(ID)	{}
	virtual				~CBoneData		()					{}
#ifdef DEBUG
	typedef svector<int,128>	BoneDebug;
	void						DebugQuery		(BoneDebug& L);
#endif

	// Calculation
	virtual void		Calculate		(CKinematics* K, Fmatrix *Parent)=0;
};

class ENGINE_API	CKinematics: public FHierrarhyVisual
{
	typedef FHierrarhyVisual	inherited;
	friend class				CBoneData;
	friend class				CSkeletonX;
protected:
	struct str_pred : public std::binary_function<ref_str, ref_str, bool>	{	
		IC bool operator()(const ref_str& x, const ref_str& y) const	{	return xr_strcmp(x,y)<0;	}
	};
public:
	typedef xr_vector<std::pair<ref_str,u32> >	accel;
	typedef xr_map<ref_str,u16,str_pred>		accel_map;
protected:
	// Globals
    CInifile*					pUserData;
	CBoneInstance*				bone_instances;	// bone instances
	vecBones*					bones;			// all bones	(shared)
	u16							iRoot;			// Root bone index

	// Fast search
	accel*						bone_map_N;		// bones  assotiations	(shared)	- sorted by name
	accel*						bone_map_P;		// bones  assotiations	(shared)	- sorted by name-pointer

	s32							Update_ID;
	u32							Update_LastTime;
	u32							Update_Frame;

    Flags64						visimask;
    
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
	u16							LL_BoneID		(const ref_str& B);
	LPCSTR						LL_BoneName_dbg	(u16 ID);

    CInifile*					LL_UserData		(){return pUserData;}
	accel*						LL_Bones		(){return bone_map_N;}
	CBoneInstance&				LL_GetBoneInstance(u16 bone_id)		{	VERIFY(bone_id<LL_BoneCount()); return bone_instances[bone_id];	}
	CBoneData&					LL_GetData		(u16 bone_id)		{	VERIFY(bone_id<LL_BoneCount()); return *((*bones)[bone_id]);	}
	u16							LL_BoneCount	()					{	return u16(bones->size());										}
	u16							LL_VisibleBoneCount()				{	u64 F=visimask.flags&((u64(1)<<u64(LL_BoneCount()))-1); return u16(btwCount1(F)); }
	Fmatrix&					LL_GetTransform	(u16 bone_id)		{	return LL_GetBoneInstance(bone_id).mTransform;					}
	Fobb&						LL_GetBox		(u16 bone_id)		{	VERIFY(bone_id<LL_BoneCount());	return (*bones)[bone_id]->obb;	}

	u16							LL_GetBoneRoot	()					{	return iRoot;													}
	void						LL_SetBoneRoot	(u16 bone_id)		{	VERIFY(bone_id<LL_BoneCount());	iRoot=bone_id;					}

    BOOL						LL_GetBoneVisible	(u16 bone_id)	{VERIFY(bone_id<LL_BoneCount()); return visimask.is(u64(1)<<bone_id);	}
	void						LL_SetBoneVisible	(u16 bone_id, BOOL val, BOOL bRecursive);
	u64							LL_GetBonesVisible	()				{return visimask.get();	}
	void						LL_SetBonesVisible	(u64 mask);

	// Main functionality
	virtual void				Calculate		(BOOL bForced=FALSE)=0;			// Recalculate skeleton
	void						Callback		(UpdateCallback C, void* Param)	{	Update_Callback	= C; Update_Callback_Param	= Param;	}

	// debug
#ifdef DEBUG
	void						DebugRender		(Fmatrix& XFORM);
#endif

	// General "Visual" stuff
    virtual void				Copy			(IRender_Visual *pFrom);
	virtual void				Load			(const char* N, IReader *data, u32 dwFlags);
	virtual void 				Spawn			();
    virtual void 				Release			();

	virtual	CKinematics*		dcast_PKinematics		()				{ return this;	}
};
IC CKinematics* PKinematics		(IRender_Visual* V)		{ return V?V->dcast_PKinematics():0; }
//---------------------------------------------------------------------------
#endif
