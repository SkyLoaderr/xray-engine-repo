// BodyInstance.h: interface for the CBodyInstance class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BODYINSTANCE_H__68A83E66_CEC3_4207_84A0_B7997A253B87__INCLUDED_)
#define AFX_BODYINSTANCE_H__68A83E66_CEC3_4207_84A0_B7997A253B87__INCLUDED_
#pragma once

#include "skeletonX.h"
#include "fhierrarhyvisual.h"

#define MAX_BLENDED			8
#define MAX_PARTS			4
#define MAX_BLENDED_POOL	(MAX_BLENDED*MAX_PARTS)
#define SAMPLE_FPS			30.f
#define SAMPLE_SPF			(1.f/SAMPLE_FPS)
#define BONE_NONE			int(-1)

// refs
class	ENGINE_API CKinematics;
struct	ENGINE_API CKey;
struct	ENGINE_API CKeyQ;
class	ENGINE_API CBoneData;
class	ENGINE_API CInifile;
class   ENGINE_API CBoneInstance;

// t-defs
typedef vector<CBoneData*>	vecBones;
typedef vecBones::iterator	vecBonesIt;

//*** Key frame definition ************************************************************************
#pragma pack(push,2)
const float KEY_Quant		= 32767.f;
const float KEY_QuantI		= 1.f/KEY_Quant;
struct ENGINE_API CKey
{
	Fquaternion	Q;	// rotation
	Fvector		T;	// translation
};
struct ENGINE_API CKeyQ
{
	short		x,y,z,w;	// rotation
	Fvector		T;			// translation
};
IC void	KEY_Interp(CKey& D, CKey& K1, CKey& K2, float delta)
{
	D.Q.slerp	(K1.Q,K2.Q,delta);
	D.T.lerp	(K1.T,K2.T,delta);
}
#pragma pack(pop)
typedef vector<CKeyQ>	vecKeys;

//*** Run-time Blend definition *******************************************************************
class ENGINE_API CBlend {
public:
	enum ECurvature
	{
		eFREE_SLOT=0,
		eFixed,
		eAccrue,
		eFalloff,
		eFORCEDWORD = DWORD(-1)
	};
public:
	float		blendAmount;
	float		timeCurrent;
	float		timeTotal;
	int			motionID;
	int			bone_or_part;	// startup parameters

	ECurvature	blend;
	float		blendAccrue;	// increasing
	float		blendFalloff;	// decreasing
	float		blendPower;			
	float		speed;

	BOOL		playing;
	BOOL		noloop;

	DWORD		dwFrame;
};
typedef svector<CBlend*,MAX_BLENDED>	BlendList;
typedef BlendList::iterator				BlendListIt;

//*** Motion Data *********************************************************************************
class ENGINE_API CMotion
{
public:
	vecKeys		Keys;
	float		GetLength() { return float(Keys.size())*SAMPLE_SPF; }
};

typedef void (__stdcall * BoneCallback) (CBoneInstance* B);

//*** Bone Instance *******************************************************************************
#pragma pack(push,8)
__declspec(align(64)) class ENGINE_API CBoneInstance
{
public:
	// data
	Fmatrix									mTransform;
	BlendList								Blend;
	BoneCallback							Callback;
	void*									Callback_Param;

	// methods
	IC	void					construct()
	{
		ZeroMemory				(this,sizeof(*this));
		mTransform.identity		();
	}
	IC	void	blend_add		(CBlend* H)
	{	Blend.push_back(H);	}
	IC	void	blend_remove	(CBlend* H)
	{
		CBlend** I = find(Blend.begin(),Blend.end(),H);
		if (I!=Blend.end())	Blend.erase(I);
	}
	IC	void	set_callback	(BoneCallback C, void* Param)
	{	Callback = C; Callback_Param = Param; }
};
#pragma pack(pop)

//*** Shared Bone Data ****************************************************************************
class ENGINE_API CBoneData
{
public:
	typedef svector<int,128>	BoneDebug;
public:
	int				SelfID;
	vecBones		Chields;	// bones which are slaves to this
	vector<CMotion>	Motions;	// all known motions
	Fobb			obb;

	// Motion control
	void			Motion_Start	(CKinematics* K, CBlend* handle);	// with recursion
	void			Motion_Start_IM	(CKinematics* K, CBlend* handle);
	void			Motion_Stop		(CKinematics* K, CBlend* handle);	// with recursion
	void			Motion_Stop_IM	(CKinematics* K, CBlend* handle);

	// Calculation
	void			Calculate		(CKinematics* K, Fmatrix *Parent);
	void			DebugQuery		(BoneDebug& L)
	{
		for (DWORD i=0; i<Chields.size(); i++)
		{
			L.push_back(SelfID);
			L.push_back(Chields[i]->SelfID);
			Chields[i]->DebugQuery(L);
		}
	}

	CBoneData			(int ID)
	{ 
		SelfID = ID;
	}
};

//*** Shared motion Data **************************************************************************
class ENGINE_API CMotionDef
{
public:
	short	bone_or_part;
	WORD	motion;
	WORD	speed;		// quantized: 0..10
	WORD	power;		// quantized: 0..10
	WORD	accrue;		// quantized: 0..10
	WORD	falloff;	// quantized: 0..10
	BOOL	noloop;

	IC float	Dequantize(WORD V) 
	{	return  float(V)/6553.4f; }
	IC WORD		Quantize(float V)
	{	clamp(V,0.f,50.f); return WORD(iFloor(V*6553.4f));	}

	void		Load		(CKinematics* P, CInifile* INI, LPCSTR section, BOOL bCycle);
	CBlend*		PlayCycle	(CKinematics* P);
	CBlend*		PlayFX		(CKinematics* P);
};

//*** Shared partition Data ***********************************************************************
class ENGINE_API CPartDef
{
public:
	LPSTR		Name;
	vector<int>	bones;
	CPartDef()	: Name(0) {};
};
class ENGINE_API CPartition
{
	CPartDef	P[MAX_PARTS];
public:
	IC CPartDef& operator[] (int id)	{ return P[id]; }
};

//*** The visual itself ***************************************************************************
class ENGINE_API CKinematics	: public FHierrarhyVisual
{
	friend class					CBoneData;
	friend class					CMotionDef;
	friend class					CSkeletonX;
private:
	struct str_pred : public binary_function<char*, char*, bool> 
	{	
		IC bool operator()(const char* x, const char* y) const
		{	return strcmp(x,y)<0;	}
	};

	typedef FHierrarhyVisual				inherited;
	typedef map<LPSTR,int,str_pred>			accel;
	typedef map<LPSTR,CMotionDef,str_pred>	mdef;
private:
	// Globals
	vecBones*								bones;			// all bones+motions	(shared)
	CBoneInstance*							bone_instances;	// bone instances
	int										iRoot;			// Root bone index

	// Fast search
	accel*									motion_map;		// motion assotiations	(shared)
	accel*									bone_map;		// bones  assotiations	(shared)
	mdef*									m_cycle;
	mdef*									m_fx;

	// Partition
	CPartition*								partition;

	// Blending
	svector<CBlend, MAX_BLENDED_POOL>		blend_pool;
	BlendList								blend_cycles	[MAX_PARTS];
	BlendList								blend_fx;

	DWORD									iUpdateID;
	DWORD									dwUpdate_LastTime;

	// internal functions
	void									IBoneInstances_Create	();
	void									IBoneInstances_Destroy	();

	IC void									IBlend_Startup	()
	{
		CBlend B; ZeroMemory(&B,sizeof(B));
		B.blend = CBlend::eFREE_SLOT;
		for (int i=0; i<MAX_BLENDED_POOL; i++)
			blend_pool.push_back(B);
	}
	IC CBlend*								IBlend_Create	()
	{
		Update();
		CBlend *I=blend_pool.begin(), *E=blend_pool.end();
		for (; I!=E; I++)
			if (I->blend == CBlend::eFREE_SLOT) return I;
		Device.Fatal("Too many blended motions requisted");
	}
	void									Update			();	// Update motions
public:
	// Low level interface
	int							LL_BoneID		(LPCSTR B);
	int							LL_MotionID		(LPCSTR B);
	int							LL_PartID		(LPCSTR B);

	IC CBoneInstance&			LL_GetInstance	(int bone_id)
	{
		VERIFY((bone_id>=0)&&(bone_id<LL_BoneCount())); 
		return bone_instances[bone_id];	
	}
	IC int						LL_BoneRoot		()
	{	return iRoot; }
	IC int						LL_BoneCount	()
	{	return bones->size(); 	}

	Fmatrix&					LL_GetTransform	(int bone_id)
	{
		return LL_GetInstance(bone_id).mTransform; 
	}
	IC Fobb&					LL_GetBox		(int bone_id)
	{
		VERIFY((bone_id>=0)&&(bone_id<LL_BoneCount())); 
		return (*bones)[bone_id]->obb;
	}

	CBlend*						LL_PlayFX		(int bone,		int motion, float blendAccrue, float blendFalloff, float Speed=1.f, float Power=1.f);
	CBlend*						LL_PlayCycle	(int partition, int motion, float blendAccrue, float blendFalloff, float Speed=1.f, BOOL noloop=FALSE);
	void						LL_FadeCycle	(int partition, float falloff);

	// Main functionality
	void						Calculate		(BOOL bLight=FALSE);	// Recalculate skeleton (Light mode can be used to avoid interpolation)

	// cycles
	CMotionDef*					ID_Cycle		(LPCSTR  N);
	CMotionDef*					ID_Cycle_Safe	(LPCSTR  N);
	CBlend*						PlayCycle		(LPCSTR  N);
	CBlend*						PlayCycle		(CMotionDef* M)
	{	VERIFY(M); return M->PlayCycle(this);	}

	// fx'es
	CMotionDef*					ID_FX			(LPCSTR  N);
	CMotionDef*					ID_FX_Safe		(LPCSTR  N);
	CBlend*						PlayFX			(LPCSTR  N);
	CBlend*						PlayFX			(CMotionDef* M)
	{	VERIFY(M); return M->PlayFX(this);	}


	// debug
	void						DebugRender		(Fmatrix& XFORM);

	// General "Visual" stuff
	virtual void				Copy			(FBasicVisual *pFrom);
	virtual void				Load			(const char* N, CStream *data, DWORD dwFlags);
	virtual void				Release			();
	virtual						~CKinematics	();
};
typedef CKinematics* PKinematics;


#endif // !defined(AFX_BODYINSTANCE_H__68A83E66_CEC3_4207_84A0_B7997A253B87__INCLUDED_)
