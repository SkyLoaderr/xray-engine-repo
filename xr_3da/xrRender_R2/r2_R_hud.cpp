#include "stdafx.h"

void __fastcall sorted_L1	(mapSorted_Node *N)
{
	IVisual *V = N->val.pVisual;
	RCache.set_Shader		(V->hShader);
	RCache.set_xform_world	(N->val.Matrix);
	V->Render				(calcLOD(N->key,V->vis.sphere.R));
}

void CRender::render_hud()
{
	ENGINE_API extern float		psHUD_FOV;

	// Change projection
	Fmatrix Pold				= Device.mProject;
	Fmatrix FTold				= Device.mFullTransform;
	Device.mProject.build_projection(
		deg2rad(psHUD_FOV*Device.fFOV*Device.fASPECT), 
		Device.fASPECT, VIEWPORT_NEAR, 
		pCreator->Environment.Current.Far);
	Device.mFullTransform.mul	(Device.mProject, Device.mView);
	RCache.set_xform_project	(Device.mProject);

	// Rendering
	rmNear						();
	mapHUD.traverseLR			(sorted_L1);
	mapHUD.clear				();
	rmNormal					();

	// Restore projection
	Device.mProject				= Pold;
	Device.mFullTransform		= FTold;
	RCache.set_xform_project	(Device.mProject);
}
