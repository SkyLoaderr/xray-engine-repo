// SkeletonX.cpp: implementation of the CSkeletonX class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#pragma warning(disable:4995)
#include <d3dx9.h>
#pragma warning(default:4995)

#include "Render.h"
#include "SkeletonX.h"
#include "SkeletonCustom.h"
#include "fmesh.h"
#include "xrCPU_Pipe.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
ref_str						sbones_array;

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
	BOOL	bSoft			= HW.Caps.geometry.bSoftware;
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
void CSkeletonX_PM::Render		(float LOD) 
{
	SetLOD		(LOD);
	_Render		(hGeom,V_Current,I_Current/3);
}
void CSkeletonX_ST::Render		(float LOD) 
{
	_Render		(hGeom,vCount,dwPrimitives);
}
void CSkeletonX::_Render		(ref_geom& hGeom, u32 vCount, u32 pCount)
{
	switch (RenderMode)
	{
	case RM_SKINNING_SOFT:
		_Render_soft			(hGeom,vCount,pCount);
		break;
	case RM_SINGLE:	
		{
			Fmatrix	W;	W.mul_43	(RCache.xforms.m_w,Parent->LL_GetTransform(RMS_boneid));
			RCache.set_xform_world	(W);
			RCache.set_Geometry		(hGeom);
			RCache.Render			(D3DPT_TRIANGLELIST,0,0,vCount,0,pCount);
		}
		break;
	case RM_SKINNING_1B:
	case RM_SKINNING_2B:
		{
			// transfer matrices
			R_constant*				array	= RCache.get_c				(sbones_array);
			u32						count	= Parent->LL_BoneCount		();
			for (u32 mid = 0; mid<count; mid++)	{
				Fmatrix&	M				= Parent->LL_GetTransform	(mid);
				u32			id				= mid*3;
				RCache.set_ca	(array,id+0,M._11,M._21,M._31,M._41);
				RCache.set_ca	(array,id+1,M._12,M._22,M._32,M._42);
				RCache.set_ca	(array,id+2,M._13,M._23,M._33,M._43);
			}

			// render
			RCache.set_Geometry		(hGeom);
			RCache.Render			(D3DPT_TRIANGLELIST,0,0,vCount,0,pCount);
		}
		break;
	}
}
void CSkeletonX::_Render_soft	(ref_geom& hGeom, u32 vCount, u32 pCount)
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
		if (*Vertices1W)
		{
			PSGP.skin1W(
				Dest,										// dest
				*Vertices1W,								// source
				vCount,										// count
				Parent->bone_instances						// bones
				);
		} else {
			PSGP.skin2W(
				Dest,										// dest
				*Vertices2W,								// source
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
void CSkeletonX_PM::Release()
{
	inherited::Release();
	xr_free		(indices);
}
void CSkeletonX_ST::Release()
{
	inherited::Release();
}
//////////////////////////////////////////////////////////////////////
void CSkeletonX::_Load(const char* N, IReader *data, u32& dwVertCount) 
{	
	sbones_array	= "sbones_array";

	//. temp
	xr_set<u32>		bids;

	// Load vertices
	R_ASSERT	(data->find_chunk(OGF_VERTICES));
			
	u32			hw_bones	= (HW.Caps.geometry.dwRegisters-20)/3;
	u32			dwVertType,size,it,bpv=0,crc;
	dwVertType	= data->r_u32(); 
	dwVertCount	= data->r_u32();

	RenderMode						= RM_SKINNING_SOFT;
	Render->shader_option_skinning	(0);
	switch		(dwVertType)
	{
	case 1*0x12071980:
		bpv			= 1;
		size		= dwVertCount*sizeof(vertBoned1W);
		crc			= crc32	(data->pointer(),size);
		Vertices1W.create(crc,dwVertCount,(vertBoned1W*)data->pointer());
		for (it=0; it<dwVertCount; it++)
			bids.insert	(Vertices1W[it].matrix);
		//Msg	("         BPV: %d, %d verts, %d bone-influences",bpv,dwVertCount,bids.size());
		if	(1==bids.size())	{
			RenderMode						= RM_SINGLE;
			RMS_boneid						= *bids.begin();
		} else if (bids.size()<hw_bones) {
			RenderMode						= RM_SKINNING_1B;
			Render->shader_option_skinning	(1);
		}
		break;
	case 2*0x12071980:
		bpv			= 2;
		size		= dwVertCount*sizeof(vertBoned2W);
		crc			= crc32	(data->pointer(),size);
		Vertices2W.create(crc,dwVertCount,(vertBoned2W*)data->pointer());
		for (it=0; it<dwVertCount; it++)	{
			bids.insert	(Vertices2W[it].matrix0);
			bids.insert	(Vertices2W[it].matrix1);
		}
		//Msg	("         BPV: %d, %d verts, %d/%d bone-influences",bpv,dwVertCount,bids.size(),bids2.size());
		if (bids.size()<hw_bones) {
			RenderMode						= RM_SKINNING_1B;
			Render->shader_option_skinning	(2);
		}
		break;
	default:
		Debug.fatal	("Invalid vertex type in skinned model '%s'",N);
		break;
	}
}

void CSkeletonX_PM::Load(const char* N, IReader *data, u32 dwFlags) 
{
	_Load							(N,data,vCount);
	inherited::Load					(N, data, dwFlags|VLOAD_NOVERTICES|VLOAD_NOINDICES);
	Render->shader_option_skinning	(0);

	// Load indices with replication in mind
	R_ASSERT			(data->find_chunk(OGF_INDICES));
	u32					dwCount = data->r_u32();
	R_ASSERT			(dwCount%3 == 0);
	indices				= LPWORD(xr_malloc(dwCount*2));
	Memory.mem_copy		(indices,data->pointer(),dwCount*2);
	dwPrimitives		= dwCount/3;
	BOOL	bSoft		= HW.Caps.geometry.bSoftware || (dwFlags&VLOAD_FORCESOFTWARE);
	u32		dwUsage		= D3DUSAGE_WRITEONLY | (bSoft?D3DUSAGE_SOFTWAREPROCESSING:0);
	D3DPOOL	dwPool		= bSoft?D3DPOOL_SYSTEMMEM:D3DPOOL_MANAGED;
	BYTE*	bytes		= 0;
	R_CHK				(HW.pDevice->CreateIndexBuffer(dwCount*2,dwUsage,D3DFMT_INDEX16,dwPool,&pIndices,0));
	R_CHK				(pIndices->Lock(0,0,(void**)&bytes,0));
	Memory.mem_copy		(bytes, indices, dwCount*2);
	pIndices->Unlock	();

	// Create HW VB in case this is possible
	_Load_hw						();
}
void CSkeletonX_ST::Load(const char* N, IReader *data, u32 dwFlags) 
{
	_Load							(N,data,vCount);
	inherited::Load					(N,data,dwFlags|VLOAD_NOVERTICES);
	Render->shader_option_skinning	(0);
	_Load_hw						();
}

static	D3DVERTEXELEMENT9 dwDecl_1W	[] =
{
	{ 0, 0,  D3DDECLTYPE_FLOAT3,	D3DDECLMETHOD_DEFAULT, 	D3DDECLUSAGE_POSITION,		0 },	// pos
	{ 0, 12, D3DDECLTYPE_FLOAT3,	D3DDECLMETHOD_DEFAULT, 	D3DDECLUSAGE_NORMAL,		0 },	// norm
	{ 0, 24, D3DDECLTYPE_FLOAT2,	D3DDECLMETHOD_DEFAULT, 	D3DDECLUSAGE_TEXCOORD,		0 },	// tc
	{ 0, 32, D3DDECLTYPE_SHORT2,	D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_BLENDINDICES,	0 },	// indices
	D3DDECL_END()
};
struct	vertHW_1W
{
	Fvector3	P,N;
	Fvector2	uv;
	short		ids	[2];
};
static	D3DVERTEXELEMENT9 dwDecl_2W	[] =
{
	{ 0, 0,  D3DDECLTYPE_FLOAT3,	D3DDECLMETHOD_DEFAULT, 	D3DDECLUSAGE_POSITION,		0 },	// pos
	{ 0, 12, D3DDECLTYPE_FLOAT3,	D3DDECLMETHOD_DEFAULT, 	D3DDECLUSAGE_NORMAL,		0 },	// norm
	{ 0, 24, D3DDECLTYPE_FLOAT2,	D3DDECLMETHOD_DEFAULT, 	D3DDECLUSAGE_TEXCOORD,		0 },	// tc
	{ 0, 32, D3DDECLTYPE_SHORT2,	D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_BLENDINDICES,	0 },	// indices
	{ 0, 36, D3DDECLTYPE_D3DCOLOR,	D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_BLENDWEIGHT,	0 },	// weight
	D3DDECL_END()
};
struct	vertHW_2W
{
	Fvector3	P,N;
	Fvector2	uv;
	short		ids	[2];
	u32			weight;
};

void CSkeletonX::_Load_hw()
{
	// Create HW VB in case this is possible
	BOOL	bSoft		= HW.Caps.geometry.bSoftware || (dwFlags&VLOAD_FORCESOFTWARE);
	u32		dwUsage		= D3DUSAGE_WRITEONLY | (bSoft?D3DUSAGE_SOFTWAREPROCESSING:0);
	switch	(RenderMode)
	{
	case RM_SKINNING_SOFT:
		Msg					("skinning: software");
		hGeom.create		(vertRenderFVF, RCache.Vertex.Buffer(), pIndices);
		break;
	case RM_SINGLE:
		Msg					("skinning: hw, 0-weight");
		{
			vBase				= 0;
			u32		vStride		= D3DXGetFVFVertexSize		(vertRenderFVF);
			BYTE*	bytes		= 0;
			R_CHK				(HW.pDevice->CreateVertexBuffer(vCount*vStride,dwUsage,0,D3DPOOL_MANAGED,&pVertices,0));
			R_CHK				(pVertices->Lock(0,0,(void**)&bytes,0));
			vertRender*		dst	= (vertRender*)bytes;
			vertBoned1W*	src = (vertBoned1W*)*Vertices1W;
			for (u32 it=0; it<vCount; it++)	{
				dst->P			= src->P;
				dst->N			= src->N;
				dst->u			= src->u;
				dst->v			= src->v;
				dst++; src++;
			}
			pVertices->Unlock	();

			hGeom.create		(vertRenderFVF, pVertices, pIndices);
		}
		break;
	case RM_SKINNING_1B:
		Msg					("skinning: hw, 1-weight");
		{
			vBase				= 0;
			u32		vStride		= D3DXGetDeclVertexSize		(dwDecl_1W,0);
			BYTE*	bytes		= 0;
			R_CHK				(HW.pDevice->CreateVertexBuffer(vCount*vStride,dwUsage,0,D3DPOOL_MANAGED,&pVertices,0));
			R_CHK				(pVertices->Lock(0,0,(void**)&bytes,0));
			vertHW_1W*		dst	= (vertHW_1W*)bytes;
			vertBoned1W*	src = (vertBoned1W*)*Vertices1W;
			for (u32 it=0; it<vCount; it++)	{
				dst->P		= src->P;
				dst->N		= src->N;
				dst->uv.set	(src->u,src->v);
				dst->ids[0]	= u16(src->matrix)*3;
				dst->ids[1] = 0;
			}
			pVertices->Unlock	();
			hGeom.create		(dwDecl_1W, pVertices, pIndices);
		}
		break;
	case RM_SKINNING_2B:
		Msg					("skinning: hw, 2-weight");
		{
			vBase				= 0;
			u32		vStride		= D3DXGetDeclVertexSize		(dwDecl_2W,0);
			BYTE*	bytes		= 0;
			R_CHK				(HW.pDevice->CreateVertexBuffer(vCount*vStride,dwUsage,0,D3DPOOL_MANAGED,&pVertices,0));
			R_CHK				(pVertices->Lock(0,0,(void**)&bytes,0));
			vertHW_2W*		dst	= (vertHW_2W*)bytes;
			vertBoned2W*	src = (vertBoned2W*)*Vertices2W;
			for (u32 it=0; it<vCount; it++)	{
				dst->P		= src->P;
				dst->N		= src->N;
				dst->uv.set	(src->u,src->v);
				dst->ids[0]	= u16(src->matrix0)*3;
				dst->ids[1] = u16(src->matrix1)*3;
				dst->weight	= color_rgba_f(0,0,0,src->w);
			}
			pVertices->Unlock	();
			hGeom.create		(dwDecl_2W, pVertices, pIndices);
		}
		break;
	}
}
