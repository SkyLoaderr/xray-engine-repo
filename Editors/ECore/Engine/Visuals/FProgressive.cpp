// FProgressive.cpp: implementation of the FProgressive class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#include "FProgressive.h"
#include "fmesh.h"

#include "render.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

FProgressive::FProgressive	() : Fvisual()
{
	pSWs		= 0;
	SW_count 	= 0;
}

FProgressive::~FProgressive	()
{

}

void FProgressive::Release	()
{
	Fvisual::Release	();
	xr_free				(pSWs);
}

void FProgressive::Load		(const char* N, IReader *data, u32 dwFlags)
{
	Fvisual::Load(N,data,dwFlags);
	
	IReader* lods 	= data->open_chunk	(OGF_P_LODS);
	R_ASSERT		(lods);
    u32 reserved;
    reserved		= lods->r_u32();	// reserved 16 bytes
    reserved		= lods->r_u32();
    reserved		= lods->r_u32();
    reserved		= lods->r_u32();
    SW_count		= lods->r_u32();
    pSWs			= xr_alloc<SlideWindow>(SW_count);
    for (u32 it=0; it<SW_count; it++){
    	SlideWindow& W = pSWs[it];
    	W.offset 	= lods->r_u32();
    	W.num_tris	= lods->r_u16();
    	W.num_verts	= lods->r_u16();
    }
}

void FProgressive::Render	(float LOD)
{
	int lod_id				= iFloor((1.f-LOD)*float(SW_count-1));
	VERIFY					(lod_id>=0 && lod_id<int(SW_count));
	SlideWindow& SW			= pSWs[lod_id];
	RCache.set_Geometry		(hGeom);
	RCache.Render			(D3DPT_TRIANGLELIST,vBase,0,SW.num_verts,iBase+SW.offset,SW.num_tris);
}

#define PCOPY(a)	a = pFrom->a
void	FProgressive::Copy	(IRender_Visual *pSrc)
{
	Fvisual::Copy	(pSrc);
	FProgressive	*pFrom = (FProgressive *)pSrc;
	PCOPY			(SW_count);
	PCOPY			(pSWs);
}
