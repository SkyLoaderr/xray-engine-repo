//---------------------------------------------------------------------------
#include 	"stdafx.h"
#pragma hdrstop

#include 	"SkeletonAnimated.h"
#include	"SkeletonX.h"
#include	"fmesh.h"
#include	"motion.h"

const float	fAA					= 1.5f;	// anim-change acceleration
extern int	psSkeletonUpdate;

//////////////////////////////////////////////////////////////////////////
// BoneInstance methods
void		CBlendInstance::construct()
{	
	ZeroMemory			(this,sizeof(*this));
}
void		CBlendInstance::blend_add	(CBlend* H)
{	
	if (Blend.size()>=MAX_BLENDED)	{
// for E3 by Dima
#ifdef _DEBUG
		Msg	("! WARNING: more than 16 blend-sequences per bone played");
#endif
		return;
	}
	Blend.push_back(H);	
}
void		CBlendInstance::blend_remove	(CBlend* H)
{
	CBlend** I = std::find(Blend.begin(),Blend.end(),H);
	if (I!=Blend.end())	Blend.erase(I);
}


//////////////////////////////////////////////////////////////////////////
// High level control
void CMotionDef::Load(CSkeletonAnimated* P, IReader* MP, u32 fl)
{
	// params
	bone_or_part= MP->r_u16(); // bCycle?part_id:bone_id;
	motion		= MP->r_u16(); // motion_id
	speed		= Quantize(MP->r_float());
	power		= Quantize(MP->r_float());
	accrue		= Quantize(MP->r_float());
	falloff		= Quantize(MP->r_float());
	flags		= fl;
	if (!(flags&esmFX) && (falloff>=accrue)) falloff = accrue-1;
}

CBlend*	CMotionDef::PlayCycle(CSkeletonAnimated* P, BOOL bMixIn, PlayCallback Callback, LPVOID Callback_Param)
{
	return P->LL_PlayCycle(
		bone_or_part,motion,bMixIn,
		fAA*Dequantize(accrue),
		fAA*Dequantize(falloff),
		Dequantize(speed),
		flags&esmStopAtEnd,
		Callback,Callback_Param
		);
}

CBlend*	CMotionDef::PlayCycle(CSkeletonAnimated* P, u16 part, BOOL bMixIn, PlayCallback Callback, LPVOID Callback_Param)
{
	return P->LL_PlayCycle(
		part,motion,bMixIn,
		fAA*Dequantize(accrue),
		fAA*Dequantize(falloff),
		Dequantize(speed),
		flags&esmStopAtEnd,
		Callback,Callback_Param
		);
}

CBlend*	CMotionDef::PlayFX(CSkeletonAnimated* P, float power_scale)
{
	return P->LL_PlayFX(
		bone_or_part,motion,
		fAA*Dequantize(accrue),
		fAA*Dequantize(falloff),
		Dequantize(speed),
		Dequantize(power)*power_scale
		);
}

// Motion control
void	CBoneDataAnimated::Motion_Start	(CSkeletonAnimated* K, CBlend* handle) 
{
	K->LL_GetBlendInstance(SelfID).blend_add		(handle);
	for (vecBonesIt I=children.begin(); I!=children.end(); I++)                        
		((CBoneDataAnimated*)(*I))->Motion_Start	(K,handle);
}
void	CBoneDataAnimated::Motion_Start_IM	(CSkeletonAnimated* K, CBlend* handle) 
{
	K->LL_GetBlendInstance(SelfID).blend_add		(handle);
}
void	CBoneDataAnimated::Motion_Stop	(CSkeletonAnimated* K, CBlend* handle) 
{
	K->LL_GetBlendInstance(SelfID).blend_remove	(handle);
	for (vecBonesIt I=children.begin(); I!=children.end(); I++)
		((CBoneDataAnimated*)(*I))->Motion_Stop	(K,handle);
}
void	CBoneDataAnimated::Motion_Stop_IM	(CSkeletonAnimated* K, CBlend* handle) 
{
	K->LL_GetBlendInstance(SelfID).blend_remove	(handle);
}

#ifdef DEBUG
LPCSTR CSkeletonAnimated::LL_MotionDefName_dbg	(u16	ID)
{
	CKinematics::accel_map::iterator _I, _E=motion_map->end();
	for (_I	= motion_map->begin(); _I!=_E; ++_I)	if (_I->second==ID) return *_I->first;
	return 0;
}
LPCSTR CSkeletonAnimated::LL_MotionDefName_dbg	(LPVOID ptr)
{
	// cycles
	mdef::const_iterator I,E;
	I = m_cycle->begin(); 
	E = m_cycle->end(); 
	for ( ; I != E; ++I) if (&(*I).second == ptr) return *(*I).first;
	// fxs
	I = m_fx->begin(); 
	E = m_fx->end(); 
	for ( ; I != E; ++I) if (&(*I).second == ptr) return *(*I).first;
	return 0;
}
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
u16	CSkeletonAnimated::LL_MotionID	(LPCSTR B)
{
	accel_map::iterator I		= motion_map->find(LPSTR(B));
	if (I==motion_map->end())	return BI_NONE;
	else						return I->second;
}
u16 CSkeletonAnimated::LL_PartID		(LPCSTR B)
{
	if (0==partition)			return BI_NONE;
	for (u16 id=0; id<MAX_PARTS; id++) {
		CPartDef&	P = (*partition)[id];
		if (0==P.Name)	continue;
		if (0==stricmp(B,*P.Name)) return id;
	}
	return BI_NONE;
}

// cycles
CMotionDef*	CSkeletonAnimated::ID_Cycle	(LPCSTR  N)
{
	mdef::iterator I = m_cycle->find(LPSTR(N));
	if (I==m_cycle->end())	{ Debug.fatal("! MODEL: can't find cycle: %s", N); return 0; }
	return &I->second;
}
CMotionDef*	CSkeletonAnimated::ID_Cycle_Safe(LPCSTR  N)
{
	mdef::iterator I = m_cycle->find(LPSTR(N));
	if(I==m_cycle->end()) return 0;
	return &I->second;
}
CBlend*	CSkeletonAnimated::PlayCycle		(LPCSTR  N, BOOL bMixIn, PlayCallback Callback, LPVOID CallbackParam)
{
	mdef::iterator I = m_cycle->find(LPSTR(N));
	if (I!=m_cycle->end())	return I->second.PlayCycle(this,bMixIn,Callback,CallbackParam);
	else					{ Debug.fatal("! MODEL: can't find cycle: %s", N); return 0; }
}

// fx'es
CMotionDef*	CSkeletonAnimated::ID_FX			(LPCSTR  N)
{
	mdef::iterator I = m_fx->find(LPSTR(N));
	if (I==m_fx->end())		{ Debug.fatal("! MODEL: can't find FX: %s", N); return 0; }
	return &I->second;
}
CMotionDef*	CSkeletonAnimated::ID_FX_Safe		(LPCSTR  N)
{
	mdef::iterator I = m_fx->find(LPSTR(N));
	if(I==m_fx->end()) return 0;
	return &I->second;
}
CBlend*	CSkeletonAnimated::PlayFX			(LPCSTR  N, float power_scale)
{
	mdef::iterator I = m_fx->find(LPSTR(N));
	if (I!=m_fx->end())		return I->second.PlayFX(this,power_scale);
	else					{ Debug.fatal("! MODEL: can't find FX: %s", N); return 0; }
}

CBlend*	CSkeletonAnimated::LL_PlayFX		(u16 bone, u16 motion, float blendAccrue, float blendFalloff, float Speed, float Power)
{
	if (BI_NONE==motion)	return 0;
	if (blend_fx.size()>=MAX_BLENDED) return 0;
	if (BI_NONE==bone)		bone = iRoot;
	
	CBlend*	B		= IBlend_Create();
	CBoneDataAnimated*	Bone= (CBoneDataAnimated*)(*bones)[bone];
	Bone->Motion_Start(this,B);
	
	B->blend		= CBlend::eAccrue;
	B->blendAmount	= EPS_S;
	B->blendAccrue	= blendAccrue;
	B->blendFalloff	= blendFalloff;
	B->blendPower	= Power;
	B->speed		= Speed;
	B->motionID		= motion;
	B->timeCurrent	= 0;
	B->timeTotal	= Bone->Motions[motion].GetLength();
	B->bone_or_part	= bone;
	
	B->playing		= TRUE;
	B->stop_at_end	= FALSE;
	
	blend_fx.push_back(B);
	return			B;
}

void	CSkeletonAnimated::LL_FadeCycle(u16 part, float falloff)
{
	BlendList&	Blend	= blend_cycles[part];
	
	for (u32 I=0; I<Blend.size(); I++)
	{
		CBlend& B			= *Blend[I];
		B.blend				= CBlend::eFalloff;
		B.blendFalloff		= falloff;
		if (B.stop_at_end)  B.playing = FALSE;		// callback не должен приходить!
	}
}

void	CSkeletonAnimated::LL_CloseCycle(u16 part)
{
	if (BI_NONE==part)		return;
	if (part>=MAX_PARTS)	return;

	// destroy cycle(s)
	BlendListIt	I = blend_cycles[part].begin(), E = blend_cycles[part].end();
	for (; I!=E; I++)
	{
		CBlend& B = *(*I);

		B.blend = CBlend::eFREE_SLOT;
		
		CPartDef& P	= (*partition)[B.bone_or_part];
		for (u32 i=0; i<P.bones.size(); i++)
			((CBoneDataAnimated*)(*bones)[P.bones[i]])->Motion_Stop(this,*I);
		/*
		blend_cycles[part].erase(I);
		E=blend_cycles[part].end(); I--; 
		*/
	}
	blend_cycles[part].clear	(); // ?
}

CBlend*	CSkeletonAnimated::LL_PlayCycle(u16 part, u16 motion, BOOL  bMixing,	float blendAccrue, float blendFalloff, float Speed, BOOL noloop, PlayCallback Callback, LPVOID CallbackParam)
{
	// validate and unroll
	if (BI_NONE==motion)	return 0;
	if (BI_NONE==part)		{
		for (u16 i=0; i<MAX_PARTS; i++)
			LL_PlayCycle(i,motion,bMixing,blendAccrue,blendFalloff,Speed,noloop,Callback,CallbackParam);
		return 0;
	}
	if (part>=MAX_PARTS)	return 0;
	if (0==(*partition)[part].Name)	return 0;

	// Process old cycles and create _new_
	if (bMixing)	LL_FadeCycle	(part,blendFalloff);
	else			LL_CloseCycle	(part);
	CPartDef& P	=	(*partition)[part];
	CBlend*	B	=	IBlend_Create();

	CBoneDataAnimated*	Bone=0;
	for (u32 i=0; i<P.bones.size(); i++)
	{
		Bone	= (CBoneDataAnimated*)(*bones)[P.bones[i]]; VERIFY(Bone);
		Bone->Motion_Start_IM	(this,B);
	}
	VERIFY		(Bone);
	blend_cycles[part].push_back(B);

	// Setup blend params
	if (bMixing)	{
		B->blend		= CBlend::eAccrue;
		B->blendAmount	= EPS_S;
	} else {
		B->blend		= CBlend::eFixed;
		B->blendAmount	= 1;
	}
	B->blendAccrue	= blendAccrue;
	B->blendFalloff	= 0; // blendFalloff used for previous cycles
	B->blendPower	= 1;
	B->speed		= Speed;
	B->motionID		= motion;
	B->timeCurrent	= 0;
	B->timeTotal	= Bone->Motions[motion].GetLength();
	B->bone_or_part	= part;
	B->playing		= TRUE;
	B->stop_at_end	= noloop;
	B->Callback		= Callback;
	B->CallbackParam= CallbackParam;
	return			B;
}

void CSkeletonAnimated::Update ()
{
	if (Update_LastTime==Device.dwTimeGlobal) return;
	u32 DT	= Device.dwTimeGlobal-Update_LastTime;
	if (DT>66) DT=66;
	float dt = float(DT)/1000.f;
	Update_LastTime = Device.dwTimeGlobal;

	BlendListIt	I,E;

	// Cycles
	for (u16 part=0; part<MAX_PARTS; part++) 
	{
		if (0==(*partition)[part].Name)	continue;

		I = blend_cycles[part].begin(); E = blend_cycles[part].end();
		for (; I!=E; I++)
		{
			CBlend& B = *(*I);
			if (B.dwFrame==Device.dwFrame)	continue;
			B.dwFrame		=	Device.dwFrame;
			if (B.playing) 	B.timeCurrent += dt*B.speed; // stop@end - time is not going 
			switch (B.blend) 
			{
			case CBlend::eFREE_SLOT: 
				NODEFAULT;
			case CBlend::eFixed:	
				B.blendAmount 		+= dt*B.blendAccrue*B.blendPower;
				if (B.blendAmount>B.blendPower)	B.blendAmount = B.blendPower; 
				if (B.stop_at_end && (B.timeCurrent > (B.timeTotal-SAMPLE_SPF) )) {
					B.timeCurrent	= B.timeTotal-SAMPLE_SPF;		// stop@end - time frozen at the end
					if (B.playing&&B.Callback)	B.Callback(&B);		// callback only once
					B.playing		= FALSE;
				}
				break;
			case CBlend::eAccrue:	
				B.blendAmount 		+= dt*B.blendAccrue*B.blendPower;
				if (B.stop_at_end && (B.timeCurrent > (B.timeTotal-SAMPLE_SPF) )) {
					B.timeCurrent	= B.timeTotal-SAMPLE_SPF;		// stop@end - time frozen at the end
					if (B.playing&&B.Callback)	B.Callback(&B);		// callback only once
					B.playing		= FALSE;
					B.blend			= CBlend::eFixed;
				} 
				if (B.blendAmount>=B.blendPower) {
					// switch to fixed
					B.blendAmount	= B.blendPower;
					B.blend			= CBlend::eFixed;
				}
				break;
			case CBlend::eFalloff:
				B.blendAmount 		-= dt*B.blendFalloff*B.blendPower;
				if (B.blendAmount<=0) {
					// destroy cycle
					B.blend 		= CBlend::eFREE_SLOT;

					CPartDef& P		= (*partition)[B.bone_or_part];
					for (u32 i=0; i<P.bones.size(); i++)
						((CBoneDataAnimated*)(*bones)[P.bones[i]])->Motion_Stop(this,*I);

					blend_cycles[part].erase(I);
					E				= blend_cycles[part].end(); I--; 
				}
				break;
			default: 
				NODEFAULT;
			}
		}
	}
	
	// FX
	I = blend_fx.begin(); E = blend_fx.end();
	for (; I!=E; I++)
	{
		CBlend& B = *(*I);
		if (!B.playing)	continue;
		B.timeCurrent += dt*B.speed;
		switch (B.blend) 
		{
		case CBlend::eFREE_SLOT: 
			NODEFAULT;
		case CBlend::eFixed:
			{
/*				B.blendAmount = B.blendPower; 
				// calc time to falloff
				float time2falloff = B.timeTotal - 1/(B.blendFalloff*B.speed);
				if (B.timeCurrent >= time2falloff) {
					// switch to falloff
					B.blend		= CBlend::eFalloff;
				}
*/
				B.blend		= CBlend::eFalloff;
			}
			break;
		case CBlend::eAccrue:
            B.blendAmount 	+= dt*B.blendAccrue*B.blendPower*B.speed;
			if (B.blendAmount>=B.blendPower) {
				// switch to fixed
				B.blendAmount	= B.blendPower;
				B.blend			= CBlend::eFixed;
			}
			break;
		case CBlend::eFalloff:
			B.blendAmount 	-= dt*B.blendFalloff*B.blendPower*B.speed;
			if (B.blendAmount<=0) {
				// destroy fx
				B.blend = CBlend::eFREE_SLOT;
				((CBoneDataAnimated*)(*bones)[B.bone_or_part])->Motion_Stop(this,*I);
				blend_fx.erase(I); 
				E=blend_fx.end(); I--; 
			}
			break;
		default: NODEFAULT;
		}
	}
}

void CSkeletonAnimated::Release()
{
	// xr_free bones
	for (u32 i=0; i<bones->size(); i++)
	{
		CBoneDataAnimated* B	= (CBoneDataAnimated*)(*bones)[i];
		B->Motions.clear		();
	}

	// destroy shared data
	xr_delete(partition);
	xr_delete(motion_map);
	xr_delete(m_cycle);
	xr_delete(m_fx);
    
    inherited::Release();
}

CSkeletonAnimated::~CSkeletonAnimated	()
{
	IBoneInstances_Destroy	();
}

void	CSkeletonAnimated::IBoneInstances_Create()
{
    inherited::IBoneInstances_Create();
	u32				size	= bones->size();
	void*			ptr		= xr_malloc(size*sizeof(CBlendInstance));
	blend_instances			= (CBlendInstance*)ptr;
	for (u32 i=0; i<size; i++)
		blend_instances[i].construct();
}

void	CSkeletonAnimated::IBoneInstances_Destroy()
{
    inherited::IBoneInstances_Destroy();
	if (blend_instances) {
		xr_free(blend_instances);
		blend_instances = NULL;
	}
}

#define PCOPY(a)	a = pFrom->a
void CSkeletonAnimated::Copy(IRender_Visual *P) 
{
	inherited::Copy	(P);

	CSkeletonAnimated* pFrom = (CSkeletonAnimated*)P;
	PCOPY(motion_map);
	PCOPY(m_cycle);
	PCOPY(m_fx);
	PCOPY(partition);

	IBlend_Startup			();
}

void CSkeletonAnimated::Spawn	()
{
	inherited::Spawn		();

	IBlend_Startup			();

	for (u32 i=0; i<bones->size(); i++)
		blend_instances[i].construct();
}

void CSkeletonAnimated::IBlend_Startup	()
{
	CBlend B; ZeroMemory(&B,sizeof(B));
	B.blend				= CBlend::eFREE_SLOT;
	blend_pool.clear	();
	for (u32 i=0; i<MAX_BLENDED_POOL; i++)
		blend_pool.push_back(B);

	// cycles+fx clear
	for (i=0; i<MAX_PARTS; i++)
		blend_cycles[i].clear();
	blend_fx.clear		();
}

CBlend*	CSkeletonAnimated::IBlend_Create	()
{
	Update();
	CBlend *I=blend_pool.begin(), *E=blend_pool.end();
	for (; I!=E; I++)
		if (I->blend == CBlend::eFREE_SLOT) return I;
	Debug.fatal("Too many blended motions requisted");
	return 0;
}

bool CSkeletonAnimated::LoadMotions(LPCSTR N, IReader *data)
{
	bool bRes		= true;
	// Load definitions
    U16Vec rm_bones	(bones->size(),BI_NONE);
	IReader* MP 	= data->open_chunk(OGF_S_SMPARAMS);
    if (MP){
	    u16 		vers 			= MP->r_u16();
        u16 		part_bone_cnt	= 0;
        string128 	buf;
        R_ASSERT3(vers==xrOGF_SMParamsVersion,"Invalid OGF/OMF version:",N);
        // partitions
        u16 part_count;
        part_count 				= MP->r_u16();
        for (u16 part_i=0; part_i<part_count; part_i++){
            CPartDef&	PART	= (*partition)[part_i];
            MP->r_stringZ		(buf);
            PART.Name			= _strlwr(buf);
            PART.bones.resize	(MP->r_u16());
//				Log					("Part:",buf);
            for (xr_vector<u32>::iterator b_it=PART.bones.begin(); b_it<PART.bones.end(); b_it++){
                MP->r_stringZ	(buf);
                u16 m_idx 		= u16		(MP->r_u32());
                *b_it			= LL_BoneID	(buf); 
//					Msg				("Bone: #%2d, ID: %2d, Name: '%s'",b_it-PART.bones.begin(),*b_it,buf);
#ifdef _EDITOR
                if (*b_it==BI_NONE){
                    bRes		= false;
                    Msg			("!Can't find bone: '%s'",buf);
                }
#else
                VERIFY3			(*b_it!=BI_NONE,"Can't find bone:",buf);
#endif
                if (bRes)		rm_bones[m_idx] = u16(*b_it);
            }
            part_bone_cnt		= part_bone_cnt + (u16)PART.bones.size();
        }

        m_cycle = xr_new<mdef> ();
        m_fx	= xr_new<mdef> ();

#ifdef _EDITOR
        if (part_bone_cnt!=(u16)bones->size()){
        	bRes = false;
            Msg("!Different bone count [Object: '%d' <-> Motions: '%d']",bones->size(),part_bone_cnt);
        }
#else
        VERIFY3(part_bone_cnt==(u16)bones->size(),"Different bone count '%s'",N);
#endif
        if (bRes){
            // motion defs (cycle&fx)
            u16 mot_count			= MP->r_u16();
            for (u16 mot_i=0; mot_i<mot_count; mot_i++){
                MP->r_stringZ(buf);
                u32 dwFlags		= MP->r_u32();
                CMotionDef	D;		D.Load(this,MP,dwFlags);
                if (dwFlags&esmFX)	m_fx->insert(mk_pair(shared_str(_strlwr(buf)),D));
                else				m_cycle->insert(mk_pair(shared_str(_strlwr(buf)),D));
            }
        }
        MP->close();
    }else{
        Debug.fatal	("Old skinned model version unsupported! (%s)",N);
    }

    if (!bRes)	return false;
    
	// Load animation
	IReader*	MS		= data->open_chunk(OGF_S_MOTIONS);
    if (!MS) 	return false;
    
    u32			dwCNT	= 0;
    MS->r_chunk_safe	(0,&dwCNT,sizeof(dwCNT));

    // set per bone motion size
    for (u32 i=0; i<bones->size(); i++)
        ((CBoneDataAnimated*)(*bones)[i])->Motions.resize(dwCNT);
    // load motions
    for (u32 m_idx=0; m_idx<dwCNT; m_idx++){
        string128			mname;
        R_ASSERT			(MS->find_chunk(m_idx+1));             
        MS->r_stringZ		(mname);

        shared_str	m_key		= shared_str(strlwr(mname));
//		CKinematics::accel_map::iterator it = motion_map->find(m_key);
//		if (it!=motion_map->end())	xr_delete (it->second);
        motion_map->insert	(mk_pair(m_key,m_idx));

        u32 dwLen			= MS->r_u32();
        for (u32 i=0; i<bones->size(); i++){
        	VERIFY2			(rm_bones[i]!=BI_NONE,"Invalid remap index.");
            CMotion&		M	= ((CBoneDataAnimated*)(*bones)[rm_bones[i]])->Motions[m_idx];
            M._count			= dwLen;
            u8	t_present		= MS->r_u8	();
            u32 crc_q			= MS->r_u32	();
			M._keysR.create		(crc_q,dwLen,(CKeyQR*)MS->pointer());
			MS->advance			(dwLen * sizeof(CKeyQR));
            if (t_present)	{
                u32 crc_t			= MS->r_u32	();
				M._keysT.create		(crc_t,dwLen,(CKeyQT*)MS->pointer());
				MS->advance			(dwLen * sizeof(CKeyQT));
                MS->r_fvector3		(M._sizeT);
                MS->r_fvector3		(M._initT);
            }else{
                MS->r_fvector3		(M._initT);
            }
        }
    }
    MS->close();
    return bRes;
}

void CSkeletonAnimated::Load(const char* N, IReader *data, u32 dwFlags)
{
	inherited::Load	(N, data, dwFlags);

	// Globals
	motion_map		= xr_new<accel_map>		();
	partition		= xr_new<CPartition>	();
	blend_instances	= NULL;

	// Load animation
    if (data->find_chunk(OGF_S_MOTION_REFS)){
    	string_path	fn,nm;
        data->r_stringZ	(nm);
        if (!FS.exist(fn, "$level$", nm, ".omf")){
            if (!FS.exist(fn, "$game_meshes$", nm, ".omf")){
#ifdef _EDITOR
                Msg			("!Can't find motion file '%s'.",nm);
                return;
#else            
                Debug.fatal("Can't find motion file '%s'.",nm);
#endif
            }
		}
		IReader* MS	= FS.r_open(fn);
        // Check compatibility
        Log			("Loading reference motions:",nm);
	    LoadMotions	(nm,MS);
        MS->close	();
    }else{
	    LoadMotions		(N,data);
    }

	// Init blend pool
	IBlend_Startup	();

	if (m_cycle->size()<2)			
		Msg("! WARNING: model '%s' has only one motion. Candidate for SkeletonRigid???",N);
}

//------------------------------------------------------------------------------
// calculate
//------------------------------------------------------------------------------
IC void	KEY_Interp	(CKey& D, CKey& K1, CKey& K2, float delta)
{
	D.Q.slerp		(K1.Q,K2.Q,delta);
	D.T.lerp		(K1.T,K2.T,delta);
}
struct ConsistantKey
{
	CKey*	K;
	float	w;

	IC void	set(CKey* _K, float _w)
	{	K = _K; w = _w; }
};

IC bool operator < (const ConsistantKey& A, const ConsistantKey& B)	// note: inverse operator
{	return A.w>B.w; }

IC void MakeKeysConsistant(ConsistantKey *keys, int count)
{
	// sort in decreasing order
	std::sort(keys,keys+count);

	// recalc
	for (int i=0; i<count-1; i++) {
		Fquaternion Q1,Q2;
		Q1.add(keys[i].K->Q,keys[i+1].K->Q);
		Q2.sub(keys[i].K->Q,keys[i+1].K->Q);
		if (Q1.magnitude()<Q2.magnitude())	keys[i+1].K->Q.inverse_with_w();
	}
}

IC void MakeKeysSelected(ConsistantKey *keys, int count)
{
	// sort in decreasing order
	std::sort(keys,keys+count);
}

void CBoneDataAnimated::Calculate(CKinematics* _K, Fmatrix *parent)
{
	CSkeletonAnimated* K 			= (CSkeletonAnimated*)_K; 

    if (K->LL_GetBoneVisible(SelfID)){
        CBoneInstance& BONE_INST	= K->LL_GetBoneInstance(SelfID);
        CBlendInstance& BLEND_INST	= K->LL_GetBlendInstance(SelfID);

        if (BONE_INST.Callback_overwrite){
            if (BONE_INST.Callback)	BONE_INST.Callback(&BONE_INST);
        } else {
            CKey				R[MAX_BLENDED];
            ConsistantKey		S[MAX_BLENDED];

            // Calculate all keys
            CKey*	D					= R;
            BlendListIt					BI;
            for (BI=BLEND_INST.Blend.begin(); BI!=BLEND_INST.Blend.end(); BI++,D++)
            {
                CBlend*			B		=	*BI;
                float			time	=	B->timeCurrent*float(SAMPLE_FPS);
                CMotion&		M		=	Motions[B->motionID];
                u32				frame	=	iFloor(time);
                float			delta	=	time-float(frame);
                u32				count	=	M._count;
                CKeyQR*			K1r		=	&M._keysR[(frame+0)%count];
                CKeyQR*			K2r		=	&M._keysR[(frame+1)%count];
                // rotation
                {
                    Fquaternion	Q1,Q2;
                    Q1.x		= float(K1r->x)*KEY_QuantI;
                    Q1.y		= float(K1r->y)*KEY_QuantI;
                    Q1.z		= float(K1r->z)*KEY_QuantI;
                    Q1.w		= float(K1r->w)*KEY_QuantI;

                    Q2.x		= float(K2r->x)*KEY_QuantI;
                    Q2.y		= float(K2r->y)*KEY_QuantI;
                    Q2.z		= float(K2r->z)*KEY_QuantI;
                    Q2.w		= float(K2r->w)*KEY_QuantI;

                    D->Q.slerp	(Q1,Q2,clampr(delta,0.f,1.f));
                }

                // translate
                if (*M._keysT){
	                CKeyQT*	K1t	= &M._keysT[(frame+0)%count];
    	            CKeyQT*	K2t	= &M._keysT[(frame+1)%count];

                    Fvector T1,T2;
                    T1.x		= float(K1t->x)*M._sizeT.x+M._initT.x;
                    T1.y		= float(K1t->y)*M._sizeT.y+M._initT.y;
                    T1.z		= float(K1t->z)*M._sizeT.z+M._initT.z;
                    T2.x		= float(K2t->x)*M._sizeT.x+M._initT.x;
                    T2.y		= float(K2t->y)*M._sizeT.y+M._initT.y;
                    T2.z		= float(K2t->z)*M._sizeT.z+M._initT.z;
                    
	                D->T.lerp	(T1,T2,delta);
                }else{
	                D->T.set	(M._initT);
                }
//                PSGP.blerp				(D,&K1,&K2,delta);
            }

            // Blend them together
            BI				= BLEND_INST.Blend.begin();

            CKey			Result;

            switch (BLEND_INST.Blend.size())
            {
            case 0:
                Result.Q.set	(0,0,0,0);
                Result.T.set	(0,0,0);
                break;
            case 1: 
                Result			= R[0];
                break;
            case 2:
                {
                    float w0 = BI[0]->blendAmount;
                    float w1 = BI[1]->blendAmount;
                    float ws = w0+w1;
                    float w;
                    if (fis_zero(ws))	w = 0;
                    else				w = w1/ws;
                    KEY_Interp	(Result,R[0],R[1], clampr(w,0.f,1.f));
                }
                break;
            default:
                {
                    int 	count 	= BLEND_INST.Blend.size();
                    float   total 	= 0;
					for (int i=0; i<count; i++){
						S[i].set(R+i,BI[i]->blendAmount);
					}
                    std::sort	(S,S+count);
                    CKey		tmp;
                    total		= S[0].w;
                    tmp			= *S[0].K;
                    for 		(int cnt=1; cnt<count; cnt++)
                    {
                    	total		+= S[cnt].w;
                    	KEY_Interp	(Result,tmp, *S[cnt].K, clampr(S[cnt].w/total,0.f,1.f) );
                        tmp 		= Result;
                    }
                }
                break;
            }

            // Build matrix
            Fmatrix					RES;
            RES.mk_xform			(Result.Q,Result.T);
            BONE_INST.mTransform.mul_43(*parent,RES);
            if (BONE_INST.Callback)		BONE_INST.Callback(&BONE_INST);
        }
        BONE_INST.mRenderTransform.mul_43(BONE_INST.mTransform,m2b_transform);

        // Calculate children
        for (xr_vector<CBoneData*>::iterator C=children.begin(); C!=children.end(); C++)
            ((CBoneDataAnimated*)(*C))->Calculate(K,&BONE_INST.mTransform);
    }
}

void CSkeletonAnimated::CalculateBones		(BOOL bForceExact)
{
	// early out.
	// check if the info is still relevant
	// skip all the computations - assume nothing changes in a small period of time :)
	if		(Device.dwTimeGlobal == UCalc_Time)										return;	// early out for "fast" update
	Update	();
	if		(!bForceExact && (Device.dwTimeGlobal < (UCalc_Time + UCalc_Interval)))	return;	// early out for "slow" update

	// here we have either:
	//	1:	timeout elapsed
	//	2:	exact computation required
	UCalc_Time			= Device.dwTimeGlobal;

	// exact computation
	// calculate bones
	Device.Statistic.Animation.Begin	();
	((CBoneDataAnimated*)(*bones)[iRoot])->Calculate	(this,&Fidentity);
	Device.Statistic.Animation.End		();

	// Calculate BOXes/Spheres if needed
	UCalc_Visibox++; 
	if (UCalc_Visibox>=psSkeletonUpdate) 
	{
		// mark
		UCalc_Visibox		= -(::Random.randI(16));

		// the update itself
		Fbox	Box; Box.invalidate();
		for (u32 b=0; b<bones->size(); b++)
		{
			if			(!LL_GetBoneVisible(u16(b)))		continue;
			Fobb&		obb		= (*bones)[b]->obb;
			Fmatrix&	Mbone	= bone_instances[b].mTransform;
			Fmatrix		Mbox;	obb.xform_get(Mbox);
			Fmatrix		X;		X.mul_43(Mbone,Mbox);
			Fvector&	S		= obb.m_halfsize;

			Fvector		P, A;
			A.set( -S.x,	-S.y,	-S.z ); X.transform_tiny(P,A); Box.modify(P);
			A.set( -S.x,	-S.y,	 S.z ); X.transform_tiny(P,A); Box.modify(P);
			A.set(  S.x,	-S.y,	 S.z ); X.transform_tiny(P,A); Box.modify(P);
			A.set(  S.x,	-S.y,	-S.z ); X.transform_tiny(P,A); Box.modify(P);
			A.set( -S.x,	 S.y,	-S.z ); X.transform_tiny(P,A); Box.modify(P);
			A.set( -S.x,	 S.y,	 S.z ); X.transform_tiny(P,A); Box.modify(P);
			A.set(  S.x,	 S.y,	 S.z ); X.transform_tiny(P,A); Box.modify(P);
			A.set(  S.x, 	 S.y,	-S.z ); X.transform_tiny(P,A); Box.modify(P);
		}

		// previous frame we have updated box - update sphere
		vis.box.min.average	(Box.min);
		vis.box.max.average	(Box.max);
		vis.box.getsphere	(vis.sphere.P,vis.sphere.R);
	}

	//
	if (Update_Callback)	Update_Callback(this);
}

