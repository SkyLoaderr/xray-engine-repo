// FProgressive.cpp: implementation of the FProgressive class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#include "../fmesh.h"
#include "FProgressive.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

FProgressive::FProgressive	() : Fvisual()
{
	pSWI			= 0;
	last_lod		= 0;
}

FProgressive::~FProgressive	()
{

}

void FProgressive::Release	()
{
	Fvisual::Release();
	xr_free			(pSWI->sw);
	xr_delete		(pSWI);
}

void FProgressive::Load		(const char* N, IReader *data, u32 dwFlags)
{
	Fvisual::Load	(N,data,dwFlags);

	IReader* lods 	= data->open_chunk	(OGF_SWIDATA);
	pSWI			= xr_new<FSlideWindowItem>();
	R_ASSERT		(lods);
    pSWI->reserved[0]= lods->r_u32();	// reserved 16 bytes
    pSWI->reserved[1]= lods->r_u32();
    pSWI->reserved[2]= lods->r_u32();
    pSWI->reserved[3]= lods->r_u32();
    pSWI->count		= lods->r_u32();
    pSWI->sw		= xr_alloc<FSlideWindow>(pSWI->count);
    for (u32 it=0; it<pSWI->count; it++){
    	FSlideWindow& W = pSWI->sw[it];
    	W.offset 	= lods->r_u32();
    	W.num_tris	= lods->r_u16();
    	W.num_verts	= lods->r_u16();
    }
}

void FProgressive::Render	(float LOD)
{
	int lod_id		= last_lod;
	if (LOD>=0.f){
		clamp		(LOD,0.f,1.f);
		lod_id		= iFloor((1.f-LOD)*float(pSWI->count-1)+0.5f);
		last_lod	= lod_id;
	}
	VERIFY			(lod_id>=0 && lod_id<int(pSWI->count));
	FSlideWindow& SW= pSWI->sw[lod_id];
	RCache.set_Geometry	(geom);
	RCache.Render		(D3DPT_TRIANGLELIST,vBase,0,SW.num_verts,iBase+SW.offset,SW.num_tris);
}

#define PCOPY(a)	a = pFrom->a
void	FProgressive::Copy	(IRender_Visual *pSrc)
{
	Fvisual::Copy	(pSrc);
	FProgressive	*pFrom = (FProgressive *)pSrc;
	PCOPY			(pSWI);
}
