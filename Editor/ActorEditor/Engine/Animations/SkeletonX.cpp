// SkeletonX.cpp: implementation of the CSkeletonX class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
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
	Parent			= NULL;
	Stream			= B->Stream;
	Vertices		= B->Vertices;
}
void CSkeletonX_PM::Copy(FBasicVisual *V) 
{
	Fvisual::Copy	(V);
	pm_copy			(V);

	CSkeletonX_PM *X = (CSkeletonX_PM*)V;
	_Copy((CSkeletonX*)X);
	indices			= X->indices;
	P.IB_Create		(0,dwPrimsCount*3,D3DUSAGE_WRITEONLY,indices);
}
void CSkeletonX_ST::Copy(FBasicVisual *P) 
{
	inherited::Copy	(P);
	CSkeletonX_ST *X = (CSkeletonX_ST*)P;
	_Copy((CSkeletonX*)X);
}
//////////////////////////////////////////////////////////////////////
void CSkeletonX_PM::Render	(float LOD) 
{
	SetLOD		(LOD);
	_Render		(V_Current,I_Current/3,P.pIndices);
}
void CSkeletonX_ST::Render	(float LOD) 
{
	_Render		(dwVertCount,dwPrimsCount,P.pIndices);
}
void CSkeletonX::_Render	(DWORD vCount, DWORD pCount, IDirect3DIndexBuffer8* IB) 
{
//	Parent->Calculate();

	DWORD vOffset;
	
	vertRender*	Dest = (vertRender*)Stream->Lock(vCount,vOffset);

	Device.Statistic.RenderDUMP_SKIN.Begin	();
	PSGP.skin1W(
		Dest,										// dest
		Vertices,									// source
		vCount,										// count
		Parent->bone_instances						// bones
		);
	Device.Statistic.RenderDUMP_SKIN.End	();

	Stream->Unlock			(vCount);
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
	dwVertType	= data->Rdword(); R_ASSERT(dwVertType=0x12071980);
	dwVertCount	= data->Rdword();
	
	Vertices = new vertBoned1W[dwVertCount];
	data->Read(Vertices,dwVertCount*sizeof(vertBoned1W));

	// Create stream
	Stream = Device.Streams.Create(vertRenderFVF,dwVertCount);
}

void CSkeletonX_PM::Load(const char* N, CStream *data, DWORD dwFlags) 
{
	_Load(N,data,dwVertCount);
	inherited::Load(N, data, dwFlags|VLOAD_NOVERTICES|VLOAD_NOINDICES);

	// Load indices with replication in mind
	R_ASSERT(data->FindChunk(OGF_INDICES));
	DWORD	dwCount = data->Rdword();
	R_ASSERT(dwCount%3 == 0);
	indices = LPWORD(malloc(dwCount*2));
	CopyMemory(indices,data->Pointer(),dwCount*2);
	dwPrimsCount = dwCount/3;

	P.IB_Create	(0,dwPrimsCount*3,D3DUSAGE_WRITEONLY,indices);
}

void CSkeletonX_ST::Load(const char* N, CStream *data, DWORD dwFlags) 
{
	_Load(N,data,dwVertCount);
	inherited::Load(N, data, dwFlags|VLOAD_NOVERTICES);
}
