#pragma once

#include "..\ispatial.h"
#include "r__dsgraph_types.h"

class R_dsgraph_structure
{
public:
	// Dynamic scene graph
	R_dsgraph::mapNormal_T										mapNormal	[2];	// 2==(priority/2)
	R_dsgraph::mapMatrix_T										mapMatrix	[2];
	R_dsgraph::mapSorted_T										mapSorted;
	R_dsgraph::mapHUD_T											mapHUD;
	R_dsgraph::mapLOD_T											mapLOD;

	// Runtime structures
	xr_vector<R_dsgraph::mapNormalVS::TNode*>					nrmVS;
	xr_vector<R_dsgraph::mapNormalPS::TNode*>					nrmPS;
	xr_vector<R_dsgraph::mapNormalCS::TNode*>					nrmCS;
	xr_vector<R_dsgraph::mapNormalStates::TNode*>				nrmStates;
	xr_vector<R_dsgraph::mapNormalTextures::TNode*>				nrmTextures;
	xr_vector<R_dsgraph::mapNormalTextures::TNode*>				nrmTexturesTemp;
	xr_vector<R_dsgraph::mapNormalVB::TNode*>					nrmVB;

	xr_vector<R_dsgraph::mapMatrixVS::TNode*>					matVS;
	xr_vector<R_dsgraph::mapMatrixPS::TNode*>					matPS;
	xr_vector<R_dsgraph::mapMatrixCS::TNode*>					matCS;
	xr_vector<R_dsgraph::mapMatrixStates::TNode*>				matStates;
	xr_vector<R_dsgraph::mapMatrixTextures::TNode*>				matTextures;
	xr_vector<R_dsgraph::mapMatrixTextures::TNode*>				matTexturesTemp;
	xr_vector<R_dsgraph::mapMatrixVB::TNode*>					matVB;

	xr_vector<R_dsgraph::_LodItem>								lstLODs;
	xr_vector<int>												lstLODgroups;
	xr_vector<ISpatial*>										lstRenderables;
public:
	void		r_dsgraph_insert_dynamic						(IRender_Visual	*pVisual, Fvector& Center);
	void		r_dsgraph_insert_static							(IRender_Visual	*pVisual);

	void		r_dsgraph_render_graph							(u32			_priority);
	void		r_dsgraph_render_hud							();
	void		r_dsgraph_render_lods							();
};
