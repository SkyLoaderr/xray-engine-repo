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
static	ref_str	sbones_array;

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
			
	u32			hw_bones	= (HW.Caps.geometry.dwRegisters-22)/3;
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
			RenderMode						= RM_SKINNING_2B;
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
	::Render->shader_option_skinning(0);

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
	vBase							= 0;
	_Load_hw						(*this);
}
void CSkeletonX_ST::Load(const char* N, IReader *data, u32 dwFlags) 
{
	_Load							(N,data,vCount);
	inherited::Load					(N,data,dwFlags|VLOAD_NOVERTICES);
	::Render->shader_option_skinning(0);
	vBase							= 0;
	_Load_hw						(*this);
}
#pragma pack(push,1)
s16	q_P		(float v)
{
	int		_v	= clampr(iFloor(v*(32767.f/12.f)), -32768, 32767);
	return	s16	(_v);
}
u8	q_N		(float v)
{
	int		_v	= clampr(iFloor(((v+1)*.5f)*255.f + .5f), 0, 255);
	return	u8	(_v);
}
s16	q_tc	(float v)
{
	int		_v	= clampr(iFloor(v*(32767.f/16.f)), -32768, 32767);
	return	s16	(_v);
}

static	D3DVERTEXELEMENT9 dwDecl_1W	[] =	// 16bytes
{
	{ 0, 0,		D3DDECLTYPE_SHORT4,		D3DDECLMETHOD_DEFAULT, 	D3DDECLUSAGE_POSITION,		0 },	// : P						: 2	: -12..+12
	{ 0, 8,		D3DDECLTYPE_D3DCOLOR,	D3DDECLMETHOD_DEFAULT, 	D3DDECLUSAGE_NORMAL,		0 },	// : N, w=index(RC, 0..1)	: 1	:  -1..+1
	{ 0, 12,	D3DDECLTYPE_SHORT2,		D3DDECLMETHOD_DEFAULT, 	D3DDECLUSAGE_TEXCOORD,		0 },	// : tc						: 1	: -16..+16
	D3DDECL_END()
};
struct	vertHW_1W
{
	s16			P	[4];
	u8			N_I	[4];
	s16			tc	[2];
	void set(Fvector3& P, Fvector3& N, Fvector2& tc, int index)
	{
		P[0]	= q_P(P.x);
		P[1]	= q_P(P.y);
		P[2]	= q_P(P.z);
		P[3]	= q_P(1);
		N_I[0]	= q_N(N.x);
		N_I[1]	= q_N(N.y);
		N_I[2]	= q_N(N.z);
		N_I[3]	= u8(index);
		tc[0]	= q_tc(tc.x);
		tc[1]	= q_tc(tc.y);
	}
};
static	D3DVERTEXELEMENT9 dwDecl_2W	[] =	// 32bytes
{
	{ 0, 0,		D3DDECLTYPE_SHORT4,		D3DDECLMETHOD_DEFAULT, 	D3DDECLUSAGE_POSITION,		0 },	// : p0					: 2	: -12..+12
	{ 0, 8,		D3DDECLTYPE_SHORT4,		D3DDECLMETHOD_DEFAULT, 	D3DDECLUSAGE_POSITION,		1 },	// : p1 				: 2	: -12..+12
	{ 0, 16,	D3DDECLTYPE_D3DCOLOR,	D3DDECLMETHOD_DEFAULT, 	D3DDECLUSAGE_NORMAL,		0 },	// : n0.xyz, w = weight	: 1	:  -1..+1, w=0..1
	{ 0, 20,	D3DDECLTYPE_D3DCOLOR,	D3DDECLMETHOD_DEFAULT, 	D3DDECLUSAGE_NORMAL,		1 },	// : n1					: 1	:  -1..+1
	{ 0, 24,	D3DDECLTYPE_SHORT4,		D3DDECLMETHOD_DEFAULT, 	D3DDECLUSAGE_TEXCOORD,		0 },	// : xy(tc), zw(indices): 2	: -16..+16, zw[0..32767]
	D3DDECL_END()
};
struct	vertHW_2W
{
	s16			P0	[4];
	s16			P1	[4];
	u8			N0_w[4];
	u8			N1	[4];
	s16			tc_i[4];
	void set(Fvector3& P0, Fvector3& P1, Fvector3& N0, Fvector3& N1, Fvector2& tc, int index0, int index1, float w)
	{
		P0[0]	= q_P(P0.x);
		P0[1]	= q_P(P0.y);
		P0[2]	= q_P(P0.z);
		P0[3]	= q_P(1);
		P1[0]	= q_P(P1.x);
		P1[1]	= q_P(P1.y);
		P1[2]	= q_P(P1.z);
		P1[3]	= q_P(1);
		N0_w[0]	= q_N(N0.x);
		N0_w[1]	= q_N(N0.y);
		N0_w[2]	= q_N(N0.z);
		N0_w[3]	= u8(clampr(iFloor(w*255.f+.5f),0,255));
		N1[0]	= q_N(N1.x);
		N1[1]	= q_N(N1.y);
		N1[2]	= q_N(N1.z);
		N1[3]	= 0;
		tc_i[0]	= q_tc(tc.x);
		tc_i[1]	= q_tc(tc.y);
		tc_i[2]	= s16(index0);
		tc_i[3]	= s16(index1);
	}
};
#pragma pack(pop)

void CSkeletonX::_Load_hw	(Fvisual& V)
{
	// Create HW VB in case this is possible
	BOOL	bSoft		= HW.Caps.geometry.bSoftware;
	u32		dwUsage		= D3DUSAGE_WRITEONLY | (bSoft?D3DUSAGE_SOFTWAREPROCESSING:0);
	switch	(RenderMode)
	{
	case RM_SKINNING_SOFT:
		Msg					("skinning: software");
		V.hGeom.create		(vertRenderFVF, RCache.Vertex.Buffer(), V.pIndices);
		break;
	case RM_SINGLE:
		Msg					("skinning: hw, 0-weight");
		{
			u32		vStride		= D3DXGetFVFVertexSize		(vertRenderFVF);
			BYTE*	bytes		= 0;
			R_CHK				(HW.pDevice->CreateVertexBuffer(V.vCount*vStride,dwUsage,0,D3DPOOL_MANAGED,&V.pVertices,0));
			R_CHK				(V.pVertices->Lock(0,0,(void**)&bytes,0));
			vertRender*		dst	= (vertRender*)bytes;
			vertBoned1W*	src = (vertBoned1W*)*Vertices1W;
			for (u32 it=0; it<V.vCount; it++)	{
				dst->P			= src->P;
				dst->N			= src->N;
				dst->u			= src->u;
				dst->v			= src->v;
				dst++; src++;
			}
			V.pVertices->Unlock	();
			V.hGeom.create		(vertRenderFVF, V.pVertices, V.pIndices);
		}
		break;
	case RM_SKINNING_1B:
		Msg					("skinning: hw, 1-weight");
		{
			u32		vStride		= D3DXGetDeclVertexSize		(dwDecl_1W,0);
			VERIFY	(vStride==sizeof(vertHW_1W));
			BYTE*	bytes		= 0;
			R_CHK				(HW.pDevice->CreateVertexBuffer(V.vCount*vStride,dwUsage,0,D3DPOOL_MANAGED,&V.pVertices,0));
			R_CHK				(V.pVertices->Lock(0,0,(void**)&bytes,0));
			vertHW_1W*		dst	= (vertHW_1W*)bytes;
			vertBoned1W*	src = (vertBoned1W*)*Vertices1W;
			for (u32 it=0; it<V.vCount; it++)	{
				Fvector2	uv; uv.set(src->u,src->v);
				dst->set	(src->P,src->N,uv,src->matrix*3);
				dst++; src++;
			}
			V.pVertices->Unlock	();
			V.hGeom.create		(dwDecl_1W, V.pVertices, V.pIndices);
		}
		break;
	case RM_SKINNING_2B:
		Msg					("skinning: hw, 2-weight");
		{
			u32		vStride		= D3DXGetDeclVertexSize		(dwDecl_2W,0);
			u32		vStruct		= sizeof(vertHW_2W);
			VERIFY	(vStride==vStruct);
			BYTE*	bytes		= 0;
			R_CHK				(HW.pDevice->CreateVertexBuffer(V.vCount*vStride,dwUsage,0,D3DPOOL_MANAGED,&V.pVertices,0));
			R_CHK				(V.pVertices->Lock(0,0,(void**)&bytes,0));
			vertHW_2W*		dst	= (vertHW_2W*)bytes;
			vertBoned2W*	src = (vertBoned2W*)*Vertices2W;
			for (u32 it=0; it<V.vCount; it++)	{
				Fvector2	uv; uv.set(src->u,src->v);
				dst->set	(src->P0,src->P1,src->N0,src->N1,uv,int(src->matrix0)*3,int(src->matrix1)*3,src->w);
				dst++;		src++;
			}
			V.pVertices->Unlock	();
			V.hGeom.create		(dwDecl_2W, V.pVertices, V.pIndices);
		}
		break;
	}
}
