#include "stdafx.h"
#include "..\fbasicvisual.h"
#include "..\customhud.h"

void CRender::Render	()
{
	Device.Statistic.RenderDUMP.Begin();

	// Target.set_gray					(.5f+sinf(Device.fTimeGlobal)/2.f);
	Target.phase_scene				();

	// HUD render
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
		/*
		rmNear						();
		mapHUD.traverseLR			(sorted_L1);
		mapHUD.clear				();
		rmNormal					();
		*/

		// Restore projection
		Device.mProject				= Pold;
		Device.mFullTransform		= FTold;
		RCache.set_xform_project	(Device.mProject);
	}

	// NORMAL			*** mostly the main level
	render_scenegraph		();

	//-----------------------------------
	RCache.set_xform_world	(Fidentity);
	Details.Render			(Device.vCameraPosition);

	// Sorted (back to front)
	// mapSorted.traverseRL	(sorted_L1);
	mapSorted.clear			();

	// HUD
	Device.Statistic.RenderDUMP_HUD.Begin	();
	pCreator->pHUD->Render_Affected			();
	Device.Statistic.RenderDUMP_HUD.End		();

	// Direct-compute
	LR_Direct.compute_xf_1					();

	// pCreator->Environment.RenderFirst	();
	// pCreator->Environment.RenderLast		();
	
	// Postprocess
	Target.phase_combine					();
	
	// HUD
	Device.Statistic.RenderDUMP_HUD.Begin	();
	pCreator->pHUD->Render_Direct			();
	Device.Statistic.RenderDUMP_HUD.End		();

	Device.Statistic.RenderDUMP.End();
}
