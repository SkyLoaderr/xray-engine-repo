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
	R_CHK			(HW.pDevice->CreateVertexShader(dwDecl,code->GetBufferPointer(),&VS_Code,0));
	_RELEASE		(code);
	_RELEASE		(errors);

	// Analyze batch-size
	DWORD batch_size= (DWORD(HW.Caps.vertex.dwRegisters)-1)/5;
	Log("* [DETAILS] Batch size: ",batch_size);

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
	DWORD dwPool	=	D3DPOOL_DEFAULT;
	if (HW.Caps.vertex.bSoftware)	{
		dwUsage	|=	D3DUSAGE_SOFTWAREPROCESSING;
		dwPool	=	D3DPOOL_SYSTEMMEM;
	}

	// Create VB/IB
	CHK_DX			(HW.pDevice->ResourceManagerDiscardBytes(0));
	R_CHK			(HW.pDevice->CreateVertexBuffer(dwVerts*vSize,dwUsage,0,dwPool,&VS_VB));
	R_CHK			(HW.pDevice->CreateIndexBuffer(dwIndices*2,dwUsage,D3DFMT_INDEX16,&VS_IB));

	// Fill VB
	vertHW*			pV;
	R_CHK			(VS_VB->Lock(0,0,&pV,0));
}

void CDetailManager::VS_Unload()
{
	// Destroy VS/VB/IB
	R_CHK			(HW.pDevice->DeleteVertexShader(&VS_Code));	VS_Code = 0;
	_RELEASE		(VS_IB);
	_RELEASE		(VS_VB);
}

void CDetailManager::VS_Render()
{
}
