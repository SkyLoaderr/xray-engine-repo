#include "stdafx.h"
#pragma hdrstop

#include "detailmanager.h"

const int			quant	= 16384;
const int			c_hdr	= 14;
const int			c_base	= c_hdr;
const int			c_size	= 4;

static DWORD dwDecl	[] =
{
    D3DVSD_STREAM	(0),
	D3DVSD_REG		(0,	D3DVSDT_FLOAT3),	// pos
	D3DVSD_REG		(1,	D3DVSDT_SHORT4),	// uv
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

void CDetailManager::hw_Load()
{	
	// Load vertex shader
	hw_VS			= Device.Shader._CreateVS	("detail",dwDecl,sizeof(vertHW));

	// Analyze batch-size
	hw_BatchSize	= (DWORD(HW.Caps.vertex.dwRegisters)-c_hdr)/c_size;
	clamp			(hw_BatchSize,0ul,46ul);
	Msg("* [DETAILS] VertexConsts(%d), Batch(%d)",DWORD(HW.Caps.vertex.dwRegisters),hw_BatchSize);

	// Pre-process objects
	DWORD			dwVerts		= 0;
	DWORD			dwIndices	= 0;
	for (u32 o=0; o<objects.size(); o++)
	{
		CDetail& D	=	*objects[o];
		dwVerts		+=	D.number_vertices*hw_BatchSize;
		dwIndices	+=	D.number_indices*hw_BatchSize;
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
	R_CHK			(HW.pDevice->CreateVertexBuffer(dwVerts*vSize,dwUsage,0,dwPool,&hw_VB));
	R_CHK			(HW.pDevice->CreateIndexBuffer(dwIndices*2,dwUsage,D3DFMT_INDEX16,dwPool,&hw_IB));
	Msg("* [DETAILS] Batch(%d), VB(%dK), IB(%dK)",hw_BatchSize,(dwVerts*vSize)/1024, (dwIndices*2)/1024);
	
	// Fill VB
	{
		vertHW*			pV;
		R_CHK			(hw_VB->Lock(0,0,(BYTE**)&pV,0));
		for (o=0; o<objects.size(); o++)
		{
			CDetail& D		=	*objects[o];
			for (u32 batch=0; batch<hw_BatchSize; batch++)
			{
				DWORD mid	=	batch*c_size+c_base;
				for (u32 v=0; v<D.number_vertices; v++)
				{
					Fvector&	vP = D.vertices[v].P;
					pV->x	=	vP.x;
					pV->y	=	vP.y;
					pV->z	=	vP.z;
					pV->u	=	QC(D.vertices[v].u);
					pV->v	=	QC(D.vertices[v].v);
					pV->t	=	QC(vP.y/(D.bv_bb.max.y-D.bv_bb.min.y));
					pV->mid	=	short(mid);
					pV++;
				}
			}
		}
		R_CHK			(hw_VB->Unlock());
	}
 
	// Fill IB
	{
		u16*			pI;
		R_CHK			(hw_IB->Lock(0,0,(BYTE**)(&pI),0));
		for (o=0; o<objects.size(); o++)
		{
			CDetail& D		=	*objects[o];
			u16		offset	=	0;
			for (u32 batch=0; batch<hw_BatchSize; batch++)
			{
				for (u32 i=0; i<u32(D.number_indices); i++)
					*pI++	=	u16(u16(D.indices[i]) + u16(offset));
				offset		=	offset+u16(D.number_vertices);
			}
		}
		R_CHK			(hw_IB->Unlock());
	}
}

void CDetailManager::hw_Unload()
{
	// Destroy VS/VB/IB
	if (hw_VS)				Device.Shader._DeleteVS	(hw_VS);
	_RELEASE				(hw_IB);
	_RELEASE				(hw_VB);
}

void CDetailManager::hw_Render()
{
	// Phase
	// float	fPhaseRange	=	PI/16;
	// float	fPhaseX		=	sinf(Device.fTimeGlobal*0.1f)	*fPhaseRange;
	// float	fPhaseZ		=	sinf(Device.fTimeGlobal*0.11f)	*fPhaseRange;

	// Render-prepare
	float	tm_rot		= PI_MUL_2*Device.fTimeGlobal/30;
	Fvector dir2D;
	dir2D.set			(sinf(tm_rot),0,cosf(tm_rot));
	dir2D.normalize		();

	CVS_Constants& VSC	=	Device.Shader.VSC;
	float scale			=	1.f/float(quant);
	VSC.set				(0,	scale,		scale,		.25f,		.9f);						// consts
	VSC.set				(1,	1.f/5.f,	1.f/7.f,	1.f/3.f,	Device.fTimeGlobal*2.f);	// wave
	VSC.set				(2,	dir2D);															// wind-dir
	VSC.set				(3,	Device.mFullTransform);
	VSC.set				(7,	1,			.1f,		0,			2);				//const c[7] = 1 0.1 0 2
	VSC.set				(8,	0,			0.5f,		1.f,		0);				//const c[8] = 0 0.5 1 0
	VSC.set				(9, 0.25f,		-9.f,		0.75f,		0.1591549f);	//const c[9] = 0.25 -9 0.75 0.1591549
	VSC.set				(10,24.9808f,	-24.9808f,	-60.14581f,	60.14581f);		//const c[10] = 24.9808 -24.9808 -60.14581 60.14581
	VSC.set				(11,85.45379f,	-85.45379f, -64.93935f, 64.93935f);		//const c[11] = 85.45379 -85.45379 -64.93935 64.93935
	VSC.set				(12,19.73921f,	-19.73921f,	-1,			1);				//const c[12] = 19.73921 -19.73921 -1 1
	VSC.set				(13,0,			0,			0,			0);				//const c[13] = 0 0 0 0
	VSC.flush			(0,	c_hdr);

	// Matrices and offsets
	DWORD		vOffset	=	0;
	DWORD		iOffset	=	0;
	
	// Iterate
	Device.Primitive.setVertices	(hw_VS->dwHandle,hw_VS->dwStride,hw_VB);
	for (DWORD O=0; O<objects.size(); O++)
	{
		vector<SlotItem*>&	vis = visible	[O];
		CDetail&	Object		= *objects	[O];
		
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
				if (dwBatch == hw_BatchSize)	
				{
					// flush
					VSC.flush						(c_base,dwBatch*c_size);
					DWORD dwCNT_verts				= dwBatch * Object.number_vertices;
					DWORD dwCNT_prims				= (dwBatch * Object.number_indices)/3;
					Device.Primitive.setIndices		(vOffset, hw_IB);
					Device.Primitive.Render			(D3DPT_TRIANGLELIST,0,dwCNT_verts,iOffset,dwCNT_prims);
					
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
				Device.Primitive.setIndices		(vOffset, hw_IB);
				Device.Primitive.Render			(D3DPT_TRIANGLELIST,0,dwCNT_verts,iOffset,dwCNT_prims);
			}
			
			// Clean up
			vis.clear	();
		}
		vOffset		+=	hw_BatchSize * Object.number_vertices;
		iOffset		+=	hw_BatchSize * Object.number_indices;
	}
}
