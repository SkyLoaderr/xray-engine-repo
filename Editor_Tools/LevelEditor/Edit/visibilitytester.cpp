#include "stdafx.h"
#pragma hdrstop
#include "visibilitytester.h"
#include "ui_main.h"

void CFrustumClipper::Projection		(float fAspect, float FOV, float N, float F)
{
	fFarPlane	= F;
	float YFov	= FOV;
	float XFov	= FOV/fAspect;

	// calc window extents in camera coords
	wR=tan(XFov*0.5f);
	wL=-wR;
	wT=tan(YFov*0.5f);
	wB=-wT;
}

void CFrustumClipper::BuildFrustum(Fvector &P, Fvector &D, Fvector &U)
{
	// calc x-axis (viewhoriz) and store cop
	// here we are assuring that vectors are perpendicular & normalized
	Fvector			R;
	D.normalize		();
	R.crossproduct	(D,U);
	R.normalize		();
	U.crossproduct	(R,D);
	U.normalize		();
	COP.set			(P);

	// calculate the corner vertices of the window
	Fvector			sPts[4];  // silhouette points (corners of window)
	Fvector			Offset,T;
	Offset.add		(D,COP);

	sPts[0].mul(R,wR);	T.direct(Offset,U,wT);	sPts[0].add(T);
	sPts[1].mul(R,wL);	T.direct(Offset,U,wT);	sPts[1].add(T);
	sPts[2].mul(R,wL);	T.direct(Offset,U,wB);	sPts[2].add(T);
	sPts[3].mul(R,wR);	T.direct(Offset,U,wB);	sPts[3].add(T);

	// find projector direction vectors (from cop through silhouette pts)
	ProjDirs[0].sub(sPts[0],COP);
	ProjDirs[1].sub(sPts[1],COP);
	ProjDirs[2].sub(sPts[2],COP);
	ProjDirs[3].sub(sPts[3],COP);

	// calc frustum "side" plane eqs (near and FarP already defined)
	Planes[fcpTop].build	(COP, sPts[0], sPts[1]);  // top
	Planes[fcpLeft].build	(COP, sPts[1], sPts[2]);  // left
	Planes[fcpBottom].build	(COP, sPts[2], sPts[3]);  // bottom
	Planes[fcpRight].build	(COP, sPts[3], sPts[0]);  // right

	// Build far plane
	Fvector F[3];
	F[0].direct(COP,ProjDirs[0],fFarPlane);
	F[1].direct(COP,ProjDirs[1],fFarPlane);
	F[2].direct(COP,ProjDirs[2],fFarPlane);
	Planes[fcpFar].build	(F[2], F[1], F[0]);
}

