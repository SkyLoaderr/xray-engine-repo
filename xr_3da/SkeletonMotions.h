//---------------------------------------------------------------------------
#ifndef SkeletonMotionsH
#define SkeletonMotionsH

#include		"skeletoncustom.h"

// refs
class CSkeletonAnimated;
class CBlend;

// callback
typedef void (__stdcall * PlayCallback)		(CBlend*		P);

const	u32		MAX_PARTS			=	4;
const	f32		SAMPLE_FPS			=	30.f;
const	f32		SAMPLE_SPF			=	(1.f/SAMPLE_FPS);
const	f32		KEY_Quant			= 32767.f;
const	f32		KEY_QuantI			= 1.f/KEY_Quant;

//*** Key frame definition ************************************************************************
#pragma pack(push,2)
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

//*** Motion Data *********************************************************************************
class ENGINE_API		CMotion
{
public:
	ref_smem<CKeyQR>	_keysR;
	ref_smem<CKeyQT>	_keysT;
    Fvector				_initT;
    Fvector				_sizeT;
	u32					_count;
	float				GetLength			(){ return float(_count)*SAMPLE_SPF; }

	u32					mem_usage			(){ 
		u32 sz			= sizeof(*this);
		if (_keysR.size()) sz += _keysR.size()*sizeof(CKeyQR)/_keysR.ref_count();
		if (_keysT.size()) sz += _keysT.size()*sizeof(CKeyQT)/_keysT.ref_count();
		return			sz;
	}
};
class ENGINE_API		CMotionDef
{
public:
	u16					bone_or_part;
	u16					motion;
	u16					speed;				// quantized: 0..10
	u16					power;				// quantized: 0..10
	u16					accrue;				// quantized: 0..10
	u16					falloff;			// quantized: 0..10
	u32					flags;

	IC float			Dequantize			(u16 V)		{	return  float(V)/655.35f; }
	IC u16				Quantize			(float V)	{	s32		t = iFloor(V*655.35f); clamp(t,0,65535); return u16(t); }

	void				Load				(IReader* MP, u32 fl);
	CBlend*				PlayCycle			(CSkeletonAnimated* P, BOOL	bMixIn, PlayCallback Callback, LPVOID Callback_Param);
	CBlend*				PlayCycle			(CSkeletonAnimated* P, u16	part, BOOL bMixIn, PlayCallback Callback, LPVOID Callback_Param);
	CBlend*				PlayFX				(CSkeletonAnimated* P, float	power_scale);

	u32					mem_usage			(){ return sizeof(*this);}
};
struct str_pred : public std::binary_function<shared_str, shared_str, bool>	{	
	IC bool operator()(const shared_str& x, const shared_str& y) const	{	return xr_strcmp(x,y)<0;	}
};
typedef xr_map<shared_str,u16,str_pred>			accel_map;
typedef xr_map<shared_str,CMotionDef,str_pred>	mdef;
DEFINE_VECTOR			(CMotion,MotionVec,MotionVecIt);

// partition
class ENGINE_API		CPartDef
{
public:
	shared_str			Name;
	xr_vector<u32>		bones;
	CPartDef()			: Name(0) {};

	u32					mem_usage			(){ return sizeof(*this)+bones.size()*sizeof(u32)+sizeof(Name);}
};
class ENGINE_API		CPartition
{
	CPartDef			P[MAX_PARTS];
public:
	IC CPartDef&		operator[] (u16 id)	{ return P[id]; }

	u32					mem_usage			(){ return P[0].mem_usage()*MAX_PARTS;}
};

// shared motions
struct ENGINE_API		motions_value
{
	accel_map			m_motion_map;			// motion associations
	mdef				m_cycle;			// motion data itself	(shared)
	mdef				m_fx;				// motion data itself	(shared)
	CPartition			m_partition;			// partition
	u32					m_dwReference;
	DEFINE_MAP			(shared_str,MotionVec,BoneMotionMap,BoneMotionMapIt);
	BoneMotionMap		m_motions;

	BOOL				load				(LPCSTR N, IReader *data, vecBones* bones);
	MotionVec*			motions				(shared_str bone_name);

	u32					mem_usage			(){ 
		u32 sz			=	sizeof(*this)+m_motion_map.size()*6+m_partition.mem_usage();
		for (mdef::iterator c_it=m_cycle.begin(); c_it!=m_cycle.end(); c_it++)
			sz			+=	c_it->second.mem_usage();
		for (mdef::iterator f_it=m_fx.begin(); f_it!=m_fx.end(); f_it++)
			sz			+=	f_it->second.mem_usage();
		for (BoneMotionMapIt bm_it=m_motions.begin(); bm_it!=m_motions.end(); bm_it++)
			for (MotionVecIt m_it=bm_it->second.begin(); m_it!=bm_it->second.end(); m_it++)
				sz		+=	m_it->mem_usage();
		return sz;
	}
};

class ENGINE_API		motions_container
{
	DEFINE_MAP			(shared_str,motions_value*,SharedMotionsMap,SharedMotionsMapIt);
	SharedMotionsMap	container;
public:
						motions_container	();
						~motions_container	();
	motions_value*		dock				(shared_str key, IReader *data, vecBones* bones);
	void				dump				();
	void				clean				(bool force_destroy);
};

ENGINE_API extern		motions_container*	g_pMotionsContainer;

class ENGINE_API		shared_motions
{
private:
	motions_value*		p_;
protected:
	// ref-counting
	void				destroy			()							{	if (0==p_) return;	p_->m_dwReference--; 	if (0==p_->m_dwReference)	p_=0;	}
public:
	void				create			(shared_str key, IReader *data, vecBones* bones){	motions_value* v = g_pMotionsContainer->dock(key,data,bones); if (0!=v) v->m_dwReference++; destroy(); p_ = v;	}
	void				create			(shared_motions const &rhs)	{	motions_value* v = rhs.p_; if (0!=v) v->m_dwReference++; destroy(); p_ = v;	}
public:
	// construction
						shared_motions	()							{	p_ = 0;											}
						shared_motions	(shared_motions const &rhs)	{	p_ = 0;	create(rhs);							}
						~shared_motions	()							{	destroy();										}

	// assignment & accessors
	shared_motions&		operator=		(shared_motions const &rhs)	{	create(rhs);return *this;	}

	// misc func
	MotionVec*			motions			(shared_str bone_name)		{	VERIFY(p_); return p_->motions(bone_name);		}
	accel_map*			motion_map		()							{	VERIFY(p_); return &p_->m_motion_map;			}
	mdef*				cycle			()							{	VERIFY(p_); return &p_->m_cycle;				}
	mdef*				fx				()							{	VERIFY(p_); return &p_->m_fx;					}
	CPartition*			partition		()							{	VERIFY(p_); return &p_->m_partition;			}
};
//---------------------------------------------------------------------------
#endif
