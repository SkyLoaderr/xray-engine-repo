#include "stdafx.h"

BOOL	R_shader::compile		(LPDIRECT3DDEVICE9 pDevice, LPCSTR name)
{
	LPD3DXBUFFER				pShaderBuf	= NULL;
	LPD3DXBUFFER				pErrorBuf	= NULL;
	LPD3DXSHADER_CONSTANTTABLE	pConstants	= NULL;
	HRESULT						hr;

	// pixel
	hr = D3DXCompileShaderFromFile	(name, NULL, NULL, "p_main", "ps_2_0", D3DXSHADER_DEBUG, &pShaderBuf, &pErrorBuf, &pConstants);
	if (SUCCEEDED(hr))
	{
		if (pShaderBuf) 
		{
			hr = pDevice->CreatePixelShader	((DWORD*)pShaderBuf->GetBufferPointer(), &ps);
			if (SUCCEEDED(hr) && pConstants)	constants.parse(pConstants,0x1);
		}
		else
			hr = E_FAIL;
	}
	SAFE_RELEASE	(pShaderBuf);
	SAFE_RELEASE	(pErrorBuf);
	pConstants		= NULL;
	if (FAILED(hr))	return FALSE;

	// vertex
	hr = D3DXCompileShaderFromFile	(name, NULL, NULL, "v_main", "vs_2_0", D3DXSHADER_DEBUG, &pShaderBuf, &pErrorBuf, &pConstants);
	if (SUCCEEDED(hr))
	{
		if (pShaderBuf) 
		{
			hr = pDevice->CreateVertexShader	((DWORD*)pShaderBuf->GetBufferPointer(), &vs);
			if (SUCCEEDED(hr) && pConstants)	constants.parse(pConstants,0x2);
		}
		else
			hr = E_FAIL;
	}
	SAFE_RELEASE	(pShaderBuf);
	SAFE_RELEASE	(pErrorBuf);
	pConstants		= NULL;
	if (FAILED(hr))	return FALSE;
}
