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

#pragma pack(push,1)
float u_P	(s16 v)
{
	return	float(v)/(32767.f/12.f);
}
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
	u16 get_bone()
	{
		return	(u16)color_get_A(_N_I)/3;
	}
	void get_pos(Fvector& p)
	{
		p.x			= u_P(_P[0]);
		p.y			= u_P(_P[1]);
		p.z			= u_P(_P[2]);
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
	void set	(Fvector3& P, Fvector3 N, Fvector3 T, Fvector3 B, Fvector2& tc, int index0, int index1, float w)
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
	float get_weight()
	{
		return	float(color_get_A(_N_w))/255.f;
	}
	u16 get_bone(u16 w)
	{
		return	(u16)_tc_i[w+2]/3;
	}
	void get_pos(Fvector& p)
	{
		p.x			= u_P(_P[0]);
		p.y			= u_P(_P[1]);
		p.z			= u_P(_P[2]);
	}
};
#pragma pack(pop)

//////////////////////////////////////////////////////////////////////
// Body Part
//////////////////////////////////////////////////////////////////////
void CSkeletonX::_Copy		(CSkeletonX *B)
{
	Parent					= NULL;
	ChildIDX				= B->ChildIDX;
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
    inherited1::Copy		(V);
	CSkeletonX_PM *X		= (CSkeletonX_PM*)(V);
	_Copy					((CSkeletonX*)X);
}
void CSkeletonX_ST::Copy	(IRender_Visual *P) 
{
	inherited1::Copy		(P);
	CSkeletonX_ST *X		= (CSkeletonX_ST*)P;
	_Copy					((CSkeletonX*)X);
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
	inherited1::Release();
}
void CSkeletonX_ST::Release()
{
	inherited1::Release();
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
	inherited1::Load				(N,data,dwFlags|VLOAD_NOVERTICES);
	::Render->shader_option_skinning(0);
	vBase							= 0;
	_Load_hw						(*this,_verts_);
}
void CSkeletonX_ST::Load(const char* N, IReader *data, u32 dwFlags) 
{
	_Load							(N,data,vCount);
	void*	_verts_					= data->pointer	();
	inherited1::Load				(N,data,dwFlags|VLOAD_NOVERTICES);
	::Render->shader_option_skinning(0);
	vBase							= 0;
	_Load_hw						(*this,_verts_);
}

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

//-----------------------------------------------------------------------------------------------------
// Wallmarks
//-----------------------------------------------------------------------------------------------------
#include "cl_intersect.h"
void CSkeletonX::_CollectBoneFaces(Fvisual* V, u32 iBase, u32 iCount)
{
	u16* indices		= 0;
	R_CHK				(V->pIndices->Lock(iBase,iCount,(void**)&indices,D3DLOCK_READONLY));
	switch	(RenderMode){
	case RM_SKINNING_SOFT:{
		if (*Vertices1W){
			vertBoned1W* vertices	= *Vertices1W;
			for (u32 idx=0; idx<iCount; idx++){
				vertBoned1W& v	= vertices[V->vBase+indices[idx]];
				CBoneData& BD	= Parent->LL_GetData((u16)v.matrix);
				BD.AppendFace	(ChildIDX,(u16)idx/3);
			}
		}else{
			VERIFY			(*Vertices2W);
			vertBoned2W* vertices	= *Vertices2W;
			for (u32 idx=0; idx<iCount; idx++){
				vertBoned2W& v	= vertices[V->vBase+indices[idx]];
				CBoneData& BD	= Parent->LL_GetData((u16)v.matrix0);
				BD.AppendFace	(ChildIDX,(u16)idx/3);
				BD				= Parent->LL_GetData((u16)v.matrix1);
				BD.AppendFace	(ChildIDX,(u16)idx/3);
			}
		}
	}break;
	case RM_SINGLE:
	case RM_SKINNING_1B:{
		vertHW_1W* vertices	= 0;
		R_CHK				(V->pVertices->Lock(V->vBase,V->vCount,(void**)&vertices,D3DLOCK_READONLY));
		for (u32 idx=0; idx<iCount; idx++){
			vertHW_1W& v	= vertices[indices[idx]];
			CBoneData& BD	= Parent->LL_GetData(v.get_bone());
			BD.AppendFace	(ChildIDX,(u16)idx/3);
		}
		V->pVertices->Unlock();
	}break;
	case RM_SKINNING_2B:{
		vertHW_2W* vertices	= 0;
		R_CHK				(V->pVertices->Lock(V->vBase,V->vCount,(void**)&vertices,D3DLOCK_READONLY));
		for (u32 idx=0; idx<iCount; idx++){
			vertHW_2W& v	= vertices[indices[idx]];
			CBoneData& BD0	= Parent->LL_GetData(v.get_bone(0));
			BD0.AppendFace	(ChildIDX,(u16)idx/3);
			CBoneData& BD1	= Parent->LL_GetData(v.get_bone(1));
			BD1.AppendFace	(ChildIDX,(u16)idx/3);
		}
		R_CHK				(V->pVertices->Unlock());
	}break;
	}
	R_CHK					(V->pIndices->Unlock());
}

void CSkeletonX_ST::AfterLoad(CKinematics* parent, u16 child_idx)
{
	inherited2::AfterLoad			(parent,child_idx);
	inherited2::_CollectBoneFaces	(this,iBase,iCount);
}

void CSkeletonX_PM::AfterLoad(CKinematics* parent, u16 child_idx)
{
	inherited2::AfterLoad			(parent,child_idx);
	SlideWindow& SW					= pSWs[0];
	inherited2::_CollectBoneFaces	(this,iBase+SW.offset,SW.num_tris*3);
}

/*
BOOL CSkeletonX::_PickBoneSoft1W	(float& dist, const Fvector& S, const Fvector& D, Fvisual* V, CBoneData::IndicesVec& indices)
{
	VERIFY				(*Vertices1W);
	BOOL intersect		= FALSE;
	for (u32 k=0; k<indices.size()/3; k++){
		Fvector* p[3];
		p[0]			= &Vertices1W[V->vBase+indices[k*3+0]].P;
		p[1]			= &Vertices1W[V->vBase+indices[k*3+1]].P;
		p[2]			= &Vertices1W[V->vBase+indices[k*3+2]].P;
		float u,v,range	= 0.f;
		if (CDB::TestRayTri(S,D,p,u,v,range,true)&&(range<dist)){
			dist		= range;
			intersect	= TRUE;
		}
	}
	return intersect;
}
BOOL CSkeletonX::_PickBoneSoft2W	(float& dist, const Fvector& S, const Fvector& D, Fvisual* V, CBoneData::IndicesVec& indices)
{
	VERIFY				(*Vertices2W);
	bool intersect		= FALSE;
	for (u32 k=0; k<indices.size()/3; k++){
		Fvector* p[3];
		p[0]			= &Vertices2W[V->vBase+indices[k*3+0]].P;
		p[1]			= &Vertices2W[V->vBase+indices[k*3+1]].P;
		p[2]			= &Vertices2W[V->vBase+indices[k*3+2]].P;
		float u,v,range	= 0.f;
		if (CDB::TestRayTri(S,D,p,u,v,range,true)&&(range<dist)){
			dist		= range;
			intersect	= TRUE;
		}
	}
	return intersect;
}*/
BOOL CSkeletonX::_PickBoneHW1W	(Fvector& normal, float& dist, const Fvector& S, const Fvector& D, Fvisual* V, u16* indices, CBoneData::FacesVec& faces)
{
	vertHW_1W* vertices;
	R_CHK				(V->pVertices->Lock(V->vBase,V->vCount,(void**)&vertices,D3DLOCK_READONLY));
	bool intersect		= FALSE;
	for (CBoneData::FacesVecIt it=faces.begin(); it!=faces.end(); it++){
		Fvector			p[3];
		u32 idx			= (*it)*3;
		for (u32 k=0; k<3; k++){
			vertHW_1W& V			= vertices[indices[idx+k]];
			const Fmatrix& xform	= Parent->LL_GetBoneInstance(V.get_bone()).mRenderTransform; 
			V.get_pos	(p[k]);		xform.transform_tiny(p[k]);
		}
		float u,v,range	= flt_max;
		if (CDB::TestRayTri(S,D,p,u,v,range,true)&&(range<dist)){
			normal.mknormal(p[0],p[1],p[2]);
			dist		= range;
			intersect	= TRUE;
		}
	}
	R_CHK				(V->pVertices->Unlock());
	return intersect;
}
BOOL CSkeletonX::_PickBoneHW2W	(Fvector& normal, float& dist, const Fvector& S, const Fvector& D, Fvisual* V, u16* indices, CBoneData::FacesVec& faces)
{
	vertHW_2W* vertices;
	R_CHK				(V->pVertices->Lock(V->vBase,V->vCount,(void**)&vertices,D3DLOCK_READONLY));
	bool intersect		= FALSE;
	for (CBoneData::FacesVecIt it=faces.begin(); it!=faces.end(); it++){
		Fvector			p[3];
		u32 idx			= (*it)*3;
		for (u32 k=0; k<3; k++){
			Fvector		P0,P1;
			vertHW_2W& V			= vertices[indices[idx+k]];
			Fmatrix& xform0			= Parent->LL_GetBoneInstance(V.get_bone(0)).mRenderTransform; 
			Fmatrix& xform1			= Parent->LL_GetBoneInstance(V.get_bone(1)).mRenderTransform; 
			V.get_pos	(P0);		xform0.transform_tiny(P0);
			V.get_pos	(P1);		xform1.transform_tiny(P1);
			p[k].lerp				(P0,P1,V.get_weight());
		}
		float u,v,range	= flt_max;
		if (CDB::TestRayTri(S,D,p,u,v,range,true)&&(range<dist)){
			normal.mknormal(p[0],p[1],p[2]);
			dist		= range;
			intersect	= TRUE;
		}
	}
	R_CHK				(V->pVertices->Unlock());
	return intersect;
}


BOOL CSkeletonX::_PickBone			(Fvector& normal, float& dist, const Fvector& start, const Fvector& dir, Fvisual* V, u16 bone_id, u32 iBase, u32 iCount)
{
	VERIFY				(Parent&&(ChildIDX!=u16(-1)));
	CBoneData& BD					= Parent->LL_GetData(bone_id);
	CBoneData::FacesVec*	faces	= &BD.child_faces[ChildIDX];
	u16* indices		= 0;
	R_CHK				(V->pIndices->Lock(iBase,iCount,		(void**)&indices,	D3DLOCK_READONLY));
	// fill vertices
	BOOL result			= FALSE;
	switch	(RenderMode){
	case RM_SKINNING_SOFT:
//		if (*Vertices1W)result = _PickBoneSoft1W	(dist,start,dir,V,indices);
//		else			result = _PickBoneSoft2W	(dist,start,dir,V,indices);
	break;
	case RM_SINGLE:
	case RM_SKINNING_1B:	result = _PickBoneHW1W	(normal,dist,start,dir,V,indices,*faces); 	break;
	case RM_SKINNING_2B:	result = _PickBoneHW2W	(normal,dist,start,dir,V,indices,*faces);	break;
	default: NODEFAULT;
	}
	R_CHK				(V->pIndices->Unlock());
	return result;
}
BOOL CSkeletonX_ST::PickBone		(Fvector& normal, float& dist, const Fvector& start, const Fvector& dir, u16 bone_id)
{
	return inherited2::_PickBone	(normal,dist,start,dir,this,bone_id,iBase,iCount);
}
BOOL CSkeletonX_PM::PickBone		(Fvector& normal, float& dist, const Fvector& start, const Fvector& dir, u16 bone_id)
{
	SlideWindow& SW					= pSWs[0];
	return inherited2::_PickBone	(normal,dist,start,dir,this,bone_id,iBase+SW.offset,SW.num_tris*3);
}

/*
// Fill Vertices
void CSkeletonX::_FillVerticesSoft1W(CSkeletonWallmark& wm, const Fvector& pt, float size, Fvisual* V, CBoneData::IndicesVec& indices, u16 bone_id)
{
	VERIFY				(*Vertices1W);
	for (u32 k=0; k<indices.size()/3; k++){
		CSkeletonWallmark::WMFace F;
		F.pos[0]	= Vertices1W[V->vBase+indices[k*3+0]].P;
		F.pos[1]	= Vertices1W[V->vBase+indices[k*3+1]].P;
		F.pos[2]	= Vertices1W[V->vBase+indices[k*3+2]].P;
		if (CDB::TestSphereTri(pt,size,F.pos)){
			F.id	= bone_id;
			wm.faces.push_back(F);
		}
	}
}
void CSkeletonX::_FillVerticesSoft2W(CSkeletonWallmark& wm, const Fvector& pt, float size, Fvisual* V, CBoneData::IndicesVec& indices, u16 bone_id)
{
	VERIFY				(*Vertices2W);
	for (u32 k=0; k<indices.size()/3; k++){
		CSkeletonWallmark::WMFace F;
		F.pos[0]	= Vertices2W[V->vBase+indices[k*3+0]].P;
		F.pos[1]	= Vertices2W[V->vBase+indices[k*3+1]].P;
		F.pos[2]	= Vertices2W[V->vBase+indices[k*3+2]].P;
		if (CDB::TestSphereTri(pt,size,F.pos)){
			F.id	= bone_id;
			wm.faces.push_back(F);
		}
	}
}
*/
void CSkeletonX::_FillVerticesHW1W(const Fmatrix& view, CSkeletonWallmark& wm, const Fvector& normal, float size, Fvisual* V, u16* indices, CBoneData::FacesVec& faces)
{
	vertHW_1W*			vertices;
	R_CHK				(V->pVertices->Lock(V->vBase,V->vCount,(void**)&vertices,D3DLOCK_READONLY));
	for (CBoneData::FacesVecIt it=faces.begin(); it!=faces.end(); it++){
		Fvector			p[3];
		u32 idx			= (*it)*3;
		CSkeletonWallmark::WMFace F;
		for (u32 k=0; k<3; k++){
			vertHW_1W& V			= vertices[indices[idx+k]];
			F.bone_id[k][0]			= V.get_bone();
			F.bone_id[k][1]			= F.bone_id[k][0];
			F.weight[k]				= 0.f;
			const Fmatrix& xform	= Parent->LL_GetBoneInstance(F.bone_id[k][0]).mRenderTransform; 
			V.get_pos				(F.vert[k]);
			xform.transform_tiny	(p[k],F.vert[k]);
		}
		Fvector test_normal;
		test_normal.mknormal	(p[0],p[1],p[2]);
		float cosa				= test_normal.dotproduct(normal);
		if (cosa<EPS)			continue;
		if (CDB::TestSphereTri(wm.contact_point,size,p))
		{
			Fvector				UV;
			for (u32 k=0; k<3; k++){
				Fvector2& uv	= F.uv[k];
				view.transform_tiny(UV,p[k]);
				uv.x			= (1+UV.x)*.5f;
				uv.y			= (1-UV.y)*.5f;
			}
			wm.s_faces.push_back(F);
		}
	}
	R_CHK				(V->pVertices->Unlock());
}
void CSkeletonX::_FillVerticesHW2W(const Fmatrix& view, CSkeletonWallmark& wm, const Fvector& normal, float size, Fvisual* V, u16* indices, CBoneData::FacesVec& faces)
{
	vertHW_2W* vertices;
	R_CHK				(V->pVertices->Lock(V->vBase,V->vCount,(void**)&vertices,D3DLOCK_READONLY));
	for (CBoneData::FacesVecIt it=faces.begin(); it!=faces.end(); it++){
		Fvector			p[3];
		u32 idx			= (*it)*3;
		CSkeletonWallmark::WMFace F;
		for (u32 k=0; k<3; k++){
			Fvector		P0,P1;
			vertHW_2W& V			= vertices[indices[idx+k]];
			F.bone_id[k][0]			= V.get_bone(0);
			F.bone_id[k][1]			= V.get_bone(1);
			F.weight[k]				= V.get_weight();
			Fmatrix& xform0			= Parent->LL_GetBoneInstance(F.bone_id[k][0]).mRenderTransform; 
			Fmatrix& xform1			= Parent->LL_GetBoneInstance(F.bone_id[k][1]).mRenderTransform; 
			V.get_pos				(F.vert[k]);		
			xform0.transform_tiny	(P0,F.vert[k]);
			xform1.transform_tiny	(P1,F.vert[k]);
			p[k].lerp				(P0,P1,F.weight[k]);
		}
		Fvector test_normal;
		test_normal.mknormal	(p[0],p[1],p[2]);
		float cosa				= test_normal.dotproduct(normal);
		if (cosa<EPS)			continue;
		if (CDB::TestSphereTri(wm.contact_point,size,p)){
			Fvector				UV;
			for (u32 k=0; k<3; k++){
				Fvector2& uv	= F.uv[k];
				view.transform_tiny(UV,p[k]);
				uv.x			= (1+UV.x)*.5f;
				uv.y			= (1-UV.y)*.5f;
			}
			wm.s_faces.push_back(F);
		}
	}
	R_CHK				(V->pVertices->Unlock());
}

void CSkeletonX::_FillVertices(const Fmatrix& view, CSkeletonWallmark& wm, const Fvector& normal, float size, Fvisual* V, u16 bone_id, u32 iBase, u32 iCount)
{
	VERIFY				(Parent&&(ChildIDX!=u16(-1)));
	CBoneData& BD					= Parent->LL_GetData(bone_id);
	CBoneData::FacesVec*	faces	= &BD.child_faces[ChildIDX];
	u16* indices		= 0;
	R_CHK				(V->pIndices->Lock(iBase,iCount,		(void**)&indices,	D3DLOCK_READONLY));
	// fill vertices
	switch	(RenderMode){
	case RM_SKINNING_SOFT:
//		if (*Vertices1W)			_FillVerticesSoft1W		(wm,pt,size,V,indices,bone_id);
//		else						_FillVerticesSoft2W		(wm,pt,size,V,indices,bone_id);
		break;
	case RM_SINGLE:
	case RM_SKINNING_1B:			_FillVerticesHW1W		(view,wm,normal,size,V,indices,*faces);		break;
	case RM_SKINNING_2B:			_FillVerticesHW2W		(view,wm,normal,size,V,indices,*faces);		break;
	}
	R_CHK				(V->pIndices->Unlock());
}

void CSkeletonX_ST::FillVertices	(const Fmatrix& view, CSkeletonWallmark& wm, const Fvector& normal, float size, u16 bone_id)
{
	inherited2::_FillVertices		(view,wm,normal,size,this,bone_id,iBase,iCount);
}
void CSkeletonX_PM::FillVertices	(const Fmatrix& view, CSkeletonWallmark& wm, const Fvector& normal, float size, u16 bone_id)
{
	SlideWindow& SW					= pSWs[0];
	inherited2::_FillVertices		(view,wm,normal,size,this,bone_id,iBase+SW.offset,SW.num_tris*3);
}
