#include "stdafx.h"
#include "build.h"

extern BOOL	hasImplicitLighting		(Face* F);
extern void Detach					(vecFace* S);

/*
WORD					dwMaterial;		// index of material
svector<_TCF,8>			tc;				// TC
svector<CLightmap*,8>	lmap_layers;
*/

IC		BOOL	FaceEqual(Face* F1, Face* F2)
{
	if (F1->dwMaterial  != F2->dwMaterial)					return FALSE;
	if (F1->tc.size()	!= F2->tc.size())					return FALSE;
	if (F1->lmap_layers.size() != F2->lmap_layers.size())	return FALSE;
	for (DWORD it=0; it<F1->lmap_layers.size(); it++)		
		if (F1->lmap_layers[it] != F2->lmap_layers.size())	return FALSE;
	return TRUE;
}

void CBuild::xrPhase_MergeGeometry	()
{
}