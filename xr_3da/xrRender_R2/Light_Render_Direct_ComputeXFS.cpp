#include "stdafx.h"
#include "light_render_direct.h"

void CLight_Render_Direct::compute_xfs_1	(u32 m_phase, light* L)
{
	// Build EYE-space xform
	Fvector						L_dir,L_up,L_right,L_pos;
	mxform.transform_dir		(L_dir,cmDir	[m_phase]);			L_dir.normalize		();
	mxform.transform_dir		(L_up,cmNorm	[m_phase]);			L_up.normalize		();
	L_right.crossproduct		(L_up,L_dir);						L_right.normalize	();
	L_pos.set					(L->sphere.P);
	L_view.build_camera_dir		(L_pos,L_dir,L_up);
	L_project.build_projection	(deg2rad(90.f),1.f,PSM_near_plane,L->sphere.R+EPS_S);
	L_combine.mul				(L_project,L_view);
}

void CLight_Render_Direct::compute_xfs_2	(u32 m_phase, light* L)
{
}
