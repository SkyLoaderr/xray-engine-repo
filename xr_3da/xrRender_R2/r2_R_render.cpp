#include "stdafx.h"

extern float		r_ssaDISCARD;
extern float		r_ssaDONTSORT;
extern float		r_ssaLOD_A;
extern float		r_ssaLOD_B;
extern float		r_ssaHZBvsTEX;

void CRender::Render	()
{
	Device.Statistic.RenderDUMP.Begin();

	// Target.set_gray					(.5f+sinf(Device.fTimeGlobal)/2.f);
	Target.Begin					();


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
		rmNear						();
		mapHUD.traverseLR			(sorted_L1);
		mapHUD.clear				();
		rmNormal					();

		// Restore projection
		Device.mProject				= Pold;
		Device.mFullTransform		= FTold;
		RCache.set_xform_project	(Device.mProject);
	}

	// Environment render
	
	// NORMAL			*** mostly the main level
	// Perform sorting based on ScreenSpaceArea
	RCache.set_xform_world			(Fidentity);

	// Sorting by SSA and changes minimizations
	if (0==mapNormal[pr][0].size())	continue;

	SceneGraph::mapNormalVS&	vs	= mapNormal	[pr];
	vs.getANY_P					(lstVS);
	std::sort					(lstVS.begin(), lstVS.end(), cmp_vs);
	for (u32 vs_id=0; vs_id<lstVS.size(); vs_id++)
	{
		SceneGraph::mapNormalVS::TNode*	Nvs			= lstVS[vs_id];
		RCache.set_VS					(Nvs->key);

		SceneGraph::mapNormalPS&		ps			= Nvs->val;
		ps.getANY_P						(lstPS);
		for (u32 texture_id=0; texture_id<lstTextures.size(); texture_id++)
		{
		}
		lstTextures.clear		();
		lstTexturesTemp.clear	();
		textures.clear			();
		textures.ssa			= 0;
	}
	lstVS.clear					();
	vs.clear					();

	//-----------------------------------
	RCache.set_xform_world	(Fidentity);
	Details.Render			(Device.vCameraPosition);

	pCreator->Environment.RenderFirst	();

	// NORMAL-matrix		*** actors and dyn. objects
	mapMatrix.traverseANY	(matrix_L1);
	mapMatrix.clear			();

	RCache.set_xform_world	(Fidentity);
	Wallmarks->Render		();		// Wallmarks has priority as normal geometry

	RCache.set_xform_world	(Fidentity);
	L_Dynamic.Render		();		// L_DB has priority the same as normal geom

	RCache.set_xform_world	(Fidentity);
	L_Shadows.render		();

	// LODs
	flush_LODs				();
	
	// Sorted (back to front)
	mapSorted.traverseRL	(sorted_L1);
	mapSorted.clear			();

	// Glows
	Glows.Render			();

	// HUD
	Device.Statistic.RenderDUMP_HUD.Begin	();
	pCreator->pHUD->Render_Affected			();
	Device.Statistic.RenderDUMP_HUD.End		();

	// Patches
	if (vecPatches.size())  {
		flush_Patches	();
	}

	pCreator->Environment.RenderLast		();
	// L_Projector.render					();
	
	// Postprocess
	Target.End				();
	L_Projector.finalize	();
	
	// HUD
	Device.Statistic.RenderDUMP_HUD.Begin	();
	pCreator->pHUD->Render_Direct			();
	Device.Statistic.RenderDUMP_HUD.End		();

	Device.Statistic.RenderDUMP.End();
}

