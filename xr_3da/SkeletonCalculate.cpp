#include "stdafx.h"
#include "BodyInstance.h"

extern int	psSkeletonUpdate;

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

// Calculation
typedef D3DXQUATERNION* PQ;
typedef D3DXMATRIX*		PM;
void CBoneData::Calculate(CKinematics* K, Fmatrix *parent)
{
	_declspec(align(64))	static CKey				R[MAX_BLENDED];
	_declspec(align(64))	static ConsistantKey	S[MAX_BLENDED];

	// Calculate all keys
	CKey*	D = R;
	CBoneInstance& INST			= K->LL_GetInstance(SelfID);
	BlendListIt					BI;
	for (BI=INST.Blend.begin(); BI!=INST.Blend.end(); BI++,D++)
	{
		CBlend*			B		=	*BI;
		float			time	=	B->timeCurrent*float(SAMPLE_FPS);
		CMotion&		M		=	Motions[B->motionID];
		int				frame	=	iFloor(time);
		float			delta	=	time-float(frame);
		int				count	=	M.Keys.size();
		CKeyQ&			K1		=	M.Keys[(frame+0)%count];
		CKeyQ&			K2		=	M.Keys[(frame+1)%count];

		/*
		D->Q.x					= float(K1.x)*KEY_QuantI;
		D->Q.y					= float(K1.y)*KEY_QuantI;
		D->Q.z					= float(K1.z)*KEY_QuantI;
		D->Q.w					= float(K1.w)*KEY_QuantI;
		D->T.set				(K1.T);
		*/

		PSGP.blerp				(D,&K1,&K2,delta);
	}

	// Blend them together
	CKey*	I		= R;
	BI				= INST.Blend.begin();

	CKey			Result;

	switch (INST.Blend.size())
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
			KEY_Interp		(Result,R[0],R[1],w);
		}
		break;
	default:
		{
			int count = INST.Blend.size();
			for (int i=0; i<count; i++)		S[i].set(R+i,BI[i]->blendAmount);
			std::sort	(S,S+count);
			KEY_Interp	(Result,*S[0].K, *S[1].K, S[1].w/(S[0].w+S[1].w));
		}
		break;
	}

	// Build matrix
	Fmatrix					RES;
	RES.mk_xform			(Result.Q,Result.T);
//	PSGP.m44_mul			(&INST.mTransform,parent,&RES);
	INST.mTransform.mul_43	(*parent,RES);
	if (INST.Callback)		INST.Callback(&INST);

	// Calculate chields
	for (vector<CBoneData*>::iterator C=Chields.begin(); C!=Chields.end(); C++)
		(*C)->Calculate(K,&INST.mTransform);
}

void CKinematics::Calculate(BOOL bLight)
{
	Update();

	if (Device.dwFrame == dwFrame) return;
	dwFrame = Device.dwFrame; 

	// Calculate bones
	Device.Statistic.Animation.Begin();
	if (CPU::ID.feature&_CPU_FEATURE_3DNOW) {
		CBoneInstance*	I	= bone_instances;
		CBoneInstance*	E	= bone_instances+bones->size();
		__asm {
			mov			eax,[I];
			mov			ecx,[E];
repeat:
			prefetch	[eax];
			prefetch	[eax+64];
			add			eax,TYPE CBoneInstance;
			cmp			eax,ecx;
			jnz			repeat;
		};
	}
	(*bones)[iRoot]->Calculate		(this,&precalc_identity);
	Device.Statistic.Animation.End	();

	// Calculate BOXes/Spheres if needed
	iUpdateID++; 
	if (int(iUpdateID)>=psSkeletonUpdate) 
	{
		// mark
		iUpdateID=0;

		// the update itself
		Fbox	Box; Box.invalidate();
		for (DWORD b=0; b<bones->size(); b++)
		{
			Fobb&		obb		= (*bones)[b]->obb;
			Fmatrix&	Mbone	= bone_instances[b].mTransform;
			Fmatrix		Mbox;	obb.xform_get(Mbox);
			Fmatrix		X;		X.mul(Mbone,Mbox);
			Fvector&	S		= obb.m_halfsize;

			Fvector			P,A;
			A.set( -S.x,	-S.y,	-S.z ); X.transform_tiny(P,A); Box.modify(P);
			A.set( -S.x,	-S.y,	 S.z ); X.transform_tiny(P,A); Box.modify(P);
			A.set(  S.x,	-S.y,	 S.z ); X.transform_tiny(P,A); Box.modify(P);
			A.set(  S.x,	-S.y,	-S.z ); X.transform_tiny(P,A); Box.modify(P);
			A.set( -S.x,	 S.y,	-S.z ); X.transform_tiny(P,A); Box.modify(P);
			A.set( -S.x,	 S.y,	 S.z ); X.transform_tiny(P,A); Box.modify(P);
			A.set(  S.x, 	 S.y,	 S.z ); X.transform_tiny(P,A); Box.modify(P);
			A.set(  S.x, 	 S.y,	-S.z ); X.transform_tiny(P,A); Box.modify(P);
		}

		// previous frame we have updated box - update sphere
		bv_BBox.min.average	(Box.min);
		bv_BBox.max.average	(Box.max);
		bv_BBox.getsphere	(bv_Position,bv_Radius);
	}
}
