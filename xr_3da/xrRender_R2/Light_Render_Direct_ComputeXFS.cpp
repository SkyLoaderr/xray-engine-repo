#include "stdafx.h"
#include "light_render_direct.h"

void CLight_Render_Direct::compute_xfs	(u32 m_phase, light* L)
{
	// Build EYE-space xform
	Fvector						L_dir,L_up,L_right,L_pos;
	L_dir.set					(L->direction);			L_dir.normalize		();

	if (L->right.square_magnitude()>EPS)				{
		// use specified 'up' and 'right', just enshure ortho-normalization
		L_right.set					(L->right);				L_right.normalize	();
		L_up.crossproduct			(L_dir,L_right);		L_up.normalize		();
		L_right.crossproduct		(L_up,L_dir);			L_right.normalize	();
	} else {
		// auto find 'up' and 'right' vectors
		L_up.set					(0,1,0);				if (_abs(L_up.dotproduct(L_dir))>.99f)	L_up.set(0,0,1);
		L_right.crossproduct		(L_up,L_dir);			L_right.normalize	();
		L_up.crossproduct			(L_dir,L_right);		L_up.normalize		();
	}

	L_pos.set						(L->position);
	L->X.S.view.build_camera_dir	(L_pos,L_dir,L_up);
	L->X.S.project.build_projection	(L->cone,1.f,SMAP_near_plane,L->range+EPS_S);
	L->X.S.combine.mul				(L->X.S.project,L->X.S.view);

	// Compute approximate screen area (treating it as an point light) - R*R/dist_sq
	L->X.S.posX	= L->X.S.posY	= 0;
	L->X.S.size					= SMAP_size;
	L->X.S.transluent			= FALSE;
	float	dist				= Device.vCameraPosition.distance_to(L->position)-L->range;
	float	ssa					= 0.333f * ps_r2_ls_squality * L->range*L->range / ((dist<=EPS)?EPS:dist*dist);
	u32		limit				= SMAP_size;
	if		(ssa >= 1)			L->X.S.size		= limit;
	else	{
		// ssa is quadratic
		L->X.S.size				= iFloor(_sqrt(ssa) * limit);
		if (L->X.S.size<16)		L->X.S.size	= 16;
	}
	//Msg		("%8X : ssa(%f), size(%d)",u32(L),ssa,S_size);
}
