// FBasicVisual.cpp: implementation of the FBasicVisual class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FBasicVisual.h"
#include "ffileops.h"

FBasicVisual*	FBasicVisual::CreateInstance(void)
{	return new FBasicVisual;	}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

_E(FBasicVisual::FBasicVisual())
	Type		= 0;
	dwRMode		= 0;
	fRadius		= 1.f;
	vPosition.set(0,0,0);

	min.set		(0,0,0);
	max.set		(0,0,0);

_end;

_E(FBasicVisual::~FBasicVisual())
_end;

_E(void FBasicVisual::Load(CStream *data))
	// header
	VERIFY(data);
	ogf_header hdr;
	if (data->ReadChunk(OGF_HEADER,&hdr))
	{
		VERIFY(hdr.format_version==2);
		Type			= hdr.type;
		dwRMode			= hdr.flags;
	} else {
		THROW;
	}

	// BBox
	ogf_bbox bbox;
	if (data->ReadChunk(OGF_BBOX,&bbox))
	{
		min.set (bbox.min);
		max.set (bbox.max);
		vPosition.sub(max,min);
		vPosition.div(2);
		fRadius  = vPosition.magnitude();
		vPosition.add(min);
//		Log("     Pos:", vPosition);
	} else {
		THROW;
	}
_end;

_E(void FBasicVisual::Dump())
	switch (Type) {
	case MT_NORMAL:
		Log("* Visual type:        normal mesh");
		break;
	case MT_PROGRESSIVE:
		Log("* Visual type:        progressive mesh");
		break;
	case MT_PROGRESSIVE_FIXED:
		Log("* Visual type:        progressive mesh, fixed resolution set");
		break;
	case MT_HIERRARHY:
		Log("* Visual type:        hierrarhy root/node");
		break;
	case MT_SHADOW_FORM:
		Log("* Visual type:        shadow form");
		break;
	case MT_FIELD_ARROWS:
		Log("* Visual type:        power field arrows");
		break;
	case MT_FIELD_WALLS:
		Log("* Visual type:        power field walls");
		break;
	}
	/*
	Log  ("* Texture name:      ",mHeader.sTextureName);
	Log  ("* Vertex size:       ",mHeader.dwVertSize);
	if (mHeader.dwVertType&D3DFVF_XYZ) {
		Log("*    ...contains position (xyz)");
	}
	if (mHeader.dwVertType&D3DFVF_XYZRHW) {
		Log("*    ...contains transformed position (xyz, rhw)");
	}
	if (mHeader.dwVertType&D3DFVF_NORMAL) {
		Log("*    ...contains normal (can be lit)");
	}
	if (mHeader.dwVertType&D3DFVF_DIFFUSE) {
		Log("*    ...contains diffuse color");
	}
	if (mHeader.dwVertType&D3DFVF_SPECULAR) {
		Log("*    ...contains specular color");
	}
	Log  ("*    ...contains texture coords:",(mHeader.dwVertType&0xf00)>>8);
	Log  ("* Vertex count:      ",mHeader.dwVertCount);
	Log  ("* Indices count:     ",mHeader.dwIdxCount);
	Log	 ("* Position:          ",mHeader.vPosition);
	Log  ("* Radius:            ",mHeader.fRadius);
	*/
_end;

#define PCOPY(a)	a = pFrom->a
void	FBasicVisual::Copy(FBasicVisual *pFrom)
{
	PCOPY(Type);
	PCOPY(dwRMode);
	PCOPY(vPosition);
	PCOPY(fRadius);
	CopyMemory (&min,&pFrom->min,sizeof(Fvector));
	CopyMemory (&max,&pFrom->max,sizeof(Fvector));
	// pCreator->xrStaticRender->pVisuals->Items.Add(this);
}

//************************************ Helper tools
int __cdecl _compare_faces( const void *arg1, const void *arg2 )
{
	WORD *p1 = (WORD *)arg1;
	WORD *p2 = (WORD *)arg2;
	int v1 = int(p1[0])+int(p1[1])+int(p1[2]);
	int v2 = int(p2[0])+int(p2[1])+int(p2[2]);
	return v1-v2;
}
void FBasicVisual::SortFaces(WORD *pIndices, int Count)
{
	qsort(pIndices,Count/3,3*sizeof(WORD),_compare_faces);
}
