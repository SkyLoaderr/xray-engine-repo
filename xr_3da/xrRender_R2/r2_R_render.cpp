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

void CRender::Render		()
{
	//******* Main calc
	Device.Statistic.RenderCALC.Begin		();
	{
		marker									++;
		phase									= PHASE_NORMAL;

		// Frustum & HOM rendering
		ViewBase.CreateFromMatrix				(Device.mFullTransform,FRUSTUM_P_LRTB|FRUSTUM_P_FAR);
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
				CPortalTraverser::VQ_HOM + CPortalTraverser::VQ_SSA
				);

			// Determine visibility for static geometry hierrarhy
			for (u32 s_it=0; s_it<PortalTraverser.r_sectors.size(); s_it++)
			{
				CSector*	sector		= (CSector*)PortalTraverser.r_sectors[s_it];
				IRender_Visual*	root	= sector->root();
				for (u32 v_it=0; v_it<sector->r_frustums.size(); v_it++)
				{
					set_Frustum			(&(sector->r_frustums[v_it]));
					add_Geometry		(root);
				}
			}

			// Traverse object database
			g_SpatialSpace.q_frustum
				(
				ISpatial_DB::O_ORDERED,
				STYPE_RENDERABLE + STYPE_LIGHTSOURCE,
				ViewBase
				);

			// (almost)Exact sorting order (front-to-back)
			xr_vector<ISpatial*>& lstRenderables	= g_SpatialSpace.q_result;
			std::sort								(lstRenderables.begin(),lstRenderables.end(),pred_sp_sort);

			// Determine visibility for dynamic part of scene
			set_Object							(0);
			g_pGameLevel->pHUD->Render_First	( );
			for (u32 o_it=0; o_it<lstRenderables.size(); o_it++)
			{
				ISpatial*	spatial		= lstRenderables[o_it];
				CSector*	sector		= (CSector*)spatial->spatial.sector;
				if	(0==sector)										continue;	// disassociated from S/P structure
				if	(PortalTraverser.i_marker != sector->r_marker)	continue;	// inactive (untouched) sector
				for (u32 v_it=0; v_it<sector->r_frustums.size(); v_it++)
				{
					CFrustum&	view	= sector->r_frustums[v_it];
					if (view.testSphere_dirty(spatial->spatial.center,spatial->spatial.radius))
					{
						if (spatial->spatial.type & STYPE_RENDERABLE)
						{
							// renderable
							IRenderable*	renderable		= dynamic_cast<IRenderable*>(spatial);
							VERIFY							(renderable);
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
					}
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
		render_hud								();
		render_scenegraph						();
		Details.Render							(Device.vCameraPosition);
		// mapSorted.traverseRL					(sorted_L1);
		mapSorted.clear							();		// unsupported
	}

	//******* Decompression on some HW :)
	if (RImplementation.b_nv3x)					Target.phase_decompress();

	//******* Directional light
	if (ps_r2_ls_flags.test(R2FLAG_SUN) )
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
				render_scenegraph						();
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

	// $$$
	// Target.phase_accumulator				();
	// Target.accum_direct					();

	// Point/spot lighting (unshadowed)
	if (1)
	{
		Target.phase_accumulator		();
		HOM.Disable						();
		xr_vector<light*>&	Lvec			= Lights.v_selected_unshadowed;
		for	(u32 pid=0; pid<Lvec.size(); pid++)
		{
			light*	L	= Lvec[pid];
			if (IRender_Light::POINT==L->flags.type)	Target.accum_point_unshadow	(L);
			else										Target.accum_spot_unshadow	(L);
		}
	}

	// Point/spot lighting (shadowed)
	if (1)
	{
		HOM.Disable								();
		xr_vector<light*>&	Lvec	= Lights.v_selected_shadowed;
		for	(u32 pid=0; pid<Lvec.size(); pid++)
		{
			light*	L	= Lvec[pid];
			if (IRender_Light::POINT==L->flags.type)	
			{
				R_ASSERT2	(!RImplementation.b_nv3x, "Shadowed point lights aren't implemented for nv3X HW");

				// Render shadowmap
				for (u32 pls_phase=0; pls_phase<6; pls_phase++)
				{
					marker									++;
					phase									= PHASE_SMAP_P;

					// calculate
					LR.compute_xfp_1						(pls_phase, L);
					render_smap_sector						(L->sector, LR.L_combine, L->position);
					LR.compute_xfp_2						(pls_phase, L);

					// rendering
					if (mapNormal.size())
					{
						Target.phase_smap_point				(pls_phase);
						RCache.set_xform_world				(Fidentity);			// ???
						RCache.set_xform_view				(LR.L_view);
						RCache.set_xform_project			(LR.L_project);
						render_scenegraph					();
					}
				}

				// Render light
				Target.phase_accumulator		();
				Target.accum_point_shadow		(L);
			}
			else
			{
				marker									++;
				phase									= PHASE_SMAP_S;

				// calculate
				LR.compute_xfs_1						(0, L);
				render_smap_sector						(L->sector, LR.L_combine, L->position);
				LR.compute_xfs_2						(0, L);

				// rendering
				if (mapNormal.size() || mapMatrix.size())
				{
					Target.phase_smap_spot				();
					RCache.set_xform_world				(Fidentity);			// ???
					RCache.set_xform_view				(LR.L_view);
					RCache.set_xform_project			(LR.L_project);
					render_scenegraph					();
				}

				// Render light
				Target.phase_accumulator		();
				Target.accum_spot_shadow		(L);
			}
		}
	}

	// Postprocess
	Target.phase_bloom						();
	Target.phase_combine					();
	
	// HUD
	Device.Statistic.RenderDUMP_HUD.Begin	();
	g_pGameLevel->pHUD->Render_Direct			();
	Device.Statistic.RenderDUMP_HUD.End		();
}
