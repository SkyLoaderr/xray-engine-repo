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
	S3.P.set	(_S3.)

	Log	("---");
	Log	("pos",S1.P);
	Log	("r  ",S1.R);

	Log	("---2n");
	Log	("pos",S2.P);
	Log	("r  ",S2.R);

	// select best one
	if (B1 && (S1.R<S2.R))
	{
		C.set	(S1.P);
		R	=	S1.R;
	} else {
		C.set	(S2.P);
		R	=	S2.R;
	}
}
