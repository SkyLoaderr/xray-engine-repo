#include "stdafx.h"
#include "igame_level.h"
#include "xr_collide_form.h"
#include "xr_object.h"
#include "xr_area.h"
#include "x_ray.h"
#include "xrLevel.h"
#include "fmesh.h"
#include "skeletoncustom.h"
#include "frustum.h"

using namespace	Collide;
//----------------------------------------------------------------------
// Class	: CXR_CFObject
// Purpose	: stores collision form
//----------------------------------------------------------------------
ICollisionForm::ICollisionForm( CObject* _owner, ECollisionFormType tp )
{
	owner				= _owner;
	m_type				= tp;
	bv_sphere.identity	( );
}

ICollisionForm::~ICollisionForm( )
{
}

//----------------------------------------------------------------------------------
CCF_Polygonal::CCF_Polygonal(CObject* O) : ICollisionForm(O,cftObject)
{
}

BOOL CCF_Polygonal::LoadModel( CInifile* ini, const char *section )
{
	R_ASSERT			(ini);
	R_ASSERT			(section);

	// Locate file
	string256			full_path;
	LPCSTR				N = ini->r_string(section,"cform");
	if (!FS.exist(full_path, "$level$", N)) {
		Debug.fatal("Can't find cform file '%s'.",N);
	}

	// Actual load
	IReader*			f	= FS.r_open(full_path);
	hdrCFORM			H;
	f->r				(&H,sizeof(hdrCFORM));
	R_ASSERT			(CFORM_CURRENT_VERSION==H.version);

	bv_box.set			(H.aabb);
	bv_box.getsphere	(bv_sphere.P,bv_sphere.R);

	Fvector*	verts	= (Fvector*)f->pointer();
	CDB::TRI*	tris	= (CDB::TRI*)(verts+H.vertcount);
	model.build			( verts, H.vertcount, tris, H.facecount );
	Msg					("* CFORM memory usage: %dK",model.memory()/1024);
	FS.r_close			(f);

	return				TRUE;
}

BOOL CCF_Polygonal::_RayQuery( const Collide::ray_defs& Q, Collide::rq_results& R)
{
	// Convert ray into local model space
	Fvector dS, dD;
	Fmatrix temp; 
	temp.invert			(owner->XFORM());
	temp.transform_tiny	(dS,Q.start);
	temp.transform_dir	(dD,Q.dir);

	// 
	if (!bv_sphere.intersect(dS,dD))	return FALSE;

	// Query
	BOOL bHIT = FALSE;
	XRC.ray_options		(Q.flags);
	XRC.ray_query		(&model,dS,dD,Q.range);
	if (XRC.r_count()) 	{
		for (int k=0; k<XRC.r_count(); k++){
			bHIT		= TRUE;
			CDB::RESULT* I	= XRC.r_begin()+k;
			R.append_result(owner,I->range,I->id,Q.flags&CDB::OPT_ONLYNEAREST);
			if (Q.flags&CDB::OPT_ONLYFIRST) return TRUE;
		}
	}
	return bHIT;
}

void CCF_Polygonal::_BoxQuery( const Fbox& B, const Fmatrix& M, u32 flags)
{
	if ((flags&clQUERY_TOPLEVEL) || (((flags&clGET_TRIS)==0) && (flags&clGET_BOXES)))
	{
		// Return only top level
		clQueryCollision& Q = g_pGameLevel->ObjectSpace.q_result;
		Q.AddBox			(owner->XFORM(),bv_box);
	} else {
		// XForm box
		const Fmatrix&	T = owner->XFORM();
		Fmatrix			w2m,b2m;
		w2m.invert		(T);
		b2m.mul_43		(w2m,M);
		
		Fvector			bc,bd;
		Fbox			xf; 
		xf.xform		(B,b2m);
		xf.get_CD		(bc,bd);
		
		// Return actual tris
		XRC.box_query (&model, bc, bd );
		if (XRC.r_count())
		{
			clQueryCollision& Q = g_pGameLevel->ObjectSpace.q_result;
			if (flags&clQUERY_ONLYFIRST) 
			{
				Q.AddTri(T,&model.get_tris()[XRC.r_begin()->id],model.get_verts());
				return;
			} else {
				CDB::RESULT* it	=XRC.r_begin();
				CDB::RESULT* end=XRC.r_end	();
				for (; it!=end; it++)
					Q.AddTri(T,&model.get_tris() [it->id], model.get_verts());
			}
		}
	}
}

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
IC BOOL RAYvsOBB(const Fmatrix& IM, Fbox& B, const Fvector &S, const Fvector &D, float &R, BOOL bCull)
{
	// XForm world-2-local
	Fvector	SL,DL,PL;
	IM.transform_tiny	(SL,S);
	IM.transform_dir	(DL,D);

	// Actual test
	Fbox::ERP_Result rp_res = B.Pick2(SL,DL,PL);
	if ((rp_res==Fbox::rpOriginOutside)||(!bCull&&(rp_res==Fbox::rpOriginInside)))
	{
		float d = PL.distance_to_sqr(SL);
		if (d<R*R) {
			R = _sqrt(d);
			return TRUE;
		}
	}
	return FALSE;
}

CCF_Skeleton::CCF_Skeleton(CObject* O) : ICollisionForm(O,cftObject)
{
	CKinematics* K	= PKinematics(O->Visual()); VERIFY3(K,"Can't create skeleton without Kinematics.",O->cNameVisual());
	base_box.set	(K->vis.box);
	bv_box.set		(K->vis.box);
	bv_box.getsphere(bv_sphere.P,bv_sphere.R);
}

void CCF_Skeleton::BuildState()
{
	dwFrame			= Device.dwFrame;
	CKinematics* K	= PKinematics(owner->Visual());
	K->Calculate	();
	
	if (K->LL_VisibleBoneCount() != models.size())
	{
		//u64 F			= K->LL_GetBonesVisible()&((u64(1)<<u64(K->LL_BoneCount()))-1); 
		models.resize	(K->LL_VisibleBoneCount());
		base_box.set	(K->vis.box);
		bv_box.set		(K->vis.box);
		bv_box.getsphere(bv_sphere.P,bv_sphere.R);
		for (u16 i=0,idx=0; i<K->LL_BoneCount(); i++){
			if (!K->LL_GetBoneVisible(i)) continue;
			models[idx].elem_id = K->LL_GetData(i).shape.flags.is(SBoneShape::sfNoPickable)?u16(-1):i;
			idx			++;
		}
	}

	const Fmatrix &L2W		= owner->XFORM();
	Fmatrix Mbox,T,TW;
	for (xr_vector<CCF_OBB>::iterator I=models.begin(); I!=models.end(); I++) 
	{
		if (!I->valid())	continue;
		Fobb& B				= K->LL_GetBox(I->elem_id);
		Fmatrix& Mbone		= K->LL_GetTransform(I->elem_id);
		B.xform_get			(Mbox);
		T.mul_43			(Mbone,Mbox);	// model space
		TW.mul_43			(L2W,T);		// world space
		I->OBB.xform_set	(TW);
		I->IM.invert		(TW);
		I->B.set			(
							-B.m_halfsize.x,-B.m_halfsize.y,-B.m_halfsize.z,
							B.m_halfsize.x,B.m_halfsize.y,B.m_halfsize.z
							);
	}
}

void CCF_Skeleton::BuildTopLevel()
{
	dwFrameTL			= Device.dwFrame;
	IRender_Visual* K	= owner->Visual();
	Fbox& B				= K->vis.box;
	bv_box.min.average	(B.min);
	bv_box.max.average	(B.max);
	bv_box.grow			(0.05f);
	bv_sphere.P.average	(K->vis.sphere.P);
	bv_sphere.R			+= K->vis.sphere.R;
	bv_sphere.R			*= 0.5f;
}

BOOL CCF_Skeleton::_RayQuery( const Collide::ray_defs& Q, Collide::rq_results& R)
{
	if (dwFrameTL!=Device.dwFrame)			BuildTopLevel();

	// Convert ray into local model space
	Fvector dS, dD;
	Fmatrix temp; 
	temp.invert			(owner->XFORM());
	temp.transform_tiny	(dS,Q.start);
	temp.transform_dir	(dD,Q.dir);

	// 
	if (!bv_sphere.intersect(dS,dD))	return FALSE;

	if (dwFrame!=Device.dwFrame)			BuildState();

	BOOL bHIT = FALSE;
	for (xr_vector<CCF_OBB>::iterator I=models.begin(); I!=models.end(); I++) 
	{
		if (!I->valid()){
			continue;
		}
		float range		= Q.range;
		if (RAYvsOBB(I->IM,I->B,Q.start,Q.dir,range,Q.flags&CDB::OPT_CULL)) 
		{
			bHIT		= TRUE;
			R.append_result(owner,range,I->elem_id,Q.flags&CDB::OPT_ONLYNEAREST);
			if (Q.flags&CDB::OPT_ONLYFIRST) return TRUE;
		}
	}
	return bHIT;
}

void CCF_Skeleton::_BoxQuery( const Fbox& B, const Fmatrix& M, u32 flags)
{
	if ((flags&clQUERY_TOPLEVEL) || ((flags&clGET_BOXES)==0))
	{
		if (dwFrameTL!=Device.dwFrame) BuildTopLevel();
		// Return only top level
		clQueryCollision& Q = g_pGameLevel->ObjectSpace.q_result;
		Q.AddBox			(owner->XFORM(),bv_box);
	} else { 
		if (dwFrame!=Device.dwFrame) BuildState();

		// Return actual boxes
		clQueryCollision& Q = g_pGameLevel->ObjectSpace.q_result;

#pragma todo("CCF_Skeleton::_BoxQuery - Actual test BOX vs SkeletonNODE")
		for (xr_vector<CCF_OBB>::iterator I=models.begin(); I!=models.end(); I++) 
		{
			Q.AddBox(I->OBB);
		}
	}
}

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
CCF_Rigid::CCF_Rigid(CObject* O) : ICollisionForm(O,cftObject)
{
	FHierrarhyVisual* pH= dynamic_cast<FHierrarhyVisual*>(O->Visual());
	if (pH){
		models.resize	(pH->children.size());
		base_box.set	(pH->vis.box);
		bv_box.set		(pH->vis.box);
		bv_box.getsphere(bv_sphere.P,bv_sphere.R);
	}else{ 
		IRender_Visual* pV	= O->Visual();
		if (pV){
			models.resize	(1);
			base_box.set	(pV->vis.box);
			bv_box.set		(pV->vis.box);
			bv_box.getsphere(bv_sphere.P,bv_sphere.R);
		}else{
			Debug.fatal("Unsuported visual type.");
		}
	}
}

void CCF_Rigid::UpdateModel(CCF_OBB& m, Fbox& box)
{
	const Fmatrix &L2W	= owner->XFORM();

	Fobb			B;
	Fvector			c;
	box.get_CD		(c,B.m_halfsize);
	Fmatrix			T,R;
	T.translate		(c);
	R.mul_43		(L2W,T);
	B.xform_set		(R);

	m.OBB.xform_set	(R);
	m.IM.invert		(R);
	m.B.set			(
		-B.m_halfsize.x,-B.m_halfsize.y,-B.m_halfsize.z,
		B.m_halfsize.x,B.m_halfsize.y,B.m_halfsize.z
		);
}
void CCF_Rigid::BuildState()
{
	dwFrame			= Device.dwFrame;
	FHierrarhyVisual* pH= dynamic_cast<FHierrarhyVisual*>(owner->Visual());
	if (pH){
		for (u32 i=0; i<models.size(); i++)
			UpdateModel		(models[i],pH->children[i]->vis.box);
	}else{
		IRender_Visual* pV	= owner->Visual();
		UpdateModel			(models[0],pV->vis.box);
	}
}

void CCF_Rigid::BuildTopLevel()
{
	dwFrameTL			= Device.dwFrame;
	IRender_Visual* K	= owner->Visual();
	Fbox& B				= K->vis.box;
	bv_box.min.average	(B.min);
	bv_box.max.average	(B.max);
	bv_box.grow			(0.05f);
	bv_sphere.P.average	(K->vis.sphere.P);
	bv_sphere.R			+= K->vis.sphere.R;
	bv_sphere.R			*= 0.5f;
}

BOOL CCF_Rigid::_RayQuery( const Collide::ray_defs& Q, Collide::rq_results& R)
{
	if (dwFrameTL!=Device.dwFrame)			BuildTopLevel();

	// Convert ray into local model space
	Fvector dS, dD;
	Fmatrix temp; 
	temp.invert			(owner->XFORM());
	temp.transform_tiny	(dS,Q.start);
	temp.transform_dir	(dD,Q.dir);

	// 
	if (!bv_sphere.intersect(dS,dD))	return FALSE;

	if (dwFrame!=Device.dwFrame)			BuildState();

	BOOL bHIT = FALSE;
	for (xr_vector<CCF_OBB>::iterator I=models.begin(); I!=models.end(); I++){
		float range		= Q.range;
		if (RAYvsOBB(I->IM,I->B,Q.start,Q.dir,range,Q.flags&CDB::OPT_CULL)){
			bHIT		= TRUE;
			R.append_result(owner,range,int(I-models.begin()),Q.flags&CDB::OPT_ONLYNEAREST);
			if (CDB::OPT_ONLYFIRST) return TRUE;
		}
	}
	return bHIT;
}

void CCF_Rigid::_BoxQuery( const Fbox& B, const Fmatrix& M, u32 flags)
{
	if ((flags&clQUERY_TOPLEVEL) || ((flags&clGET_BOXES)==0))
	{
		if (dwFrameTL!=Device.dwFrame) BuildTopLevel();
		// Return only top level
		clQueryCollision& Q = g_pGameLevel->ObjectSpace.q_result;
		Q.AddBox			(owner->XFORM(),bv_box);
	} else { 
		if (dwFrame!=Device.dwFrame) BuildState();

		// Return actual boxes
		clQueryCollision& Q = g_pGameLevel->ObjectSpace.q_result;

#pragma todo("CCF_Rigid::_BoxQuery - Actual test BOX vs SkeletonNODE")
		for (xr_vector<CCF_OBB>::iterator I=models.begin(); I!=models.end(); I++) 
		{
			Q.AddBox(I->OBB);
		}
	}
}
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------

CCF_EventBox::CCF_EventBox( CObject* O ) : ICollisionForm(O,cftShape)
{
	Fvector A[8],B[8];
	A[0].set( -1, -1, -1);
	A[1].set( -1, -1, +1);
	A[2].set( -1, +1, +1);
	A[3].set( -1, +1, -1);
	A[4].set( +1, +1, +1);
	A[5].set( +1, +1, -1);
	A[6].set( +1, -1, +1);
	A[7].set( +1, -1, -1);

	const Fmatrix& T = O->XFORM();
	for (int i=0; i<8; i++) {
		A[i].mul(.5f);
		T.transform_tiny(B[i],A[i]);
	}
	bv_box.set		(-.5f,-.5f,-.5f,+.5f,+.5f,+.5f);
	Fvector R; R.set(bv_box.min);
	T.transform_dir	(R);
	bv_sphere.R		= R.magnitude();

	Planes[0].build(B[0],B[3],B[5]);
	Planes[1].build(B[1],B[2],B[3]);
	Planes[2].build(B[6],B[5],B[4]);
	Planes[3].build(B[4],B[2],B[1]);
	Planes[4].build(B[3],B[2],B[4]);
	Planes[5].build(B[1],B[0],B[6]);
}

BOOL CCF_EventBox::Contact(CObject* O)
{
	IRender_Visual*	V		= O->Visual();
	Fvector&		P	= V->vis.sphere.P;
	float			R	= V->vis.sphere.R;
	
	Fvector			PT;
	O->XFORM().transform_tiny(PT,P);
	
	for (int i=0; i<6; i++) {
		if (Planes[i].classify(PT)>R) return FALSE;
	}
	return TRUE;
}
BOOL CCF_EventBox::_RayQuery(const Collide::ray_defs& Q, Collide::rq_results& R)
{	return FALSE; }
void CCF_EventBox::_BoxQuery(const Fbox& B, const Fmatrix& M, u32 flags)
{   return; }
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
CCF_Shape::CCF_Shape(CObject* _owner) : ICollisionForm(_owner,cftShape)
{
}
BOOL CCF_Shape::_RayQuery(const Collide::ray_defs& Q, Collide::rq_results& R)
{	
	// Convert ray into local model space
	Fvector dS, dD;
	Fmatrix temp; 
	temp.invert			(owner->XFORM());
	temp.transform_tiny	(dS,Q.start);
	temp.transform_dir	(dD,Q.dir);

	// 
	if (!bv_sphere.intersect(dS,dD))	return FALSE;

	BOOL bHIT = FALSE;
	for (u32 el=0; el<shapes.size(); el++)
	{
		shape_def& shape= shapes[el];
		float range		= Q.range;
		switch (shape.type)
		{
		case 0: // sphere
			if (shape.data.sphere.intersect(dS,dD,range)){
				bHIT	= TRUE;
				R.append_result(owner,range,el,Q.flags&CDB::OPT_ONLYNEAREST);
				if (Q.flags&CDB::OPT_ONLYFIRST) return TRUE;
			}
			break;
		case 1: // box
			{
				Fbox				box;
				box.identity		();
				Fmatrix& B			= shape.data.ibox;
				Fvector				S1,D1,P;
				B.transform_tiny	(S1,dS);
				B.transform_dir		(D1,dD);
				Fbox::ERP_Result	rp_res 	= box.Pick2(S1,D1,P);
				if ((rp_res==Fbox::rpOriginOutside)||(!(Q.flags&CDB::OPT_CULL)&&(rp_res==Fbox::rpOriginInside))){
					float d			= P.distance_to_sqr(dS);
					if (d<range*range) {
						range		= _sqrt(d);
						bHIT		= TRUE;
						R.append_result(owner,range,el,Q.flags&CDB::OPT_ONLYNEAREST);
						if (CDB::OPT_ONLYFIRST) return TRUE;
					}
				}
			}
			break;
		}
	}
	return bHIT;
}
void CCF_Shape::_BoxQuery(const Fbox& B, const Fmatrix& M, u32 flags)
{   return; }

void CCF_Shape::add_sphere	(Fsphere& S )
{
	shapes.push_back(shape_def());
	shapes.back().type	= 0;
	shapes.back().data.sphere.set(S);
}

void CCF_Shape::add_box		(Fmatrix& B )
{
	shapes.push_back(shape_def());
	shapes.back().type	= 1;
	shapes.back().data.box.set(B);
	shapes.back().data.ibox.invert(B);
}

void CCF_Shape::ComputeBounds()
{
	bv_box.invalidate	();

	for (u32 el=0; el<shapes.size(); el++)
	{
		switch (shapes[el].type)
		{
		case 0: // sphere
			{
				Fsphere		T		= shapes[el].data.sphere;
				Fvector		P;
				P.set		(T.P);	P.sub(T.R);	bv_box.modify(P);
				P.set		(T.P);	P.add(T.R);	bv_box.modify(P);
			}
			break;
		case 1:	// box
			{
				Fvector		A,B;
				Fmatrix&	T		= shapes[el].data.box;
				
				// Build points
				A.set( -.5f, -.5f, -.5f); T.transform_tiny	(B,A); bv_box.modify(B);
				A.set( -.5f, -.5f, +.5f); T.transform_tiny	(B,A); bv_box.modify(B);
				A.set( -.5f, +.5f, +.5f); T.transform_tiny	(B,A); bv_box.modify(B);
				A.set( -.5f, +.5f, -.5f); T.transform_tiny	(B,A); bv_box.modify(B);
				A.set( +.5f, +.5f, +.5f); T.transform_tiny	(B,A); bv_box.modify(B);
				A.set( +.5f, +.5f, -.5f); T.transform_tiny	(B,A); bv_box.modify(B);
				A.set( +.5f, -.5f, +.5f); T.transform_tiny	(B,A); bv_box.modify(B);
				A.set( +.5f, -.5f, -.5f); T.transform_tiny	(B,A); bv_box.modify(B);
			}
			break;
		}
	}
	bv_box.getsphere		(bv_sphere.P,bv_sphere.R);
}

BOOL CCF_Shape::Contact		( CObject* O )
{
	// Build object-sphere in World-Space
	Fsphere			S;
	if (O->Visual()){
		O->Center		(S.P);
		S.R				= O->Radius();
	}else if (O->CFORM()){
		S = O->CFORM()->getSphere();
		O->XFORM().transform_tiny(S.P);
	}else return FALSE;
	
	// Get our matrix
	const Fmatrix& XF	= Owner()->XFORM();
	
	// Iterate
	for (u32 el=0; el<shapes.size(); el++)
	{
		switch (shapes[el].type)
		{
		case 0: // sphere
			{
				Fsphere		Q;
				Fsphere&	T		= shapes[el].data.sphere;
				XF.transform_tiny	(Q.P,T.P);
				Q.R					= T.R;
				if (S.intersect(Q))	return TRUE;
			}
			break;
		case 1:	// box
			{
				Fmatrix		Q;
				Fmatrix&	T		= shapes[el].data.box;
				Q.mul_43			( XF,T);

				// Build points
				Fvector A,B[8];
				Fplane  P;
				A.set(-.5f, -.5f, -.5f);	Q.transform_tiny(B[0],A);
				A.set(-.5f, -.5f, +.5f);	Q.transform_tiny(B[1],A);
				A.set(-.5f, +.5f, +.5f);	Q.transform_tiny(B[2],A);
				A.set(-.5f, +.5f, -.5f);	Q.transform_tiny(B[3],A);
				A.set(+.5f, +.5f, +.5f);	Q.transform_tiny(B[4],A);
				A.set(+.5f, +.5f, -.5f);	Q.transform_tiny(B[5],A);
				A.set(+.5f, -.5f, +.5f);	Q.transform_tiny(B[6],A);
				A.set(+.5f, -.5f, -.5f);	Q.transform_tiny(B[7],A);

				P.build(B[0],B[3],B[5]);	if (P.classify(S.P)>S.R) break;
				P.build(B[1],B[2],B[3]);	if (P.classify(S.P)>S.R) break;
				P.build(B[6],B[5],B[4]);	if (P.classify(S.P)>S.R) break;
				P.build(B[4],B[2],B[1]);	if (P.classify(S.P)>S.R) break;
				P.build(B[3],B[2],B[4]);	if (P.classify(S.P)>S.R) break;
				P.build(B[1],B[0],B[6]);	if (P.classify(S.P)>S.R) break;
				return TRUE;
			}
			break;
		}
	}
	return FALSE;
}
