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
static	shared_str	sbones_array;

//////////////////////////////////////////////////////////////////////
// Body Part
//////////////////////////////////////////////////////////////////////
void CSkeletonX::_Copy		(CSkeletonX *B)
{
	Parent					= NULL;
	Vertices1W				= B->Vertices1W;
	Vertices2W				= B->Vertices2W;

	// caution - overlapped (union)
	cache_DiscardID			= B->cache_DiscardID;
	cache_vCount			= B->cache_vCount;
	cache_vOffset			= B->cache_vOffset;
	RenderMode				= B->RenderMode;
	RMS_boneid				= B->RMS_boneid;
	RMS_bonecount			= B->RMS_bonecount;
}
void CSkeletonX_PM::Copy	(IRender_Visual *V) 
{
//.	Fvisual::Copy			(V);
    FProgressive::Copy		(V);
//.	pm_copy					(V);

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
	int lod_id				= iFloor((1.f-LOD)*float(SW_count-1));
	SlideWindow& SW			= pSWs[lod_id];
	_Render		(hGeom,SW.num_verts,SW.offset,SW.num_tris);
}
void CSkeletonX_ST::Render		(float LOD) 
{
	_Render		(hGeom,vCount,0,dwPrimitives);
}
void CSkeletonX::_Render		(ref_geom& hGeom, u32 vCount, u32 iOffset, u32 pCount)
{
	switch (RenderMode)
	{
	case RM_SKINNING_SOFT:
		_Render_soft			(hGeom,vCount,iOffset,pCount);
		break;
	case RM_SINGLE:	
		{
			Fmatrix	W;	W.mul_43	(RCache.xforms.m_w,Parent->LL_GetTransform_R	(u16(RMS_boneid)));
			RCache.set_xform_world	(W);
			RCache.set_Geometry		(hGeom);
			RCache.Render			(D3DPT_TRIANGLELIST,0,0,vCount,iOffset,pCount);
		}
		break;
	case RM_SKINNING_1B:
	case RM_SKINNING_2B:
		{
			// transfer matrices
			R_constant*				array	= RCache.get_c				(sbones_array);
			u32						count	= RMS_bonecount;
			for (u32 mid = 0; mid<count; mid++)	{
				Fmatrix&	M				= Parent->LL_GetTransform_R				(u16(mid));
				u32			id				= mid*3;
				RCache.set_ca	(array,id+0,M._11,M._21,M._31,M._41);
				RCache.set_ca	(array,id+1,M._12,M._22,M._32,M._42);
				RCache.set_ca	(array,id+2,M._13,M._23,M._33,M._43);
			}

			// render
			RCache.set_Geometry		(hGeom);
			RCache.Render			(D3DPT_TRIANGLELIST,0,0,vCount,iOffset,pCount);
		}
		break;
	}
}
void CSkeletonX::_Render_soft	(ref_geom& hGeom, u32 vCount, u32 iOffset, u32 pCount)
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
	RCache.Render			(D3DPT_TRIANGLELIST,vOffset,0,vCount,iOffset,pCount);
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
void CSkeletonX::_Load	(const char* N, IReader *data, u32& dwVertCount) 
{	
	sbones_array	= "sbones_array";
	xr_vector<u32>	bids;

	// Load vertices
	R_ASSERT	(data->find_chunk(OGF_VERTICES));
			
	u32			hw_bones	= (HW.Caps.geometry.dwRegisters-22)/3;
	u32			sw_bones	= 0;
#ifdef _EDITOR
	hw_bones	= 0;
#endif

	u32			dwVertType,size,it,crc;
	dwVertType	= data->r_u32(); 
	dwVertCount	= data->r_u32();

	RenderMode						= RM_SKINNING_SOFT;
	Render->shader_option_skinning	(-1);
	switch		(dwVertType)
	{
	case OGF_VERTEXFORMAT_FVF_1L: // 1-Link
		{
			size			= dwVertCount*sizeof(vertBoned1W);
#ifdef _EDITOR
			// software
			crc					= crc32	(data->pointer(),size);
			Vertices1W.create	(crc,dwVertCount,(vertBoned1W*)data->pointer());
#else
			vertBoned1W* VO = (vertBoned1W*)data->pointer();
			for (it=0; it<dwVertCount; it++)	{
				u32		mid = VO[it].matrix;
				if		(bids.end() == std::find(bids.begin(),bids.end(),mid))	bids.push_back(mid);
				if		(mid>sw_bones)	sw_bones = mid;
			}
            
			if	(1==bids.size())	{
				// HW- single bone
				RenderMode						= RM_SINGLE;
				RMS_boneid						= *bids.begin();
				Render->shader_option_skinning	(0);
			} else if (sw_bones<=hw_bones) {
				// HW- one weight
				RenderMode						= RM_SKINNING_1B;
				RMS_bonecount					= sw_bones+1;
				Render->shader_option_skinning	(1);
			} else {
				// software
				crc					= crc32	(data->pointer(),size);
				Vertices1W.create	(crc,dwVertCount,(vertBoned1W*)data->pointer());
				Render->shader_option_skinning	(-1);
			}
#endif        
		}
		break;
	case OGF_VERTEXFORMAT_FVF_2L: // 2-Link
		{
			size			= dwVertCount*sizeof(vertBoned2W);
			vertBoned2W* VO = (vertBoned2W*)data->pointer();
			for (it=0; it<dwVertCount; it++)	{
				if (VO[it].matrix0>sw_bones)	sw_bones = VO[it].matrix0;
				if (VO[it].matrix1>sw_bones)	sw_bones = VO[it].matrix1;
			}
			if (sw_bones<=hw_bones) {
				// HW- two weights
				RenderMode						= RM_SKINNING_2B;
				RMS_bonecount					= sw_bones+1;
				Render->shader_option_skinning	(2);
			} else {
				// software
				crc			= crc32	(data->pointer(),size);
				Vertices2W.create	(crc,dwVertCount,(vertBoned2W*)data->pointer());
				Render->shader_option_skinning	(-1);
			}
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
	void*	_verts_					= data->pointer	();
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
	vBase				= 0;
	_Load_hw			(*this,_verts_);
}
void CSkeletonX_ST::Load(const char* N, IReader *data, u32 dwFlags) 
{
	_Load							(N,data,vCount);
	void*	_verts_					= data->pointer	();
	inherited::Load					(N,data,dwFlags|VLOAD_NOVERTICES);
	::Render->shader_option_skinning(0);
	vBase							= 0;
	_Load_hw						(*this,_verts_);
}
#pragma pack(push,1)
s16	q_P		(float v)
{
	int		_v	= clampr(iFloor(v*(32767.f/12.f)), -32768, 32767);
	return	s16	(_v);
}
u8	q_N		(float v)
{
	int		_v	= clampr(iFloor((v+1.f)*127.5f), 0, 255);
	return	u8	(_v);
}
s16	q_tc	(float v)
{
	int		_v	= clampr(iFloor(v*(32767.f/16.f)), -32768, 32767);
	return	s16	(_v);
}
#ifdef _DEBUG
float errN	(Fvector3 v, u8* qv)
{
	Fvector3	uv;	
	uv.set		(float(qv[0]),float(qv[1]),float(qv[2])).div(255.f).mul(2.f).sub(1.f);
	uv.normalize();
	return		v.dotproduct(uv);
}
#else
float errN	(Fvector3 v, u8* qv)	{ return 0; }
#endif

static	D3DVERTEXELEMENT9 dwDecl_01W	[] =	// 24bytes
{
	{ 0, 0,		D3DDECLTYPE_SHORT4,		D3DDECLMETHOD_DEFAULT, 	D3DDECLUSAGE_POSITION,		0 },	// : P						: 2	: -12..+12
	{ 0, 8,		D3DDECLTYPE_D3DCOLOR,	D3DDECLMETHOD_DEFAULT, 	D3DDECLUSAGE_NORMAL,		0 },	// : N, w=index(RC, 0..1)	: 1	:  -1..+1
	{ 0, 12,	D3DDECLTYPE_D3DCOLOR,	D3DDECLMETHOD_DEFAULT, 	D3DDECLUSAGE_TANGENT,		0 },	// : T						: 1	:  -1..+1
	{ 0, 16,	D3DDECLTYPE_D3DCOLOR,	D3DDECLMETHOD_DEFAULT, 	D3DDECLUSAGE_BINORMAL,		0 },	// : B						: 1	:  -1..+1
	{ 0, 20,	D3DDECLTYPE_SHORT2,		D3DDECLMETHOD_DEFAULT, 	D3DDECLUSAGE_TEXCOORD,		0 },	// : tc						: 1	: -16..+16
	D3DDECL_END()
};
struct	vertHW_1W
{
	s16			_P		[4];
	u32			_N_I	;
	u32			_T		;
	u32			_B		;
	s16			_tc		[2];
	void set	(Fvector3& P, Fvector3 N, Fvector3 T, Fvector3 B, Fvector2& tc, int index)
	{
		N.normalize_safe();
		T.normalize_safe();
		B.normalize_safe();
		_P[0]		= q_P(P.x);
		_P[1]		= q_P(P.y);
		_P[2]		= q_P(P.z);
		_P[3]		= q_P(1);
		_N_I		= color_rgba(q_N(N.x), q_N(N.y), q_N(N.z), u8(index));
		_T			= color_rgba(q_N(T.x), q_N(T.y), q_N(T.z), 0);
		_B			= color_rgba(q_N(B.x), q_N(B.y), q_N(B.z), 0);
		_tc[0]		= q_tc(tc.x);
		_tc[1]		= q_tc(tc.y);
	}
};

static	D3DVERTEXELEMENT9 dwDecl_2W	[] =	// 28bytes
{
	{ 0, 0,		D3DDECLTYPE_SHORT4,		D3DDECLMETHOD_DEFAULT, 	D3DDECLUSAGE_POSITION,		0 },	// : p					: 2	: -12..+12
	{ 0, 8,		D3DDECLTYPE_D3DCOLOR,	D3DDECLMETHOD_DEFAULT, 	D3DDECLUSAGE_NORMAL,		0 },	// : n.xyz, w = weight	: 1	:  -1..+1, w=0..1
	{ 0, 12,	D3DDECLTYPE_D3DCOLOR,	D3DDECLMETHOD_DEFAULT, 	D3DDECLUSAGE_TANGENT,		0 },	// : T						: 1	:  -1..+1
	{ 0, 16,	D3DDECLTYPE_D3DCOLOR,	D3DDECLMETHOD_DEFAULT, 	D3DDECLUSAGE_BINORMAL,		0 },	// : B						: 1	:  -1..+1
	{ 0, 20,	D3DDECLTYPE_SHORT4,		D3DDECLMETHOD_DEFAULT, 	D3DDECLUSAGE_TEXCOORD,		0 },	// : xy(tc), zw(indices): 2	: -16..+16, zw[0..32767]
	D3DDECL_END()
};
struct	vertHW_2W
{
	s16			_P		[4];
	u32			_N_w	;
	u32			_T		;
	u32			_B		;
	s16			_tc_i	[4];
	void set(Fvector3& P, Fvector3 N, Fvector3 T, Fvector3 B, Fvector2& tc, int index0, int index1, float w)
	{
		N.normalize_safe	();
		T.normalize_safe	();
		B.normalize_safe	();
		_P[0]		= q_P	(P.x);
		_P[1]		= q_P	(P.y);
		_P[2]		= q_P	(P.z);
		_P[3]		= 1;
		_N_w		= color_rgba(q_N(N.x), q_N(N.y), q_N(N.z), u8(clampr(iFloor(w*255.f+.5f),0,255)));
		_T			= color_rgba(q_N(T.x), q_N(T.y), q_N(T.z), 0);
		_B			= color_rgba(q_N(B.x), q_N(B.y), q_N(B.z), 0);
		_tc_i[0]	= q_tc	(tc.x);
		_tc_i[1]	= q_tc	(tc.y);
		_tc_i[2]	= s16	(index0);
		_tc_i[3]	= s16	(index1);
	}
};
#pragma pack(pop)

void CSkeletonX::_Load_hw	(Fvisual& V, void *	_verts_)
{
	// Create HW VB in case this is possible
	BOOL	bSoft				= HW.Caps.geometry.bSoftware;
	u32		dwUsage				= D3DUSAGE_WRITEONLY | (bSoft?D3DUSAGE_SOFTWAREPROCESSING:0);
	switch	(RenderMode)
	{
	case RM_SKINNING_SOFT:
		//Msg					("skinning: software");
		V.hGeom.create			(vertRenderFVF, RCache.Vertex.Buffer(), V.pIndices);
		break;
	case RM_SINGLE:
	case RM_SKINNING_1B:
		//Msg					("skinning: hw, 1-weight");
		{
			u32		vStride		= D3DXGetDeclVertexSize		(dwDecl_01W,0);
			VERIFY	(vStride==sizeof(vertHW_1W));
			BYTE*	bytes		= 0;
			R_CHK				(HW.pDevice->CreateVertexBuffer(V.vCount*vStride,dwUsage,0,D3DPOOL_MANAGED,&V.pVertices,0));
			R_CHK				(V.pVertices->Lock(0,0,(void**)&bytes,0));
			vertHW_1W*		dst	= (vertHW_1W*)bytes;
			vertBoned1W*	src = (vertBoned1W*)_verts_;
			for (u32 it=0; it<V.vCount; it++)	{
				Fvector2	uv; uv.set(src->u,src->v);
				dst->set	(src->P,src->N,src->T,src->B,uv,src->matrix*3);
				dst++; src++;
			}
			V.pVertices->Unlock	();
			V.hGeom.create		(dwDecl_01W, V.pVertices, V.pIndices);
		}  
		break;
	case RM_SKINNING_2B:
		//Msg					("skinning: hw, 2-weight");
		{
			u32		vStride		= D3DXGetDeclVertexSize		(dwDecl_2W,0);
			VERIFY	(vStride==sizeof(vertHW_2W));
			BYTE*	bytes		= 0;
			R_CHK				(HW.pDevice->CreateVertexBuffer(V.vCount*vStride,dwUsage,0,D3DPOOL_MANAGED,&V.pVertices,0));
			R_CHK				(V.pVertices->Lock(0,0,(void**)&bytes,0));
			vertHW_2W*		dst	= (vertHW_2W*)bytes;
			vertBoned2W*	src = (vertBoned2W*)_verts_;
			for (u32 it=0; it<V.vCount; it++)	{
				Fvector2	uv; uv.set(src->u,src->v);
				dst->set	(src->P,src->N,src->T,src->B,uv,int(src->matrix0)*3,int(src->matrix1)*3,src->w);
				dst++;		src++;
			}
			V.pVertices->Unlock	();
			V.hGeom.create		(dwDecl_2W, V.pVertices, V.pIndices);
		}
		break;
	}
}
