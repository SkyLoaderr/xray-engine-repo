#include "stdafx.h"
#include "xrstripify.h"

#include "NvTriStrip.h"
#include "VertexCache.h"

int xrSimulate (xr_vector<u16> &indices, int iCacheSize )
{
	VertexCache C(iCacheSize);

	int count=0;
	for (u32 i=0; i<indices.size(); i++)
	{
		int id = indices[i];
		if (C.InCache(id)) continue;
		count			++;
		C.AddEntry		(id);
	}
	return count;
}

void xrStripify		(xr_vector<u16> &indices, xr_vector<u16> &perturb, int iCacheSize, int iMinStripLength)
{
	SetCacheSize	(iCacheSize);
	SetMinStripSize	(iMinStripLength);
	SetListsOnly	(true);

	// Generate strips
	xr_vector<PrimitiveGroup>	PGROUP;
	GenerateStrips	(&*indices.begin(),indices.size(),PGROUP);
	R_ASSERT		(PGROUP.size()==1);
	R_ASSERT		(PGROUP[0].type==PT_LIST);
	R_ASSERT		(indices.size()==PGROUP[0].numIndices);

	// Remap indices
	xr_vector<PrimitiveGroup>	xPGROUP;
	RemapIndices	(PGROUP,u16(perturb.size()),xPGROUP);
	R_ASSERT		(xPGROUP.size()==1);
	R_ASSERT		(xPGROUP[0].type==PT_LIST);

	// Build perturberation table
	for(u32 index = 0; index < PGROUP[0].numIndices; index++)
	{
		u16 oldIndex = PGROUP[0].indices	[index];
		int newIndex = xPGROUP[0].indices	[index];
		R_ASSERT(oldIndex<(int)perturb.size());
		R_ASSERT(newIndex<(int)perturb.size());
		perturb[newIndex] = oldIndex;
	}

	// Copy indices
	Memory.mem_copy	(&*indices.begin(),xPGROUP[0].indices,indices.size()*sizeof(u16));

	// Release memory
	xPGROUP.clear	();
	PGROUP.clear	();
}
