#include "stdafx.h"
#pragma hdrstop

void CRT::Create	(LPCSTR Name, u32 w, u32 h)
{
	R_ASSERT	(HW.pDevice && Name && Name[0] && w && h);
	HRESULT		_hr;
	
	// Get caps
	D3DCAPS8	caps;
	R_CHK		(HW.pDevice->GetDeviceCaps(&caps));
	
	// Pow2
	if (!btwIsPow2(w) || !btwIsPow2(h))
	{
		if (!HW.Caps.pixel.bNonPow2)	return;
	}

	// Check width-and-height of render target surface
	if (w>caps.MaxTextureWidth)			return;
	if (h>caps.MaxTextureHeight)		return;

	// Validate render-target usage
	_hr = HW.pD3D->CheckDeviceFormat(
		D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		HW.Caps.fTarget,
		D3DUSAGE_RENDERTARGET,
		D3DRTYPE_TEXTURE,
		HW.Caps.fTarget
		);
	if (FAILED(_hr))					return;

	// Try to create texture/surface
	Device.Shader.Evict				();
	_hr = HW.pDevice->CreateTexture	(w, h, 1, D3DUSAGE_RENDERTARGET, HW.Caps.fTarget, D3DPOOL_DEFAULT, &pSurface);
	if (FAILED(_hr) || (0==pSurface))	return;
	
	// OK
	R_CHK		(pSurface->GetSurfaceLevel	(0,&pRT));
	pTexture	= Device.Shader._CreateTexture	(Name);
	pTexture->surface_set	(pSurface);
}

void CRT::Destroy()
{
	pTexture->surface_set		(0);
	Device.Shader._DeleteTexture(pTexture);
	_RELEASE	(pRT		);
	_RELEASE	(pSurface	);
}
