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

void Fvisual::Load		(const char* N, CStream *data, DWORD dwFlags)
{
	CVisual::Load		(N,data,dwFlags);

	// read vertices
	if ((dwFlags&VLOAD_NOVERTICES)==0) {
		if (data->FindChunk(OGF_VCONTAINER)) {
#ifndef _EDITOR
			DWORD ID			= data->Rdword			();
			vBase				= data->Rdword			();
			vCount				= data->Rdword			();
			vShader				= ::Render->getFVF		(ID);
			vSize				= D3DXGetFVFVertexSize	(vShader);
			pVertices			= ::Render->getVB		(ID);
			pVertices->AddRef	();
#endif
		} else {
			R_ASSERT			(data->FindChunk(OGF_VERTICES));
			vBase				= 0;
			vShader				= data->Rdword			();
			vCount				= data->Rdword			();
			vSize				= D3DXGetFVFVertexSize	(vShader);

			BOOL	bSoft		= HW.Caps.vertex.bSoftware || (dwFlags&VLOAD_FORCESOFTWARE);
			DWORD	dwUsage		= D3DUSAGE_WRITEONLY | (bSoft?D3DUSAGE_SOFTWAREPROCESSING:0);
			D3DPOOL	dwPool		= bSoft?D3DPOOL_SYSTEMMEM:D3DPOOL_DEFAULT;
			BYTE*	bytes		= 0;
			R_CHK				(HW.pDevice->CreateVertexBuffer(vCount*vSize,dwUsage,vShader,dwPool,&pVertices));
			R_CHK				(pVertices->Lock(0,0,&bytes,0));
			PSGP.memCopy		(bytes, data->Pointer(), vCount*vSize);
			pVertices->Unlock	();
		}
	}

	// indices
	dwPrimitives = 0;
	if ((dwFlags&VLOAD_NOINDICES)==0) {
		if (data->FindChunk(OGF_ICONTAINER)) {
			DWORD ID			= data->Rdword			();
			iBase				= data->Rdword			();
			iCount				= data->Rdword			();
			dwPrimitives		= iCount/3;
			pIndices			= ::Render->getIB		(ID);
			pIndices->AddRef	();
		} else {
			R_ASSERT			(data->FindChunk(OGF_INDICES));
			iBase				= 0;
			iCount				= data->Rdword();
			dwPrimitives		= iCount/3;

			BOOL	bSoft		= HW.Caps.vertex.bSoftware || (dwFlags&VLOAD_FORCESOFTWARE);
			DWORD	dwUsage		= D3DUSAGE_WRITEONLY | (bSoft?D3DUSAGE_SOFTWAREPROCESSING:0);
			D3DPOOL	dwPool		= bSoft?D3DPOOL_SYSTEMMEM:D3DPOOL_DEFAULT;
			BYTE*	bytes		= 0;

			R_CHK				(HW.pDevice->CreateIndexBuffer(iCount*2,dwUsage,D3DFMT_INDEX16,dwPool,&pIndices));
			R_CHK				(pIndices->Lock(0,0,&bytes,0));
			PSGP.memCopy		(bytes, data->Pointer(), iCount*2);
			pVertices->Unlock	();
		}
	}
}

void Fvisual::Render	(float LOD)
{
	Device.Primitive.setVertices	(vShader,vSize,pVertices);
	Device.Primitive.setIndices		(vBase,	pIndices);
	Device.Primitive.Render			(D3DPT_TRIANGLELIST,0,vCount,iBase,dwPrimitives);
	UPDATEC							(vCount,dwPrimitives,1);
}

#define PCOPY(a)	a = pFrom->a
void	Fvisual::Copy			(CVisual *pSrc)
{
	CVisual::Copy				(pSrc);

	Fvisual	*pFrom				= dynamic_cast<Fvisual*> (pSrc);
	
	PCOPY(pVertices);
	PCOPY(vBase);
	PCOPY(vCount);
	PCOPY(vShader);
	PCOPY(vSize);

	PCOPY(pIndices);
	PCOPY(iBase);
	PCOPY(iCount);
	PCOPY(dwPrimitives);
}
