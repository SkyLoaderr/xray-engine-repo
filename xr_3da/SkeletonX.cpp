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
	Stream				= B->Stream;
	Vertices			= B->Vertices;
	cache_DiscardID		= 0xffffffff;
}
void CSkeletonX_PM::Copy(CVisual *V) 
{
	Fvisual::Copy		(V);
	pm_copy				(V);

	CSkeletonX_PM *X	= (CSkeletonX_PM*)(V);
	_Copy				((CSkeletonX*)X);
	indices				= X->indices;

	DWORD	dwCount		= dwPrimitives*3;
	BOOL	bSoft		= HW.Caps.vertex.bSoftware;
	DWORD	dwUsage		= D3DUSAGE_WRITEONLY | (bSoft?D3DUSAGE_SOFTWAREPROCESSING:0);
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
	_Render		(V_Current,I_Current/3,pIndices);
}
void CSkeletonX_ST::Render	(float LOD) 
{
	_Render		(vCount,dwPrimitives,pIndices);
}
void CSkeletonX::_Render	(DWORD vCount, DWORD pCount, IDirect3DIndexBuffer8* IB)
{
	DWORD vOffset			= cache_vOffset;

	if (cache_DiscardID!=Stream->getDiscard() || vCount>=cache_vCount )
	{
		vertRender*	Dest	= (vertRender*)Stream->Lock(vCount,vOffset);
		cache_DiscardID		= Stream->getDiscard();
		cache_vCount		= vCount;
		cache_vOffset		= vOffset;
		
		Device.Statistic.RenderDUMP_SKIN.Begin	();
		PSGP.skin1W(
			Dest,										// dest
			Vertices,									// source
			vCount,										// count
			Parent->bone_instances						// bones
			);
		Device.Statistic.RenderDUMP_SKIN.End	();
		Stream->Unlock			(vCount);
	}

	Device.Primitive.Draw	(Stream,vCount,pCount,vOffset,IB);
}
//////////////////////////////////////////////////////////////////////
void CSkeletonX::_Release()
{
	_DELETEARRAY(Vertices);
}
void CSkeletonX_PM::Release()
{
	inherited::Release();
	_Release	();
	_FREE		(indices);
}
void CSkeletonX_ST::Release()
{
	inherited::Release();
	_Release();
}
//////////////////////////////////////////////////////////////////////
void CSkeletonX::_Load(const char* N, CStream *data, DWORD& dwVertCount) 
{
	// Load vertices
	R_ASSERT(data->FindChunk(OGF_VERTICES));
			
	DWORD dwVertType;
	dwVertType	= data->Rdword(); R_ASSERT(dwVertType==0x12071980);
	dwVertCount	= data->Rdword();
	
	Vertices = new vertBoned1W[dwVertCount];
	data->Read(Vertices,dwVertCount*sizeof(vertBoned1W));

	// Create stream
	Stream = Device.Streams.Create(vertRenderFVF,dwVertCount);
}

void CSkeletonX_PM::Load(const char* N, CStream *data, DWORD dwFlags) 
{
	_Load				(N,data,vCount);
	inherited::Load		(N, data, dwFlags|VLOAD_NOVERTICES|VLOAD_NOINDICES);

	// Load indices with replication in mind
	R_ASSERT			(data->FindChunk(OGF_INDICES));
	DWORD				dwCount = data->Rdword();
	R_ASSERT			(dwCount%3 == 0);
	indices				= LPWORD(xr_malloc(dwCount*2));
	PSGP.memCopy		(indices,data->Pointer(),dwCount*2);
	dwPrimitives		= dwCount/3;

	BOOL	bSoft		= HW.Caps.vertex.bSoftware || (dwFlags&VLOAD_FORCESOFTWARE);
	DWORD	dwUsage		= D3DUSAGE_WRITEONLY | (bSoft?D3DUSAGE_SOFTWAREPROCESSING:0);
	D3DPOOL	dwPool		= bSoft?D3DPOOL_SYSTEMMEM:D3DPOOL_DEFAULT;
	BYTE*	bytes		= 0;

	R_CHK				(HW.pDevice->CreateIndexBuffer(dwCount*2,dwUsage,D3DFMT_INDEX16,dwPool,&pIndices));
	R_CHK				(pIndices->Lock(0,0,&bytes,0));
	PSGP.memCopy		(bytes, indices, dwCount*2);
	pIndices->Unlock	();
}

void CSkeletonX_ST::Load(const char* N, CStream *data, DWORD dwFlags) 
{
	_Load				(N,data,vCount);
	inherited::Load		(N, data, dwFlags|VLOAD_NOVERTICES);
}
