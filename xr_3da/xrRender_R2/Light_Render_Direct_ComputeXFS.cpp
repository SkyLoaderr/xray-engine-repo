#include "stdafx.h"
#include "light_render_direct.h"

void CLight_Render_Direct::compute_xfs_1	(u32 m_phase, light* L)
{
	// Build EYE-space xform
	Fvector						L_dir,L_up,L_right,L_pos;
	L_dir.set					(L->direction);			L_dir.normalize		();
	L_up.set					(0,1,0);				if (_abs(L_up.dotproduct(L_dir))>.99f)	L_up.set(0,0,1);
	L_right.crossproduct		(L_up,L_dir);			L_right.normalize	();
	L_up.crossproduct			(L_dir,L_right);		L_up.normalize		();
	L_pos.set					(L->position);
	L_view.build_camera_dir		(L_pos,L_dir,L_up);
	L_project.build_projection	(L->cone,1.f,SSM_near_plane,L->range+EPS_S);
	L_combine.mul				(L_project,L_view);
}

void CLight_Render_Direct::compute_xfs_2	(u32 m_phase, light* L)
{
}
