//---------------------------------------------------------------------------
#ifndef SkeletonAnimatedH
#define SkeletonAnimatedH

#include		"skeletoncustom.h"

// consts
const	u32		MAX_BLENDED			=	16;
const	u32		MAX_PARTS			=	4;
const	u32		MAX_BLENDED_POOL	=	(MAX_BLENDED*MAX_PARTS);
const	f32		SAMPLE_FPS			=	30.f;
const	f32		SAMPLE_SPF			=	(1.f/SAMPLE_FPS);

// refs
class   ENGINE_API CBlend;
class 	ENGINE_API CSkeletonAnimated;
class	ENGINE_API CBoneDataAnimated;
class   ENGINE_API CBoneInstanceAnimated;
struct	ENGINE_API CKey;
class	ENGINE_API CInifile;

// callback
typedef void (__stdcall * PlayCallback)		(CBlend*		P);

//*** Key frame definition ************************************************************************
#pragma pack(push,2)
const float KEY_Quant		= 32767.f;
const float KEY_QuantI		= 1.f/KEY_Quant;
struct ENGINE_API CKey
{
	Fquaternion	Q;			// rotation
	Fvector		T;			// translation
};
struct ENGINE_API CKeyQR
{
	s16			x,y,z,w;	// rotation
};
struct ENGINE_API CKeyQT
{
	s8			x,y,z;
};
#pragma pack(pop)

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

//*** Motion Data *********************************************************************************
class ENGINE_API		CMotion
{
public:
	ref_smem<CKeyQR>	_keysR;
	ref_smem<CKeyQT>	_keysT;
    Fvector				_initT;
    Fvector				_sizeT;
	u32					_count;
	float				GetLength()				{ return float(_count)*SAMPLE_SPF; }
};

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
	xr_vector<CMotion>	Motions;		// all known motions
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

//*** Shared motion Data **************************************************************************
class ENGINE_API CMotionDef
{
public:
	u16			bone_or_part;
	u16			motion;
	u16			speed;		// quantized: 0..10
	u16			power;		// quantized: 0..10
	u16			accrue;		// quantized: 0..10
	u16			falloff;	// quantized: 0..10
    u32			flags;

	IC float	Dequantize	(u16 V)		{	return  float(V)/655.35f; }
	IC u16		Quantize	(float V)	{	s32		t = iFloor(V*655.35f); clamp(t,0,65535); return u16(t); }

	void		Load		(CSkeletonAnimated* P, CInifile* INI, LPCSTR section, BOOL bCycle);
	void		Load		(CSkeletonAnimated* P, IReader* MP, u32 fl);
	CBlend*		PlayCycle	(CSkeletonAnimated* P, BOOL	bMixIn, PlayCallback Callback, LPVOID Callback_Param);
	CBlend*		PlayCycle	(CSkeletonAnimated* P, u16	part, BOOL bMixIn, PlayCallback Callback, LPVOID Callback_Param);
	CBlend*		PlayFX		(CSkeletonAnimated* P, float	power_scale);
};

//*** Shared partition Data ***********************************************************************
class ENGINE_API	CPartDef
{
public:
	shared_str			Name;
	xr_vector<u32>	bones;
	CPartDef()		: Name(0) {};
};
class ENGINE_API	CPartition
{
	CPartDef		P[MAX_PARTS];
public:
	IC CPartDef&	operator[] (u16 id)	{ return P[id]; }
};

//*** The visual itself ***************************************************************************
class ENGINE_API	CSkeletonAnimated	: public CKinematics
{
	typedef CKinematics							inherited;
	friend class								CBoneData;
	friend class								CMotionDef;
	friend class								CSkeletonX;
public: 
	typedef xr_map<shared_str,CMotionDef,str_pred>	mdef;
#ifdef _EDITOR
public:
#else
private:
#endif
	CBlendInstance*								blend_instances;

	// Fast search
	accel_map*									motion_map;			// motion assotiations	(shared) - by name
	mdef*										m_cycle;			// motion data itself	(shared)
	mdef*										m_fx;				// motion data itself	(shared)

	// Partition
	CPartition*									partition;

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

	bool						LoadMotions				(LPCSTR N, IReader *data);
public:
#ifdef DEBUG
	LPCSTR						LL_MotionDefName_dbg	(u16	ID);
	LPCSTR						LL_MotionDefName_dbg	(LPVOID ptr);
#endif

	// Low level interface
	u16							LL_MotionID		(LPCSTR B);
	u16							LL_PartID		(LPCSTR B);

	accel_map*					LL_Motions		(){return motion_map;}

    u32							LL_CycleCount	(){return m_cycle->size();}
    u32							LL_FXCount		(){return m_fx->size();}
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
