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
		HOM.Render								(ViewBase);
		rmNormal								();

		// Calculate sector(s) and their objects
		set_Object								(0);
		if (0!=pLastSector) pLastSector->Render	(ViewBase);
		pCreator->pHUD->Render_Calculate		();
	}
	Device.Statistic.RenderCALC.End			();

	//******* Main render
	Device.Statistic.RenderDUMP.Begin();
	{
		Target.phase_scene					();

		render_hud							();
		render_scenegraph					();
		Details.Render						(Device.vCameraPosition);
		// mapSorted.traverseRL				(sorted_L1);
		mapSorted.clear						();
	}
	Device.Statistic.RenderDUMP.End();

	//******* Direct lighting shadow
	HOM.Disable								();
	LR_Direct.compute_xf_1					();

	
	// Postprocess
	Target.phase_combine					();
	
	// HUD
	Device.Statistic.RenderDUMP_HUD.Begin	();
	pCreator->pHUD->Render_Direct			();
	Device.Statistic.RenderDUMP_HUD.End		();

	// pCreator->Environment.RenderFirst	();
	// pCreator->Environment.RenderLast		();
}
