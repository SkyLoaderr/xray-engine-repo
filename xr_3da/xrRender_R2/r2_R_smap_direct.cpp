#include "stdafx.h"

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
		add_Geometry				(S->Root());
		S->Render_objects			(ViewBase);
	}

	// Restore
	ViewBase						= ViewSave;
	View							= 0;
}

void	CRender::render_smap_sector(CSector* S, Fmatrix& mCombined, Fvector& C)
{
	R_ASSERT						(S);

	// Save and build new frustum, disable HOM
	CFrustum	ViewSave			= ViewBase;
	ViewBase.CreateFromMatrix		(mCombined,FRUSTUM_P_LRTB|FRUSTUM_P_FAR);
	View							= &ViewBase;
	S->Render_objects_s				(ViewBase,C,mCombined);

	// Restore
	ViewBase						= ViewSave;
	View							= 0;
}
