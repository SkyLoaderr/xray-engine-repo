// SkeletonX.cpp: implementation of the CSkeletonX class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#pragma warning(disable:4995)
#include <d3dx9.h>
#pragma warning(default:4995)

#include "SkeletonX.h"
#include "SkeletonCustom.h"
#include "fmesh.h"
#include "xrCPU_Pipe.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Body Part
//////////////////////////////////////////////////////////////////////
void CSkeletonX::_Copy		(CSkeletonX *B)
{
	Parent					= NULL;
	Vertices1W				= B->Vertices1W;
	Vertices2W				= B->Vertices2W;
	cache_DiscardID			= 0xffffffff;
	RenderMode				= B->RenderMode;
	RMS_boneid				= B->RMS_boneid;
}
void CSkeletonX_PM::Copy	(IRender_Visual *V) 
{
	Fvisual::Copy			(V);
	pm_copy					(V);

	CSkeletonX_PM *X		= (CSkeletonX_PM*)(V);
	_Copy					((CSkeletonX*)X);
	indices					= X->indices;

	u32		dwCount			= dwPrimitives*3;
	BOOL	bSoft			= HW.Caps.vertex.bSoftware;
	u32		dwUsage			= D3DUSAGE_WRITEONLY | (bSoft?D3DUSAGE_SOFTWAREPROCESSING:0);
	D3DPOOL	dwPool			= bSoft?D3DPOOL_SYSTEMMEM:D3DPOOL_MANAGED;
	BYTE*	bytes			= 0;

	R_CHK					(HW.pDevice->CreateIndexBuffer(dwCount*2,dwUsage,D3DFMT_INDEX16,dwPool,&pIndices,0));
	R_CHK					(pIndices->Lock(0,0,(void**)&bytes,0));
	Memory.mem_copy			(bytes, indices, dwCount*2);
	pIndices->Unlock		();
}
void CSkeletonX_ST::Copy	(IRender_Visual *P) 
{
	inherited::Copy		(P);
	CSkeletonX_ST *X	= (CSkeletonX_ST*)P;
	_Copy				((CSkeletonX*)X);
}
//////////////////////////////////////////////////////////////////////
void CSkeletonX_PM::Render	(float LOD) 
{
	SetLOD		(LOD);
	if			(RM_SINGLE==RenderMode)	{
		VERIFY					(pVertices);
		Fmatrix	W;	W.mul_43	(RCache.xforms.m_w,Parent->LL_GetTransform(RMS_boneid));
		RCache.set_xform_world	(W);
		RCache.set_Geometry		(hGeom);
		RCache.Render			(D3DPT_TRIANGLELIST,0,0,vCount,0,I_Current/3);
	} else {
		_Render		(hGeom,V_Current,I_Current/3);
	}
}
void CSkeletonX_ST::Render	(float LOD) 
{
	if			(RM_SINGLE==RenderMode)	{
		VERIFY					(pVertices);
		Fmatrix	W;	W.mul_43	(RCache.xforms.m_w,Parent->LL_GetTransform(RMS_boneid));
		RCache.set_xform_world	(W);
		RCache.set_Geometry		(hGeom);
		RCache.Render			(D3DPT_TRIANGLELIST,0,0,vCount,0,dwPrimitives);
	} else {
		_Render		(hGeom,vCount,dwPrimitives);
	}
}
void CSkeletonX::_Render	(ref_geom& hGeom, u32 vCount, u32 pCount)
{
	u32 vOffset				= cache_vOffset;

	_VertexStream&	_VS		= RCache.Vertex;
	if (cache_DiscardID!=_VS.DiscardID() || vCount>=cache_vCount )
	{
		vertRender*	Dest	= (vertRender*)_VS.Lock(vCount,hGeom->vb_stride,vOffset);
		cache_DiscardID		= _VS.DiscardID();
		cache_vCount		= vCount;
		cache_vOffset		= vOffset;
		
		Device.Statistic.RenderDUMP_SKIN.Begin	();
		if (Vertices1W)
		{
			PSGP.skin1W(
				Dest,										// dest
				Vertices1W,									// source
				vCount,										// count
				Parent->bone_instances						// bones
				);
		} else {
			PSGP.skin2W(
				Dest,										// dest
				Vertices2W,									// source
				vCount,										// count
				Parent->bone_instances						// bones
				);
		}
		Device.Statistic.RenderDUMP_SKIN.End	();
		_VS.Unlock			(vCount,hGeom->vb_stride);
	}

	RCache.set_Geometry		(hGeom);
	RCache.Render			(D3DPT_TRIANGLELIST,vOffset,0,vCount,0,pCount);
}

//////////////////////////////////////////////////////////////////////
void CSkeletonX::_Release	()
{
	xr_free		(Vertices1W);
	xr_free		(Vertices2W);
}
void CSkeletonX_PM::Release()
{
	inherited::Release();
	_Release	();
	xr_free		(indices);
}
void CSkeletonX_ST::Release()
{
	inherited::Release();
	_Release	();
}
//////////////////////////////////////////////////////////////////////
void CSkeletonX::_Load(const char* N, IReader *data, u32& dwVertCount) 
{	
	//. temp
	xr_set<u32>		bids,bids2;

	// Load vertices
	R_ASSERT	(data->find_chunk(OGF_VERTICES));
			
	u32			dwVertType,size,it,bpv=0;
	dwVertType	= data->r_u32(); 
	dwVertCount	= data->r_u32();

	RenderMode	= RM_SCINNING;
	switch		(dwVertType)
	{
	case 1*0x12071980:
		bpv			= 1;
		size		= dwVertCount*sizeof(vertBoned1W);
		Vertices1W	= (vertBoned1W*) xr_malloc(size);
		Vertices2W	= NULL;
		data->r		(Vertices1W,size);
		for (it=0; it<dwVertCount; it++)
			bids.insert	(Vertices1W[it].matrix);
		Msg	("         BPV: %d, %d verts, %d bone-influences",bpv,dwVertCount,bids.size());
		if	(1==bids.size())	{
			RenderMode	= RM_SINGLE;
			RMS_boneid	= *bids.begin();
		}
		break;
	case 2*0x12071980:
		bpv			= 2;
		size		= dwVertCount*sizeof(vertBoned2W);
		Vertices1W	= NULL;
		Vertices2W	= (vertBoned2W*) xr_malloc(size);
		data->r		(Vertices2W,size);
		for (it=0; it<dwVertCount; it++)
		{
			bids.insert				(Vertices2W[it].matrix0);
			bids.insert				(Vertices2W[it].matrix1);
			float	w				= Vertices2W[it].w;
			if (fsimilar(w,0,.01f))			bids2.insert(Vertices2W[it].matrix0);
			else if (fsimilar(w,1,.01f))	bids2.insert(Vertices2W[it].matrix1);
			else {
				bids2.insert(Vertices2W[it].matrix0);
				bids2.insert(Vertices2W[it].matrix1);
			}
		}
		Msg	("         BPV: %d, %d verts, %d/%d bone-influences",bpv,dwVertCount,bids.size(),bids2.size());
		break;
	default:
		Debug.fatal	("Invalid vertex type in skinned model '%s'",N);
		break;
	}
}

void CSkeletonX_PM::Load(const char* N, IReader *data, u32 dwFlags) 
{
	_Load				(N,data,vCount);
	inherited::Load		(N, data, dwFlags|VLOAD_NOVERTICES|VLOAD_NOINDICES);

	// Load indices with replication in mind
	R_ASSERT			(data->find_chunk(OGF_INDICES));
	u32					dwCount = data->r_u32();
	R_ASSERT			(dwCount%3 == 0);
	indices				= LPWORD(xr_malloc(dwCount*2));
	Memory.mem_copy		(indices,data->pointer(),dwCount*2);
	dwPrimitives		= dwCount/3;
	BOOL	bSoft		= HW.Caps.vertex.bSoftware || (dwFlags&VLOAD_FORCESOFTWARE);
	u32		dwUsage		= D3DUSAGE_WRITEONLY | (bSoft?D3DUSAGE_SOFTWAREPROCESSING:0);
	D3DPOOL	dwPool		= bSoft?D3DPOOL_SYSTEMMEM:D3DPOOL_MANAGED;
	BYTE*	bytes		= 0;
	R_CHK				(HW.pDevice->CreateIndexBuffer(dwCount*2,dwUsage,D3DFMT_INDEX16,dwPool,&pIndices,0));
	R_CHK				(pIndices->Lock(0,0,(void**)&bytes,0));
	Memory.mem_copy		(bytes, indices, dwCount*2);
	pIndices->Unlock	();

	// Create HW VB in case this is possible
	if (RenderMode==RM_SINGLE)
	{
		VERIFY				(Vertices1W);
		vBase				= 0;
		u32		vStride		= D3DXGetFVFVertexSize		(vertRenderFVF);
		BOOL	bSoft		= HW.Caps.vertex.bSoftware || (dwFlags&VLOAD_FORCESOFTWARE);
		u32		dwUsage		= D3DUSAGE_WRITEONLY | (bSoft?D3DUSAGE_SOFTWAREPROCESSING:0);
		BYTE*	bytes		= 0;
		R_CHK				(HW.pDevice->CreateVertexBuffer(vCount*vStride,dwUsage,0,D3DPOOL_MANAGED,&pVertices,0));
		R_CHK				(pVertices->Lock(0,0,(void**)&bytes,0));
		vertRender*		dst	= (vertRender*)bytes;
		vertBoned1W*	src = (vertBoned1W*)Vertices1W;
		for (u32 it=0; it<vCount; it++)	{
			dst->P			= src->P;
			dst->N			= src->N;
			dst->u			= src->u;
			dst->v			= src->v;
			dst++; src++;
		}
		pVertices->Unlock	();

		hGeom.create		(vertRenderFVF, pVertices, pIndices);
	} else {
		hGeom.create		(vertRenderFVF, RCache.Vertex.Buffer(), pIndices);
	}
}

void CSkeletonX_ST::Load(const char* N, IReader *data, u32 dwFlags) 
{
	_Load				(N,data,vCount);
	inherited::Load		(N,data,dwFlags|VLOAD_NOVERTICES);

	// Create HW VB in case this is possible
	if (RenderMode==RM_SINGLE)
	{
		VERIFY				(Vertices1W);
		vBase				= 0;
		u32		vStride		= D3DXGetFVFVertexSize		(vertRenderFVF);
		BOOL	bSoft		= HW.Caps.vertex.bSoftware || (dwFlags&VLOAD_FORCESOFTWARE);
		u32		dwUsage		= D3DUSAGE_WRITEONLY | (bSoft?D3DUSAGE_SOFTWAREPROCESSING:0);
		BYTE*	bytes		= 0;
		R_CHK				(HW.pDevice->CreateVertexBuffer(vCount*vStride,dwUsage,0,D3DPOOL_MANAGED,&pVertices,0));
		R_CHK				(pVertices->Lock(0,0,(void**)&bytes,0));
		vertRender*		dst	= (vertRender*)bytes;
		vertBoned1W*	src = (vertBoned1W*)Vertices1W;
		for (u32 it=0; it<vCount; it++)	{
			dst->P			= src->P;
			dst->N			= src->N;
			dst->u			= src->u;
			dst->v			= src->v;
			dst++; src++;
		}
		pVertices->Unlock	();

		hGeom.create		(vertRenderFVF, pVertices, pIndices);
	} else {
		hGeom.create		(vertRenderFVF, RCache.Vertex.Buffer(), pIndices);
	}
}
