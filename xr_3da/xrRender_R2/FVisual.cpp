// FVisual.cpp: implementation of the FVisual class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#pragma warning(disable:4995)
#include <d3dx9.h>
#pragma warning(default:4995)

#include "../fmesh.h"
#include "fvisual.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Fvisual::Fvisual()  : IRender_Visual()
{
	m_fast	=	0;
}

Fvisual::~Fvisual()
{
	xr_delete	(m_fast);
}

void Fvisual::Release	()
{
	IRender_Visual::Release	();
}

void Fvisual::Load		(const char* N, IReader *data, u32 dwFlags)
{
	IRender_Visual::Load		(N,data,dwFlags);

	D3DVERTEXELEMENT9	dcl		[MAX_FVF_DECL_SIZE];
	D3DVERTEXELEMENT9*	vFormat	= 0;
	dwPrimitives				= 0;
	BOOL				loaded_v=false,loaded_f=false;

	if (data->find_chunk(OGF_GCONTAINER)) {
		// verts
		u32 ID				= data->r_u32					();
		vBase				= data->r_u32					();
		vCount				= data->r_u32					();
		pVertices			= RImplementation.getVB			(ID);
		pVertices->AddRef	();
		vFormat				= RImplementation.getVB_Format	(ID);
		loaded_v			= true;

		// indices
		ID					= data->r_u32			();
		iBase				= data->r_u32			();
		iCount				= data->r_u32			();
		dwPrimitives		= iCount/3;
		pIndices			= RImplementation.getIB		(ID);
		pIndices->AddRef	();
		loaded_f			= true;

#if RENDER==R_R2
		// check for fast-vertices
		if (data->find_chunk(OGF_FASTPATH))		{
			IReader*	geomdef			= data->open_chunk		(OGF_FASTPATH);
			IReader*	def				= geomdef->open_chunk	(OGF_GCONTAINER);

			// we have fast-mesh
			m_fast						= xr_new<IRender_Mesh>	();

			// verts
			D3DVERTEXELEMENT9*	fmt		= 0;
			ID							= def->r_u32			();
			m_fast->vBase				= def->r_u32			();
			m_fast->vCount				= def->r_u32			();
			m_fast->pVertices			= RImplementation.getVB	(ID,true);
			m_fast->pVertices->AddRef	();
			fmt							= RImplementation.getVB_Format(ID,true);

			// indices
			ID							= def->r_u32			();
			m_fast->iBase				= def->r_u32			();
			m_fast->iCount				= def->r_u32			();
			m_fast->dwPrimitives		= iCount/3;
			m_fast->pIndices			= RImplementation.getIB		(ID);
			m_fast->pIndices->AddRef	();

			// geom
			m_fast->geom.create			(fmt,m_fast->pVertices,m_fast->pIndices);

			// cleanup
			geomdef->close_chunk		(def);
			data->close_chunk			(geomdef);
		}
#endif
	}

	// read vertices
	if (!loaded_v && (dwFlags&VLOAD_NOVERTICES)==0) {
		if (data->find_chunk(OGF_VCONTAINER)) {
#ifndef _EDITOR
			u32 ID				= data->r_u32				();
			vBase				= data->r_u32				();
			vCount				= data->r_u32				();
			pVertices			= RImplementation.getVB			(ID);
			pVertices->AddRef	();
			vFormat				= RImplementation.getVB_Format	(ID);
#endif
		} else {
			R_ASSERT			(data->find_chunk(OGF_VERTICES));
			vBase				= 0;
			u32 fvf				= data->r_u32				();
			CHK_DX				(D3DXDeclaratorFromFVF(fvf,dcl));
			vFormat				= dcl;
			vCount				= data->r_u32				();
			u32 vStride			= D3DXGetFVFVertexSize		(fvf);

			BOOL	bSoft		= HW.Caps.geometry.bSoftware || (dwFlags&VLOAD_FORCESOFTWARE);
			u32		dwUsage		= D3DUSAGE_WRITEONLY | (bSoft?D3DUSAGE_SOFTWAREPROCESSING:0);
			BYTE*	bytes		= 0;
			R_CHK				(HW.pDevice->CreateVertexBuffer(vCount*vStride,dwUsage,0,D3DPOOL_MANAGED,&pVertices,0));
			R_CHK				(pVertices->Lock(0,0,(void**)&bytes,0));
			Memory.mem_copy		(bytes, data->pointer(), vCount*vStride);
			pVertices->Unlock	();
		}
	}

	// indices
	if (!loaded_v && (dwFlags&VLOAD_NOINDICES)==0) {
		dwPrimitives = 0;
		if (data->find_chunk(OGF_ICONTAINER)) {
#ifndef _EDITOR
			u32 ID				= data->r_u32			();
			iBase				= data->r_u32			();
			iCount				= data->r_u32			();
			dwPrimitives		= iCount/3;
			pIndices			= RImplementation.getIB	(ID);
			pIndices->AddRef	();
#endif
		} else {
			R_ASSERT			(data->find_chunk(OGF_INDICES));
			iBase				= 0;
			iCount				= data->r_u32();
			dwPrimitives		= iCount/3;

			BOOL	bSoft		= HW.Caps.geometry.bSoftware || (dwFlags&VLOAD_FORCESOFTWARE);
			u32		dwUsage		= /*D3DUSAGE_WRITEONLY |*/ (bSoft?D3DUSAGE_SOFTWAREPROCESSING:0);	// indices are read in model-wallmarks code
			BYTE*	bytes		= 0;

			R_CHK				(HW.pDevice->CreateIndexBuffer(iCount*2,dwUsage,D3DFMT_INDEX16,D3DPOOL_MANAGED,&pIndices,0));
			R_CHK				(pIndices->Lock(0,0,(void**)&bytes,0));
			Memory.mem_copy		(bytes, data->pointer(), iCount*2);
			pIndices->Unlock	();
		}
	}

	if (dwFlags&VLOAD_NOVERTICES || dwFlags&VLOAD_NOINDICES)	return;
	else	geom.create		(vFormat,pVertices,pIndices);
}

void Fvisual::Render		(float LOD)
{
#if RENDER==R_R2
	if (m_fast && RImplementation.phase==CRender::PHASE_SMAP)
	{
		RCache.set_Geometry		(m_fast->geom);
		RCache.Render			(D3DPT_TRIANGLELIST,m_fast->vBase,0,m_fast->vCount,m_fast->iBase,m_fast->dwPrimitives);
	} else {
		RCache.set_Geometry		(geom);
		RCache.Render			(D3DPT_TRIANGLELIST,vBase,0,vCount,iBase,dwPrimitives);
	}
#else
	RCache.set_Geometry		(geom);
	RCache.Render			(D3DPT_TRIANGLELIST,vBase,0,vCount,iBase,dwPrimitives);
#endif
}

#define PCOPY(a)	a = pFrom->a
void	Fvisual::Copy			(IRender_Visual *pSrc)
{
	IRender_Visual::Copy		(pSrc);

	Fvisual	*pFrom				= dynamic_cast<Fvisual*> (pSrc);

	PCOPY	(geom);

	PCOPY	(pVertices);
	PCOPY	(vBase);
	PCOPY	(vCount);

	PCOPY	(pIndices);
	PCOPY	(iBase);
	PCOPY	(iCount);
	PCOPY	(dwPrimitives);

	PCOPY	(m_fast);
}
