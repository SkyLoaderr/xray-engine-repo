// BodyInstance.cpp: implementation of the CKinematics class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#include "BodyInstance.h"
#include "fmesh.h"
#include "motion.h"

int			psSkeletonUpdate	= 32;
const float	fAA					= 1.5f;	// anim-change acceleration

// High level control
void CMotionDef::Load(CKinematics* P, IReader* MP, u32 fl)
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
void CMotionDef::Load(CKinematics* P, CInifile* INI, LPCSTR  section, BOOL bCycle)
{
	int	b = -1;
	if (bCycle)	{
		// partition
		LPCSTR  B	= INI->r_string	(section,"part");
		b			= P->LL_PartID		(B);
	} else {
		// bone
		LPCSTR  B	= INI->r_string	(section,"bone");
		b			= P->LL_BoneID		(B); 
		if (b<0) b	= P->LL_BoneRoot	();
	}

	// motion
	LPCSTR  M = INI->r_string	(section,"motion");
	_strlwr((char*)M);
	int		m = P->LL_MotionID(M);
	if (m<0) Debug.fatal("Can't find motion '%s'",M);
	R_ASSERT(m>=0);

	// params
	bone_or_part= short	(b);
	motion		= WORD	(m);
	speed		= Quantize(INI->r_float(section,"speed"));
	power		= Quantize(INI->r_float(section,"power"));
	accrue		= Quantize(INI->r_float(section,"accrue"));
	falloff		= Quantize(INI->r_float(section,"falloff"));
	flags		= (INI->r_bool(section,"stop@end")?esmStopAtEnd:0);

	if (bCycle && (falloff>=accrue)) falloff = accrue-1;
}

CBlend*	CMotionDef::PlayCycle(CKinematics* P, BOOL bMixIn, PlayCallback Callback, LPVOID Callback_Param)
{
	return P->LL_PlayCycle(
		int(bone_or_part),int(motion),bMixIn,
		fAA*Dequantize(accrue),
		fAA*Dequantize(falloff),
		Dequantize(speed),
		flags&esmStopAtEnd,
		Callback,Callback_Param
		);
}

CBlend*	CMotionDef::PlayCycle(CKinematics* P, int part, BOOL bMixIn, PlayCallback Callback, LPVOID Callback_Param)
{
	return P->LL_PlayCycle(
		part,int(motion),bMixIn,
		fAA*Dequantize(accrue),
		fAA*Dequantize(falloff),
		Dequantize(speed),
		flags&esmStopAtEnd,
		Callback,Callback_Param
		);
}

CBlend*	CMotionDef::PlayFX(CKinematics* P, float power_scale)
{
	return P->LL_PlayFX(
		int(bone_or_part),int(motion),
		fAA*Dequantize(accrue),
		fAA*Dequantize(falloff),
		Dequantize(speed),
		Dequantize(power)*power_scale
		);
}

// Motion control
void	CBoneData::Motion_Start	(CKinematics* K, CBlend* handle) 
{
	K->LL_GetInstance(SelfID).blend_add		(handle);
	for (vecBonesIt I=children.begin(); I!=children.end(); I++)
		(*I)->Motion_Start	(K,handle);
}
void	CBoneData::Motion_Start_IM	(CKinematics* K, CBlend* handle) 
{
	K->LL_GetInstance(SelfID).blend_add		(handle);
}
void	CBoneData::Motion_Stop	(CKinematics* K, CBlend* handle) 
{
	K->LL_GetInstance(SelfID).blend_remove	(handle);
	for (vecBonesIt I=children.begin(); I!=children.end(); I++)
		(*I)->Motion_Stop	(K,handle);
}
void	CBoneData::Motion_Stop_IM	(CKinematics* K, CBlend* handle) 
{
	K->LL_GetInstance(SelfID).blend_remove	(handle);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
int	CKinematics::LL_BoneID		(LPCSTR B)
{
	accel::iterator I = bone_map->find(LPSTR(B));
	if (I==bone_map->end())		return -1;
	else						return I->second;
}
int	CKinematics::LL_MotionID	(LPCSTR B)
{
	accel::iterator I = motion_map->find(LPSTR(B));
	if (I==motion_map->end())	return -1;
	else						return I->second;
}
int CKinematics::LL_PartID		(LPCSTR B)
{
	if (0==partition)			return -1;
	for (int id=0; id<MAX_PARTS; id++) {
		CPartDef&	P = (*partition)[id];
		if (0==P.Name)	continue;
		if (0==stricmp(B,P.Name)) return id;
	}
	return -1;
}

// cycles
CMotionDef*	CKinematics::ID_Cycle	(LPCSTR  N)
{
	mdef::iterator I = m_cycle->find(LPSTR(N));
	if (I==m_cycle->end())	{ Debug.fatal("! MODEL: can't find cycle: %s", N); return 0; }
	return &I->second;
}
CMotionDef*	CKinematics::ID_Cycle_Safe(LPCSTR  N)
{
	mdef::iterator I = m_cycle->find(LPSTR(N));
	if(I==m_cycle->end()) return 0;
	return &I->second;
}
CBlend*	CKinematics::PlayCycle		(LPCSTR  N, BOOL bMixIn, PlayCallback Callback, LPVOID CallbackParam)
{
	mdef::iterator I = m_cycle->find(LPSTR(N));
	if (I!=m_cycle->end())	return I->second.PlayCycle(this,bMixIn,Callback,CallbackParam);
	else					{ Debug.fatal("! MODEL: can't find cycle: %s", N); return 0; }
}

// fx'es
CMotionDef*	CKinematics::ID_FX			(LPCSTR  N)
{
	mdef::iterator I = m_fx->find(LPSTR(N));
	if (I==m_fx->end())		{ Debug.fatal("! MODEL: can't find FX: %s", N); return 0; }
	return &I->second;
}
CMotionDef*	CKinematics::ID_FX_Safe		(LPCSTR  N)
{
	mdef::iterator I = m_fx->find(LPSTR(N));
	if(I==m_fx->end()) return 0;
	return &I->second;
}
CBlend*	CKinematics::PlayFX			(LPCSTR  N, float power_scale)
{
	mdef::iterator I = m_fx->find(LPSTR(N));
	if (I!=m_fx->end())		return I->second.PlayFX(this,power_scale);
	else					{ Debug.fatal("! MODEL: can't find FX: %s", N); return 0; }
}

CBlend*	CKinematics::LL_PlayFX(int bone, int motion, float blendAccrue, float blendFalloff, float Speed, float Power)
{
	if (motion<0)	return 0;
	if (bone<0)		bone = iRoot;
	
	CBlend*	B		= IBlend_Create();
	CBoneData*	Bone	= (*bones)[bone];
	Bone->Motion_Start(this,B);
	
	B->blend		= CBlend::eAccrue;
	B->blendAmount	= 0;
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

void	CKinematics::LL_FadeCycle(int part, float falloff)
{
	BlendList&	Blend	= blend_cycles[part];
	
	for (u32 I=0; I<Blend.size(); I++)
	{
		CBlend& B		= *Blend[I];
		B.blend			= CBlend::eFalloff;
		B.blendFalloff	= falloff;
//		if (!B.playing)	{
		if (!B.playing&&!B.stop_at_end){ //.
			B.playing		= TRUE;
			B.stop_at_end	= FALSE;
			B.blendAmount	= EPS_S;
		}
        if (B.stop_at_end)  B.playing = FALSE;  //.
	}
}

void	CKinematics::LL_CloseCycle(int part)
{
	// destroy cycle(s)
	BlendListIt	I = blend_cycles[part].begin(), E = blend_cycles[part].end();
	for (; I!=E; I++)
	{
		CBlend& B = *(*I);

		B.blend = CBlend::eFREE_SLOT;
		
		CPartDef& P	= (*partition)[B.bone_or_part];
		for (int i=0; i<int(P.bones.size()); i++)
			(*bones)[P.bones[i]]->Motion_Stop(this,*I);
		
		blend_cycles[part].erase(I);
		E=blend_cycles[part].end(); I--; 
	}
}

CBlend*	CKinematics::LL_PlayCycle(int part, int motion, BOOL  bMixing,	float blendAccrue, float blendFalloff, float Speed, BOOL noloop, PlayCallback Callback, LPVOID CallbackParam)
{
	// validate and unroll
	if (motion<0)			return 0;
	if (part>=MAX_PARTS)	return 0;
	if (part<0)	{
		for (int i=0; i<MAX_PARTS; i++)
			LL_PlayCycle(i,motion,bMixing,blendAccrue,blendFalloff,Speed,noloop,Callback,CallbackParam);
		return 0;
	}
	if (0==(*partition)[part].Name)	return 0;

	// Process old cycles and create _new_
	if (bMixing)	LL_FadeCycle	(part,blendFalloff);
	else			LL_CloseCycle	(part);
	CPartDef& P	=	(*partition)[part];
	CBlend*	B	=	IBlend_Create();

	CBoneData*	Bone=0;
	for (int i=0; i<int(P.bones.size()); i++)
	{
		Bone	= (*bones)[P.bones[i]];
		Bone->Motion_Start_IM	(this,B);
	}
	VERIFY		(Bone);
	blend_cycles[part].push_back(B);

	// Setup blend params
	if (bMixing)	{
		B->blend		= CBlend::eAccrue;
		B->blendAmount	= 0;
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

void CKinematics::Update ()
{
	if (Update_LastTime==Device.dwTimeGlobal) return;
	u32 DT = Device.dwTimeGlobal-Update_LastTime;
	if (DT>66) DT=66;
	float dt = float(DT)/1000.f;
	Update_LastTime = Device.dwTimeGlobal;

	BlendListIt	I,E;

	// Cycles
	for (u32 part=0; part<MAX_PARTS; part++) 
	{
		if (0==(*partition)[part].Name)	continue;

		I = blend_cycles[part].begin(); E = blend_cycles[part].end();
		for (; I!=E; I++)
		{
			CBlend& B = *(*I);
//			if (!B.playing) continue;
			if (!B.playing&&!B.stop_at_end)	continue; //.
			if (B.dwFrame==Device.dwFrame)	continue;
			B.dwFrame		=	Device.dwFrame;
//			B.timeCurrent += dt*B.speed;
			if (B.playing) 	B.timeCurrent += dt*B.speed; //.
			switch (B.blend) 
			{
			case CBlend::eFREE_SLOT: 
				NODEFAULT;
			case CBlend::eFixed:	
				B.blendAmount 		= B.blendPower; 
				if (B.stop_at_end && (B.timeCurrent > (B.timeTotal-SAMPLE_SPF) )) {
					B.timeCurrent	= B.timeTotal-SAMPLE_SPF;
					B.playing		= FALSE;
					if (B.Callback)	B.Callback(&B);
				}
				break;
			case CBlend::eAccrue:	
				B.blendAmount 		+= dt*B.blendAccrue*B.blendPower;
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
					for (int i=0; i<int(P.bones.size()); i++)
						(*bones)[P.bones[i]]->Motion_Stop(this,*I);

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
				B.blendAmount = B.blendPower; 
				// calc time to falloff
				float time2falloff = B.timeTotal - 1/(B.blendFalloff*B.speed);
				if (B.timeCurrent >= time2falloff) {
					// switch to falloff
					B.blend		= CBlend::eFalloff;
				}
			}
			break;
		case CBlend::eAccrue:	
			B.blendAmount += dt*B.blendAccrue*B.blendPower;
			if (B.blendAmount>=B.blendPower) {
				// switch to fixed
				B.blendAmount	= B.blendPower;
				B.blend			= CBlend::eFixed;
			}
			break;
		case CBlend::eFalloff:	
			B.blendAmount -= dt*B.blendFalloff*B.blendPower;
			if (B.timeCurrent>=B.timeTotal || B.blendAmount<=0) {
				// destroy fx
				B.blend = CBlend::eFREE_SLOT;
				(*bones)[B.bone_or_part]->Motion_Stop(this,*I);
				blend_fx.erase(I); 
				E=blend_fx.end(); I--; 
			}
			break;
		default: NODEFAULT;
		}
	}
}

void CKinematics::DebugRender(Fmatrix& XFORM)
{
	Calculate();

	CBoneData::BoneDebug	dbgLines;
	(*bones)[iRoot]->DebugQuery	(dbgLines);

	Fvector Z;  Z.set(0,0,0);
	Fvector H1; H1.set(0.01f,0.01f,0.01f);
	Fvector H2; H2.mul(H1,2);
	for (u32 i=0; i<dbgLines.size(); i+=2)
	{
		Fmatrix& M1 = bone_instances[dbgLines[i]].mTransform;
		Fmatrix& M2 = bone_instances[dbgLines[i+1]].mTransform;

		Fvector P1,P2;
		M1.transform_tiny(P1,Z);
		M2.transform_tiny(P2,Z);
		RCache.dbg_DrawLINE(XFORM,P1,P2,D3DCOLOR_XRGB(0,255,0));

		Fmatrix M;
		M.mul_43(XFORM,M2);
		RCache.dbg_DrawOBB(M,H1,D3DCOLOR_XRGB(255,255,255));
		RCache.dbg_DrawOBB(M,H2,D3DCOLOR_XRGB(255,255,255));
	}

	for (u32 b=0; b<bones->size(); b++)
	{
		Fobb&		obb		= (*bones)[b]->obb;
		Fmatrix&	Mbone	= bone_instances[b].mTransform;
		Fmatrix		Mbox;	obb.xform_get(Mbox);
		Fmatrix		X;		X.mul(Mbone,Mbox);
		Fmatrix		W;		W.mul(XFORM,X);
		RCache.dbg_DrawOBB(W,obb.m_halfsize,D3DCOLOR_XRGB(0,0,255));
	}
}

void CKinematics::Release()
{
	// xr_free accels
	accel::iterator A;
	for (A=motion_map->begin(); A!=motion_map->end(); A++)
		xr_free((char*)A->first);
	for (A=bone_map->begin(); A!=bone_map->end(); A++)
		xr_free((char*)A->first);

	// xr_free MDef's
	mdef::iterator B;
	for (B=m_cycle->begin(); B!=m_cycle->end(); B++)
		xr_free((char*)B->first);
	for (B=m_fx->begin(); B!=m_fx->end(); B++)
		xr_free((char*)B->first);

	// xr_free partition
	for (u32 i=0; i<MAX_PARTS; i++)
		xr_free((*partition)[i].Name);
	
	// xr_free bones
	for (i=0; i<bones->size(); i++)
	{
		CBoneData* &B = (*bones)[i];
		xr_delete(B);
	}

	// destroy shared data
	xr_delete(bones);
	xr_delete(motion_map);
	xr_delete(bone_map);
	xr_delete(m_cycle);
	xr_delete(m_fx);
	xr_delete(partition);
}

CKinematics::~CKinematics	()
{
	IBoneInstances_Destroy	();
}

void	CKinematics::IBoneInstances_Create()
{
	R_ASSERT		(bones);
	u32				size	= bones->size();
	void*			ptr		= xr_malloc(size*sizeof(CBoneInstance));
	bone_instances			= (CBoneInstance*)ptr;
	for (u32 i=0; i<size; i++)
		bone_instances[i].construct();
}

void	CKinematics::IBoneInstances_Destroy()
{
	if (bone_instances) {
		xr_free(bone_instances);
		bone_instances = NULL;
	}
}

#define PCOPY(a)	a = pFrom->a
void CKinematics::Copy(IVisual *P) 
{
	inherited::Copy	(P);

	CKinematics* pFrom = (CKinematics*)P;
	PCOPY(bones);
	PCOPY(iRoot);
	PCOPY(motion_map);
	PCOPY(bone_map);
	PCOPY(m_cycle);
	PCOPY(m_fx);
	PCOPY(partition);

	IBlend_Startup			();
	IBoneInstances_Create	();

	for (u32 i=0; i<children.size(); i++) 
	{
		IVisual*	V = children[i];
		CSkeletonX*		B = NULL;
		switch (V->Type)
		{
		case MT_SKELETON_PART:
			{
				CSkeletonX_PM*	X = (CSkeletonX_PM*)V;
				B = (CSkeletonX*)X;
			}
			break;
		case MT_SKELETON_PART_STRIPPED:
			{
				CSkeletonX_ST*	X = (CSkeletonX_ST*)V;
				B = (CSkeletonX*)X;
			}
			break;
		default: NODEFAULT;
		}
		R_ASSERT(B);
		B->SetParent(this);
	}

	Update_ID		= ::Random.randI(psSkeletonUpdate);
}

void CKinematics::Load(const char* N, IReader *data, u32 dwFlags)
{
	inherited::Load(N,data, dwFlags);

	// Globals
	motion_map		= xr_new<accel> ();
	bone_map		= xr_new<accel> ();
	bones			= xr_new<vecBones> ();
	partition		= xr_new<CPartition> ();
	bone_instances	= NULL;

	// Load bones
	vector<LPSTR>	L_parents;

	R_ASSERT(data->find_chunk(OGF_BONE_NAMES));
	u32 dwCount = data->r_u32();

	for (; dwCount; dwCount--)
	{
		char buf[256];

		// Bone
		int		ID = bones->size();
		data->r_stringZ(buf);	strlwr(buf);
		CBoneData*	pBone = xr_new<CBoneData> (ID);
		bones->push_back(pBone);
		bone_map->insert(make_pair(xr_strdup(buf),ID));

		// It's parent
		data->r_stringZ(buf);	strlwr(buf);
		L_parents.push_back(xr_strdup(buf));

		data->r(&pBone->obb,sizeof(Fobb));
	}

	// Attach bones to their parents
	iRoot = -1;
	for (u32 i=0; i<bones->size(); i++) {
		LPCSTR 	P = L_parents[i];
		CBoneData*	B = (*bones)[i];
		if (!P[0]) {
			// no parent - this is root bone
			R_ASSERT(-1==iRoot);
			iRoot = i;
			continue;
		} else {
			int ID = LL_BoneID(P);
			R_ASSERT(ID>=0);
			(*bones)[ID]->children.push_back(B);
		}
	}
	R_ASSERT(-1 != iRoot);

	// Free parents
	for (u32 aaa=0; aaa<L_parents.size(); aaa++)
		xr_free(L_parents[aaa]);

	// Load animation
	IReader* MS = data->open_chunk(OGF_MOTIONS);
	u32 dwCNT = 0;
	MS->r_chunk_safe	(0,&dwCNT,sizeof(dwCNT));
	for (u32 M=0; M<dwCNT; M++)
	{
		R_ASSERT(MS->find_chunk(M+1));
        char mname[128];
		MS->r_stringZ(mname);
		motion_map->insert(make_pair(xr_strdup(strlwr(mname)),M));

		u32 dwLen = MS->r_u32();
		for (i=0; i<bones->size(); i++)
		{
			CMotion TMP;
			TMP.Keys.reserve(dwLen);
			for (u32 k=0; k<dwLen; k++)
			{
				CKeyQ K;
				MS->r(&K,sizeof(K));
				TMP.Keys.push_back(K);
			}
			(*bones)[i]->Motions.push_back(TMP);
		}
	}
	MS->close();

	// Load definitions
	IReader* MP = data->open_chunk(OGF_SMPARAMS2);
    if (MP){
	    WORD vers = MP->r_u16();
        R_ASSERT(vers==xrOGF_SMParamsVersion);
        // partitions
        WORD part_count;
        part_count = MP->r_u16();
        string128 buf;
        for (WORD part_i=0; part_i<part_count; part_i++){
            CPartDef&	PART	= (*partition)[part_i];
            MP->r_stringZ(buf);
            PART.Name			= _strlwr(xr_strdup(buf));
            PART.bones.resize	(MP->r_u16());
            MP->r				(&*PART.bones.begin(),PART.bones.size()*sizeof(int));
        }

        m_cycle = xr_new<mdef> ();
        m_fx	= xr_new<mdef> ();

        // motion defs (cycle&fx)
        WORD mot_count			= MP->r_u16();
        for (WORD mot_i=0; mot_i<mot_count; mot_i++){
            MP->r_stringZ(buf);
	        u32 dwFlags		= MP->r_u32();
            CMotionDef	D;		D.Load(this,MP,dwFlags);
            if (dwFlags&esmFX)	m_fx->insert(make_pair(_strlwr(xr_strdup(buf)),D));
            else				m_cycle->insert(make_pair(_strlwr(xr_strdup(buf)),D));
        }
        MP->close();
    }else{
		IReader* MP = data->open_chunk(OGF_SMPARAMS);
        if (MP){
            // partitions
            WORD part_count;
            part_count = MP->r_u16();
            string128 buf;
            for (WORD part_i=0; part_i<part_count; part_i++){
                CPartDef&	PART	= (*partition)[part_i];
                MP->r_stringZ(buf);
                PART.Name			= _strlwr(xr_strdup(buf));
                PART.bones.resize	(MP->r_u16());
                MP->r				(&*PART.bones.begin(),PART.bones.size()*sizeof(int));
            }

            m_cycle = xr_new<mdef> ();
            m_fx	= xr_new<mdef> ();

            // motion defs (cycle&fx)
            WORD mot_count			= MP->r_u16();
            for (WORD mot_i=0; mot_i<mot_count; mot_i++){
                MP->r_stringZ(buf);
                BYTE bCycle			=	MP->r_u8();
				CMotionDef	D;		D.Load(this,MP,bCycle?0:esmFX);
				BYTE bNoLoop		=	MP->r_u8();
				D.flags				|=	(bNoLoop?esmStopAtEnd:0);
                if (bCycle)			m_cycle->insert(make_pair(_strlwr(xr_strdup(buf)),D));
                else				m_fx->insert(make_pair(_strlwr(xr_strdup(buf)),D));
            }
            MP->close();
        }else{
            // old variant (read params from ltx)
            R_ASSERT(N && N[0]);
            char def_N[MAX_PATH];
            if (0==strext(N))	strconcat(def_N,N,".ltx");
            else				{
                strcpy(def_N,N);
                strcpy(strext(def_N),".ltx");
            }
            m_cycle = xr_new<mdef> ();
            m_fx	= xr_new<mdef> ();

            CInifile DEF(def_N);
            CInifile::SectIt I;

            // partitions
            CInifile::Sect& S = DEF.r_section("partition");
            int pid = 0;
            for (I=S.begin(); I!=S.end(); I++,pid++)
            {
                if (pid>=MAX_PARTS)	Debug.fatal("Too many partitions in motion description '%s'",def_N);
                CPartDef&	PART		= (*partition)[pid];
                LPSTR	N				= _strlwr(xr_strdup(I->first));
                PART.Name				= N;
                CInifile::Sect&		P	= DEF.r_section(N);
                CInifile::SectIt	B	= P.begin();
                for (; B!=P.end(); B++)
                {
                    int bone			= LL_BoneID(B->first);
                    if (bone<0)			Debug.fatal("Partition '%s' has incorrect bone name ('%s')",N,B->first);
                    PART.bones.push_back(bone);
                }
            }

            // cycles
            {
                CInifile::Sect& S = DEF.r_section("cycle");
                for (I=S.begin(); I!=S.end(); I++)
                {
                    CMotionDef	D;
                    D.Load(this,&DEF,I->first, true);
                    m_cycle->insert(make_pair(_strlwr(xr_strdup(I->first)),D));
                }
            }

            // FXes
            {
                CInifile::Sect& F = DEF.r_section("fx");
                for (I=F.begin(); I!=F.end(); I++)
                {
                    CMotionDef	D;
                    D.Load(this,&DEF,I->first, false);
                    m_fx->insert(make_pair(_strlwr(xr_strdup(I->first)),D));
                }
            }
        }
    }


	// Init blend pool
	IBlend_Startup();
}
