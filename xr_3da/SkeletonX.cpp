// SkeletonX.cpp: implementation of the CSkeletonX class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#include "SkeletonX.h"
#include "BodyInstance.h"
#include "fmesh.h"
#include "xrCPU_Pipe.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Body Part
//////////////////////////////////////////////////////////////////////
void CSkeletonX::_Copy(CSkeletonX *B)
{
	Parent				= NULL;
	Vertices1W			= B->Vertices1W;
	Vertices2W			= B->Vertices2W;
	cache_DiscardID		= 0xffffffff;
}
void CSkeletonX_PM::Copy(CVisual *V) 
{
	Fvisual::Copy		(V);
	pm_copy				(V);

	CSkeletonX_PM *X	= (CSkeletonX_PM*)(V);
	_Copy				((CSkeletonX*)X);
	indices				= X->indices;

	u32	dwCount		= dwPrimitives*3;
	BOOL	bSoft		= HW.Caps.vertex.bSoftware;
	u32	dwUsage		= D3DUSAGE_WRITEONLY | (bSoft?D3DUSAGE_SOFTWAREPROCESSING:0);
	D3DPOOL	dwPool		= bSoft?D3DPOOL_SYSTEMMEM:D3DPOOL_DEFAULT;
	BYTE*	bytes		= 0;

	R_CHK				(HW.pDevice->CreateIndexBuffer(dwCount*2,dwUsage,D3DFMT_INDEX16,dwPool,&pIndices));
	R_CHK				(pIndices->Lock(0,0,&bytes,0));
	PSGP.memCopy		(bytes, indices, dwCount*2);
	pIndices->Unlock	();
}
void CSkeletonX_ST::Copy(CVisual *P) 
{
	inherited::Copy		(P);
	CSkeletonX_ST *X	= (CSkeletonX_ST*)P;
	_Copy				((CSkeletonX*)X);
}
//////////////////////////////////////////////////////////////////////
void CSkeletonX_PM::Render	(float LOD) 
{
	SetLOD		(LOD);
	_Render		(hVS,V_Current,I_Current/3,pIndices);
}
void CSkeletonX_ST::Render	(float LOD) 
{
	_Render		(hVS,vCount,dwPrimitives,pIndices);
}
void CSkeletonX::_Render	(CVS* hVS, u32 vCount, u32 pCount, IDirect3DIndexBuffer8* IB)
{
	u32 vOffset			= cache_vOffset;

	_VertexStream&	_VS		= Device.Streams.Vertex;
	if (cache_DiscardID!=_VS.DiscardID() || vCount>=cache_vCount )
	{
		vertRender*	Dest	= (vertRender*)_VS.Lock(vCount,hVS->dwStride,vOffset);
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
		_VS.Unlock			(vCount,hVS->dwStride);
	}

	Device.Primitive.setVertices	(hVS->dwHandle,hVS->dwStride,_VS.Buffer());
	Device.Primitive.setIndices		(vOffset,IB);
	Device.Primitive.Render			(D3DPT_TRIANGLELIST,0,vCount,0,pCount);
}
//////////////////////////////////////////////////////////////////////
void CSkeletonX::_Release()
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
void CSkeletonX::_Load(const char* N, CStream *data, u32& dwVertCount) 
{
	// Load vertices
	R_ASSERT(data->FindChunk(OGF_VERTICES));
			
	u32 dwVertType,size;
	dwVertType	= data->Rdword(); 
	dwVertCount	= data->Rdword();

	switch		(dwVertType)
	{
	case 1*0x12071980:
		size		= dwVertCount*sizeof(vertBoned1W);
		Vertices1W	= (vertBoned1W*) xr_malloc(size);
		Vertices2W	= NULL;
		data->Read	(Vertices1W,size);
		break;
	case 2*0x12071980:
		size		= dwVertCount*sizeof(vertBoned2W);
		Vertices1W	= NULL;
		Vertices2W	= (vertBoned2W*) xr_malloc(size);
		data->Read	(Vertices2W,size);
		break;
	default:
		Debug.fatal	("Invalid vertex type in skinned model '%s'",N);
		break;
	}
}

void CSkeletonX_PM::Load(const char* N, CStream *data, u32 dwFlags) 
{
	_Load				(N,data,vCount);
	inherited::Load		(N, data, dwFlags|VLOAD_NOVERTICES|VLOAD_NOINDICES);
	hVS					= Device.Shader._CreateVS	(vertRenderFVF);

	// Load indices with replication in mind
	R_ASSERT			(data->FindChunk(OGF_INDICES));
	u32				dwCount = data->Rdword();
	R_ASSERT			(dwCount%3 == 0);
	indices				= LPWORD(xr_malloc(dwCount*2));
	PSGP.memCopy		(indices,data->Pointer(),dwCount*2);
	dwPrimitives		= dwCount/3;

	BOOL	bSoft		= HW.Caps.vertex.bSoftware || (dwFlags&VLOAD_FORCESOFTWARE);
	u32	dwUsage		= D3DUSAGE_WRITEONLY | (bSoft?D3DUSAGE_SOFTWAREPROCESSING:0);
	D3DPOOL	dwPool		= bSoft?D3DPOOL_SYSTEMMEM:D3DPOOL_DEFAULT;
	BYTE*	bytes		= 0;

	R_CHK				(HW.pDevice->CreateIndexBuffer(dwCount*2,dwUsage,D3DFMT_INDEX16,dwPool,&pIndices));
	R_CHK				(pIndices->Lock(0,0,&bytes,0));
	PSGP.memCopy		(bytes, indices, dwCount*2);
	pIndices->Unlock	();
}

void CSkeletonX_ST::Load(const char* N, CStream *data, u32 dwFlags) 
{
	_Load				(N,data,vCount);
	inherited::Load		(N, data, dwFlags|VLOAD_NOVERTICES);
	hVS					= Device.Shader._CreateVS	(vertRenderFVF);
}
