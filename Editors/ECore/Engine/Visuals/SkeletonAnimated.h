//---------------------------------------------------------------------------
#ifndef SkeletonAnimatedH
#define SkeletonAnimatedH

#include		"skeletoncustom.h"
#include		"skeletonmotions.h"

// consts
const	u32		MAX_BLENDED			=	16;
const	u32		MAX_BLENDED_POOL	=	(MAX_BLENDED*MAX_PARTS);

// refs
class   ENGINE_API CBlend;
class 	ENGINE_API CSkeletonAnimated;
class	ENGINE_API CBoneDataAnimated;
class   ENGINE_API CBoneInstanceAnimated;
struct	ENGINE_API CKey;
class	ENGINE_API CInifile;

//*** Run-time Blend definition *******************************************************************
class ENGINE_API CBlend {
public:
	enum ECurvature
	{
		eFREE_SLOT=0,
		eFixed,
		eAccrue,
		eFalloff,
		eFORCEDWORD = u32(-1)
	};
public:
	float			blendAmount;
	float			timeCurrent;
	float			timeTotal;
	u16				motionID;
	u16				bone_or_part;	// startup parameters

	ECurvature		blend;
	float			blendAccrue;	// increasing
	float			blendFalloff;	// decreasing
	float			blendPower;			
	float			speed;

	BOOL			playing;
	BOOL			stop_at_end;

	PlayCallback	Callback;
	void*			CallbackParam;
	
	u32				dwFrame;
};
typedef svector<CBlend*,MAX_BLENDED>	BlendList;
typedef BlendList::iterator				BlendListIt;

//*** Bone Instance *******************************************************************************
#pragma pack(push,8)
class ENGINE_API		CBlendInstance	// Bone Instance Blend List (per-bone data)
{
public:
	BlendList			Blend;
	// methods
	void				construct		();
	void				blend_add		(CBlend* H);
	void				blend_remove	(CBlend* H);
};
#pragma pack(pop)

//*** Shared Bone Data ****************************************************************************
class ENGINE_API		CBoneDataAnimated: public CBoneData             
{
public:
	MotionVec*			Motions;		// all known motions
public:
						CBoneDataAnimated(u16 ID):CBoneData(ID){}
	// Motion control
	void				Motion_Start	(CSkeletonAnimated* K, CBlend* handle);	// with recursion
	void				Motion_Start_IM	(CSkeletonAnimated* K, CBlend* handle);
	void				Motion_Stop		(CSkeletonAnimated* K, CBlend* handle);	// with recursion
	void				Motion_Stop_IM	(CSkeletonAnimated* K, CBlend* handle);

	// Calculation
	virtual void		Calculate		(CKinematics* K, Fmatrix *Parent);
};

//*** The visual itself ***************************************************************************
class ENGINE_API	CSkeletonAnimated	: public CKinematics
{
	typedef CKinematics							inherited;
	friend class								CBoneData;
	friend class								CMotionDef;
	friend class								CSkeletonX;
public: 
#ifdef _EDITOR
public:
#else
private:
#endif
	CBlendInstance*								blend_instances;

	shared_motions								motions;
	// Fast search
//.	accel_map*									motion_map;			// motion assotiations	(shared) - by name
//.	mdef*										m_cycle;			// motion data itself	(shared)
//.	mdef*										m_fx;				// motion data itself	(shared)
	// Partition
//.	CPartition*									partition;

	// Blending
	svector<CBlend, MAX_BLENDED_POOL>			blend_pool;
	BlendList									blend_cycles	[MAX_PARTS];
	BlendList									blend_fx;
protected:
	// internal functions
    virtual CBoneData*			CreateBoneData			(u16 ID){return xr_new<CBoneDataAnimated>(ID);}
	virtual void				IBoneInstances_Create	();
	virtual void				IBoneInstances_Destroy	();

	void						IBlend_Startup			();
	CBlend*						IBlend_Create			();

//.	bool						LoadMotions				(LPCSTR N, IReader *data);
public:
#ifdef DEBUG
	LPCSTR						LL_MotionDefName_dbg	(u16	ID);
	LPCSTR						LL_MotionDefName_dbg	(LPVOID ptr);
#endif

	// Low level interface
	u16							LL_MotionID		(LPCSTR B);
	u16							LL_PartID		(LPCSTR B);

	accel_map*					LL_Motions		(){return motions.motion_map();}

    u32							LL_CycleCount	(){return motions.cycle()->size();}
    u32							LL_FXCount		(){return motions.fx()->size();}
	CBlend*						LL_PlayFX		(u16 bone,		u16		motion, float blendAccrue,	float blendFalloff, float Speed, float Power);
	CBlend*						LL_PlayCycle	(u16 partition, u16		motion, BOOL  bMixing,		float blendAccrue,	float blendFalloff, float Speed, BOOL noloop, PlayCallback Callback, LPVOID Callback_Param);
	void						LL_FadeCycle	(u16 partition, float	falloff);
	void						LL_CloseCycle	(u16 partition);
	CBlendInstance&				LL_GetBlendInstance	(u16 bone_id)	{	VERIFY(bone_id<LL_BoneCount()); return blend_instances[bone_id];	}
	
	// Main functionality
	void						Update			();								// Update motions
	virtual void				CalculateBones	(BOOL bForced=FALSE);			// Recalculate skeleton 

	// cycles
	CMotionDef*					ID_Cycle		(LPCSTR  N);
	CMotionDef*					ID_Cycle_Safe	(LPCSTR  N);
	CBlend*						PlayCycle		(LPCSTR  N,  BOOL bMixIn=TRUE, PlayCallback Callback=0, LPVOID CallbackParam=0);
	CBlend*						PlayCycle		(CMotionDef* M,  BOOL bMixIn=TRUE, PlayCallback Callback=0, LPVOID CallbackParam=0)
	{	VERIFY(M); return M->PlayCycle(this,bMixIn,Callback,CallbackParam); }

	// fx'es
	CMotionDef*					ID_FX			(LPCSTR  N);
	CMotionDef*					ID_FX_Safe		(LPCSTR  N);
	CBlend*						PlayFX			(LPCSTR  N, float power_scale);
	CBlend*						PlayFX			(CMotionDef* M, float power_scale)
	{	VERIFY(M); return M->PlayFX(this,power_scale);	}

	// General "Visual" stuff
	virtual void				Copy			(IRender_Visual *pFrom);
	virtual void				Load			(const char* N, IReader *data, u32 dwFlags);
	virtual void				Release			();
	virtual void				Spawn			();
	virtual	CSkeletonAnimated*	dcast_PSkeletonAnimated	()				{ return this;	}
	virtual						~CSkeletonAnimated	();
};
IC CSkeletonAnimated* PSkeletonAnimated(IRender_Visual* V) { return V?V->dcast_PSkeletonAnimated():0; }
//---------------------------------------------------------------------------
#endif
