#include "stdafx.h"
#include "light_render_direct.h"
#include "..\igame_persistent.h"
#include "..\environment.h"

//								+X,				-X,				+Y,				-Y,			+Z,				-Z
static	Fvector cmNorm[6]	= {{0.f,1.f,0.f}, {0.f,1.f,0.f}, {0.f,0.f,-1.f},{0.f,0.f,1.f}, {0.f,1.f,0.f}, {0.f,1.f,0.f}};
static	Fvector cmDir[6]	= {{1.f,0.f,0.f}, {-1.f,0.f,0.f},{0.f,1.f,0.f}, {0.f,-1.f,0.f},{0.f,0.f,1.f}, {0.f,0.f,-1.f}};

extern	void ComputeFrustum	(Fvector* _F, float p_FOV, float p_A, float p_FAR, Fvector& camD, Fvector& camN, Fvector& camR, Fvector& camP);

BOOL CLight_Render_Direct::compute_xfp_1	(u32 m_phase, light* L)
{
	// Build EYE-space xform
	Fmatrix						mxform;
	mxform.invert				(Device.mView);
	Fvector						L_dir,L_up,L_right,L_pos;
	mxform.transform_dir		(L_dir,cmDir	[m_phase]);			L_dir.normalize		();
	mxform.transform_dir		(L_up,cmNorm	[m_phase]);			L_up.normalize		();
	L_right.crossproduct		(L_up,L_dir);						L_right.normalize	();
	L_pos.set					(L->position);
	P_view.build_camera_dir		(L_pos,L_dir,L_up);
	P_project.build_projection	(deg2rad(90.f),1.f,PSM_near_plane,L->range+EPS_S);
	P_combine.mul				(P_project,P_view);

	// Quick exit for frustum-frustum intersection
	// If the center of light lies inside, all frustums will cause intersection
	if (RImplementation.ViewBase.testSphere_dirty(L->position,EPS_S))	return			TRUE;

	// Test for visibility of result
	{
		Fvector		points			[5];
		ComputeFrustum				(points,deg2rad(90.f),1.f,L->range+EPS_S,L_dir,L_up,L_right,L_pos);
		sPoly	sSrc(points,4), sDest;
		if (0!=RImplementation.ViewBase.ClipPoly(sSrc, sDest))		return TRUE;
		sSrc.clear();sDest.clear();sSrc.push_back(L_pos);sSrc.push_back(points[0]);sSrc.push_back(points[1]);
		if (0!=RImplementation.ViewBase.ClipPoly(sSrc, sDest))		return TRUE;
		sSrc.clear();sDest.clear();sSrc.push_back(L_pos);sSrc.push_back(points[1]);sSrc.push_back(points[2]);
		if (0!=RImplementation.ViewBase.ClipPoly(sSrc, sDest))		return TRUE;
		sSrc.clear();sDest.clear();sSrc.push_back(L_pos);sSrc.push_back(points[2]);sSrc.push_back(points[3]);
		if (0!=RImplementation.ViewBase.ClipPoly(sSrc, sDest))		return TRUE;
		sSrc.clear();sDest.clear();sSrc.push_back(L_pos);sSrc.push_back(points[3]);sSrc.push_back(points[0]);
		if (0!=RImplementation.ViewBase.ClipPoly(sSrc, sDest))		return TRUE;
	}

	// The last possibility - our main frustum is totally inside test frustum
	{
		Fvector		points			[5];
		float		p_FOV			= Device.fFOV;
		float		p_DIST			= g_pGamePersistent->Environment.CurrentEnv.far_plane;
		Fmatrix		mCam;			mCam.invert			(Device.mView);
		Fvector						camD,camN,camR,camP;
		ComputeFrustum				(points,p_FOV,1.f,p_DIST,mCam.k,mCam.j,mCam.i,mCam.c);
		CFrustum	fTest;			fTest.CreateFromMatrix			(P_combine,FRUSTUM_P_ALL);
		sPoly	sSrc(points,4), sDest;
		if (0!=fTest.ClipPoly(sSrc, sDest))		return TRUE;
		sSrc.clear();sDest.clear();sSrc.push_back(L_pos);sSrc.push_back(points[0]);sSrc.push_back(points[1]);
		if (0!=fTest.ClipPoly(sSrc, sDest))		return TRUE;
		sSrc.clear();sDest.clear();sSrc.push_back(L_pos);sSrc.push_back(points[1]);sSrc.push_back(points[2]);
		if (0!=fTest.ClipPoly(sSrc, sDest))		return TRUE;
		sSrc.clear();sDest.clear();sSrc.push_back(L_pos);sSrc.push_back(points[2]);sSrc.push_back(points[3]);
		if (0!=fTest.ClipPoly(sSrc, sDest))		return TRUE;
		sSrc.clear();sDest.clear();sSrc.push_back(L_pos);sSrc.push_back(points[3]);sSrc.push_back(points[0]);
		if (0!=fTest.ClipPoly(sSrc, sDest))		return TRUE;
	}

	return FALSE;
}

void CLight_Render_Direct::compute_xfp_2	(u32 m_phase, light* L)
{
}
