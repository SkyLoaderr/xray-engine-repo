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
	xSWI			= 0;
	last_lod		= 0;
}

FProgressive::~FProgressive	()
{

}

void FProgressive::Release	()
{
	Fvisual::Release();
	if (pSWI)		{
		xr_free			(nSWI.sw);
		xr_delete		(pSWI);
	}
	if (xSWI)		{
		xr_free			(xSWI->sw);
		xr_delete		(xSWI);
	}
}

void FProgressive::Load		(const char* N, IReader *data, u32 dwFlags)
{
	Fvisual::Load	(N,data,dwFlags);

	// normal SWI
	destructor<IReader> lods (data->open_chunk	(OGF_SWIDATA));
    nSWI.reserved[0]	= lods()->r_u32();	// reserved 16 bytes
    nSWI.reserved[1]	= lods()->r_u32();
    nSWI.reserved[2]	= lods()->r_u32();
    nSWI.reserved[3]	= lods()->r_u32();
    nSWI.count			= lods()->r_u32();
    nSWI.sw				= xr_alloc<FSlideWindow>(nSWI.count);
	lods()->r			(nSWI.sw,nSWI.count*sizeof(FSlideWindow));

	// fast
	if (m_fast)			{
		destructor<IReader>	geomdef	(data->open_chunk		(OGF_FASTPATH));
		destructor<IReader>	def		(geomdef()->open_chunk	(OGF_SWIDATA));

		xSWI				= xr_new<FSlideWindowItem>();
		xSWI->reserved[0]	= def()->r_u32();	// reserved 16 bytes
		xSWI->reserved[1]	= def()->r_u32();
		xSWI->reserved[2]	= def()->r_u32();
		xSWI->reserved[3]	= def()->r_u32();
		xSWI->count			= def()->r_u32();
		xSWI->sw			= xr_alloc<FSlideWindow>(xSWI->count);
		def()->r			(xSWI->sw,xSWI->count*sizeof(FSlideWindow));
	}
}

void FProgressive::Render	(float LOD)
{
	int lod_id		= last_lod;
	if (LOD>=0.f){
		clamp		(LOD,0.f,1.f);
		lod_id		= iFloor((1.f-LOD)*float(nSWI.count-1)+0.5f);
		last_lod	= lod_id;
	}
	VERIFY			(lod_id>=0 && lod_id<int(nSWI.count));
	FSlideWindow& SW	= nSWI.sw[lod_id];
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
