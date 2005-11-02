#include "stdafx.h"
#include "IKLimb.h"
#include "../../SkeletonCustom.h"
#include "../ode_include.h"
#include "../MathUtils.h"
#include "../GameObject.h"
#include "../Level.h"
const Matrix	IKLocalJoint	={0,0,1,0,  -1,0,0,0, 0,-1,0,0, 0,0,0,1};//. in XGlobal
const Fmatrix	XLocalJoint		={0,-1,0,0, -1,0,0,0, 0,0,1,0,  0,0,0,1};

const Fmatrix	xm2im			={0,0,1,0,	0,1,0,0, 1,0,0,0, 0,0,0,1};

const Fvector	xgproj_axis		={0,-1,0};
const Fvector	xgpos_axis		={0,0,1};

const Fvector	xlproj_axis		={1,0,0};
const Fvector	xlpos_axis		={0,0,1};
typedef float IVektor [3];

const IVektor	lproj_vector	={0,0,1};
const IVektor	lpos_vector		={1,0,0};

const IVektor	gproj_vector	={0,0,1};//. in XGlobal
const IVektor	gpos_vector		={1,0,0};
const float		peak_dist		=1.f	;

CIKLimb::CIKLimb()
{
	Invalidate();

}
void CIKLimb::Invalidate()
{
	m_tri					=NULL												;
	m_tri_hight				=-dInfinity											;
	m_toe_position			.set(0,0,0)											;
	m_bones[0]				=BI_NONE											;	
	m_bones[1]				=BI_NONE											;	
	m_bones[2]				=BI_NONE											;	
	m_bones[3]				=BI_NONE											;	
}
void XM2IM(const Fmatrix	&XM,Matrix	&IM)
{
	((Fmatrix*)(&IM))->mul_43(xm2im,XM);
}
void IM2XM(const Matrix	&IM,Fmatrix	&XM)
{
	XM.mul_43(xm2im,*((Fmatrix*)(&IM)));
}
void XV2IV(const Fvector	&XV,IVektor &IV)
{
	xm2im.transform_dir(cast_fv(IV),XV);
}

void IV2XV(const IVektor &IV,Fvector	&XV)
{
	xm2im.transform_dir(XV),cast_fv(IV);
}
void CIKLimb::Calculate(CKinematics* K)
{
	Collide(K);
	if(m_tri)
	{
		Matrix		m	;
		GoalMatrix	(m,K)	;
		if(m_limb.SetGoal(m,TRUE))
		{
			float x[7];
			Fvector pos;
			pos.set(K->LL_GetTransform(m_bones[1]).c);
			K->LL_GetTransform(m_bones[0]).transform_tiny(pos);
			if(m_limb.SolveByPos(cast_fp(pos),x))
										CalculateBones(K,x);
		}
	}
}

void CIKLimb::Create(CKinematics* K,u16 bones[4],const Fvector& toe_pos,Etype tp)
{
	m_bones[0]=bones[0];m_bones[1]=bones[1];m_bones[2]=bones[2];m_bones[3]=bones[3];
	m_toe_position.set(toe_pos);
//////////////////////////////////////////////////////////////////////
	xr_vector<Fmatrix> binds;
	K->LL_GetBindTransform(binds);
	Fmatrix XT,XS;
	XT.set(binds[bones[0]]);XT.invert();XT.mulB_43(binds[bones[1]]);
	XS.set(binds[bones[1]]);XS.invert();XS.mulB_43(binds[bones[2]]);
	Matrix T,S;XM2IM(XT,T);XM2IM(XS,S);
/////////////////////////////////////////////////////////////////////
	float lmin[7],lmax[7];
	SJointLimit* limits;
	limits=K->LL_GetData(bones[0]).IK_data.limits;
	lmin[0]= limits[0].limit.x;lmax[0]=limits[0].limit.y;
	lmin[1]= limits[1].limit.x;lmax[1]=limits[1].limit.y;
	lmin[2]= limits[2].limit.x;lmax[2]=limits[2].limit.y;

	limits=K->LL_GetData(bones[1]).IK_data.limits;
	lmin[3]= limits[1].limit.x;lmax[3]=limits[1].limit.y;

	limits=K->LL_GetData(bones[2]).IK_data.limits;
	lmin[4]= limits[0].limit.x;lmax[4]=limits[0].limit.y;
	lmin[5]= limits[1].limit.x;lmax[5]=limits[1].limit.y;
	lmin[6]= limits[2].limit.x;lmax[6]=limits[2].limit.y;
	
	switch(tp) {
	case tpRight:
		m_limb.init(T,S,ZXY,ZXY,lproj_vector,lpos_vector,lmin,lmax);
		break;
	case tpLeft:
		m_limb.init(T,S,zxY, zxY,lproj_vector,lpos_vector,lmin,lmax);
		break;
	default:NODEFAULT;
	}
	
}

void CIKLimb::Destroy()
{
	
}
void CIKLimb::Collide(CKinematics* K)
{
	m_tri=NULL;
	Fvector pos;
	K->LL_GetTransform(m_bones[2]).transform_tiny(pos,m_toe_position);
	pos.y+=peak_dist;

	collide::rq_result	R;
	CGameObject *O=(CGameObject*)K->Update_Callback_Param;
	if(g_pGameLevel->ObjectSpace.RayPick( pos, Fvector().set(0,-1,0), peak_dist, collide::rqtBoth, R, O))
	{
		if(!R.O)
		{
			//Fvector*	pVerts	= Level().ObjectSpace.GetStaticVerts();
			m_tri	= Level().ObjectSpace.GetStaticTris() + R.element;
			//normal.mknormal	(pVerts[pTri->verts[0]],pVerts[pTri->verts[1]],pVerts[pTri->verts[2]]);
			m_tri_hight=peak_dist-R.range;
			return;
		}
	}

#ifdef DEBUG
	m_tri_hight=-dInfinity;
#endif

}
void CIKLimb::GoalMatrix(Matrix &M,CKinematics *K)
{
		VERIFY(m_tri&&m_tri_hight!=-dInfinity);
		Fmatrix xm;
		xm.set(K->LL_GetTransform(m_bones[2]));
		xm.c.y+=m_tri_hight;
		Fvector*	pVerts	= Level().ObjectSpace.GetStaticVerts();
		Fvector normal;
		normal.mknormal	(pVerts[m_tri->verts[0]],pVerts[m_tri->verts[1]],pVerts[m_tri->verts[2]]);
		VERIFY(!fis_zero(normal.magnitude()));
		prg_dir_on_plane(normal,xm.j,xm.j);
		prg_dir_on_plane(normal,xm.k,xm.k);

		xm.j.normalize();
		xm.k.normalize();
		xm.i.crossproduct(xm.j,xm.k);
		Fmatrix G; G.set(K->LL_GetTransform(m_bones[0]));
		G.invert();
		G.mulB_43(xm);
		
		XM2IM(xm,M);
}
void CIKLimb::CalculateBones(CKinematics* K,const float x[])
{
		K->LL_GetTransform(m_bones[1]).setHPB(x[0],x[1],x[2]);
		K->LL_GetData(m_bones[1]).Calculate(K,&Fidentity);

		Fvector ra;K->LL_GetTransform(m_bones[2]).getHPB(ra);
		ra.y=x[3];
		K->LL_GetTransform(m_bones[2]).setHPB(ra.x,ra.y,ra.z);
		K->LL_GetData(m_bones[2]).Calculate(K,&Fidentity);
		K->LL_GetTransform(m_bones[2]).setHPB(x[4],x[5],x[6]);

		K->LL_GetTransform(m_bones[3]).setHPB(x[0],x[1],x[2]);
		K->LL_GetData(m_bones[3]).Calculate(K,&Fidentity);


}