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
		lstRenderables,
		ISpatial_DB::O_ORDERED,
		STYPE_RENDERABLE,
		ViewBase
		);

	// Determine visibility for dynamic part of scene
	for (u32 o_it=0; o_it<lstRenderables.size(); o_it++)
	{
		ISpatial*		spatial			= lstRenderables[o_it];
		IRenderable*	renderable		= spatial->dcast_Renderable();
		VERIFY							(renderable);
		renderable->renderable_Render	();
	}

	// Restore
	ViewBase						= ViewSave;
	View							= 0;
}
