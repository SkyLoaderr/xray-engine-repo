#include "stdafx.h"
#include "..\irenderable.h"

void	CRender::render_smap_direct		(Fmatrix& mCombined)
{
	// Save and build new frustum, disable HOM
	CFrustum	ViewSave			= ViewBase;
	ViewBase.CreateFromMatrix		(mCombined,FRUSTUM_P_LRTB|FRUSTUM_P_FAR);
	View							= &ViewBase;

	// Dumb!!! Very brute-force rendering!!!
	for (u32 s=0; s<Sectors.size(); s++)
	{
		CSector*	S				= (CSector*)Sectors[s];
		add_Geometry				(S->root());
	}

	// Traverse object database
	g_SpatialSpace->q_frustum
		(
		ISpatial_DB::O_ORDERED,
		STYPE_RENDERABLE,
		ViewBase
		);

	// Determine visibility for dynamic part of scene
	xr_vector<ISpatial*>& lstRenderables	= g_SpatialSpace->q_result;
	for (u32 o_it=0; o_it<lstRenderables.size(); o_it++)
	{
		ISpatial*		spatial			= lstRenderables[o_it];
		IRenderable*	renderable		= dynamic_cast<IRenderable*>(spatial);
		VERIFY							(renderable);
		renderable->renderable_Render	();
	}

	// Restore
	ViewBase						= ViewSave;
	View							= 0;
}

void	CRender::render_smap_sector(CSector* _sector, Fmatrix& mCombined, Fvector& _cop)
{
	R_ASSERT						(_sector);

	// Save and build new frustum, disable HOM
	CFrustum	ViewSave			= ViewBase;
	ViewBase.CreateFromMatrix		(mCombined,FRUSTUM_P_LRTB|FRUSTUM_P_FAR);
	View							= &ViewBase;

	// Traverse sector/portal structure
	PortalTraverser.traverse		( _sector, ViewBase, _cop, mCombined, 0 );

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
	g_SpatialSpace->q_frustum
		(
		ISpatial_DB::O_ORDERED,
		STYPE_RENDERABLE,
		ViewBase
		);

	// Determine visibility for dynamic part of scene
	xr_vector<ISpatial*>& lstRenderables	= g_SpatialSpace->q_result;
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
				// renderable
				IRenderable*	renderable		= dynamic_cast<IRenderable*>(spatial);
				VERIFY							(renderable);
				renderable->renderable_Render	();
			}

		}
	}

	// Restore
	ViewBase						= ViewSave;
	View							= 0;
}
