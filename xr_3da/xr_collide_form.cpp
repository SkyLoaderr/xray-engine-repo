#include "stdafx.h"
#include "xr_collide_form.h"
#include "xr_object.h"
#include "xr_ini.h"
#include "xr_func.h"
#include "xr_creator.h"
#include "xr_area.h"
#include "collide\cl_defs.h"
#include "collide\cl_collector.h"
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
	last_rect.invalidate( );
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
	CFileStream			f(full_path);
	hdrCFORM			H;
	f.Read				(&H,sizeof(hdrCFORM));
	R_ASSERT			(CFORM_CURRENT_VERSION==H.version);

	H.aabb.getcenter	(Sphere.P);
	Sphere.R			 = H.aabb.getradius	();
	s_box.set			(H.aabb);

	Fvector*	verts	= (Fvector*)f.Pointer();
	RAPID::tri*	tris	= (RAPID::tri*)(verts+H.vertcount);
	model.BuildModel	( verts, H.vertcount, tris, H.facecount );
	Msg("* CFORM memory usage: %dK",model.MemoryUsage()/1024);

	return				TRUE;
}

BOOL CCF_Polygonal::_clRayTest( RayQuery& Q)
{
	if (!Sphere.intersect(Q.start,Q.dir))	return FALSE;
	XRC.RayPick(&owner->clTransform,&model,Q.start,Q.dir,Q.range);
	const RAPID::raypick_info* R = XRC.GetMinRayPickInfo();
	if (R) {
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
	XRC.RayPick(&owner->svTransform,&model,Q.start,Q.dir,Q.range);
	const RAPID::raypick_info* R = XRC.GetMinRayPickInfo();
	if (R) {
		if (R->range < Q.range) {
			Q.range		= R->range;
			Q.element	= R->id;
			return TRUE;
		}
	}
	return FALSE;
}

void CCF_Polygonal::_BoxQuery( const Fbox& B, const Fmatrix& M, DWORD flags)
{
	if ((flags&clQUERY_TOPLEVEL) || (flags&clGET_TRIS==0 && flags&clGET_BOXES))
	{
		// Return only top level
		clQueryCollision& Q = pCreator->ObjectSpace.q_result;
		Q.AddBox	(owner->svTransform,s_box);
	} else {
		// Return actual tris
		Fmatrix& T = owner->svTransform;
		XRC.BBoxCollide	(T, &model, M, B );
		if (XRC.GetBBoxContactCount())
		{
			clQueryCollision& Q = pCreator->ObjectSpace.q_result;
			if (flags&clQUERY_ONLYFIRST) 
			{
				Q.AddTri(T,&model.tris[XRC.BBoxContact[0].id]);
				return;
			} else {
				for (DWORD i=0; i<XRC.GetBBoxContactCount(); i++)
					Q.AddTri(T,&model.tris[XRC.BBoxContact[i].id]);
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
	PKinematics	K	= PKinematics(O->Visual());
	R_ASSERT		(K->Type == MT_SKELETON);
	model.resize	(K->LL_BoneCount());

	base_box.set	(K->bv_BBox);
	s_box.set		(K->bv_BBox);
	Sphere.R		= K->bv_Radius;
}

void CCF_Skeleton::BuildState()
{
	dwFrame			= Device.dwFrame;
	PKinematics	K	= PKinematics(owner->Visual());
	K->Calculate	();
	
	Fmatrix &L2W	= owner->svTransform;
	Fmatrix Mbox,T,TW;
	for (DWORD i=0; i<model.size(); i++)
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
	dwFrameTL			=Device.dwFrame;
	FBasicVisual* K		= owner->Visual();
	Fbox& B				= K->bv_BBox;
	s_box.min.average	(B.min);
	s_box.max.average	(B.max);
	s_box.grow			(0.05f);
	Fvector X; s_box.getcenter(X);
	Sphere.P.set		(owner->Position());
	Sphere.P.add		(X);
	Sphere.R			+= K->bv_Radius;
	Sphere.R			*= 0.5f;
}

BOOL CCF_Skeleton::_svRayTest( RayQuery& Q)
{
	if (dwFrameTL!=Device.dwFrame)			BuildTopLevel();
	if (!Sphere.intersect(Q.start,Q.dir))	return FALSE;

	if (dwFrame!=Device.dwFrame)	BuildState();

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

void CCF_Skeleton::_BoxQuery( const Fbox& B, const Fmatrix& M, DWORD flags)
{
	if ((flags&clQUERY_TOPLEVEL) || (flags&clGET_BOXES==0))
	{
		if (dwFrameTL!=Device.dwFrame) BuildTopLevel();
		// Return only top level
		clQueryCollision& Q = pCreator->ObjectSpace.q_result;
		Q.AddBox			(owner->svTransform,s_box);
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

	Fmatrix& T = O->svTransform;
	for (int i=0; i<8; i++) {
		A[i].mul(.5f);
		T.transform_tiny(B[i],A[i]);
	}
	s_box.set		(-.5f,-.5f,-.5f,+.5f,+.5f,+.5f);
//	s_box.getsphere	(Sphere.P,Sphere.R); // не правильно!!!! box скейлится
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
	FBasicVisual*	V = O->Visual();
	Fvector&		P = V->bv_Position;
	float			R = V->bv_Radius;

	Fvector			PT;
	O->svTransform.transform_tiny(PT,P);

	for (int i=0; i<6; i++) {
		if (Planes[i].classify(PT)>R) return FALSE;
	}
	return TRUE;
}
BOOL CCF_EventBox::_svRayTest(RayQuery& Q)
{	return FALSE; }
BOOL CCF_EventBox::_clRayTest(RayQuery& Q)
{	return FALSE; }
void CCF_EventBox::_BoxQuery(const Fbox& B, const Fmatrix& M, DWORD flags)
{   return; }
