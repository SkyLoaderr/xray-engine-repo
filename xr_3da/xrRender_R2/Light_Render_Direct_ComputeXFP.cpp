#include "stdafx.h"
#include "light_render_direct.h"

//								+X,				-X,				+Y,				-Y,			+Z,				-Z
static Fvector cmNorm[6]	= {{0.f,1.f,0.f}, {0.f,1.f,0.f}, {0.f,0.f,-1.f},{0.f,0.f,1.f}, {0.f,1.f,0.f}, {0.f,1.f,0.f}};
static Fvector cmDir[6]		= {{1.f,0.f,0.f}, {-1.f,0.f,0.f},{0.f,1.f,0.f}, {0.f,-1.f,0.f},{0.f,0.f,1.f}, {0.f,0.f,-1.f}};

void CLight_Render_Direct::compute_xfp_1	(u32 m_phase, light* L)
{
	// Build WORLD-space xform
	Fvector						L_dir,L_up,L_right,L_pos;
	L_dir.set					(cmDir	[m_phase]);					
	L_up.set					(cmNorm	[m_phase]);
	L_right.crossproduct		(L_up,L_dir);						L_right.normalize	();
	L_pos.set					(L->sphere.P);
	L_view.build_camera_dir		(L_pos,L_dir,L_up);
	L_project.build_projection	(deg2rad(90.f),1.f,PSM_near_plane,L->sphere.R+EPS_S);
}
