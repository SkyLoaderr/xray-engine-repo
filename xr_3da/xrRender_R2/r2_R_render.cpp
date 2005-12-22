#include "stdafx.h"
#include "..\igame_persistent.h"
#include "..\fbasicvisual.h"
#include "..\customhud.h"
#include "..\xr_object.h"

IC	bool	pred_sp_sort	(ISpatial*	_1, ISpatial* _2)
{
	float	d1		= _1->spatial.sphere.P.distance_to_sqr(Device.vCameraPosition);
	float	d2		= _2->spatial.sphere.P.distance_to_sqr(Device.vCameraPosition);
	return	d1<d2;
}

void CRender::render_main	(Fmatrix&	m_ViewProjection, bool _fportals)
{
//	Msg						("---begin");
	marker									++;

	// Calculate sector(s) and their objects
	if (pLastSector)
	{
		// Traverse sector/portal structure
		PortalTraverser.traverse	
			(
			pLastSector,
			ViewBase,
			Device.vCameraPosition,
			m_ViewProjection,
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
		u32 uID_LTRACK						= 0xffffffff;
		if (phase==PHASE_NORMAL)			{
			uLastLTRACK	++;
			if (lstRenderables.size())		uID_LTRACK	= uLastLTRACK%lstRenderables.size();

			// update light-vis for current entity / actor
			CObject*	O					= g_pGameLevel->CurrentViewEntity();
			if (O)		{
				CROS_impl*	R					= (CROS_impl*) O->ROS();
				if (R)		R->update			(O);
			}
		}
		for (u32 o_it=0; o_it<lstRenderables.size(); o_it++)
		{
			ISpatial*	spatial		= lstRenderables[o_it];		spatial->spatial_updatesector	();
			CSector*	sector		= (CSector*)spatial->spatial.sector;
			if	(0==sector)										continue;	// disassociated from S/P structure
			if	(PortalTraverser.i_marker != sector->r_marker)	continue;	// inactive (untouched) sector
			for (u32 v_it=0; v_it<sector->r_frustums.size(); v_it++)	{
				CFrustum&	view	= sector->r_frustums[v_it];
				if (!view.testSphere_dirty(spatial->spatial.sphere.P,spatial->spatial.sphere.R))	continue;

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
					v_orig.marker					= v_copy.marker;
					v_orig.accept_frame				= v_copy.accept_frame;
					v_orig.hom_frame				= v_copy.hom_frame;
					v_orig.hom_tested				= v_copy.hom_tested;
					if (!bVisible)					break;	// exit loop on frustums

					// Rendering
					if (o_it==uID_LTRACK)	{
						// track lighting environment
						// VERIFY					(renderable->renderable.ROS);
						CROS_impl*		T = (CROS_impl*)renderable->renderable_ROS	();
						if (T)			T->update	(renderable);
					}
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
		if (g_pGameLevel && (phase==PHASE_NORMAL))	g_pGameLevel->pHUD->Render_Last();		// HUD
	}
	else
	{
		set_Object											(0);
		if (g_pGameLevel && (phase==PHASE_NORMAL))	g_pGameLevel->pHUD->Render_Last();		// HUD
	}
}

void CRender::render_menu	()
{
	//	Globals
	RCache.set_CullMode				(CULL_CCW);
	RCache.set_Stencil				(FALSE);
	RCache.set_ColorWriteEnable		();

	// Main Render
	{
		Target->u_setrt						(Target->rt_Generic_0,0,0,HW.pBaseZB);		// LDR RT
		g_pGamePersistent->OnRenderPPUI_main()	;	// PP-UI
	}
	// Distort
	{
		Target->u_setrt						(Target->rt_Generic_1,0,0,HW.pBaseZB);		// Now RT is a distortion mask
		CHK_DX(HW.pDevice->Clear			( 0L, NULL, D3DCLEAR_TARGET, color_rgba(127,127,0,127), 1.0f, 0L));
		g_pGamePersistent->OnRenderPPUI_PP	()	;	// PP-UI
	}

	// Actual Display
	Target->u_setrt					( Device.dwWidth,Device.dwHeight,HW.pBaseRT,NULL,NULL,HW.pBaseZB);
	RCache.set_Shader				( Target->s_menu	);
	RCache.set_Geometry				( Target->g_menu	);

	Fvector2						p0,p1;
	u32								Offset;
	u32		C						= color_rgba	(255,255,255,255);
	float	_w						= float(Device.dwWidth);
	float	_h						= float(Device.dwHeight);
	float	d_Z						= EPS_S;
	float	d_W						= 1.f;
	p0.set							(.5f/_w, .5f/_h);
	p1.set							((_w+.5f)/_w, (_h+.5f)/_h );

	FVF::TL* pv						= (FVF::TL*) RCache.Vertex.Lock	(4,Target->g_menu->vb_stride,Offset);
	pv->set							(EPS,			float(_h+EPS),	d_Z,	d_W, C, p0.x, p1.y);	pv++;
	pv->set							(EPS,			EPS,			d_Z,	d_W, C, p0.x, p0.y);	pv++;
	pv->set							(float(_w+EPS),	float(_h+EPS),	d_Z,	d_W, C, p1.x, p1.y);	pv++;
	pv->set							(float(_w+EPS),	EPS,			d_Z,	d_W, C, p1.x, p0.y);	pv++;
	RCache.Vertex.Unlock			(4,Target->g_menu->vb_stride);
	RCache.Render					(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
}

void CRender::Render		()
{
	VERIFY					(0==mapDistort.size());

	bool	_menu_pp		= g_pGamePersistent?g_pGamePersistent->OnRenderPPUI_query():false;
	if (_menu_pp)			{
		render_menu			()	;
		return					;
	};
	VERIFY					(g_pGameLevel && g_pGameLevel->pHUD);

	// Configure
	RImplementation.o.distortion				= FALSE;		// disable distorion
	Fcolor					sun_color			= ((light*)Lights.sun_adapted._get())->color;
	BOOL					bSUN				= ps_r2_ls_flags.test(R2FLAG_SUN) && (u_diffuse2s(sun_color.r,sun_color.g,sun_color.b)>EPS);

	// HOM
	ViewBase.CreateFromMatrix					(Device.mFullTransform, FRUSTUM_P_LRTB + FRUSTUM_P_FAR);
	View										= 0;
	HOM.Enable									();
	HOM.Render									(ViewBase);

	//******* Z-prefill calc - DEFERRER RENDERER
	if (ps_r2_ls_flags.test(R2FLAG_ZFILL))		{
		Device.Statistic.RenderCALC.Begin			();
		float		z_distance	= ps_r2_zfill		;
		Fmatrix		m_zfill, m_project				;
		m_project.build_projection	(
			deg2rad(Device.fFOV*Device.fASPECT), 
			Device.fASPECT, VIEWPORT_NEAR, 
			z_distance * g_pGamePersistent->Environment.CurrentEnv.far_plane);
		m_zfill.mul	(m_project,Device.mView);
		r_pmask										(true,false);	// enable priority "0"
		set_Recorder								(NULL)		;
		phase										= PHASE_SMAP;
		render_main									(m_zfill,false)	;
		r_pmask										(true,false);	// disable priority "1"
		Device.Statistic.RenderCALC.End				( )			;

		// flush
		Target->phase_scene_prepare					();
		RCache.set_ColorWriteEnable					(FALSE);
		r_dsgraph_render_graph						(0);
		RCache.set_ColorWriteEnable					( );
	} else {
		Target->phase_scene_prepare					();
	}

	//******* Main calc - DEFERRER RENDERER
	// Main calc
	Device.Statistic.RenderCALC.Begin			();
	r_pmask										(true,false);	// enable priority "0"
	if (bSUN)									set_Recorder	(&main_coarse_structure);
	else										set_Recorder	(NULL);
	phase										= PHASE_NORMAL;
	render_main									(Device.mFullTransform,true);
	set_Recorder								(NULL);
	r_pmask										(true,false);	// disable priority "1"
	Device.Statistic.RenderCALC.End				( );

	//******* Main render
	{
		// level
		Target->phase_scene_begin				();
		r_dsgraph_render_hud					();
		r_dsgraph_render_graph					(0);
		r_dsgraph_render_lods					(true,true);
		if(Details)	Details->Render				();
		Target->phase_scene_end					();
	}

	//******* Occlusion testing of volume-limited light-sources
	Target->phase_occq							();
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
		count				= _max	(count,LP.v_point.size());
		count				= _max	(count,LP.v_spot.size());
		count				= _max	(count,LP.v_shadowed.size());
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
	LP_normal.sort							();
	LP_pending.sort							();

	// Wall marks
	if(Wallmarks)		{
		Target->phase_wallmarks					();
		Wallmarks->Render						();				// wallmarks has priority as normal geometry
	}

	// Update incremental shadowmap-visibility solver
	{
		for (u32 it=0; it<Lights_LastFrame.size(); it++)
			Lights_LastFrame[it]->svis.flushoccq	();
		Lights_LastFrame.clear	();
	}

	// Directional light - fucking sun
	if (bSUN)	{
//		Lights_LastFrame.push_back			(Lights.sun_adapted);
		RImplementation.stats.l_visible		++;
		render_sun_near						();
		render_sun							();
		render_sun_filtered					();
		Target->accum_direct_blend			();
	}

	// Lighting, non dependant on OCCQ
	Target->phase_accumulator				();
	HOM.Disable								();
	render_lights							(LP_normal);
	
	// Sync-Point
	Device.Statistic.RenderDUMP_Wait.Begin	();
	{
		CTimer	T;							T.Start	();
		BOOL	result						= FALSE;
		HRESULT	hr							= S_FALSE;
		while	((hr=q_sync_point->GetData	(&result,sizeof(result),D3DGETDATA_FLUSH))==S_FALSE) {
			if (!SwitchToThread())			Sleep(0);
			if (T.GetElapsed_ms() > 500)	{
				result	= FALSE;
				break;
			}
		}
	}
	Device.Statistic.RenderDUMP_Wait.End	();
	
	// Lighting, dependant on OCCQ
	render_lights							(LP_pending);

	// Postprocess
	Target->phase_combine					();
	VERIFY	(0==mapDistort.size());
}

void CRender::render_forward				()
{
	VERIFY	(0==mapDistort.size());
	RImplementation.o.distortion				= RImplementation.o.distortion_enabled;	// enable distorion

	//******* Main render - second order geometry (the one, that doesn't support deffering)
	//.todo: should be done inside "combine" with estimation of of luminance, tone-mapping, etc.
	{
		// level
		r_pmask									(false,true);			// enable priority "1"
		phase									= PHASE_NORMAL;
		render_main								(Device.mFullTransform,false);//
		r_dsgraph_render_graph					(1)	;					// normal level, secondary priority
		PortalTraverser.fade_render				()	;					// faded-portals
		r_dsgraph_render_sorted					()	;					// strict-sorted geoms
		g_pGamePersistent->Environment.RenderLast()	;					// rain/thunder-bolts
	}

	RImplementation.o.distortion				= FALSE;				// disable distorion
}
