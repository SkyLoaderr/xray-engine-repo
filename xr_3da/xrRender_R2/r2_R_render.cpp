#include "stdafx.h"
#include "..\fbasicvisual.h"
#include "..\customhud.h"

void CRender::Render	()
{
	//******* Main calc
	Device.Statistic.RenderCALC.Begin	();
	{
		marker									++;

		// Frustum & HOM rendering
		ViewBase.CreateFromMatrix				(Device.mFullTransform,FRUSTUM_P_LRTB|FRUSTUM_P_FAR);
		View									= 0;
		HOM.Enable								();
		HOM.Render								(ViewBase);

		// Calculate sector(s) and their objects
		set_Object								(0);
		if (0!=pLastSector) pLastSector->Render	(ViewBase);
		pCreator->pHUD->Render_Calculate		();
	}
	Device.Statistic.RenderCALC.End			();

	//******* Main render
	Device.Statistic.RenderDUMP.Begin		();
	{
		Target.phase_scene					();
		rmNormal							();

		render_hud							();
		render_scenegraph					();
		Details.Render						(Device.vCameraPosition);
		// mapSorted.traverseRL				(sorted_L1);
		mapSorted.clear						();
	}
	Device.Statistic.RenderDUMP.End			();

	//******* Direct lighting+shadow		::: Calculate
	Device.Statistic.RenderCALC.Begin		();
	{
		HOM.Disable								();
		LR_Direct.compute_xf_1					();
		render_smap_direct						(LR_Direct.L_combine);
		LR_Direct.compute_xf_2					();
	}
	Device.Statistic.RenderCALC.End			();
	
	//******* Direct lighting+shadow		::: Render
	Device.Statistic.RenderCALC.Begin		();
	{
		render_scenegraph						();
	}
	Device.Statistic.RenderCALC.End			();

	// Postprocess
	Target.phase_combine					();
	
	// HUD
	Device.Statistic.RenderDUMP_HUD.Begin	();
	pCreator->pHUD->Render_Direct			();
	Device.Statistic.RenderDUMP_HUD.End		();

	// pCreator->Environment.RenderFirst	();
	// pCreator->Environment.RenderLast		();
}
