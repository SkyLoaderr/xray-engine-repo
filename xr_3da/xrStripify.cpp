#include "stdafx.h"
#include "xrstripify.h"

#include "NvTriStrip.h"
#include "VertexCache.h"

int xrSimulate (std::vector<WORD> &indices, int iCacheSize )
{
	VertexCache C(iCacheSize);

	int count=0;
	for (DWORD i=0; i<indices.size(); i++)
	{
		int id = indices[i];
		if (C.InCache(id)) continue;
		count			++;
		C.AddEntry		(id);
	}
	return count;
}

void xrStripify		(std::vector<WORD> &indices, std::vector<WORD> &perturb, int iCacheSize, int iMinStripLength)
{
	SetCacheSize	(iCacheSize);
	SetMinStripSize	(iMinStripLength);
	SetListsOnly	(true);

	// Generate strips
	PrimitiveGroup*	PGROUP=0;
	WORD			PGROUP_Count=0;
	GenerateStrips	(indices.begin(),indices.size(),&PGROUP,&PGROUP_Count);
	R_ASSERT		(PGROUP);
	R_ASSERT		(PGROUP_Count==1);
	R_ASSERT		(PGROUP->type==PT_LIST);
	R_ASSERT		(indices.size()==PGROUP->numIndices);

	// Remap indices
	PrimitiveGroup*	xPGROUP=0;
	RemapIndices	(PGROUP,PGROUP_Count,perturb.size(),&xPGROUP);
	R_ASSERT		(xPGROUP);

	// Build perturberation table
	for(DWORD index = 0; index < PGROUP->numIndices; index++)
	{
		int oldIndex = PGROUP->indices	[index];
		int newIndex = xPGROUP->indices	[index];
		R_ASSERT(oldIndex<(int)perturb.size());
		R_ASSERT(newIndex<(int)perturb.size());
		perturb[newIndex] = oldIndex;
	}

	// Copy indices
	CopyMemory	(indices.begin(),xPGROUP->indices,indices.size()*sizeof(WORD));

	// Release memory
	delete[]	xPGROUP;
	delete[]	PGROUP;
}
