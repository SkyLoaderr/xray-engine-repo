#include "stdafx.h"
#include "..\frustum.h"
#include "..\HW.h"

void MK_Frustum(CFrustum& F, float FOV, float _FAR, float A, Fvector &P, Fvector &D, Fvector &U)
{
	float YFov	= deg2rad(FOV);
	float XFov	= deg2rad(FOV/A);

	// calc window extents in camera coords
	float wR=tanf(XFov*0.5f);
	float wL=-wR;
	float wT=tanf(YFov*0.5f);
	float wB=-wT;

	// calc x-axis (viewhoriz) and store cop
	// here we are assuring that vectors are perpendicular & normalized
	Fvector			R,COP;
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
	Fvector ProjDirs[4];
	ProjDirs[0].sub(sPts[0],COP);
	ProjDirs[1].sub(sPts[1],COP);
	ProjDirs[2].sub(sPts[2],COP);
	ProjDirs[3].sub(sPts[3],COP);

	Fvector _F[4];
	_F[0].direct(COP, ProjDirs[0], _FAR);
	_F[1].direct(COP, ProjDirs[1], _FAR);
	_F[2].direct(COP, ProjDirs[2], _FAR);
	_F[3].direct(COP, ProjDirs[3], _FAR);

	F.CreateFromPoints(COP,_F,4);
}

void dbg_draw_frustum (float FOV, float _FAR, float A, Fvector &P, Fvector &D, Fvector &U)
{
	float YFov	= deg2rad(FOV);
	float XFov	= deg2rad(FOV/A);

	// calc window extents in camera coords
	float wR=tanf(XFov*0.5f);
	float wL=-wR;
	float wT=tanf(YFov*0.5f);
	float wB=-wT;

	// calc x-axis (viewhoriz) and store cop
	// here we are assuring that vectors are perpendicular & normalized
	Fvector			R,COP;
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
	Fvector ProjDirs[4];
	ProjDirs[0].sub(sPts[0],COP);
	ProjDirs[1].sub(sPts[1],COP);
	ProjDirs[2].sub(sPts[2],COP);
	ProjDirs[3].sub(sPts[3],COP);

    CHK_DX(HW.pDevice->SetRenderState	(D3DRS_CULLMODE,	D3DCULL_NONE		));
	CHK_DX(HW.pDevice->SetRenderState	(D3DRS_AMBIENT,		0xffffffff			));

	Fvector _F[4];
	_F[0].direct(COP, ProjDirs[0], _FAR);
	_F[1].direct(COP, ProjDirs[1], _FAR);
	_F[2].direct(COP, ProjDirs[2], _FAR);
	_F[3].direct(COP, ProjDirs[3], _FAR);

	DWORD CT	= D3DCOLOR_RGBA(255,255,255,64);
	DWORD CL	= D3DCOLOR_RGBA(0,255,0,255);
	Fmatrix& M	= precalc_identity;
	Device.Primitive.dbg_DrawTRI	(M,COP,_F[0],_F[1],CT);
	Device.Primitive.dbg_DrawTRI	(M,COP,_F[1],_F[2],CT);
	Device.Primitive.dbg_DrawTRI	(M,COP,_F[2],_F[3],CT);
	Device.Primitive.dbg_DrawTRI	(M,COP,_F[3],_F[0],CT);
	Device.Primitive.dbg_DrawLINE	(M,COP,_F[0],CL);
	Device.Primitive.dbg_DrawLINE	(M,COP,_F[1],CL);
	Device.Primitive.dbg_DrawLINE	(M,COP,_F[2],CL);
	Device.Primitive.dbg_DrawLINE	(M,COP,_F[3],CL);

    CHK_DX(HW.pDevice->SetRenderState	(D3DRS_CULLMODE,	D3DCULL_CCW			));
	CHK_DX(HW.pDevice->SetRenderState	(D3DRS_AMBIENT,	0						));
}
