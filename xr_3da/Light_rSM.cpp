#include "stdafx.h"
#include "light_rSM.h"

void CLight_rSM::compute_xf_direct	(Fmatrix& mDest, Fmatrix& mView, float p_FOV, float p_A, float p_FAR )
{
	// calc window extents in camera coords
	float YFov		=	deg2rad(p_FOV);
	float XFov		=	deg2rad(p_FOV/p_A);
	float wR		=	tanf(XFov*0.5f);
	float wL		=	-wR;
	float wT		=	tanf(YFov*0.5f);
	float wB		=	-wT;

	// calculate the corner vertices of the window
	// R,N,D,P = i,j,k,c
	Fvector	sPts	[4];  // silhouette points (corners of window)
	Fvector	Offset,	T;
	Offset.add		(mView.k,mView.c);
	sPts[0].mul		(mView.i,wR);		T.mad(Offset,mView.j,wT);	sPts[0].add(T);
	sPts[1].mul		(mView.i,wL);		T.mad(Offset,mView.j,wT);	sPts[1].add(T);
	sPts[2].mul		(mView.i,wL);		T.mad(Offset,mView.j,wB);	sPts[2].add(T);
	sPts[3].mul		(mView.i,wR);		T.mad(Offset,mView.j,wB);	sPts[3].add(T);

	// find projector direction vectors (from cop through silhouette pts)
	Fvector ProjDirs[4];
	ProjDirs[0].sub	(sPts[0],mView.c);	ProjDirs[0].normalize();
	ProjDirs[1].sub	(sPts[1],mView.c);	ProjDirs[1].normalize();
	ProjDirs[2].sub	(sPts[2],mView.c);	ProjDirs[2].normalize();
	ProjDirs[3].sub	(sPts[3],mView.c);	ProjDirs[3].normalize();

	// that's all 5 cornet points, excluding "near" plane
	Fvector _F[5];
	_F[0].mad		(mView.c, ProjDirs[0], p_FAR);
	_F[1].mad		(mView.c, ProjDirs[1], p_FAR);
	_F[2].mad		(mView.c, ProjDirs[2], p_FAR);
	_F[3].mad		(mView.c, ProjDirs[3], p_FAR);
	_F[5].set		(mView.c);
}
