// xrSTRIPS.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "NV_Library\NvTriStrip.h"
#include "NV_Library\VertexCache.h"
#include "xrStrips.h"

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////////////
// OptimizeVertices()
//
// Function which optimizes the vertices in the mesh to minimize page misses
//
//
/*
void OptimizeVertices(NvFaceInfoVec& Faces, WordVec& perturb)
{
	int iVertexCount = perturb.size();

	//caches oldIndex --> newIndex conversion
	int *indexCache = new int[iVertexCount];
	memset(indexCache, -1, sizeof(int)*iVertexCount);
	
	int iPerturbWP	= 0;
	for(int i = 0; i < Faces.size(); i++)
	{
		int v0 = Faces[i]->m_v0;
		int v1 = Faces[i]->m_v1;
		int v2 = Faces[i]->m_v2;
		
		//v0
		int index = indexCache[v0];
		if(index == -1)
		{
			perturb[iPerturbWP++] = v0;
			Faces[i]->m_v0 = iPerturbWP - 1;
			indexCache[v0] = Faces[i]->m_v0;
		}
		else
		{
			Faces[i]->m_v0 = index;
		}
		
		//v1
		index = indexCache[v1];
		if(index == -1)
		{
			perturb[iPerturbWP++] = v1;
			Faces[i]->m_v1 = iPerturbWP - 1;        
			indexCache[v1] = Faces[i]->m_v1;
		}
		else
		{
			Faces[i]->m_v1 = index;
		}
		
		//v2
		index = indexCache[v2];
		if(index == -1)
		{
			perturb[iPerturbWP++] = v2;
			Faces[i]->m_v2 = iPerturbWP - 1;  
			
			indexCache[v2] = Faces[i]->m_v2;
		}
		else
		{
			Faces[i]->m_v2 = index;
		}
	}

	delete[] indexCache;
	R_ASSERT(iPerturbWP == iVertexCount);
}

void DumpFaces(FILE* F, char* title, std::vector<WORD> &indices)
{
	int cnt = indices.size()/3;
	fprintf(F,"***FACE LIST (%s)\n",title);
	for (int i=0; i<cnt; i++)
		fprintf(F,"#%3d: %3d %3d %3d\n",i,indices[i*3+0],indices[i*3+1],indices[i*3+2]);
}
void Simulator(FILE* F, char *T, int iCacheSize, std::vector<WORD> &indices)
{
	fprintf(F,"%16s: %3d(%2d), %3d(%2d), %3d(%2d), %3d(%2d), %3d(%2d), %3d(%2d)\n",
		T,
		Simulate(iCacheSize,  indices),iCacheSize,
		Simulate(iCacheSize+1,indices),iCacheSize+1,
		Simulate(iCacheSize+2,indices),iCacheSize+2,
		Simulate(20,indices),20,
		Simulate(24,indices),24,
		Simulate(12,indices),12,
		Simulate(iCacheSize-1,indices),iCacheSize-1
		);
}
SP_API void __cdecl xrStripify (std::vector<WORD> &indices, std::vector<WORD> &perturb, int iCacheSize, int iMinStripLength)
{
	NvStripifier *stripifier = new NvStripifier();
	
	//actually do the stripification
	NvStripInfoVec	strips;
	NvFaceInfoVec	leftoverFaces;
	WordVec			stripIndices;
	stripifier->Stripify(indices, iCacheSize, iMinStripLength, strips, leftoverFaces);
	delete stripifier;

	// convert to huge triangle list
	NvFaceInfoVec	faces;
	faces.reserve	(indices.size()/3);
	for(int i = 0; i < strips.size(); i++)
		faces.insert(faces.end(),strips[i]->m_faces.begin(),strips[i]->m_faces.end());
	faces.insert(faces.end(),leftoverFaces.begin(),leftoverFaces.end());
	R_ASSERT(faces.size()*3 == indices.size());

	// reorder vertices to minimize page misses
	OptimizeVertices(faces,perturb);

	// transfer to indices
	int wptr = 0, vcnt=perturb.size();
	for (i=0; i<faces.size(); i++)
	{
		int id;

		id = faces[i]->m_v0; R_ASSERT(id<vcnt); indices[wptr++]=id;
		id = faces[i]->m_v1; R_ASSERT(id<vcnt); indices[wptr++]=id;
		id = faces[i]->m_v2; R_ASSERT(id<vcnt); indices[wptr++]=id;
	}
	// Simulator(F,"After",iCacheSize,indices);
	// fclose(F);
}
*/
SP_API int __cdecl xrSimulate (std::vector<WORD> &indices, int iCacheSize )
{
	VertexCache C(iCacheSize);

	int count=0;
	for (int i=0; i<indices.size(); i++)
	{
		int id = indices[i];
		if (C.InCache(id)) continue;
		count++;
		C.AddEntry(id);
	}
	return count;
}

SP_API void __cdecl xrStripify(std::vector<WORD> &indices, std::vector<WORD> &perturb, int iCacheSize, int iMinStripLength)
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
	for(int index = 0; index < PGROUP->numIndices; index++)
	{
		int oldIndex = PGROUP->indices	[index];
		int newIndex = xPGROUP->indices	[index];
		R_ASSERT(oldIndex<perturb.size());
		R_ASSERT(newIndex<perturb.size());
		perturb[newIndex] = oldIndex;
	}

	// Copy indices
	CopyMemory(indices.begin(),xPGROUP->indices,indices.size()*sizeof(WORD));

	// Release memory
	delete[]	xPGROUP;
	delete[]	PGROUP;
}
