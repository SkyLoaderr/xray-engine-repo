// FVisual.cpp: implementation of the FVisual class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "fmesh.h"
#include "fvisual.h"
#include "ffileops.h"

FBasicVisual*	Fvisual::CreateInstance(void)
{	return new Fvisual;	}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

_E(Fvisual::Fvisual()  : FBasicVisual())
	pVertexBuffer	= NULL;
	Indices.clear	();
_end;

_E(Fvisual::~Fvisual())
	_RELEASE(pVertexBuffer);
	Indices.clear();
_end;

_E(void Fvisual::Load(CStream *data))
	FBasicVisual::Load(data);

	// read vertices
	if (data->FindChunk(OGF_VERTICES)) {
		DWORD dwVertType, dwDestVertType;

		dwVertType  = data->ReadDWORD();
		dwVertCount = data->ReadDWORD();
		dwDestVertType = D3DFVF_XYZ | D3DFVF_DIFFUSE;
		pVertexBuffer = VB.CreateFromData(
			dwDestVertType, dwVertType, dwVertCount, data->Pointer());
	} else {
		VERIFY2(0,"Can't find OGF_VERTICES");
		THROW;
	}

	// indices
	if (data->FindChunk(OGF_INDICES)) {
		DWORD dwCount = data->ReadDWORD();
		VERIFY(dwCount%3 == 0);
		Indices.resize(dwCount);
		data->Read(Indices.begin(), sizeof(WORD)*dwCount);
	} else {
		VERIFY2(0,"Can't find OGF_INDICES");
		THROW;
	}

//	Msg("   %d vertices, %d polys.",dwVertCount,Indices.size()/3);

	// material
	if (!data->ReadChunk(OGF_MATERIAL,&mmx)) {
		VERIFY2(0,"Can't find OGF_MATERIAL");
		THROW;
	}
_end;

_E(void Fvisual::Render(float LOD))
	CHK_DX(Device()->DrawIndexedPrimitiveVB(
		D3DPT_TRIANGLELIST,
		pVertexBuffer,
		0,
		dwVertCount,
		Indices.begin(),
		Indices.size(),
		0
	));
_end;

_E(void Fvisual::PerformLighting(void))
	VERIFY(pVertexBuffer);
_end;

_E(void Fvisual::PerformOptimize(void))
	VERIFY(pVertexBuffer);
	CHK_DX(pVertexBuffer->Optimize(Device(),0));
	SortFaces(Indices.begin(),Indices.size());
_end;

_E(void Fvisual::Dump(void))
	FBasicVisual::Dump();
_end;

#define PCOPY(a)	a = pFrom->a
void	Fvisual::Copy(FBasicVisual *pSrc)
{
	FBasicVisual::Copy(pSrc);

	Fvisual	*pFrom = (Fvisual *)pSrc;
	PCOPY(pVertexBuffer);	pVertexBuffer->AddRef();
	PCOPY(dwVertCount);
	CopyMemory (&mmx,&pFrom->mmx,sizeof(Fmaterial));
	Indices.clear();
	Indices.insert(Indices.end(),pFrom->Indices.begin(),pFrom->Indices.end());
}
