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

void CDetailManager::VS_Load()
{
	// Load vertex shader
	LPD3DXBUFFER	code;
	LPD3DXBUFFER	errors;
	R_CHK			(D3DXAssembleShaderFromFile("data\\shaders\\detail.vs",0,NULL,&code,&errors));
	R_CHK			(HW.pDevice->CreateVertexShader(dwDecl,code->GetBufferPointer(),&VS_Code,0));
	_RELEASE		(code);
	_RELEASE		(errors);

	// Process objects
}

void CDetailManager::VS_Unload()
{
	// Destroy vertex shader
	R_CHK			(HW.pDevice->DeleteVertexShader(&VS_Code));	VS_Code = 0;
}
void CDetailManager::VS_Render()
{
}
