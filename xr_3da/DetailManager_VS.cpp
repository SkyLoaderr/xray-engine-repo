#include "stdafx.h"
#include "detailmanager.h"

const int			quant	= 16384;
const int			c_hdr	= 5;
const int			c_base	= c_hdr;
const int			c_size	= 4;

static DWORD dwDecl	[] =
{
    D3DVSD_STREAM	(0),
	D3DVSD_REG		(D3DVSDE_POSITION,	D3DVSDT_FLOAT3),	// pos
	D3DVSD_REG		(D3DVSDE_TEXCOORD0,	D3DVSDT_SHORT4),	// uv
	D3DVSD_END		()
};
#pragma pack(push,1)
struct	vertHW
{
	float		x,y,z;
	short		u,v,t,mid;
};
#pragma pack(pop)

short QC (float v)
{
	int t=iFloor(v*float(quant));// clamp(t,-quant,quant);
	return short(t&0xffff);
}

void CDetailManager::VS_Load()
{	
	// Load vertex shader
	VS_Code			= Device.Shader._CreateVS	("detail",dwDecl);

	// Analyze batch-size
	VS_BatchSize	= (DWORD(HW.Caps.vertex.dwRegisters)-c_hdr)/c_size;

	// Pre-process objects
	DWORD			dwVerts		= 0;
	DWORD			dwIndices	= 0;
	for (u32 o=0; o<objects.size(); o++)
	{
		CDetail& D	=	objects[o];
		dwVerts		+=	D.number_vertices*VS_BatchSize;
		dwIndices	+=	D.number_indices*VS_BatchSize;
	}
	DWORD			vSize		= sizeof(vertHW);
	Msg("* [DETAILS] %d v(%d), %d p",dwVerts,vSize,dwIndices/3);

	// Determine POOL & USAGE
	DWORD dwUsage	=	D3DUSAGE_WRITEONLY;
	D3DPOOL dwPool	=	D3DPOOL_DEFAULT;
	if (HW.Caps.vertex.bSoftware)	{
		dwUsage	|=	D3DUSAGE_SOFTWAREPROCESSING;
		dwPool	=	D3DPOOL_SYSTEMMEM;
	}

	// Create VB/IB
	Device.Shader.Evict		();
	R_CHK			(HW.pDevice->CreateVertexBuffer(dwVerts*vSize,dwUsage,0,dwPool,&VS_VB));
	R_CHK			(HW.pDevice->CreateIndexBuffer(dwIndices*2,dwUsage,D3DFMT_INDEX16,dwPool,&VS_IB));
	Msg("* [DETAILS] Batch(%d), VB(%dK), IB(%dK)",VS_BatchSize,(dwVerts*vSize)/1024, (dwIndices*2)/1024);
	
	// Fill VB
	{
		vertHW*			pV;
		R_CHK			(VS_VB->Lock(0,0,(BYTE**)&pV,0));
		for (o=0; o<objects.size(); o++)
		{
			CDetail& D		=	objects[o];
			for (u32 batch=0; batch<VS_BatchSize; batch++)
			{
				DWORD mid	=	batch*c_size+c_base;
				DWORD M		=	D3DCOLOR_RGBA	(mid,mid,mid,mid);
				for (u32 v=0; v<D.number_vertices; v++)
				{
					Fvector&	vP = D.vertices[v].P;
					pV->x	=	vP.x;
					pV->y	=	vP.y;
					pV->z	=	vP.z;
					pV->u	=	QC(D.vertices[v].u);
					pV->v	=	QC(D.vertices[v].v);
					pV->t	=	0;
					pV->mid	=	short(mid);
					pV++;
				}
			}
		}
		R_CHK			(VS_VB->Unlock());
	}

	// Fill IB
	{
		u16*			pI;
		R_CHK			(VS_IB->Lock(0,0,(BYTE**)(&pI),0));
		for (o=0; o<objects.size(); o++)
		{
			CDetail& D		=	objects[o];
			u16		offset	=	0;
			for (u32 batch=0; batch<VS_BatchSize; batch++)
			{
				for (u32 i=0; i<D.number_indices; i++)
					*pI++	=	D.indices[i] + offset;
				offset		+=	u16(D.number_vertices);
			}
		}
		R_CHK			(VS_IB->Unlock());
	}
}

void CDetailManager::VS_Unload()
{
	// Destroy VS/VB/IB
	if (VS_Code)			Device.Shader._DeleteVS	(VS_Code);
	_RELEASE				(VS_IB);
	_RELEASE				(VS_VB);
}

void CDetailManager::VS_Render()
{
	// Phase
	float	fPhaseRange	=	PI/16;
	float	fPhaseX		=	sinf(Device.fTimeGlobal*0.1f)	*fPhaseRange;
	float	fPhaseZ		=	sinf(Device.fTimeGlobal*0.11f)	*fPhaseRange;

	// Render-prepare
	CVS_Constants& VSC	=	Device.Shader.VSC;
	float scale			=	1.f/float(quant);
	VSC.set					(0,scale,scale,scale,1.f);
	VSC.set					(1,Device.mFullTransform);
	VSC.flush				(0,c_hdr);
	
	// Matrices and offsets
	DWORD		vOffset	=	0;
	DWORD		iOffset	=	0;
	
	// Iterate
	Device.Primitive.setVerticesUC	(VS_Code->dwHandle,sizeof(vertHW),VS_VB);
	for (DWORD O=0; O<objects.size(); O++)
	{
		CList<SlotItem*>&	vis = visible	[O];
		CDetail&	Object		= objects	[O];
		
		if (!vis.empty())
		{
			// Setup matrices + colors (and flush it as nesessary)
			Device.Shader.set_Shader		(Object.shader);

			DWORD dwBatch	= 0;
			for (DWORD item = 0; item<vis.size(); item++)
			{
				SlotItem&	Instance	= *(vis[item]);
				float	scale			= Instance.scale_calculated;
				DWORD	cBase			= dwBatch*c_size+c_base;
				float	C				= Instance.C;
				
				// Build matrix
				Fmatrix& M				= Instance.mRotY;
				VSC.set					(cBase+0,	M._11*scale,	M._21*scale,	M._31*scale,	M._41	);
				VSC.set					(cBase+1,	M._12*scale,	M._22*scale,	M._32*scale,	M._42	);
				VSC.set					(cBase+2,	M._13*scale,	M._23*scale,	M._33*scale,	M._43	);
				VSC.set					(cBase+3,	C,				C,				C,				1.f		);
				
				dwBatch	++;
				if (dwBatch == VS_BatchSize)	
				{
					// flush
					VSC.flush						(c_base,dwBatch*c_size);
					DWORD dwCNT_verts				= dwBatch * Object.number_vertices;
					DWORD dwCNT_prims				= (dwBatch * Object.number_indices)/3;
					Device.Primitive.setIndices		(vOffset, VS_IB);
					Device.Primitive.Render			(D3DPT_TRIANGLELIST,0,dwCNT_verts,iOffset,dwCNT_prims);
					UPDATEC							(dwCNT_verts,dwCNT_prims,2);
					
					// restart
					dwBatch							= 0;
				}
			}
			
			// flush if nessecary
			if (dwBatch)
			{
				VSC.flush						(c_base,dwBatch*c_size);
				DWORD dwCNT_verts				= dwBatch * Object.number_vertices;
				DWORD dwCNT_prims				= (dwBatch * Object.number_indices)/3;
				Device.Primitive.setIndices		(vOffset, VS_IB);
				Device.Primitive.Render			(D3DPT_TRIANGLELIST,0,dwCNT_verts,iOffset,dwCNT_prims);
				UPDATEC							(dwCNT_verts,dwCNT_prims,2);
			}
			
			// Clean up
			vis.clear	();
		}
		vOffset		+=	VS_BatchSize * Object.number_vertices;
		iOffset		+=	VS_BatchSize * Object.number_indices;
	}
}
