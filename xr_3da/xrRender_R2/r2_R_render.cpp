#include "stdafx.h"
#include "..\fbasicvisual.h"
#include "..\customhud.h"
#include "..\irenderable.h"

IC	bool	pred_sp_sort	(ISpatial* _1, ISpatial* _2)
{
	float	d1		= _1->spatial.center.distance_to_sqr(Device.vCameraPosition);
	float	d2		= _2->spatial.center.distance_to_sqr(Device.vCameraPosition);
	return	d1<d2;
}
IC	bool	pred_light_cmp	(light*	_1, light* _2)
{
	if	(_1->vis.pending)
	{
		if (_2->vis.pending)	return	_1->vis.query_order > _2->vis.query_order;	// q-order
		else					return	false;										// _2 should be first
	} else {
		if (_2->vis.pending)	return	true;										// _1 should be first 
		else					return	_1->range > _2->range;						// sort by range
	}
}

void CRender::Render		()
{
	VERIFY					(g_pGameLevel && g_pGameLevel->pHUD);

	//******* Main calc
	Device.Statistic.RenderCALC.Begin		();
	{
		marker									++;
		phase									= PHASE_NORMAL;

		// Frustum & HOM rendering
		ViewBase.CreateFromMatrix				(Device.mFullTransform,FRUSTUM_P_LRTB + FRUSTUM_P_FAR);
		View									= 0;
		HOM.Enable								();
		HOM.Render								(ViewBase);

		// Calculate sector(s) and their objects
		if (pLastSector)
		{
			// Traverse sector/portal structure
			PortalTraverser.traverse	
				(
				pLastSector,
				ViewBase,
				Device.vCameraPosition,
				Device.mFullTransform,
				CPortalTraverser::VQ_HOM + CPortalTraverser::VQ_SSA + 
					(HW.Caps.bScissor?CPortalTraverser::VQ_SCISSOR:0)	// generate scissoring info
				);

			// Determine visibility for static geometry hierrarhy
			for (u32 s_it=0; s_it<PortalTraverser.r_sectors.size(); s_it++)
			{
				CSector*	sector		= (CSector*)PortalTraverser.r_sectors[s_it];
				IRender_Visual*	root	= sector->root();
				for (u32 v_it=0; v_it<sector->r_frustums.size(); v_it++)	{
					set_Frustum			(&(sector->r_frustums[v_it]));
					add_Geometry		(root);
				}
			}

			// Traverse object database
			g_SpatialSpace->q_frustum
				(
				lstRenderables,
				ISpatial_DB::O_ORDERED,
				STYPE_RENDERABLE + STYPE_LIGHTSOURCE,
				ViewBase
				);

			// (almost)Exact sorting order (front-to-back)
			std::sort			(lstRenderables.begin(),lstRenderables.end(),pred_sp_sort);

			// Determine visibility for dynamic part of scene
			set_Object							(0);
			//. g_pGameLevel->pHUD->Render_First( );
			for (u32 o_it=0; o_it<lstRenderables.size(); o_it++)
			{
				ISpatial*	spatial		= lstRenderables[o_it];
				CSector*	sector		= (CSector*)spatial->spatial.sector;
				if	(0==sector)										continue;	// disassociated from S/P structure
				if	(PortalTraverser.i_marker != sector->r_marker)	continue;	// inactive (untouched) sector
				for (u32 v_it=0; v_it<sector->r_frustums.size(); v_it++)
				{
					CFrustum&	view	= sector->r_frustums[v_it];
					if (!view.testSphere_dirty(spatial->spatial.center,spatial->spatial.radius))	continue;

					if (spatial->spatial.type & STYPE_RENDERABLE)
					{
						// renderable
						IRenderable*	renderable		= dynamic_cast<IRenderable*>(spatial);
						VERIFY							(renderable);

						// Occlusion
						vis_data&		v_orig			= renderable->renderable.visual->vis;
						vis_data		v_copy			= v_orig;
						v_copy.box.xform				(renderable->renderable.xform);
						BOOL			bVisible		= HOM.visible(v_copy);
						v_orig.frame					= v_copy.frame;
						v_orig.hom_frame				= v_copy.hom_frame;
						v_orig.hom_tested				= v_copy.hom_tested;
						if (!bVisible)					break;	// exit loop on frustums

						// Rendering
						set_Object						(renderable);
						renderable->renderable_Render	();
						set_Object						(0);
					}
					else 
					{
						VERIFY							(spatial->spatial.type & STYPE_LIGHTSOURCE);
						// lightsource
						light*			L				= dynamic_cast<light*>		(spatial);
						VERIFY							(L);
						Lights.add_light				(L);
					}
					
					break;	// exit loop on frustums
				}
			}
			g_pGameLevel->pHUD->Render_Last						();	
		}
		else
		{
			set_Object											(0);
			g_pGameLevel->pHUD->Render_First					();	
			g_pGameLevel->pHUD->Render_Last						();	
		}
	}
	Device.Statistic.RenderCALC.End				();

	//******* Main render
	{
		// level
		Target.phase_scene						();
		r_dsgraph_render_hud					();
		r_dsgraph_render_graph					(0);
		r_dsgraph_render_lods					();
		Details.Render							();
	}

	//******* Occlusion testing of volume-limited light-sources
	Target.phase_occq							();
	{
		// perform tests
		int	count	= 0;
		count		= _max(count,Lights.v_point_s.size());
		count		= _max(count,Lights.v_spot_s.size());
		count		= _max(count,Lights.v_point.size());
		count		= _max(count,Lights.v_spot.size());
		for (int it=0; it<count; it++)	{
			if (it<Lights.v_point_s.size())	Lights.v_point_s[it]->vis_prepare	();
			if (it<Lights.v_point.size())	Lights.v_point	[it]->vis_prepare	();
			if (it<Lights.v_spot_s.size())	Lights.v_spot_s	[it]->vis_prepare	();
			if (it<Lights.v_spot.size())	Lights.v_spot	[it]->vis_prepare	();
		}
		// resort lights (pending -> at the end), maintain stable order
		std::stable_sort	(Lights.v_point_s.begin(),	Lights.v_point_s.end(),	pred_light_cmp);
		std::stable_sort	(Lights.v_point.begin(),	Lights.v_point.end(),	pred_light_cmp);
		std::stable_sort	(Lights.v_spot_s.begin(),	Lights.v_spot_s.end(),	pred_light_cmp);
		std::stable_sort	(Lights.v_spot.begin(),		Lights.v_spot.end(),	pred_light_cmp);
	}

	// stats
	stats.l_shadowed	= Lights.v_point_s.size() + Lights.v_spot_s.size();
	stats.l_unshadowed	= Lights.v_point.size() + Lights.v_spot.size();
	stats.l_total		= stats.l_shadowed + stats.l_unshadowed;
	stats.l_point_s		= Lights.v_point_s.size	();
	stats.l_point		= Lights.v_point.size	();
	stats.l_spot_s		= Lights.v_spot_s.size	();
	stats.l_spot		= Lights.v_spot.size	();

	//******* Decompression on some HW :)
	Target.phase_decompress						();

	//******* Directional light
	/*
	if (0 && ps_r2_ls_flags.test(R2FLAG_SUN) )
	{
		for (u32 dls_phase=0; dls_phase<2; dls_phase++)
		{
			//******* Direct lighting+shadow		::: Calculate
			Device.Statistic.RenderCALC.Begin		();
			{
				marker									++;
				phase									= PHASE_SMAP_D;

				HOM.Disable								();
				LR.compute_xfd_1						(dls_phase);
				render_smap_direct						(LR.L_combine);
				LR.compute_xfd_2						(dls_phase);
			}
			Device.Statistic.RenderCALC.End			();

			//******* Direct lighting+shadow		::: Render
			{
				Target.phase_smap_direct				();

				RCache.set_xform_world					(Fidentity);
				RCache.set_xform_view					(LR.L_view);
				RCache.set_xform_project				(LR.L_project);
				r_dsgraph_render_graph					(0);
			}

			//******* Direct lighting+shadow		::: Accumulate
			{
				Target.phase_accumulator				();
				Target.shadow_direct					(dls_phase);
			}
		}

		// Multiply by lighting contribution
		Target.phase_accumulator				();
		Target.accum_direct						();
	}
	*/

	// $$$
	// Target.phase_accumulator			();
	HOM.Disable							();
	// Target.accum_direct				();

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

	/*
	Msg		(
		"total(%d), ps(%d), ss(%d), p(%d), s(%d)",
		Lights.v_point_s.size()+Lights.v_spot_s.size()+Lights.v_point.size()+Lights.v_spot.size(),
		Lights.v_point_s.size(),Lights.v_spot_s.size(),Lights.v_point.size(),Lights.v_spot.size()
	);
	*/

	while	(Lights.v_point_s.size() || Lights.v_spot_s.size() )
	{
		// if (has_point_shadowed)
		light*	L_point_s	= 0;	
		if		(!Lights.v_point_s.empty())	{
			// generate point shadowmap
			light*	L		= L_point_s	= Lights.v_point_s.back();	Lights.v_point_s.pop_back();
			L->vis_update	();
			if		(!L->vis.visible)	L_point_s = 0;
			else	{
				for (u32 pls_phase=0; pls_phase<6; pls_phase++)		{
					phase									= PHASE_SMAP_P;
					if (!LR.compute_xfp_1(pls_phase, L))	continue;	// frustum doesn't touch primary frustum
					r_dsgraph_render_subspace				(L->spatial.sector, L->X.P.combine, L->position, TRUE);
					LR.compute_xfp_2						(pls_phase, L);
					if (mapNormal[0].size() || mapMatrix[0].size())	{
						Target.phase_smap_point				(L,pls_phase);
						RCache.set_xform_world				(Fidentity);
						RCache.set_xform_view				(L->X.P.view);
						RCache.set_xform_project			(L->X.P.project);
						r_dsgraph_render_graph				(0);
					}
				}
			}
		}

		// if (has_spot_shadowed)
		light*	L_spot_s	= 0;	
		if		(!Lights.v_spot_s.empty())	{
			// generate spot shadowmap
			light*	L	= L_spot_s	= Lights.v_spot_s.back();	Lights.v_spot_s.pop_back();
			L->vis_update	();
			if		(!L->vis.visible)	L_spot_s = 0;
			else	{
				phase									= PHASE_SMAP_S;
				LR.compute_xfs_1						(0, L);
				r_dsgraph_render_subspace				(L->spatial.sector, L->X.S.combine, L->position, TRUE);
				LR.compute_xfs_2						(0, L);
				if (mapNormal[0].size() || mapMatrix[0].size())	{
					Target.phase_smap_spot				(L);
					RCache.set_xform_world				(Fidentity);
					RCache.set_xform_view				(L->X.S.view);
					RCache.set_xform_project			(L->X.S.project);
					r_dsgraph_render_graph				(0);
				}
			}
		}

		//		switch-to-accumulator
		Target.phase_accumulator			();
		HOM.Disable							();

		//		if (has_point_unshadowed)	-> 	accum point unshadowed
		if		(!Lights.v_point.empty())	{
			light*	L	= Lights.v_point.back	();		Lights.v_point.pop_back		();
			L->vis_update				();
			if (L->vis.visible)			{ 
				Target.accum_point		(L);
				render_indirect			(L);
			}
		}

		//		if (has_spot_unshadowed)	-> 	accum spot unshadowed
		if		(!Lights.v_spot.empty())	{
			light*	L	= Lights.v_spot.back	();		Lights.v_spot.pop_back			();
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
	if (!Lights.v_point.empty())		{
		xr_vector<light*>&	Lvec		= Lights.v_point;
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
	if (!Lights.v_spot.empty())		{
		xr_vector<light*>&	Lvec		= Lights.v_spot;
		for	(u32 pid=0; pid<Lvec.size(); pid++)	{
			Lvec[pid]->vis_update		();
			if (Lvec[pid]->vis.visible)	{
				render_indirect			(Lvec[pid]);
				Target.accum_spot		(Lvec[pid]);
			}
		}
		Lvec.clear	();
	}

	// Postprocess
	// Target.phase_bloom					();
	Target.phase_combine					();
	
	//******* Main render - second order geometry (the one, that doesn't support deffering)
	{
		// level
		r_dsgraph_render_graph					(1);			// normal level, secondary priority
		r_dsgraph_render_sorted					();				// strict-sorted geoms
	}

	// PortalTraverser.dbg_draw				();

	// HUD
	Device.Statistic.RenderDUMP_HUD.Begin	();
	g_pGameLevel->pHUD->Render_Direct		();
	Device.Statistic.RenderDUMP_HUD.End		();

	// Msg					("--- %d : was(%d)",Device.dwFrame,stats.l_visible);	//.
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
	for (int it=0; it<Lvec.size(); it++)	{
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
