#include "StdAfx.h"
#include "light_render_direct.h"

void ComputeFrustum				(Fvector* _F, float p_FOV, float p_A, float p_FAR, Fvector& camD, Fvector& camN, Fvector& camR, Fvector& camP)
{
	// calc window extents in camera coords
	float YFov			=	deg2rad(p_FOV*p_A);
	float XFov			=	deg2rad(p_FOV);
	float wR			=	tanf(XFov*0.5f);
	float wL			=	-wR;
	float wT			=	tanf(YFov*0.5f);
	float wB			=	-wT;

	// calculate the corner vertices of the window
	Fvector	sPts[4],Offset,T,ProjDirs[4];
	Offset.add			(camD,camP);
	sPts[0].mul			(camR,wR);		T.mad(Offset,camN,wT);	sPts[0].add(T);
	sPts[1].mul			(camR,wL);		T.mad(Offset,camN,wT);	sPts[1].add(T);
	sPts[2].mul			(camR,wL);		T.mad(Offset,camN,wB);	sPts[2].add(T);
	sPts[3].mul			(camR,wR);		T.mad(Offset,camN,wB);	sPts[3].add(T);
	ProjDirs[0].sub		(sPts[0],camP);	
	ProjDirs[1].sub		(sPts[1],camP);	
	ProjDirs[2].sub		(sPts[2],camP);	
	ProjDirs[3].sub		(sPts[3],camP);	
	_F[0].mad			(camP, ProjDirs[0], p_FAR);
	_F[1].mad			(camP, ProjDirs[1], p_FAR);
	_F[2].mad			(camP, ProjDirs[2], p_FAR);
	_F[3].mad			(camP, ProjDirs[3], p_FAR);
	_F[4].set			(camP);
	_F[5].mad			(camP, camD,		p_FAR);
}

void	CLight_Render_Direct::compute_xfd_1	(u32 m_phase)
{
	float p_FOV				= Device.fFOV;
	float p_DIST			= m_phase?DSM_distance_2:DSM_distance_1;

	// Shadow-test
	Fmatrix		mCam;		mCam.invert			(Device.mView);
	Fvector					camD,camN,camR,camP;
	Fvector					_F	[6];

	// 1
	camD.set				(mCam.k);
	camN.set				(mCam.j);
	camR.set				(mCam.i);
	camP.set				(mCam.c);
	ComputeFrustum			(_F,p_FOV,1.f,p_DIST,camD,camN,camR,camP);

	// Build L-view vectors
	Fvector					L_dir,L_up,L_right,L_pos;

	L_dir.set				(RImplementation.Lights.sun_dir);	L_dir.normalize		();
	L_up.set				(0,0,-1);							L_up.normalize		();
	L_right.crossproduct	(L_up,L_dir);						L_right.normalize	();
	L_up.crossproduct		(L_dir,L_right);					L_up.normalize		();
	L_pos.set				(0,0,0);
	L_view.build_camera_dir	(L_pos,L_dir,L_up);

	//
	Fbox	bb;
	Fvector bbc,bbd,T;

	// L-view corner points and box
	bb.invalidate();		for (int i=0; i<5; i++)	{ L_view.transform_tiny	(T,_F[i]); bb.modify (T); }
	bb.get_CD				(bbc,bbd);

	// Back project center
	Fmatrix inv;
	inv.invert				(L_view);
	inv.transform_tiny		(L_pos,bbc);

	// L-view matrix
	L_pos.mad				(L_dir, -DSM_d_range);
	L_view.build_camera_dir	(L_pos,L_dir,L_up);

	// L-view corner points and box
	bb.invalidate();		for (int i=0; i<5; i++)	{ L_view.transform_tiny	(T,_F[i]); bb.modify (T); }
	bb.get_CD				(bbc,bbd);

	// L_project
	float				d	= 2*p_DIST/cosf(p_FOV/2);	
	float				dx	= 2*bbd.x;
	float				dy	= 2*bbd.y;
	L_project.build_projection_ortho(dx,dy,1.f,DSM_d_range);

	// 
	L_combine.mul			(L_project,L_view);
}

void CLight_Render_Direct::compute_xfd_2(u32 m_phase)
{
}
