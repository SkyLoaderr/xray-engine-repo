// FVisual.cpp: implementation of the FVisual class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#include "fvisual.h"
#include "fmesh.h"
#ifndef _EDITOR
	#include "render.h"
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Fvisual::Fvisual()  : CVisual()
{
	pVertices			= 0;
	pIndices			= 0;
}

Fvisual::~Fvisual()
{
}

void Fvisual::Release	()
{
	CVisual::Release	();
	_RELEASE			(pVertices);
	_RELEASE			(pIndices);
}

void Fvisual::Load		(const char* N, CStream *data, u32 dwFlags)
{
	CVisual::Load		(N,data,dwFlags);

	DWORD				vFormat;

	// read vertices
	if ((dwFlags&VLOAD_NOVERTICES)==0) {
		if (data->FindChunk(OGF_VCONTAINER)) {
#ifndef _EDITOR
			u32 ID				= data->Rdword				();
			vBase				= data->Rdword				();
			vCount				= data->Rdword				();
			pVertices			= ::Render->getVB			(ID);
			pVertices->AddRef	();
			vFormat				= ::Render->getFVF			(ID);
#endif
		} else {
			R_ASSERT			(data->FindChunk(OGF_VERTICES));
			vBase				= 0;
			vFormat				= data->Rdword				();
			vCount				= data->Rdword				();
			u32 vStride			= D3DXGetFVFVertexSize		(vFormat);

			BOOL	bSoft		= HW.Caps.vertex.bSoftware || (dwFlags&VLOAD_FORCESOFTWARE);
			u32		dwUsage		= D3DUSAGE_WRITEONLY | (bSoft?D3DUSAGE_SOFTWAREPROCESSING:0);
			BYTE*	bytes		= 0;
			R_CHK				(HW.pDevice->CreateVertexBuffer(vCount*vStride,dwUsage,0,D3DPOOL_MANAGED,&pVertices,0));
			R_CHK				(pVertices->Lock(0,0,(void**)&bytes,0));
			Memory.mem_copy		(bytes, data->Pointer(), vCount*vStride);
			pVertices->Unlock	();
		}
	}

	// indices
	dwPrimitives = 0;
	if ((dwFlags&VLOAD_NOINDICES)==0) {
		if (data->FindChunk(OGF_ICONTAINER)) {
#ifndef _EDITOR
			u32 ID			= data->Rdword			();
			iBase				= data->Rdword			();
			iCount				= data->Rdword			();
			dwPrimitives		= iCount/3;
			pIndices			= ::Render->getIB		(ID);
			pIndices->AddRef	();
#endif
		} else {
			R_ASSERT			(data->FindChunk(OGF_INDICES));
			iBase				= 0;
			iCount				= data->Rdword();
			dwPrimitives		= iCount/3;

			BOOL	bSoft		= HW.Caps.vertex.bSoftware || (dwFlags&VLOAD_FORCESOFTWARE);
			u32	dwUsage			= D3DUSAGE_WRITEONLY | (bSoft?D3DUSAGE_SOFTWAREPROCESSING:0);
			D3DPOOL	dwPool		= bSoft?D3DPOOL_SYSTEMMEM:D3DPOOL_DEFAULT;
			BYTE*	bytes		= 0;

			R_CHK				(HW.pDevice->CreateIndexBuffer(iCount*2,dwUsage,D3DFMT_INDEX16,D3DPOOL_MANAGED,&pIndices,0));
			R_CHK				(pIndices->Lock(0,0,(void**)&bytes,0));
			Memory.mem_copy		(bytes, data->Pointer(), iCount*2);
			pIndices->Unlock	();
		}
	}

	if (dwFlags&VLOAD_NOVERTICES || dwFlags&VLOAD_NOINDICES)	return;
	else hGeom	= Device.Shader.CreateGeom	(vFormat,pVertices,pIndices);
}

void Fvisual::Render	(float LOD)
{
	RCache.set_Geometry		(hGeom);
	RCache.Render			(D3DPT_TRIANGLELIST,vBase,0,vCount,iBase,dwPrimitives);
}

#define PCOPY(a)	a = pFrom->a
void	Fvisual::Copy			(CVisual *pSrc)
{
	CVisual::Copy				(pSrc);

	Fvisual	*pFrom				= dynamic_cast<Fvisual*> (pSrc);
	
	PCOPY(pVertices);
	PCOPY(vBase);
	PCOPY(vCount);

	PCOPY(pIndices);
	PCOPY(iBase);
	PCOPY(iCount);
	PCOPY(dwPrimitives);
}
