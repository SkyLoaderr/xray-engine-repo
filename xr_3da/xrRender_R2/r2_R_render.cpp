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

void CRender::render_main	()
{
	// Msg		("---------------------------");
	marker									++;
	phase									= PHASE_NORMAL;

	// Calculate sector(s) and their objects
	set_Recorder	(&main_coarse_structure);
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
			ISpatial*	spatial		= lstRenderables[o_it];		spatial->spatial_updatesector	();
			CSector*	sector		= (CSector*)spatial->spatial.sector;
			if	(0==sector)										continue;	// disassociated from S/P structure
			if	(PortalTraverser.i_marker != sector->r_marker)	continue;	// inactive (untouched) sector
			for (u32 v_it=0; v_it<sector->r_frustums.size(); v_it++)	{
				CFrustum&	view	= sector->r_frustums[v_it];
				if (!view.testSphere_dirty(spatial->spatial.center,spatial->spatial.radius))	continue;

				if (spatial->spatial.type & STYPE_RENDERABLE)
				{
					// renderable
					IRenderable*	renderable		= spatial->dcast_Renderable	();
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
					light*			L				= (light*)	(spatial->dcast_Light());
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
	set_Recorder			(NULL);
}

void CRender::Render		()
{
	VERIFY					(g_pGameLevel && g_pGameLevel->pHUD);

	//******* Main calc
	Device.Statistic.RenderCALC.Begin		();

	// Frustum & HOM rendering
	ViewBase.CreateFromMatrix					(Device.mFullTransform,FRUSTUM_P_LRTB + FRUSTUM_P_FAR);
	View										= 0;
	HOM.Enable									();
	HOM.Render									(ViewBase);

	// Main calc
	r_pmask										(true,false);	// enable priority "0"
	render_main									();
	r_pmask										(true,false);	// disable priority "1"
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
	CHK_DX										(q_sync_point->Issue(D3DISSUE_END));
	LP_normal.clear								();
	LP_pending.clear							();
	{
		// perform tests
		u32	count			= 0;
		light_Package&	LP	= Lights.package;

		// stats
		stats.l_shadowed	= LP.v_shadowed.size();
		stats.l_unshadowed	= LP.v_point.size() + LP.v_spot.size();
		stats.l_total		= stats.l_shadowed + stats.l_unshadowed;

		// perform tests
		count				= _max(count,LP.v_point.size());
		count				= _max(count,LP.v_spot.size());
		count				= _max(count,LP.v_shadowed.size());
		for (u32 it=0; it<count; it++)	{
			if (it<LP.v_point.size())		{
				light*	L			= LP.v_point	[it];
				L->vis_prepare		();
				if (L->vis.pending)	LP_pending.v_point.push_back	(L);
				else				LP_normal.v_point.push_back		(L);
			}
			if (it<LP.v_spot.size())		{
				light*	L			= LP.v_spot		[it];
				L->vis_prepare		();
				if (L->vis.pending)	LP_pending.v_spot.push_back		(L);
				else				LP_normal.v_spot.push_back		(L);
			}
			if (it<LP.v_shadowed.size())	{
				light*	L			= LP.v_shadowed	[it];
				L->vis_prepare		();
				if (L->vis.pending)	LP_pending.v_shadowed.push_back	(L);
				else				LP_normal.v_shadowed.push_back	(L);
			}
		}
	}
	LP_normal.sort			();
	LP_pending.sort			();

	// Wall marks
	Target.phase_wallmarks					();
	Wallmarks->Render						();				// wallmarks has priority as normal geometry

	// Update incremental shadowmap-visibility solver
	{
		for (u32 it=0; it<Lights_LastFrame.size(); it++)
			Lights_LastFrame[it]->svis.flushoccq	();
		Lights_LastFrame.clear	();
	}

	// Directional light - fucking sun
	// render_sun								();

	// Lighting, non dependant on OCCQ
	Target.phase_accumulator				();
	HOM.Disable								();
	render_lights							(LP_normal);
	
	// Sync-Point
	{
		CTimer	T;							T.Start	();
		BOOL	result						= FALSE;
		HRESULT	hr							= S_FALSE;
		while	((hr=q_sync_point->GetData	(&result,sizeof(result),D3DGETDATA_FLUSH))==S_FALSE) {
			Sleep(0);
			if (T.GetElapsed_ms() > 500)	{
				result	= FALSE;
				break;
			}
		}
	}
	
	// Lighting, dependant on OCCQ
	render_lights							(LP_pending);

	// Postprocess
	Target.phase_combine					();

	//******* Main render - second order geometry (the one, that doesn't support deffering)
	{
		// level
		r_pmask									(false,true);	// enable priority "1"
		render_main								();
		r_dsgraph_render_graph					(1);			// normal level, secondary priority
		r_dsgraph_render_sorted					();				// strict-sorted geoms
	}

	// PortalTraverser.dbg_draw				();

	// HUD
	Device.Statistic.RenderDUMP_HUD.Begin	();
	g_pGameLevel->pHUD->Render_Direct		();
	Device.Statistic.RenderDUMP_HUD.End		();
}

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
