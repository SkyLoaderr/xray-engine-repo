// FStaticVisibility.cpp: implementation of the FStaticVisibility class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ftimer.h"
#include "FStaticVisibility.h"
#include "ffileops.h"
#include "x_ray.h"
#include "xr_smallfont.h"
#include "xrLevel.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
FStaticVisibility::FStaticVisibility()
{
	pMap	=0;
	pLights	=0;
	pNodes	=0;
}

FStaticVisibility::~FStaticVisibility()
{
}

// Warning!!! Assuming virtual,paging,read-only stream
void FStaticVisibility::Load(CStream *fs)
{
	DWORD size;

	R_ASSERT(fs);

	R_ASSERT(fs->ReadChunk(fsV_HEADER,&vHeader));

	// nodes
	size	= fs->FindChunk(fsV_NODES);	R_ASSERT(size);
	pNodes	= (DWORD*)fs->Pointer();

	// lights
	size	= fs->FindChunk(fsV_LIGHTS);R_ASSERT(size);
	pLights	= (DWORD*)fs->Pointer();

	// glows
	size	= fs->FindChunk(fsV_GLOWS);	R_ASSERT(size);
	pGlows	= (DWORD*)fs->Pointer();

	// occluders
	size	= fs->FindChunk(fsV_OCCLUDERS);R_ASSERT(size);
	pOccluders=(DWORD*)fs->Pointer();

	// map
	size	= fs->FindChunk(fsV_MAP);	R_ASSERT(size);
	pMap	= (DWORD*)fs->Pointer();
}

#define RRC		4	// Relevance Records Count
#define RR_GE	0
#define RR_LI	1
#define RR_GL	2
#define RR_OC	3

__forceinline int convert(Fvector &pos, int id, V_Header &H)
{
	int T=iROUND((pos[id]-H.start[id])/H.relevance);
	clamp(T,0,int(H.N[id]-1));
	return T;
}

DWORD FStaticVisibility::RelevantGeometry(Fvector &pos, WORD **G)
{
	// 3D Selection
	int px=convert(pos,0,vHeader);
	int py=convert(pos,1,vHeader);
	int pz=convert(pos,2,vHeader);

	DWORD*	pSlot = pMap + pz*vHeader.nX*vHeader.nY*RRC + px*vHeader.nY*RRC + py*RRC + RR_GE;
	WORD *P = LPWORD((char *)pNodes + *pSlot);
	*G = P+1;
	return *P;
}

DWORD FStaticVisibility::RelevantLights(Fvector &pos, WORD **L)
{
	// 3D Selection
	int px=convert(pos,0,vHeader);
	int py=convert(pos,1,vHeader);
	int pz=convert(pos,2,vHeader);

	DWORD*	pSlot = pMap + pz*vHeader.nX*vHeader.nY*RRC + px*vHeader.nY*RRC + py*RRC + RR_LI;
	WORD *P = LPWORD((char *)pLights + *pSlot);
	*L = P+1;
	return *P;
}

DWORD FStaticVisibility::RelevantGlows(Fvector &pos, WORD **G)
{
	// 3D Selection
	int px=convert(pos,0,vHeader);
	int py=convert(pos,1,vHeader);
	int pz=convert(pos,2,vHeader);

	DWORD*	pSlot = pMap + pz*vHeader.nX*vHeader.nY*RRC + px*vHeader.nY*RRC + py*RRC + RR_GL;
	WORD *P = LPWORD((char *)pGlows + *pSlot);
	*G = P+1;
	return *P;
}

DWORD FStaticVisibility::RelevantOccluders(Fvector &pos, WORD **O)
{
	// 3D Selection
	int px=convert(pos,0,vHeader);
	int py=convert(pos,1,vHeader);
	int pz=convert(pos,2,vHeader);

	DWORD*	pSlot = pMap + pz*vHeader.nX*vHeader.nY*RRC + px*vHeader.nY*RRC + py*RRC + RR_OC;
	WORD *P = LPWORD((char *)pOccluders + *pSlot);
	*O = P+1;
	return *P;
}
