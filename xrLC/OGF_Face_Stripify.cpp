#include "stdafx.h"
#include "build.h"
#include "ogf_face.h"

#include "NV_Library\NvTriStrip.h"
#include "NV_Library\VertexCache.h"

int xrSimulate (xr_vector<u16> &indices, int iCacheSize )
{
	VertexCache C(iCacheSize);

	int count=0;
	for (u32 i=0; i<indices.size(); i++)
	{
		int id = indices[i];
		if (C.InCache(id)) continue;
		count++;
		C.AddEntry(id);
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
	GenerateStrips	(&*indices.begin(),(u32)indices.size(),PGROUP);
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
	Memory.mem_copy	(&*indices.begin(),xPGROUP[0].indices,(u32)indices.size()*sizeof(u16));

	// Release memory
	xPGROUP.clear	();
	PGROUP.clear	();
}

void OGF::Stripify	()
{
	if (m_SWI.count||(I_Current>=0))	return;	// Mesh already progressive - don't stripify it

	// fast verts
	try {
		xr_vector<u16>	indices,permute;

		// Stripify
		u16* F			= (u16*)&*x_faces.begin(); 
		indices.assign	(F,F+(faces.size()*3)	);
		permute.resize	(x_vertices.size()		);
		xrStripify		(indices,permute,c_vCacheSize,0);

		// Copy faces
		CopyMemory		(&*x_faces.begin(),&*indices.begin(),(u32)indices.size()*sizeof(u16));

		// Permute vertices
		vecOGF_V temp_list = x_vertices;
		for(u32 i=0; i<temp_list.size(); i++)
			x_vertices[i]=temp_list[permute[i]];
	} catch (...)	{
		clMsg		("ERROR: [fast-vert] Stripifying failed. Dump below.");
	}

	// normal verts
	try {
		xr_vector<u16>	indices,permute;
		
		// Stripify
		u16* F			= (u16*)&*faces.begin(); 
		indices.assign	(F,F+(faces.size()*3));
		permute.resize	(vertices.size());
		xrStripify		(indices,permute,c_vCacheSize,0);
		
		// Copy faces
		CopyMemory		(&*faces.begin(),&*indices.begin(),(u32)indices.size()*sizeof(u16));
		
		// Permute vertices
		vecOGF_V temp_list = vertices;
		for(u32 i=0; i<temp_list.size(); i++)
			vertices[i]=temp_list[permute[i]];
	} catch (...)	{
		clMsg		("ERROR: [slow-vert] Stripifying failed. Dump below.");
		DumpFaces	();
	}
}

void OGF::DumpFaces()
{
	for (u32 i=0; i<faces.size(); i++)
		clMsg("Face #%4d: %4d %4d %4d",i,int(faces[i].v[0]),int(faces[i].v[1]),int(faces[i].v[2]));
}
