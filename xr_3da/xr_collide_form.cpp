#include "stdafx.h"
#include "igame_level.h"
#include "xr_collide_form.h"
#include "xr_object.h"
#include "xr_area.h"
#include "x_ray.h"
#include "xrLevel.h"
#include "fmesh.h"
#include "bodyinstance.h"
#include "frustum.h"

using namespace	Collide;

//----------------------------------------------------------------------
// Class	: CXR_CFObject
// Purpose	: stores collision form
//----------------------------------------------------------------------
ICollisionForm::ICollisionForm( CObject* _owner )
{
	owner				= _owner;
	bv_sphere.identity	( );
}

ICollisionForm::~ICollisionForm( )
{
}

//----------------------------------------------------------------------------------
CCF_Polygonal::CCF_Polygonal(CObject* O) : ICollisionForm(O)
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

BOOL CCF_Polygonal::_RayTest( RayQuery& Q)
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
	XRC.ray_query		(&model,dS,dD,Q.range);
	if (XRC.r_count()) 	{
		CDB::RESULT* R	= XRC.r_begin();
		if (R->range < Q.range) {
			Q.range		= R->range;
			Q.element	= R->id;
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CCF_Polygonal::_RayPick( RayPickResult& result, const Fvector& _start, const Fvector& _dir, float _range, u32 _flags)
{
	// Convert ray into local model space
	Fvector dS, dD;
	Fmatrix temp; 
	temp.invert			(owner->XFORM());
	temp.transform_tiny	(dS,_start);
	temp.transform_dir	(dD,_dir);

	// 
	if (!bv_sphere.intersect(dS,dD))	return FALSE;

	// Query
	BOOL bHIT = FALSE;
	XRC.ray_query		(&model,dS,dD,_range);
	if (XRC.r_count()) 	{
		for (int k=0; k<XRC.r_count(); k++){
			bHIT		= TRUE;
			CDB::RESULT* R	= XRC.r_begin()+k;
			result.AppendResult(R->range,R->id,_flags&CDB::OPT_ONLYNEAREST);
			if (_flags&CDB::OPT_ONLYFIRST) return TRUE;
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
				Q.AddTri(T,&model.get_tris()[XRC.r_begin()->id]);
				return;
			} else {
				CDB::RESULT* it	=XRC.r_begin();
				CDB::RESULT* end=XRC.r_end	();
				for (; it!=end; it++)
					Q.AddTri(T,&model.get_tris() [it->id]);
			}
		}
	}
}

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
IC BOOL RAYvsOBB(CCF_OBB &B, const Fvector &S, const Fvector &D, float &R)
{
	// XForm world-2-local
	Fvector	SL,DL,PL;
	B.IM.transform_tiny	(SL,S);
	B.IM.transform_dir	(DL,D);

	// Actual test
	if (B.B.Pick2(SL,DL,PL))  
	{
		float d = PL.distance_to_sqr(SL);
		if (d<R*R) {
			R = _sqrt(d);
			return true;
		}
	}
	return false;
}

CCF_Skeleton::CCF_Skeleton(CObject* O) : ICollisionForm(O)
{
	CKinematics* K	= PKinematics(O->Visual());
	model.resize	(K->LL_BoneCount());

	base_box.set	(K->vis.box);
	bv_box.set		(K->vis.box);
	bv_box.getsphere(bv_sphere.P,bv_sphere.R);
}

void CCF_Skeleton::BuildState()
{
	dwFrame			= Device.dwFrame;
	CKinematics* K	= PKinematics(owner->Visual());
	K->Calculate	();
	
	const Fmatrix &L2W	= owner->XFORM();
	Fmatrix Mbox,T,TW;
	for (u16 i=0; i<model.size(); i++)
	{
		Fobb& B				=	K->LL_GetBox(i);
		Fmatrix& Mbone		=	K->LL_GetTransform(i);
		B.xform_get				(Mbox);
		T.mul_43				(Mbone,Mbox);	// model space
		TW.mul_43				(L2W,T);		// world space
		model[i].OBB.xform_set	(TW);
		model[i].IM.invert		(TW);
		model[i].B.set			(
			-B.m_halfsize.x,-B.m_halfsize.y,-B.m_halfsize.z,
			B.m_halfsize.x,B.m_halfsize.y,B.m_halfsize.z
			);
	}
}

void CCF_Skeleton::BuildTopLevel()
{
	dwFrameTL			= Device.dwFrame;
	IRender_Visual* K			= owner->Visual();
	Fbox& B				= K->vis.box;
	bv_box.min.average	(B.min);
	bv_box.max.average	(B.max);
	bv_box.grow			(0.05f);
	bv_sphere.P.average	(K->vis.sphere.P);
	bv_sphere.R			+= K->vis.sphere.R;
	bv_sphere.R			*= 0.5f;
}

BOOL CCF_Skeleton::_RayTest( RayQuery& Q)
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
	for (xr_vector<CCF_OBB>::iterator I=model.begin(); I!=model.end(); I++) 
	{
		if (RAYvsOBB(*I,Q.start,Q.dir,Q.range)) 
		{
			bHIT = TRUE;
			Q.element = I-model.begin();
		}
	}
	return bHIT;
}

BOOL CCF_Skeleton::_RayPick( RayPickResult& result, const Fvector& _start, const Fvector& _dir, float _range, u32 _flags)
{
	if (dwFrameTL!=Device.dwFrame)			BuildTopLevel();

	// Convert ray into local model space
	Fvector dS, dD;
	Fmatrix temp; 
	temp.invert			(owner->XFORM());
	temp.transform_tiny	(dS,_start);
	temp.transform_dir	(dD,_dir);

	// 
	if (!bv_sphere.intersect(dS,dD))	return FALSE;

	if (dwFrame!=Device.dwFrame)			BuildState();

	BOOL bHIT = FALSE;
	for (xr_vector<CCF_OBB>::iterator I=model.begin(); I!=model.end(); I++) 
	{
		float range		= _range;
		if (RAYvsOBB(*I,_start,_dir,range)) 
		{
			bHIT		= TRUE;
			result.AppendResult(range,I-model.begin(),_flags&CDB::OPT_ONLYNEAREST);
			if (_flags&CDB::OPT_ONLYFIRST) return TRUE;
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
		for (xr_vector<CCF_OBB>::iterator I=model.begin(); I!=model.end(); I++) 
		{
			Q.AddBox(I->OBB);
		}
	}
}

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
CCF_Rigid::CCF_Rigid(CObject* O) : ICollisionForm(O)
{
	FHierrarhyVisual* pH= dynamic_cast<FHierrarhyVisual*>(O->Visual());
	if (pH){
		model.resize	(pH->children.size());
		base_box.set	(pH->vis.box);
		bv_box.set		(pH->vis.box);
		bv_box.getsphere(bv_sphere.P,bv_sphere.R);
	}else{ 
		IRender_Visual* pV	= O->Visual();
		if (pV){
			model.resize	(1);
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
		for (u32 i=0; i<model.size(); i++)
			UpdateModel		(model[i],pH->children[i]->vis.box);
	}else{
		IRender_Visual* pV	= owner->Visual();
		UpdateModel			(model[0],pV->vis.box);
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

BOOL CCF_Rigid::_RayTest( RayQuery& Q)
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
	for (xr_vector<CCF_OBB>::iterator I=model.begin(); I!=model.end(); I++) 
	{
		if (RAYvsOBB(*I,Q.start,Q.dir,Q.range)) 
		{
			bHIT = TRUE;
			Q.element = I-model.begin();
		}
	}
	return bHIT;
}

BOOL CCF_Rigid::_RayPick( RayPickResult& result, const Fvector& _start, const Fvector& _dir, float _range, u32 _flags)
{
	if (dwFrameTL!=Device.dwFrame)			BuildTopLevel();

	// Convert ray into local model space
	Fvector dS, dD;
	Fmatrix temp; 
	temp.invert			(owner->XFORM());
	temp.transform_tiny	(dS,_start);
	temp.transform_dir	(dD,_dir);

	// 
	if (!bv_sphere.intersect(dS,dD))	return FALSE;

	if (dwFrame!=Device.dwFrame)			BuildState();

	BOOL bHIT = FALSE;
	for (xr_vector<CCF_OBB>::iterator I=model.begin(); I!=model.end(); I++) 
	{
		float range		= _range;
		if (RAYvsOBB(*I,_start,_dir,range)) 
		{
			bHIT		= TRUE;
			result.AppendResult(range,I-model.begin(),_flags&CDB::OPT_ONLYNEAREST);
			if (_flags&CDB::OPT_ONLYFIRST) return TRUE;
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
		for (xr_vector<CCF_OBB>::iterator I=model.begin(); I!=model.end(); I++) 
		{
			Q.AddBox(I->OBB);
		}
	}
}
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------

CCF_EventBox::CCF_EventBox( CObject* O ) : ICollisionForm(O)
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
BOOL CCF_EventBox::_RayTest(RayQuery& Q)
{	return FALSE; }
BOOL CCF_EventBox::_RayPick( RayPickResult& R, const Fvector& _start, const Fvector& _dir, float _range, u32 _flags)
{	return FALSE; }
void CCF_EventBox::_BoxQuery(const Fbox& B, const Fmatrix& M, u32 flags)
{   return; }
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
CCF_Shape::CCF_Shape(CObject* _owner) : ICollisionForm(_owner)
{
}
BOOL CCF_Shape::_RayTest(RayQuery& Q)
{	return FALSE; }
BOOL CCF_Shape::_RayPick( RayPickResult& R, const Fvector& _start, const Fvector& _dir, float _range, u32 _flags)
{	return FALSE; }
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
