// FProgressive.cpp: implementation of the FProgressive class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FProgressive.h"
#include "fmesh.h"
#include "fstaticrender.h"

FBasicVisual*	FProgressive::CreateInstance(void)
{	return new FProgressive;	}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

FProgressive::FProgressive	() : FBasicVisual()
{

}

FProgressive::~FProgressive	()
{

}

void FProgressive::Release	()
{
	FBasicVisual::Release	();

	for (DWORD I=0; I<LODs.size(); I++)
		LODs[I].P.Release();
}

void FProgressive::Load		(const char* N, CStream *data, DWORD dwFlags)
{
	FBasicVisual::Load(N,data,dwFlags);
	
	LODs.reserve(8);
	CStream*	lods = data->OpenChunk	(OGF_P_LODS);
	R_ASSERT	(lods);
	DWORD		lod_id=0;
	while (lods->FindChunk(lod_id))
	{
		CStream* ONE = lods->OpenChunk(lod_id);
		primLOD	LOD;
		
		// read vertices
		DWORD dwVertStart=0;
		if (ONE->FindChunk(OGF_VCONTAINER)) 
		{
			DWORD ID		= ONE->Rdword();
			dwVertStart		= ONE->Rdword();
			LOD.dwVertCount	= ONE->Rdword();
			LOD.P.VB_Attach	(::Render.FVF[ID],::Render.VB[ID]);
		} else {
			R_ASSERT(ONE->FindChunk(OGF_VERTICES));
			
			DWORD dwVertType;
			
			dwVertStart		= 0;
			dwVertType		= ONE->Rdword();
			LOD.dwVertCount	= ONE->Rdword();
			
			// But texture coords must be expanded
			LOD.P.VB_Create	(dwVertType,LOD.dwVertCount,
				D3DUSAGE_WRITEONLY | ((dwFlags&VLOAD_FORCESOFTWARE)?D3DUSAGE_SOFTWAREPROCESSING:0),
				dwVertType,ONE->Pointer());
		}
		LOD.P.IB_SetBase(dwVertStart);
		
		// indices
		LOD.dwPrimsCount = 0;

		R_ASSERT(ONE->FindChunk(OGF_INDICES));
		DWORD	dwCount = ONE->Rdword();
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
	Device.Primitive.Draw	(L.P,L.dwVertCount,L.dwPrimsCount);
}

#define PCOPY(a)	a = pFrom->a
void	FProgressive::Copy		(FBasicVisual *pSrc)
{
	FBasicVisual::Copy(pSrc);

	FProgressive	*pFrom = (FProgressive *)pSrc;
	PCOPY(LODs);
}
