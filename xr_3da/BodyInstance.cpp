// BodyInstance.cpp: implementation of the CKinematics class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BodyInstance.h"
#include "fmesh.h"
#include "xr_ini.h"
#include "xr_func.h"

int			psSkeletonUpdate	= 32;
const float	fAA					= 1.5f;	// anim-change acceleration

// High level control
void CMotionDef::Load(CKinematics* P, CInifile* INI, LPCSTR  section, BOOL bCycle)
{
	int	b = -1;
	if (bCycle)	{
		// partition
		LPCSTR  B	= INI->ReadSTRING	(section,"part");
		b			= P->LL_PartID		(B);
	} else {
		// bone
		LPCSTR  B	= INI->ReadSTRING	(section,"bone");
		b			= P->LL_BoneID		(B); 
		if (b<0) b	= P->LL_BoneRoot	();
	}

	// motion
	LPCSTR  M = INI->ReadSTRING	(section,"motion");
	_strlwr((char*)M);
	int		m = P->LL_MotionID(M);
	if (m<0) Device.Fatal("Can't find motion '%s'",M);
	R_ASSERT(m>=0);

	// params
	bone_or_part= short	(b);
	motion		= WORD	(m);
	speed		= Quantize(INI->ReadFLOAT(section,"speed"));
	power		= Quantize(INI->ReadFLOAT(section,"power"));
	accrue		= Quantize(INI->ReadFLOAT(section,"accrue"));
	falloff		= Quantize(INI->ReadFLOAT(section,"falloff"));
	noloop		= INI->ReadBOOL(section,"stop@end");

	if (bCycle && (falloff>=accrue)) falloff = accrue-1;
}

CBlend*	CMotionDef::PlayCycle(CKinematics* P)
{
	return P->LL_PlayCycle(
		int(bone_or_part),int(motion),
		fAA*Dequantize(accrue),
		fAA*Dequantize(falloff),
		Dequantize(speed),
		noloop
		);
}

CBlend*	CMotionDef::PlayFX(CKinematics* P)
{
	return P->LL_PlayFX(
		int(bone_or_part),int(motion),
		fAA*Dequantize(accrue),
		fAA*Dequantize(falloff),
		Dequantize(speed),
		Dequantize(power) 
		);
}

// Motion control
void	CBoneData::Motion_Start	(CKinematics* K, CBlend* handle) 
{
	K->LL_GetInstance(SelfID).blend_add		(handle);
	for (vecBonesIt I=Chields.begin(); I!=Chields.end(); I++)
		(*I)->Motion_Start	(K,handle);
}
void	CBoneData::Motion_Start_IM	(CKinematics* K, CBlend* handle) 
{
	K->LL_GetInstance(SelfID).blend_add		(handle);
}
void	CBoneData::Motion_Stop	(CKinematics* K, CBlend* handle) 
{
	K->LL_GetInstance(SelfID).blend_remove	(handle);
	for (vecBonesIt I=Chields.begin(); I!=Chields.end(); I++)
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
	R_ASSERT(I!=m_cycle->end());
	return &I->second;
}
CMotionDef*	CKinematics::ID_Cycle_Safe(LPCSTR  N)
{
	mdef::iterator I = m_cycle->find(LPSTR(N));
	if(I==m_cycle->end()) return 0;
	return &I->second;
}
CBlend*	CKinematics::PlayCycle		(LPCSTR  N)
{
	mdef::iterator I = m_cycle->find(LPSTR(N));
	if (I!=m_cycle->end())	return I->second.PlayCycle(this);
	else {
		Msg("ANIM::Cycle '%s' not found.",N);
		return NULL;
	}
}

// fx'es
CMotionDef*	CKinematics::ID_FX			(LPCSTR  N)
{
	mdef::iterator I = m_fx->find(LPSTR(N));
	R_ASSERT(I!=m_fx->end());
	return &I->second;
}
CMotionDef*	CKinematics::ID_FX_Safe		(LPCSTR  N)
{
	mdef::iterator I = m_fx->find(LPSTR(N));
	if(I==m_fx->end()) return 0;
	return &I->second;
}
CBlend*	CKinematics::PlayFX			(LPCSTR  N)
{
	mdef::iterator I = m_fx->find(LPSTR(N));
	if (I!=m_fx->end())		return I->second.PlayFX(this);
	else {
		Msg("ANIM::FX '%s' not found.",N);
		return NULL;
	}
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
	B->noloop		= FALSE;

	blend_fx.push_back(B);
	return			B;
}

void	CKinematics::LL_FadeCycle(int part, float falloff)
{
	BlendList&	Blend	= blend_cycles[part];

	for (DWORD I=0; I<Blend.size(); I++)
	{
		CBlend& B		= *Blend[I];
		B.blend			= CBlend::eFalloff;
		B.blendFalloff	= falloff;
		if (!B.playing)	{
			B.playing	= TRUE;
			B.noloop	= FALSE;
			B.blendAmount=EPS_S;
		}
	}
}

CBlend*	CKinematics::LL_PlayCycle(int part, int motion, float blendAccrue, float blendFalloff, float Speed, BOOL noloop)
{
	// validate and unroll
	if (motion<0)			return 0;
	if (part>=MAX_PARTS)	return 0;
	if (part<0)	{
		for (int i=0; i<MAX_PARTS; i++)
			LL_PlayCycle(i,motion,blendAccrue,blendFalloff,Speed,noloop);
		return 0;
	}
	if (0==(*partition)[part].Name)	return 0;

	// Process old cycles and create new
	LL_FadeCycle	(part,blendFalloff);
	CPartDef& P	=	(*partition)[part];
	CBlend*	B	=	IBlend_Create();

	CBoneData*	Bone;
	for (int i=0; i<int(P.bones.size()); i++)
	{
		Bone	= (*bones)[P.bones[i]];
		Bone->Motion_Start_IM	(this,B);
	}
	blend_cycles[part].push_back(B);

	// Setup blend params
	B->blend		= CBlend::eAccrue;
	B->blendAmount	= 0;
	B->blendAccrue	= blendAccrue;
	B->blendFalloff	= 0; // blendFalloff used for previous cycles
	B->blendPower	= 1;
	B->speed		= Speed;
	B->motionID		= motion;
	B->timeCurrent	= 0;
	B->timeTotal	= Bone->Motions[motion].GetLength();
	B->bone_or_part	= part;

	B->playing		= TRUE;
	B->noloop		= noloop;
	return			B;
}

void CKinematics::Update ()
{
	if (dwUpdate_LastTime==Device.dwTimeGlobal) return;
	DWORD DT = Device.dwTimeGlobal-dwUpdate_LastTime;
	if (DT>66) DT=66;
	float dt = float(DT)/1000.f;
	dwUpdate_LastTime = Device.dwTimeGlobal;

	BlendListIt	I,E;

	// Cycles
	for (DWORD part=0; part<MAX_PARTS; part++) 
	{
		if (0==(*partition)[part].Name)	continue;

		I = blend_cycles[part].begin(); E = blend_cycles[part].end();
		for (; I!=E; I++)
		{
			CBlend& B = *(*I);
			if (!B.playing)					continue;
			if (B.dwFrame==Device.dwFrame)	continue;
			B.dwFrame		=	Device.dwFrame;
			B.timeCurrent	+=	dt*B.speed;
			switch (B.blend) 
			{
			case CBlend::eFREE_SLOT: 
				NODEFAULT;
			case CBlend::eFixed:	
				B.blendAmount = B.blendPower; 
				if (B.noloop && (B.timeCurrent > (B.timeTotal-SAMPLE_SPF) )) {
					B.timeCurrent	= B.timeTotal-SAMPLE_SPF;
					B.playing		= FALSE;
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
				if (B.blendAmount<=0) {
					// destroy cycle
					B.blend = CBlend::eFREE_SLOT;

					CPartDef& P	= (*partition)[B.bone_or_part];
					for (int i=0; i<int(P.bones.size()); i++)
						(*bones)[P.bones[i]]->Motion_Stop(this,*I);

					blend_cycles[part].erase(I);
					E=blend_cycles[part].end(); I--; 
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
	for (DWORD i=0; i<dbgLines.size(); i+=2)
	{
		Fmatrix& M1 = bone_instances[dbgLines[i]].mTransform;
		Fmatrix& M2 = bone_instances[dbgLines[i+1]].mTransform;

		Fvector P1,P2;
		M1.transform_tiny(P1,Z);
		M2.transform_tiny(P2,Z);
		Device.Primitive.dbg_DrawLINE(XFORM,P1,P2,D3DCOLOR_XRGB(0,255,0));

		Fmatrix M;
		M.mul_43(XFORM,M2);
		Device.Primitive.dbg_DrawOBB(M,H1,D3DCOLOR_XRGB(255,255,255));
		Device.Primitive.dbg_DrawOBB(M,H2,D3DCOLOR_XRGB(255,255,255));
	}

	for (DWORD b=0; b<bones->size(); b++)
	{
		Fobb&		obb		= (*bones)[b]->obb;
		Fmatrix&	Mbone	= bone_instances[b].mTransform;
		Fmatrix		Mbox;	obb.xform_get(Mbox);
		Fmatrix		X;		X.mul(Mbone,Mbox);
		Fmatrix		W;		W.mul(XFORM,X);
		Device.Primitive.dbg_DrawOBB(W,obb.m_halfsize,D3DCOLOR_XRGB(0,0,255));
	}
}

void CKinematics::Release()
{
	// free accels
	accel::iterator A;
	for (A=motion_map->begin(); A!=motion_map->end(); A++)
		free(A->first);
	for (A=bone_map->begin(); A!=bone_map->end(); A++)
		free(A->first);

	// free MDef's
	mdef::iterator B;
	for (B=m_cycle->begin(); B!=m_cycle->end(); B++)
		free(B->first);
	for (B=m_fx->begin(); B!=m_fx->end(); B++)
		free(B->first);

	// free partition
	for (DWORD i=0; i<MAX_PARTS; i++)
		_FREE((*partition)[i].Name);
	
	// free bones
	for (i=0; i<bones->size(); i++)
	{
		CBoneData* &B = (*bones)[i];
		_DELETE(B);
	}

	// destroy shared data
	_DELETE(bones);
	_DELETE(motion_map);
	_DELETE(bone_map);
	_DELETE(m_cycle);
	_DELETE(m_fx);
	_DELETE(partition);
}

CKinematics::~CKinematics	()
{
	IBoneInstances_Destroy	();
}

void	CKinematics::IBoneInstances_Create()
{
	R_ASSERT		(bones);
	DWORD			size	= bones->size();
	void*			ptr		= _aligned_malloc(size*sizeof(CBoneInstance),64);
	R_ASSERT		(DWORD(ptr)%64 == 0);
	bone_instances			= (CBoneInstance*)ptr;
	for (DWORD i=0; i<size; i++)
		bone_instances[i].construct();
}

void	CKinematics::IBoneInstances_Destroy()
{
	if (bone_instances) {
		_aligned_free(bone_instances);
		bone_instances = NULL;
	}
}

#define PCOPY(a)	a = pFrom->a
void CKinematics::Copy(FBasicVisual *P) 
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

	for (DWORD i=0; i<chields.size(); i++) 
	{
		FBasicVisual*	V = chields[i];
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

	iUpdateID	= rand()%psSkeletonUpdate;
}

void CKinematics::Load(const char* N, CStream *data, DWORD dwFlags)
{
	inherited::Load(N,data, dwFlags);

	// Globals
	motion_map		= new accel;
	bone_map		= new accel;
	bones			= new vecBones;
	partition		= new CPartition;
	bone_instances	= NULL;

	// Load bones
	vector<LPSTR>	L_parents;

	R_ASSERT(data->FindChunk(OGF_BONE_NAMES));
	DWORD dwCount = data->Rdword();

	for (; dwCount; dwCount--){
		char buf[256];

		// Bone
		int		ID = bones->size();
		data->RstringZ(buf);	strlwr(buf);
		CBoneData*	pBone = new CBoneData(ID);
		bones->push_back(pBone);
		bone_map->insert(make_pair(strdup(buf),ID));

		// It's parent
		data->RstringZ(buf);	strlwr(buf);
		L_parents.push_back(strdup(buf));

		data->Read(&pBone->obb,sizeof(Fobb));
	}

	// Attach bones to their parents
	iRoot = -1;
	for (DWORD i=0; i<bones->size(); i++) {
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
			(*bones)[ID]->Chields.push_back(B);
		}
	}
	R_ASSERT(-1 != iRoot);

	// Free parents
	for (DWORD aaa=0; aaa<L_parents.size(); aaa++)
		_FREE(L_parents[aaa]);

	// Load animation
	CStream* MS = data->OpenChunk(OGF_MOTIONS);
	DWORD dwCNT = 0;
	MS->ReadChunk(0,&dwCNT);
	for (DWORD M=0; M<dwCNT; M++)
	{
		R_ASSERT(MS->FindChunk(M+1));
        char mname[128];
		MS->RstringZ(mname);
		motion_map->insert(make_pair(strdup(strlwr(mname)),M));

		DWORD dwLen = MS->Rdword();
		for (i=0; i<bones->size(); i++)
		{
			CMotion TMP;
			TMP.Keys.reserve(dwLen);
			for (DWORD k=0; k<dwLen; k++)
			{
				CKeyQ K;
				MS->Read(&K,sizeof(K));
				TMP.Keys.push_back(K);
			}
			(*bones)[i]->Motions.push_back(TMP);
		}
	}
	MS->Close();

	// Load definitions
	R_ASSERT(N && N[0]);
	char def_N[MAX_PATH];
	if (0==strext(N))	strconcat(def_N,N,".ltx");
	else				{
		strcpy(def_N,N);
		strcpy(strext(def_N),".ltx");
	}
	m_cycle = new mdef;
	m_fx	= new mdef;

	CInifile DEF(def_N);
	CInifile::SectIt I;

	// partitions
	CInifile::Sect& S = DEF.ReadSection("partition");
	int pid = 0;
	for (I=S.begin(); I!=S.end(); I++,pid++) 
	{
		if (pid>=MAX_PARTS)	Device.Fatal("Too many partitions in motion description '%s'",def_N);
		CPartDef&	PART		= (*partition)[pid];
		LPSTR	N				= _strlwr(strdup(I->first));
		PART.Name				= N;
		CInifile::Sect&		P	= DEF.ReadSection(N);
		CInifile::SectIt	B	= P.begin();
		for (; B!=P.end(); B++)
		{
			int bone			= LL_BoneID(B->first);
			if (bone<0)			Device.Fatal("Partition '%s' has incorrect bone name ('%s')",N,B->first);
			PART.bones.push_back(bone);
		}
	}
	
	// cycles
	{
		CInifile::Sect& S = DEF.ReadSection("cycle");
		for (I=S.begin(); I!=S.end(); I++) 
		{
			CMotionDef	D;
			D.Load(this,&DEF,I->first, true);
			m_cycle->insert(make_pair(_strlwr(strdup(I->first)),D));
		}
	}
	
	// FXes
	{
		CInifile::Sect& F = DEF.ReadSection("fx");
		for (I=F.begin(); I!=F.end(); I++) 
		{
			CMotionDef	D;
			D.Load(this,&DEF,I->first, false);
			m_fx->insert(make_pair(_strlwr(strdup(I->first)),D));
		}
	}

	// Init blend pool
	IBlend_Startup();
}
