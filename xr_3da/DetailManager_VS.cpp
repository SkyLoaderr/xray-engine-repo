#include "stdafx.h"
#include "detailmanager.h"

static DWORD dwDecl[] =
{
    D3DVSD_STREAM	(0),
	D3DVSD_REG		(0,	D3DVSDT_FLOAT3),		// pos
	D3DVSD_REG		(1,	D3DVSDT_D3DCOLOR),		// matrix id
	D3DVSD_REG		(2,	D3DVSDT_FLOAT2),		// uv
	D3DVSD_END		()
};
#pragma pack(push,1)
struct	vertHW
{
	Fvector		P;
	DWORD		M;
	Fvector2	UV;
};
#pragma pack(pop)

void CDetailManager::VS_Load()
{
	// Load vertex shader
	LPD3DXBUFFER	code;
	LPD3DXBUFFER	errors;
	R_CHK			(D3DXAssembleShaderFromFile("data\\shaders\\detail.vs",0,NULL,&code,&errors));
	R_CHK			(HW.pDevice->CreateVertexShader(dwDecl,LPDWORD(code->GetBufferPointer()),&VS_Code,0));
	_RELEASE		(code);
	_RELEASE		(errors);

	// Analyze batch-size
	DWORD batch_size= (DWORD(HW.Caps.vertex.dwRegisters)-1)/5;

	// Pre-process objects
	DWORD			dwVerts		= 0;
	DWORD			dwIndices	= 0;
	for (u32 o=0; o<objects.size(); o++)
	{
		CDetail& D	=	objects[o];
		dwVerts		+=	D.number_vertices*batch_size;
		dwIndices	+=	D.number_indices*batch_size;
	}
	DWORD			vSize		= (3+1+2)*4;

	// Determine POOL & USAGE
	DWORD dwUsage	=	D3DUSAGE_WRITEONLY;
	D3DPOOL dwPool	=	D3DPOOL_DEFAULT;
	if (HW.Caps.vertex.bSoftware)	{
		dwUsage	|=	D3DUSAGE_SOFTWAREPROCESSING;
		dwPool	=	D3DPOOL_SYSTEMMEM;
	}

	// Create VB/IB
	CHK_DX			(HW.pDevice->ResourceManagerDiscardBytes(0));
	R_CHK			(HW.pDevice->CreateVertexBuffer(dwVerts*vSize,dwUsage,0,dwPool,&VS_VB));
	R_CHK			(HW.pDevice->CreateIndexBuffer(dwIndices*2,dwUsage,D3DFMT_INDEX16,dwPool,&VS_IB));
	Msg("* [DETAILS] Batch(%d), VB(%dK), IB(%dK)",batch_size,(dwVerts*vSize)/1024, (dwIndices*2)/1024);
	
	// Fill VB
	{
		vertHW*			pV;
		R_CHK			(VS_VB->Lock(0,0,(BYTE**)&pV,0));
		for (o=0; o<objects.size(); o++)
		{
			CDetail& D		=	objects[o];
			for (u32 batch=0; batch<batch_size; batch++)
			{
				DWORD mid	=	batch*5+1;
				DWORD M		=	D3DCOLOR_RGBA	(mid,mid,mid,mid);
				for (u32 v=0; v<D.number_vertices; v++)
				{
					pV->P.set	(D.vertices[v].P);
					pV->M	=	M;
					pV->UV.set	(D.vertices[v].u,D.vertices[v].v);
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
			for (u32 batch=0; batch<batch_size; batch++)
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
	R_CHK			(HW.pDevice->DeleteVertexShader(VS_Code));	VS_Code = 0;
	_RELEASE		(VS_IB);
	_RELEASE		(VS_VB);
}

void CDetailManager::VS_Render()
{
}
