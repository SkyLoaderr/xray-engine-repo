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

extern int					emapslice;
void CRender::Render		()
{
	VERIFY					(g_pGameLevel && g_pGameLevel->pHUD);

	//******* Main calc
	Device.Statistic.RenderCALC.Begin		();
	r_pmask									(true,true);	// enable priority "0" and "1"
	lstRecorded.clear						();
	if (b_emap)	set_RecordMP				(true);
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
	set_RecordMP								(false);	
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
	LP_normal.clear								();
	LP_pending.clear							();
	{
		// perform tests
		u32	count			= 0;
		light_Package&	LP	= Lights.package;

		// stats
		stats.l_shadowed	= LP.v_point_s.size() + LP.v_spot_s.size();
		stats.l_unshadowed	= LP.v_point.size() + LP.v_spot.size();
		stats.l_total		= stats.l_shadowed + stats.l_unshadowed;
		stats.l_point_s		= LP.v_point_s.size	();
		stats.l_point		= LP.v_point.size	();
		stats.l_spot_s		= LP.v_spot_s.size	();
		stats.l_spot		= LP.v_spot.size	();

		// perform tests
		count				= _max(count,LP.v_point_s.size());
		count				= _max(count,LP.v_spot_s.size());
		count				= _max(count,LP.v_point.size());
		count				= _max(count,LP.v_spot.size());
		for (u32 it=0; it<count; it++)	{
			if (it<LP.v_point_s.size())	{ 
				light*	L			= LP.v_point_s	[it];
				L->vis_prepare		(); 
				if (L->vis.pending)	LP_pending.v_point_s.push_back	(L);
				else				LP_normal.v_point_s.push_back	(L);
			}
			if (it<LP.v_point.size())	{
				light*	L			= LP.v_point	[it];
				L->vis_prepare		();
				if (L->vis.pending)	LP_pending.v_point.push_back	(L);
				else				LP_normal.v_point.push_back		(L);
			}
			if (it<LP.v_spot_s.size())	{
				light*	L			= LP.v_spot_s	[it];
				L->vis_prepare		();
				if (L->vis.pending)	LP_pending.v_spot_s.push_back	(L);
				else				LP_normal.v_spot_s.push_back	(L);
			}
			if (it<LP.v_spot.size())	{
				light*	L			= LP.v_spot		[it];
				L->vis_prepare		();
				if (L->vis.pending)	LP_pending.v_spot.push_back		(L);
				else				LP_normal.v_spot.push_back		(L);
			}
		}
	}
	LP_normal.sort			();
	LP_pending.sort			();

	//******* Elevation maps render
	if (b_emap)
	{
		// level
		marker									++;
		phase									= PHASE_EMAP;
		Target.phase_scene						();
		for (int it=0; it<lstRecorded.size(); it++)	{
			r_dsgraph_insert_static				(lstRecorded[it]);
		}
		for (emapslice=ps_r2_emap_slices-1; emapslice>=0; emapslice--)
		{
			bool	b_last				= false;
			if		(0==emapslice)		b_last	= true;
			r_dsgraph_render_graph		(0,b_last ? true : false);
		}
	}

	//******* Decompression on some HW :)
	Target.phase_decompress				();

	// Update incremental shadowmap-visibility solver
	{
		for (u32 it=0; it<Lights_LastFrame.size(); it++)	{
			Lights_LastFrame[it]->svis[0].flushoccq	();
			Lights_LastFrame[it]->svis[1].flushoccq	();
			Lights_LastFrame[it]->svis[2].flushoccq	();
			Lights_LastFrame[it]->svis[3].flushoccq	();
			Lights_LastFrame[it]->svis[4].flushoccq	();
			Lights_LastFrame[it]->svis[5].flushoccq	();
		}
		Lights_LastFrame.clear	();
	}

	// Lighting and sync-point
	Target.phase_accumulator			();
	HOM.Disable							();
	render_lights						(LP_normal);
	render_lights						(LP_pending);

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

	/*
	if (0==::Random.randI(100))	
	{
		u32		clr4clear							= color_rgba(0,0,0,0);	// 0x00
		CHK_DX	(HW.pDevice->Clear					( 0L, NULL, D3DCLEAR_TARGET, clr4clear, 1.0f, 0L));
	}
	*/
	// Msg					("--- %d : was(%d)",Device.dwFrame,stats.l_visible);	//.
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
