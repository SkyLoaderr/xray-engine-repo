#include "stdafx.h"
#include "xr_collide_form.h"
#include "xr_object.h"
#include "xr_ini.h"
#include "xr_creator.h"
#include "xr_area.h"
#include "cl_collector.h"
#include "x_ray.h"
#include "xrLevel.h"
#include "fmesh.h"
#include "bodyinstance.h"

using namespace	Collide;

//----------------------------------------------------------------------
// Class	: CXR_CFObject
// Purpose	: stores collision form
//----------------------------------------------------------------------
CCFModel::CCFModel( CObject* _owner )
{
	owner				= _owner;
	Sphere.identity		( );
	last_rect.null		( );
	enabled_prev=enabled= true;
}

CCFModel::~CCFModel( )
{
	R_ASSERT(pCreator);
	pCreator->ObjectSpace.Object_Unregister(owner);
}

void CCFModel::OnMove( )
{
	VERIFY( pCreator );
	VERIFY( owner );

	Fvector X; s_box.getcenter(X);
	Sphere.P.set(owner->Position());
	Sphere.P.add(X);
	pCreator->ObjectSpace.Object_Move(owner);
}
//----------------------------------------------------------------------------------
CCF_Polygonal::CCF_Polygonal(CObject* O) : CCFModel(O)
{

}

BOOL CCF_Polygonal::LoadModel( CInifile* ini, const char *section )
{
	R_ASSERT			(ini);
	R_ASSERT			(section);

	// Locate file
	FILE_NAME			full_path;
	LPCSTR				N = ini->ReadSTRING(section,"cform");
	if (!Engine.FS.Exist(full_path, Path.Current, N))
		if (!Engine.FS.Exist(full_path, Path.CForms, N)){
			Msg("Can't find cform file '%s'.",N);
			THROW;
		}

	// Actual load
	CStream*			f	= Engine.FS.Open(full_path);
	hdrCFORM			H;
	f->Read				(&H,sizeof(hdrCFORM));
	R_ASSERT			(CFORM_CURRENT_VERSION==H.version);

	H.aabb.getcenter	(Sphere.P);
	Sphere.R			 = H.aabb.getradius	();
	s_box.set			(H.aabb);

	Fvector*	verts	= (Fvector*)f->Pointer();
	CDB::TRI*	tris	= (CDB::TRI*)(verts+H.vertcount);
	model.build			( verts, H.vertcount, tris, H.facecount );
	Msg("* CFORM memory usage: %dK",model.memory()/1024);
	Engine.FS.Close		(f);

	return				TRUE;
}

BOOL CCF_Polygonal::_clRayTest( RayQuery& Q)
{
	if (!Sphere.intersect(Q.start,Q.dir))	return FALSE;
	
	// Convert ray into local model space
	Fvector dS, dD;
	Fmatrix temp; 
	temp.invert			(owner->clXFORM());
	temp.transform_tiny	(dS,Q.start);
	temp.transform_dir	(dD,Q.dir);
	
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

BOOL CCF_Polygonal::_svRayTest( RayQuery& Q)
{
	if (!Sphere.intersect(Q.start,Q.dir))	return FALSE;
	
	// Convert ray into local model space
	Fvector dS, dD;
	Fmatrix temp; 
	temp.invert			(owner->svXFORM());
	temp.transform_tiny	(dS,Q.start);
	temp.transform_dir	(dD,Q.dir);
	
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

void CCF_Polygonal::_BoxQuery( const Fbox& B, const Fmatrix& M, u32 flags)
{
	if ((flags&clQUERY_TOPLEVEL) || (((flags&clGET_TRIS)==0) && (flags&clGET_BOXES)))
	{
		// Return only top level
		clQueryCollision& Q = pCreator->ObjectSpace.q_result;
		Q.AddBox			(owner->svXFORM(),s_box);
	} else {
		// XForm box
		const Fmatrix&	T = owner->svXFORM();
		Fmatrix		w2m,b2m;
		w2m.invert	(T);
		b2m.mul_43	(w2m,M);
		
		Fvector		bc,bd;
		Fbox		xf; 
		xf.xform	(B,b2m);
		xf.get_CD	(bc,bd);
		
		// Return actual tris
		XRC.box_query (&model, bc, bd );
		if (XRC.r_count())
		{
			clQueryCollision& Q = pCreator->ObjectSpace.q_result;
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
IC BOOL RAYvsOBB(CCF_Skeleton::xOBB &B, Fvector &S, Fvector &D, float &R)
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
			R = sqrtf(d);
			return true;
		}
	}
	return false;
}

CCF_Skeleton::CCF_Skeleton(CObject* O) : CCFModel(O)
{
	CKinematics* K	= PKinematics(O->Visual());
	model.resize	(K->LL_BoneCount());

	base_box.set	(K->bv_BBox);
	s_box.set		(K->bv_BBox);
	Sphere.R		= K->bv_Radius;
}

void CCF_Skeleton::BuildState()
{
	dwFrame			= Device.dwFrame;
	CKinematics* K	= PKinematics(owner->Visual());
	K->Calculate	();
	
	const Fmatrix &L2W	= owner->clXFORM();
	Fmatrix Mbox,T,TW;
	for (u32 i=0; i<model.size(); i++)
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
	CVisual* K			= owner->Visual();
	Fbox& B				= K->bv_BBox;
	s_box.min.average	(B.min);
	s_box.max.average	(B.max);
	s_box.grow			(0.05f);

	owner->clCenter		(Sphere.P);
	Sphere.R			+= K->bv_Radius;
	Sphere.R			*= 0.5f;
}

BOOL CCF_Skeleton::_svRayTest( RayQuery& Q)
{
	if (dwFrameTL!=Device.dwFrame)			BuildTopLevel();
	if (!Sphere.intersect(Q.start,Q.dir))	return FALSE;

	if (dwFrame!=Device.dwFrame)			BuildState();

	BOOL bHIT = FALSE;
	for (vector<xOBB>::iterator I=model.begin(); I!=model.end(); I++) 
	{
		if (RAYvsOBB(*I,Q.start,Q.dir,Q.range)) 
		{
			bHIT = TRUE;
			Q.element = I-model.begin();
		}
	}
	return bHIT;
}
BOOL CCF_Skeleton::_clRayTest( RayQuery& Q)
{
#pragma todo("CCF_Skeleton::_clRayTest")
	return _svRayTest(Q);
}

void CCF_Skeleton::_BoxQuery( const Fbox& B, const Fmatrix& M, u32 flags)
{
	if ((flags&clQUERY_TOPLEVEL) || ((flags&clGET_BOXES)==0))
	{
		if (dwFrameTL!=Device.dwFrame) BuildTopLevel();
		// Return only top level
		clQueryCollision& Q = pCreator->ObjectSpace.q_result;
		Q.AddBox			(owner->svXFORM(),s_box);
	} else { 
		if (dwFrame!=Device.dwFrame) BuildState();

		// Return actual boxes
		clQueryCollision& Q = pCreator->ObjectSpace.q_result;

#pragma todo("CCF_Skeleton::_BoxQuery - Actual test BOX vs SkeletonNODE")
		for (vector<xOBB>::iterator I=model.begin(); I!=model.end(); I++) 
		{
			Q.AddBox(I->OBB);
		}
	}
}
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
CCF_EventBox::CCF_EventBox( CObject* O ) : CCFModel(O)
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

	const Fmatrix& T = O->svXFORM();
	for (int i=0; i<8; i++) {
		A[i].mul(.5f);
		T.transform_tiny(B[i],A[i]);
	}
	s_box.set		(-.5f,-.5f,-.5f,+.5f,+.5f,+.5f);
	Fvector R; R.set(s_box.min);
	T.transform_dir	(R);
	Sphere.R		= R.magnitude();

	Planes[0].build(B[0],B[3],B[5]);
	Planes[1].build(B[1],B[2],B[3]);
	Planes[2].build(B[6],B[5],B[4]);
	Planes[3].build(B[4],B[2],B[1]);
	Planes[4].build(B[3],B[2],B[4]);
	Planes[5].build(B[1],B[0],B[6]);
}

BOOL CCF_EventBox::Contact(CObject* O)
{
	CVisual*	V = O->Visual();
	Fvector&		P = V->bv_Position;
	float			R = V->bv_Radius;
	
	Fvector			PT;
	O->svXFORM().transform_tiny(PT,P);
	
	for (int i=0; i<6; i++) {
		if (Planes[i].classify(PT)>R) return FALSE;
	}
	return TRUE;
}
BOOL CCF_EventBox::_svRayTest(RayQuery& Q)
{	return FALSE; }
BOOL CCF_EventBox::_clRayTest(RayQuery& Q)
{	return FALSE; }
void CCF_EventBox::_BoxQuery(const Fbox& B, const Fmatrix& M, u32 flags)
{   return; }
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
CCF_Shape::CCF_Shape(CObject* _owner) : CCFModel(_owner)
{
}
BOOL CCF_Shape::_svRayTest(RayQuery& Q)
{	return TRUE; }
BOOL CCF_Shape::_clRayTest(RayQuery& Q)
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
	s_box.invalidate	();

	for (u32 el=0; el<shapes.size(); el++)
	{
		switch (shapes[el].type)
		{
		case 0: // sphere
			{
				Fsphere		T		= shapes[el].data.sphere;
				Fvector		P;
				P.set		(T.P);	P.sub(T.R);	s_box.modify(P);
				P.set		(T.P);	P.add(T.R);	s_box.modify(P);
			}
			break;
		case 1:	// box
			{
				Fvector		A,B;
				Fmatrix&	T		= shapes[el].data.box;
				
				// Build points
				A.set( -.5f, -.5f, -.5f); T.transform_tiny	(B,A); s_box.modify(B);
				A.set( -.5f, -.5f, +.5f); T.transform_tiny	(B,A); s_box.modify(B);
				A.set( -.5f, +.5f, +.5f); T.transform_tiny	(B,A); s_box.modify(B);
				A.set( -.5f, +.5f, -.5f); T.transform_tiny	(B,A); s_box.modify(B);
				A.set( +.5f, +.5f, +.5f); T.transform_tiny	(B,A); s_box.modify(B);
				A.set( +.5f, +.5f, -.5f); T.transform_tiny	(B,A); s_box.modify(B);
				A.set( +.5f, -.5f, +.5f); T.transform_tiny	(B,A); s_box.modify(B);
				A.set( +.5f, -.5f, -.5f); T.transform_tiny	(B,A); s_box.modify(B);
			}
			break;
		}
	}
	s_box.getsphere(Sphere.P,Sphere.R);
}

BOOL CCF_Shape::Contact		( CObject* O )
{
	// Build object-sphere
	Fsphere			S;
	CVisual* V = O->Visual();
	if (0==V)		return FALSE;
	Fvector& P		= V->bv_Position;
	S.R				= V->bv_Radius;
	O->svXFORM().transform_tiny	(S.P,P);
	if (Sphere.intersect(S))	return TRUE;
	else						return FALSE;
	
/*	
	// Get our matrix
	Fmatrix& XF		= Owner()->svTransform;
	
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
				Fplane		P;
				Fvector		A[8],B[8];
				Fmatrix&	T		= shapes[el].data.box;
				Q.mul_43			(XF,T);

				// Build points
				A[0].set( -.5f, -.5f, -.5f); A[0].mul(2.f);	Q.transform_tiny	(B[0],A[0]);
				A[1].set( -.5f, -.5f, +.5f); A[1].mul(2.f);	Q.transform_tiny	(B[1],A[1]);
				A[2].set( -.5f, +.5f, +.5f); A[2].mul(2.f);	Q.transform_tiny	(B[2],A[2]);
				A[3].set( -.5f, +.5f, -.5f); A[3].mul(2.f);	Q.transform_tiny	(B[3],A[3]);
				A[4].set( +.5f, +.5f, +.5f); A[4].mul(2.f);	Q.transform_tiny	(B[4],A[4]);
				A[5].set( +.5f, +.5f, -.5f); A[5].mul(2.f);	Q.transform_tiny	(B[5],A[5]);
				A[6].set( +.5f, -.5f, +.5f); A[6].mul(2.f);	Q.transform_tiny	(B[6],A[6]);
				A[7].set( +.5f, -.5f, -.5f); A[7].mul(2.f);	Q.transform_tiny	(B[7],A[7]);

				P.build(B[0],B[3],B[5]);	if (P.classify(S.P)<S.R) break;
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
*/
}
