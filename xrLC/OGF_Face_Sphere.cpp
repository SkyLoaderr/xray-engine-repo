#include "stdafx.h"
#include "build.h"
#include "ogf_face.h"
#include "MgcCont3DMinSphere.h"

#pragma comment(lib,"MagicFM.lib")

BOOL				SphereValid	(vector<Fvector>& geom, Fsphere& test)
{
	if (fis_gremlin(test.P.x) || fis_denormal(test.R))	return FALSE;

	Fsphere	S	=	test;
	S.R			+=	EPS_L;
	for (vector<Fvector>::iterator	I = geom.begin(); I!=geom.end(); I++)
		if (!S.contains(*I))	return FALSE;
	return TRUE;
}

void				OGF_Base::CalcBounds	() 
{
	// get geometry
	vector<Fvector>				V;
	vector<Fvector>::iterator	I;
	V.reserve					(4096);
	GetGeometry					(V);
	FPU::m64					();

	// 1: calc first variation
	Fsphere	S1;
	S1.compute					(V.begin(),V.size());
	BOOL B1						= SphereValid(V,S1);

	// 2: calc ordinary algorithm
	Fsphere	S2;
	bbox.invalidate				();
	for (I=V.begin(); I!=V.end(); I++)	bbox.modify(*I);
	bbox.grow					(EPS_S);
	bbox.getsphere				(S2.P,S2.R);
	S2.R = -1;
	for (I=V.begin(); I!=V.end(); I++)	{
		float d = S2.P.distance_to(*I);
		if (d>S2.R) S2.R=d;
	}
	BOOL B2						= SphereValid(V,S2);
	R_ASSERT					(B2);

	// 3: calc magic-fm
	Mgc::Sphere _S3				= Mgc::MinSphere(V.size(), (const Mgc::Vector3*) V.begin());
	Fsphere	S3;
	S3.P.set					(_S3.Center().x,_S3.Center().y,_S3.Center().z);
	S3.R						= _S3.Radius();
	BOOL B3						= SphereValid(V,S3);

	// select best one
	if (B1 && (S1.R<S2.R))
	{
		// miniball or FM
		if (B3 && (S3.R<S1.R))
		{
			// FM wins
			C.set	(S3.P);
			R	=	S3.R;
		} else {
			// MiniBall wins
			C.set	(S1.P);
			R	=	S1.R;
		}
	} else {
		// base or FM
		if (B3 && (S3.R<S2.R))
		{
			// FM wins
			C.set	(S3.P);
			R	=	S3.R;
		} else {
			// Base wins :)
			C.set	(S2.P);
			R	=	S2.R;
		}
	}
}
