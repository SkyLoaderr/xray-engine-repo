#include "stdafx.h"

void	CRender::render_lights	(light_Package& LP)
{
	//////////////////////////////////////////////////////////////////////////
	// sort lights by importance???
	// while (has_any_lights_that_cast_shadows) {
	//		if (has_point_shadowed)		->	generate point shadowmap
	//		if (has_spot_shadowed)		->	generate spot shadowmap
	//		switch-to-accumulator
	//		if (has_point_unshadowed)	-> 	accum point unshadowed
	//		if (has_spot_unshadowed)	-> 	accum spot unshadowed
	//		if (was_point_shadowed)		->	accum point shadowed
	//		if (was_spot_shadowed)		->	accum spot shadowed
	//	}
	//	if (left_some_lights_that_doesn't cast shadows)
	//		accumulate them

	while	(LP.v_point_s.size() || LP.v_spot_s.size() )
	{
		// if (has_point_shadowed)
		light*	L_point_s	= 0;	
		if		(!LP.v_point_s.empty())	{
			// generate point shadowmap
			light*	L		= L_point_s	= LP.v_point_s.back();	LP.v_point_s.pop_back();
			L->vis_update	();
			if		(!L->vis.visible)	L_point_s = 0;
			else	{
				Lights_LastFrame.push_back		(L);
				for (u32 pls_phase=0; pls_phase<6; pls_phase++)		{
					phase									= PHASE_SMAP_P;
					if (!LR.compute_xfp_1(pls_phase, L))	continue;	// frustum doesn't touch primary frustum
					L->svis[pls_phase].begin				();
					r_dsgraph_render_subspace				(L->spatial.sector, L->X.P.combine, L->position, TRUE);
					LR.compute_xfp_2						(pls_phase, L);
					if (mapNormal[0].size() || mapMatrix[0].size())	{
						Target.phase_smap_point				(L,pls_phase);
						RCache.set_xform_world				(Fidentity);
						RCache.set_xform_view				(L->X.P.view);
						RCache.set_xform_project			(L->X.P.project);
						r_dsgraph_render_graph				(0);
					}
					L->svis[pls_phase].end					();
				}
			}
		}

		// if (has_spot_shadowed)
		light*	L_spot_s	= 0;	
		if		(!LP.v_spot_s.empty())	{
			// generate spot shadowmap
			light*	L	= L_spot_s	= LP.v_spot_s.back();	LP.v_spot_s.pop_back();
			L->vis_update	();
			if		(!L->vis.visible)	L_spot_s = 0;
			else	{
				Lights_LastFrame.push_back				(L);
				phase									= PHASE_SMAP_S;
				LR.compute_xfs_1						(0, L);
				L->svis.begin							();
				r_dsgraph_render_subspace				(L->spatial.sector, L->X.S.combine, L->position, TRUE);
				LR.compute_xfs_2						(0, L);
				if (mapNormal[0].size() || mapMatrix[0].size())	{
					Target.phase_smap_spot				(L);
					RCache.set_xform_world				(Fidentity);
					RCache.set_xform_view				(L->X.S.view);
					RCache.set_xform_project			(L->X.S.project);
					r_dsgraph_render_graph				(0);
				}
				L->svis.end								();
			}
		}

		//		switch-to-accumulator
		Target.phase_accumulator			();
		HOM.Disable							();

		//		if (has_point_unshadowed)	-> 	accum point unshadowed
		if		(!LP.v_point.empty())	{
			light*	L	= LP.v_point.back	();		LP.v_point.pop_back		();
			L->vis_update				();
			if (L->vis.visible)			{ 
				Target.accum_point		(L);
				render_indirect			(L);
			}
		}

		//		if (has_spot_unshadowed)	-> 	accum spot unshadowed
		if		(!LP.v_spot.empty())	{
			light*	L	= LP.v_spot.back	();		LP.v_spot.pop_back			();
			L->vis_update				();
			if (L->vis.visible)			{ 
				Target.accum_spot		(L);
				render_indirect			(L);
			}
		}

		//		if (was_point_shadowed)		->	accum point shadowed
		if		(L_point_s)					{
			Target.accum_point			(L_point_s);
			render_indirect				(L_point_s);
		}

		//		if (was_spot_shadowed)		->	accum spot shadowed
		if		(L_spot_s)					{ 
			Target.accum_spot			(L_spot_s);
			render_indirect				(L_spot_s);
		}
	}

	// Point lighting (unshadowed, if left)
	if (!LP.v_point.empty())		{
		xr_vector<light*>&	Lvec		= LP.v_point;
		for	(u32 pid=0; pid<Lvec.size(); pid++)	{
			Lvec[pid]->vis_update		();
			if (Lvec[pid]->vis.visible)	{
				render_indirect			(Lvec[pid]);
				Target.accum_point		(Lvec[pid]);
			}
		}
		Lvec.clear	();
	}

	// Spot lighting (unshadowed, if left)
	if (!LP.v_spot.empty())		{
		xr_vector<light*>&	Lvec		= LP.v_spot;
		for	(u32 pid=0; pid<Lvec.size(); pid++)	{
			Lvec[pid]->vis_update		();
			if (Lvec[pid]->vis.visible)	{
				render_indirect			(Lvec[pid]);
				Target.accum_spot		(Lvec[pid]);
			}
		}
		Lvec.clear	();
	}
}

void	CRender::render_indirect			(light* L)
{
	if (!ps_r2_ls_flags.test(R2FLAG_GI))	return;

	light									LIGEN;
	LIGEN.set_type							(IRender_Light::REFLECTED);
	LIGEN.set_shadow						(false);
	LIGEN.set_cone							(PI_DIV_2*2.f);

	xr_vector<light_indirect>&	Lvec		= L->indirect;
	if (Lvec.empty())						return;
	float	LE								= L->color.intensity	();
	for (u32 it=0; it<Lvec.size(); it++)	{
		light_indirect&	LI				= Lvec[it];

		// energy and color
		float	LIE						= LE*LI.E;
		if (LIE < ps_r2_GI_clip)		continue;
		Fvector T; T.set(L->color.r,L->color.g,L->color.b).mul(LI.E);
		LIGEN.set_color					(T.x,T.y,T.z);

		// geometric
		LIGEN.spatial.sector			= LI.S;
		LIGEN.set_position				(LI.P);
		LIGEN.set_rotation				(LI.D,T.set(0,0,0));

		// range
		// dist^2 / range^2 = A - has infinity number of solutions
		// approximate energy by linear fallof Emax / (1 + x) = Emin
		float	Emax					= LIE;
		float	Emin					= 1.f / 255.f;
		float	x						= (Emax - Emin)/Emin;
		if		(x < 0.1f)				continue;
		LIGEN.set_range					(x);

		Target.accum_reflected			(&LIGEN);
	}
}
