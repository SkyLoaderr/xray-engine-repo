#include "stdafx.h"
#include "IKLimb.h"
#include "../../SkeletonCustom.h"
#include "../ode_include.h"
#include "../MathUtils.h"
#include "../GameObject.h"
#include "../Level.h"
#include "../game_object_space.h"
#ifdef DEBUG
#include "../PHDebug.h"
#endif
const Matrix	IKLocalJoint	={0,0,1,0,  -1,0,0,0, 0,-1,0,0, 0,0,0,1};//. in XGlobal
const Fmatrix	XLocalJoint		={0,-1,0,0, -1,0,0,0, 0,0,1,0,  0,0,0,1};

const Fmatrix	xm2im			={0,0,1,0,	0,1,0,0, 1,0,0,0, 0,0,0,1};

const Fvector	xgproj_axis		={0,1,0};
const Fvector	xgpos_axis		={0,0,1};

const Fvector	xlproj_axis		={1,0,0};
const Fvector	xlpos_axis		={0,0,1};
typedef float IVektor [3];

const IVektor	lproj_vector	={0,0,1};
const IVektor	lpos_vector		={-1,0,0};

const IVektor	gproj_vector	={0,0,1};//. in XGlobal
const IVektor	gpos_vector		={1,0,0};
const float		peak_dist		=1.f	;
struct SCalculateData
{
	float	const	*m_angles	;
	CKinematics		*m_K		;
	CIKLimb			*m_limb		;
#ifdef DEBUG
	Fmatrix	const	*obj		;
#endif
};
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
void XM_IM(const Fmatrix	&XM,Fmatrix	&IM)
{
	IM.mul_43(xm2im,XM);
}
void XM_IM(const Fmatrix	&XM,Matrix	&IM)
{
	//((Fmatrix*)(&IM))->mul_43(xm2im,XM);
	XM_IM(XM,*((Fmatrix*)(&IM)));
}
void IM_XM(const Matrix	&IM,Fmatrix	&XM)
{
	XM.mul_43(xm2im,*((Fmatrix*)(&IM)));
}

void XM2IM(const Fmatrix	&XM,Fmatrix	&IM)
{
	//IM=xm2im*XM*xm2im^-1
	Fmatrix tmp;
	tmp.mul_43(xm2im,XM);
	IM.mul_43(tmp,xm2im);
}
void XM2IM(const Fmatrix &XM,Matrix &IM)
{
	XM2IM(XM,*((Fmatrix*)(&IM)));
}
void IM2XM(const Matrix &IM,Fmatrix &XM)
{
	XM2IM(*((Fmatrix*)(&IM)),XM);
}
void XV2IV(const Fvector	&XV,IVektor &IV)
{
	xm2im.transform_dir(cast_fv(IV),XV);
}

void IV2XV(const IVektor &IV,Fvector	&XV)
{
	xm2im.transform_dir(XV),cast_fv(IV);
}
void CIKLimb::Calculate(CKinematics* K,const Fmatrix &obj)
{
#ifdef DEBUG
	dbg_calculate_data->obj=&obj;
#endif
	Collide(K,obj);
	if(m_tri)
	{
		Matrix		m	;
		GoalMatrix	(m,K,obj)	;
		

		//Fmatrix bind_knee;bind_knee.mul_43(K->LL_GetTransform(K->LL_GetData(m_bones[1]).GetParentID()),K->LL_GetData(m_bones[1]).bind_transform);
		//hip.invert();

		//Fmatrix T;
		//Fmatrix ibind_knee;
		//ibind_knee.invert(bind_knee);
		//T.mul_43(ibind_knee,K->LL_GetTransform(m_bones[1]));
		//Fvector a;T.getXYZ(a);
		//Fvector sp;sp.set(T.c);
		//T.setXYZ(a.x,0,a.z);
		//T.c.set(sp);
		//bind_knee.mulB_43(T);
		//T.mul_43(Fmatrix().invert(K->LL_GetTransform(K->LL_GetData(m_bones[1]).GetParentID())),bind_knee);
		//
		//Matrix IT;
		//XM2IM(T,IT);
		//m_limb.SetTMatrix(IT);
		if(m_limb.SetGoal(m,FALSE))
		{
			Fmatrix hip;
			CBoneData& BD=K->LL_GetData(m_bones[0]);
			hip.mul_43(K->LL_GetTransform(BD.GetParentID()),BD.bind_transform);
			hip.invert();
			float x[7];
			Fvector pos;
			pos.set(K->LL_GetTransform(m_bones[1]).c);
			hip.transform_tiny(pos);
			xm2im.transform_tiny(pos);
			if(m_limb.SolveByPos(cast_fp(pos),x))
										CalculateBones(K,x);
		}
	}
}

void CIKLimb::Create(CKinematics* K,u16 bones[4],const Fvector& toe_pos,Etype tp)
{
#ifdef DEBUG
	dbg_calculate_data=xr_new<SCalculateData>();
#endif
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
	lmin[0]=-limits[0].limit.y;lmax[0]=-limits[0].limit.x;
	lmin[1]=-limits[1].limit.y;lmax[1]=-limits[1].limit.x;
	lmin[2]=-limits[2].limit.y;lmax[2]=-limits[2].limit.x;

	limits=K->LL_GetData(bones[1]).IK_data.limits;
	lmin[3]= -limits[1].limit.y;lmax[3]=-limits[1].limit.x;

	limits=K->LL_GetData(bones[2]).IK_data.limits;
	lmin[4]=-limits[0].limit.y;lmax[4]=-limits[0].limit.x;
	lmin[5]=-limits[1].limit.y;lmax[5]=-limits[1].limit.x;
	lmin[6]=-limits[2].limit.y;lmax[6]=-limits[2].limit.x;
	
	switch(tp) {
	case tpRight:
		m_limb.init(T,S,ZXY,ZXY,gproj_vector,gpos_vector,lmin,lmax);
		break;
	case tpLeft:
		m_limb.init(T,S,ZXY, ZXY,gproj_vector,gpos_vector,lmin,lmax);
		break;
	default:NODEFAULT;
	}
	
}

void CIKLimb::Destroy()
{
#ifdef DEBUG
	xr_delete(dbg_calculate_data);
#endif
}
void CIKLimb::Collide(CKinematics* K,const Fmatrix &obj)
{
	m_tri=NULL;
	Fvector pos;
	K->LL_GetTransform(m_bones[2]).transform_tiny(pos,m_toe_position);//!!
	obj.transform_tiny(pos);
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
void CIKLimb::GoalMatrix(Matrix &M,CKinematics *K,const Fmatrix &obj)
{
		VERIFY(m_tri&&m_tri_hight!=-dInfinity);

		Fvector*	pVerts	= Level().ObjectSpace.GetStaticVerts();
		Fvector normal;
		normal.mknormal	(pVerts[m_tri->verts[0]],pVerts[m_tri->verts[1]],pVerts[m_tri->verts[2]]);
		VERIFY(!fis_zero(normal.magnitude()));

		Fmatrix iobj;iobj.invert(obj);iobj.transform_dir(normal);

		Fmatrix xm;xm.set(K->LL_GetTransform(m_bones[2]));

		Fvector dbg;
		dbg.set(Fvector().mul(normal,normal.y*m_tri_hight
			-normal.dotproduct(xm.i)*m_toe_position.x
			-normal.dotproduct(xm.j)*m_toe_position.y
			-normal.dotproduct(xm.k)*m_toe_position.z-m_toe_position.x
			));
		
		normal.invert();
		Fvector ax;ax.crossproduct(normal,xm.i);
		float s=ax.magnitude();
		if(!fis_zero(s))
		{
			ax.mul(1.f/s);

			xm.mulA_43(Fmatrix().rotation(ax,asinf(-s)));
		}

		Fvector otri;iobj.transform_tiny(otri,pVerts[m_tri->verts[0]]);
		float tp=normal.dotproduct(otri);
		Fvector add;
		add.set(Fvector().mul(normal,-m_toe_position.x+tp-xm.c.dotproduct(normal)));

		xm.c.add(add);
		
	
		Fmatrix H;
		CBoneData& bd=K->LL_GetData(m_bones[0]);
		H.set(bd.bind_transform);
	
		H.mulA_43(K->LL_GetTransform(bd.GetParentID()));
		H.c.set(K->LL_GetTransform(m_bones[0]).c);

	
#ifdef DEBUG
		if(ph_dbg_draw_mask.test(phDbgIKAnimGoalOnly))	xm.set(K->LL_GetTransform(m_bones[2]));
		if(ph_dbg_draw_mask.test(phDbgDrawIKGoal))
		{
			Fmatrix DBGG;
			DBGG.mul_43(obj,xm);
			DBG_DrawMatrix(DBGG,0.2f);

			
			DBGG.mul_43(obj,H);
			DBG_DrawMatrix(DBGG,0.2f);
		}
#endif
		H.invert();
		Fmatrix G; 
		G.mul_43(H,xm);
		XM2IM(G,M);
}
void CIKLimb::CalculateBones(CKinematics* K,const float x[])
{
#ifdef DEBUG
		SCalculateData &cd=*dbg_calculate_data;
#else
		SCalculateData cd	;
#endif
		
		cd.m_angles		=	x		;
		cd.m_K			=	K		;
		cd.m_limb		=	this	;
		K->LL_GetBoneInstance(m_bones[0]).set_callback(bctPhysics,BonesCallback0,&cd);
		K->LL_GetBoneInstance(m_bones[1]).set_callback(bctPhysics,BonesCallback1,&cd);
		K->LL_GetBoneInstance(m_bones[2]).set_callback(bctPhysics,BonesCallback2,&cd);
		K->LL_GetBoneInstance(m_bones[0]).Callback_overwrite=TRUE;
		K->LL_GetBoneInstance(m_bones[1]).Callback_overwrite=TRUE;
		K->LL_GetBoneInstance(m_bones[2]).Callback_overwrite=TRUE;

		
		CBoneData& BD=K->LL_GetData(m_bones[0]);
		Fmatrix t;t.set(K->LL_GetTransform(BD.GetParentID()));
		//t.mulB_43(BD.bind_transform);
		BD.Calculate(K,&t);
		//Fvector tmp_pos;
		//{
		//	Fmatrix &bm=K->LL_GetTransform(m_bones[0]);
		//	tmp_pos.set(bm.c);


		//	bm.setXYZ(-x[2],-x[1],-x[0]);
		//	bm.c.set(tmp_pos);
		//	CBoneData& BD=K->LL_GetData(m_bones[0]);
		//	Fmatrix parent;parent.set(K->LL_GetTransform(BD.GetParentID()));
		//	parent.invert();
		//	BD.Calculate(K,);

		//}
		//{
		//	Fmatrix &bm=K->LL_GetTransform(m_bones[1]);
		//	Fvector ra;bm.getXYZ(ra);
		//	ra.y=-x[3];
		//	tmp_pos.set(bm.c);
		//	bm.setXYZ(ra.x,ra.y,ra.z);
		//	bm.c.set(tmp_pos);
		//	CBoneData& BD=K->LL_GetData(m_bones[1]);
		//	BD.Calculate(K,&K->LL_GetTransform(BD.GetParentID()));

		//}
		//{
		//	Fmatrix &bm=K->LL_GetTransform(m_bones[2]);
		//	tmp_pos.set(bm.c);
		//	bm.setXYZ(-x[6],-x[5],-x[4]);
		//	bm.c.set(tmp_pos);
		//	CBoneData& BD=K->LL_GetData(m_bones[2]);
		//	BD.Calculate(K,&K->LL_GetTransform(BD.GetParentID()));

		//}
		K->LL_GetBoneInstance(m_bones[0]).set_callback(bctPhysics,NULL,NULL);
		K->LL_GetBoneInstance(m_bones[1]).set_callback(bctPhysics,NULL,NULL);
		K->LL_GetBoneInstance(m_bones[2]).set_callback(bctPhysics,NULL,NULL);
		K->LL_GetBoneInstance(m_bones[0]).Callback_overwrite=FALSE;
		K->LL_GetBoneInstance(m_bones[1]).Callback_overwrite=FALSE;
		K->LL_GetBoneInstance(m_bones[2]).Callback_overwrite=FALSE;

}

void 	CIKLimb::BonesCallback0				(CBoneInstance* B)
{
	SCalculateData* D=(SCalculateData*)B->Callback_Param;
	CIKLimb*		L=D->m_limb							;
	
	float	const	*x=D->m_angles						;
	

	Fmatrix 	bm;
	//tmp_pos		.set(bm.c)						;
	//Fvector		ra;B->mTransform.getXYZ(ra)			;
	Matrix tbm;
	float t[3];
	t[0] = x[2]; t[1] = x[1]; t[2] = x[0]			;
	EulerEval(ZXY,t,tbm)							;
	IM2XM(tbm,bm)									;
	Fmatrix cmp_save;cmp_save.set(B->mTransform)	;
	CKinematics	*K=D->m_K;
	CBoneData& BD=K->LL_GetData(L->m_bones[0]);
	B->mTransform.mul_43(K->LL_GetTransform(BD.GetParentID()),BD.bind_transform);
#ifdef DEBUG

	if(ph_dbg_draw_mask.test(phDbgDrawIKGoal))
	{
		Fmatrix DBGG;
		DBGG.mul_43(*D->obj,B->mTransform);
		DBG_DrawMatrix(DBGG,0.3f);
	}
#endif
	B->mTransform.mulB_43(bm)		;
#ifdef DEBUG
	if(ph_dbg_draw_mask.test(phDbgDrawIKGoal))
	{
		Fmatrix DBGG;
		DBGG.mul_43(*D->obj,B->mTransform);
		DBG_DrawMatrix(DBGG,0.3f);
	}
#endif
	Fmatrix cmp_savei;cmp_savei.invert(cmp_save);
	Fmatrix dif;dif.mul_43(cmp_savei,B->mTransform);

	
}
void 	CIKLimb::BonesCallback1				(CBoneInstance* B)
{
	SCalculateData* D=(SCalculateData*)B->Callback_Param;
	CIKLimb*		L=D->m_limb							;

	float	const	*x=D->m_angles						;
	
	
	Fmatrix 	bm;//B->mTransform					;
	bm.rotateY(x[3])								;
	CKinematics	*K=D->m_K;
	CBoneData& BD=K->LL_GetData(L->m_bones[1]);

	Fmatrix cmp_save;cmp_save.set(B->mTransform)	;
	B->mTransform.mul_43(K->LL_GetTransform(BD.GetParentID()),BD.bind_transform);

	Fmatrix dd;dd.mul_43(Fmatrix().invert(B->mTransform),cmp_save);
	Fvector a;dd.getXYZ(a);
	B->mTransform.mulB_43(bm)		;

	Fmatrix cmp_savei;cmp_savei.invert(cmp_save);
	Fmatrix dif;dif.mul_43(cmp_savei,B->mTransform);

	
}
void 	CIKLimb::BonesCallback2				(CBoneInstance* B)
{
	SCalculateData* D=(SCalculateData*)B->Callback_Param;
	CIKLimb*		L=D->m_limb							;
	float	const	*x=D->m_angles						;
	{
		Fmatrix 	bm;//=B->mTransform					;
		//tmp_pos		.set(bm.c)						;
	//	Fvector		ra;B->mTransform.getXYZ(ra)			;
	//	bm			.setXYZ(-x[4],-x[5],-x[6])				;
		Matrix tbm;
		float t[3];
		
		 t[0] = x[6]; t[1] = x[5]; t[2] = x[4]			;
		EulerEval(ZXY,t,tbm)							;
		IM2XM(tbm,bm)									;
		CKinematics	*K=D->m_K;
		CBoneData& BD=K->LL_GetData(L->m_bones[2])		;
		Fmatrix start;start.mul_43(K->LL_GetTransform(BD.GetParentID()),BD.bind_transform);
		Fmatrix inv_start;inv_start.invert(start);
		Fmatrix dif;dif.mul_43(inv_start,B->mTransform);
		Fmatrix ikdif;
		Fvector a;
		dif.getXYZ(a);
		XM2IM(dif,ikdif);

		//B->mTransform.mul_43(K->LL_GetTransform(BD.GetParentID()),BD.bind_transform);
		//B->mTransform.mulB_43(bm)						;
		
		B->mTransform.mul_43(start,bm);
#ifdef DEBUG

		if(ph_dbg_draw_mask.test(phDbgDrawIKGoal))
		{
			Fmatrix DBGG;
			DBGG.mul_43(*D->obj,B->mTransform);
			DBG_DrawMatrix(DBGG,0.3f);
			
			
			DBGG.mul_43(*D->obj,start);
			DBG_DrawMatrix(DBGG,0.3f);

		}
#endif
		//bm.c		.set(tmp_pos)						;
	}
}