#define STRICT
#include <Windows.h>
#include <commctrl.h>
#include <stdio.h>
#include <math.h>
#include <D3DX9.h>
#include "DXUtil.h"
#include "Util.h"

//-----------------------------------------------------------------------------
// Name: LoadVertexShader()
// Desc: Creates and loads vertex shader from file
//-----------------------------------------------------------------------------
HRESULT LoadVertexShader(LPDIRECT3DDEVICE9 pd3dDevice, TCHAR* fileName, LPDIRECT3DVERTEXSHADER9* pShader)
{
	LPD3DXBUFFER	pShaderBuf	= NULL;
    LPD3DXBUFFER	pErrorBuf	= NULL;
	HRESULT			hr;

    hr = D3DXAssembleShaderFromFile(fileName, NULL, NULL, D3DXSHADER_DEBUG, &pShaderBuf, &pErrorBuf);
	if (SUCCEEDED(hr))
	{
	   if (pShaderBuf) 
		   hr = pd3dDevice->CreateVertexShader((DWORD*)pShaderBuf->GetBufferPointer(), pShader);
	   else
		   hr = E_FAIL;
	}
	SAFE_RELEASE(pShaderBuf);
	SAFE_RELEASE(pErrorBuf);

	return hr;
}

HRESULT hlsl_VertexShader(LPDIRECT3DDEVICE9 pd3dDevice, TCHAR* fileName, LPDIRECT3DVERTEXSHADER9* pShader)
{
	LPD3DXBUFFER	pShaderBuf	= NULL;
	LPD3DXBUFFER	pErrorBuf	= NULL;
	HRESULT			hr;

	hr = D3DXCompileShaderFromFile(fileName, NULL, NULL, "v_main", "vs_2_0", D3DXSHADER_DEBUG, &pShaderBuf, &pErrorBuf, NULL);
	if (SUCCEEDED(hr))
	{
		if (pShaderBuf) 
			hr = pd3dDevice->CreateVertexShader((DWORD*)pShaderBuf->GetBufferPointer(), pShader);
		else
			hr = E_FAIL;
	}
	SAFE_RELEASE	(pShaderBuf);
	SAFE_RELEASE	(pErrorBuf);

	return hr;
}

//-----------------------------------------------------------------------------
// Name: LoadPixelShader()
// Desc: Creates and loads pixel shader from file
//-----------------------------------------------------------------------------
HRESULT LoadPixelShader(LPDIRECT3DDEVICE9 pd3dDevice, TCHAR* fileName, LPDIRECT3DPIXELSHADER9* pShader)
{
	LPD3DXBUFFER pShaderBuf = NULL;
    LPD3DXBUFFER pErrorBuf = NULL;
	HRESULT hr;

    hr = D3DXAssembleShaderFromFile(fileName, NULL, NULL, D3DXSHADER_DEBUG, &pShaderBuf, &pErrorBuf);
	if (SUCCEEDED(hr))
	{
	   if (pShaderBuf) 
		   hr = pd3dDevice->CreatePixelShader((DWORD*)pShaderBuf->GetBufferPointer(), pShader);
	   else
		   hr = E_FAIL;
	}
	SAFE_RELEASE(pShaderBuf);
	SAFE_RELEASE(pErrorBuf);

	return hr;
}

HRESULT hlsl_PixelShader(LPDIRECT3DDEVICE9 pd3dDevice, TCHAR* fileName, LPDIRECT3DPIXELSHADER9* pShader)
{
	LPD3DXBUFFER	pShaderBuf	= NULL;
	LPD3DXBUFFER	pErrorBuf	= NULL;
	HRESULT			hr;

	hr = D3DXCompileShaderFromFile(fileName, NULL, NULL, "p_main", "ps_2_0", D3DXSHADER_DEBUG, &pShaderBuf, &pErrorBuf, NULL);
	if (SUCCEEDED(hr))
	{
		if (pShaderBuf) 
			hr = pd3dDevice->CreatePixelShader((DWORD*)pShaderBuf->GetBufferPointer(), pShader);
		else
			hr = E_FAIL;
	}
	SAFE_RELEASE	(pShaderBuf);
	SAFE_RELEASE	(pErrorBuf);

	return hr;
}
