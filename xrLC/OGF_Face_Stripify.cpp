#include "stdafx.h"
#include "build.h"
#include "ogf_face.h"

#include "NV_Library\NvTriStrip.h"
#include "NV_Library\VertexCache.h"

int xrSimulate (std::vector<WORD> &indices, int iCacheSize )
{
	VertexCache C(iCacheSize);

	int count=0;
	for (DWORD i=0; i<indices.size(); i++)
	{
		int id = indices[i];
		if (C.InCache(id)) continue;
		count++;
		C.AddEntry(id);
	}
	return count;
}

void xrStripify(std::vector<WORD> &indices, std::vector<WORD> &perturb, int iCacheSize, int iMinStripLength)
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
	CopyMemory(indices.begin(),xPGROUP->indices,indices.size()*sizeof(WORD));

	// Release memory
	delete[]	xPGROUP;
	delete[]	PGROUP;
}

void OGF::Stripify()
{
	if (I_Current>=0)			return;	// Mesh already progressive - don't stripify it
	if (!g_params.m_bStripify)	return; // user forced no strippification at all

	set_status("Stripifying",treeID,faces.size(),vertices.size());
	try {
		vector<WORD>	indices,permute;
		
		// Stripify
		WORD* F			= (WORD*)faces.begin(); 
		indices.assign	(F,F+(faces.size()*3));
		permute.resize	(vertices.size());
		xrStripify		(indices,permute,g_params.m_vCacheSize,0);
		
		// Copy faces
		CopyMemory(faces.begin(),indices.begin(),indices.size()*sizeof(WORD));
		
		// Permute vertices
		vecOGF_V temp_list = vertices;
		for(DWORD i=0; i<temp_list.size(); i++)
			vertices[i]=temp_list[permute[i]];
	} catch (...) {
		Log("ERROR: Stripifying failed. Dump below.");
		DumpFaces();
	}
}

void OGF::DumpFaces()
{
	for (DWORD i=0; i<faces.size(); i++)
		Msg("Face #%4d: %4d %4d %4d",i,int(faces[i].v[0]),int(faces[i].v[1]),int(faces[i].v[2]));
}
