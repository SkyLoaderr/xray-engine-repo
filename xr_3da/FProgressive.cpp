// FProgressive.cpp: implementation of the FProgressive class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#include "FProgressive.h"
#include "fmesh.h"

#ifndef _EDITOR
	#include "render.h"
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

FProgressive::FProgressive	() : IRender_Visual()
{

}

FProgressive::~FProgressive	()
{

}

void FProgressive::Release	()
{
	IRender_Visual::Release	();

	for (u32 I=0; I<LODs.size(); I++)
		LODs[I].P.Release();
}

void FProgressive::Load		(const char* N, IReader *data, u32 dwFlags)
{
	IRender_Visual::Load(N,data,dwFlags);
	
	LODs.reserve(8);
	IReader*	lods = data->open_chunk	(OGF_P_LODS);
	R_ASSERT	(lods);
	u32		lod_id=0;
	while (lods->find_chunk(lod_id))
	{
		IReader* ONE = lods->open_chunk(lod_id);
		primLOD	LOD;
		
		// read vertices
		u32 dwVertStart=0;
		if (ONE->find_chunk(OGF_VCONTAINER)) 
		{
#ifndef _EDITOR
			u32 ID		= ONE->r_u32();
			dwVertStart		= ONE->r_u32();
			LOD.dwVertCount	= ONE->r_u32();
			LOD.P.VB_Attach	(::Render->getFVF(ID),::Render->getVB(ID));
#endif
		} else {
			R_ASSERT(ONE->find_chunk(OGF_VERTICES));
			
			u32 dwVertType;
			
			dwVertStart		= 0;
			dwVertType		= ONE->r_u32();
			LOD.dwVertCount	= ONE->r_u32();
			
			// But texture coords must be expanded
			LOD.P.VB_Create	(dwVertType,LOD.dwVertCount,
				D3DUSAGE_WRITEONLY | ((dwFlags&VLOAD_FORCESOFTWARE)?D3DUSAGE_SOFTWAREPROCESSING:0),
				dwVertType,ONE->Pointer());
		}
		LOD.P.IB_SetBase(dwVertStart);
		
		// indices
		LOD.dwPrimsCount = 0;

		R_ASSERT(ONE->find_chunk(OGF_INDICES));
		u32	dwCount = ONE->r_u32();
		R_ASSERT(dwCount%3 == 0);
		
		LOD.P.IB_Create(dwVertStart,dwCount,
			D3DUSAGE_WRITEONLY | ((dwFlags&VLOAD_FORCESOFTWARE)?D3DUSAGE_SOFTWAREPROCESSING:0),
			ONE->Pointer());
		LOD.dwPrimsCount = dwCount/3;

		// prepare to next
		LODs.push_back	(LOD);
		ONE->Close		();
		lod_id			++;
	}
	lods->Close		();
}

void FProgressive::Render		(float LOD)
{
	int lod_id				= iFloor(LOD*float(LODs.size()));
	VERIFY					(lod_id>=0 && lod_id<int(LODs.size()));
	primLOD& L				= LODs[lod_id];
	RCache.Draw	(L.P,L.dwVertCount,L.dwPrimsCount);
}

#define PCOPY(a)	a = pFrom->a
void	FProgressive::Copy		(IRender_Visual *pSrc)
{
	IRender_Visual::Copy(pSrc);

	FProgressive	*pFrom = (FProgressive *)pSrc;
	PCOPY(LODs);
}
