#include "stdafx.h"
#include "..\fbasicvisual.h"
#include "..\customhud.h"

void CRender::Render	()
{
	//******* Main calc
	Device.Statistic.RenderCALC.Begin		();
	{
		marker									++;
		phase									= PHASE_NORMAL;

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
		Target.phase_scene						();

		render_hud								();
		render_scenegraph						();
		Details.Render							(Device.vCameraPosition);
		// mapSorted.traverseRL					(sorted_L1);
		mapSorted.clear							();
	}
	Device.Statistic.RenderDUMP.End			();

	for (u32 dls_phase=0; dls_phase<1; dls_phase++)
	{
		//******* Direct lighting+shadow		::: Calculate
		Device.Statistic.RenderCALC.Begin		();
		{
			marker									++;
			phase									= PHASE_SMAP_D;

			HOM.Disable								();
			LR_Direct.compute_xf_1					(dls_phase);
			render_smap_direct						(LR_Direct.L_combine);
			LR_Direct.compute_xf_2					(dls_phase);
		}
		Device.Statistic.RenderCALC.End			();

		//******* Direct lighting+shadow		::: Render
		Device.Statistic.RenderDUMP.Begin		();
		{
			Target.phase_smap_direct				();

			RCache.set_xform_world					(Fidentity);
			RCache.set_xform_view					(LR_Direct.L_view);
			RCache.set_xform_project				(LR_Direct.L_project);
			render_scenegraph						();
		}
		Device.Statistic.RenderDUMP.End			();

		//******* Direct lighting+shadow		::: Accumulate
		Device.Statistic.RenderDUMP.Begin		();
		{
			Target.phase_accumulator				();
			Target.accum_direct						(dls_phase);
		}
		Device.Statistic.RenderDUMP.End			();
	}

	// Postprocess
	Target.phase_combine					();
	
	// HUD
	Device.Statistic.RenderDUMP_HUD.Begin	();
	pCreator->pHUD->Render_Direct			();
	Device.Statistic.RenderDUMP_HUD.End		();

	// pCreator->Environment.RenderFirst	();
	// pCreator->Environment.RenderLast		();
}
