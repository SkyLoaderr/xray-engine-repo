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

	// 
	L->X.S.posX	= L->X.S.posY	= 0;
	L->X.S.size					= SMAP_adapt_max;
	L->X.S.transluent			= FALSE;

	// Compute approximate screen area (treating it as an point light) - R*R/dist_sq
	// Note: we clamp screen space area to ONE, although it is not correct at all
	float	dist				= Device.vCameraPosition.distance_to(L->spatial.center)-L->spatial.radius;
			if (dist<0)	dist	= 0;
	float	ssa					= clampr	(L->range*L->range / (1.f+dist*dist),0.f,1.f);

	// compute intensity
	float	intensity			= (L->color.r + L->color.g + L->color.b)/3.f;

	// compute how much duelling frusta occurs	[-1..1]->[-0.5 .. +0.5]  += 1
	float	duel_dot			= 1.f -	0.5f*Device.vCameraDirection.dotproduct(L_dir);

	// factors
	float	factor0				= powf	(ssa,		1.f/2.f);		// ssa is quadratic
	float	factor1				= powf	(intensity, 1.f/4.f);		// less perceptually important?
	float	factor2				= powf	(duel_dot,	1.f/2.f);		// difficult to change here -> visible
	float	factor				= ps_r2_ls_squality * factor0 * factor1 * factor2;
	
	// 
	L->X.S.size					= iFloor( factor * SMAP_adapt_optimal );
	if (L->X.S.size<SMAP_adapt_min)		L->X.S.size	= SMAP_adapt_min;
	if (L->X.S.size>SMAP_adapt_max)		L->X.S.size	= SMAP_adapt_max;
	//Msg		("%8X : ssa(%f), size(%d)",u32(L),ssa,S_size);
}
