#include "stdafx.h"
#pragma hdrstop

CRT::CRT			()
{
	pSurface		= NULL;
	pRT				= NULL;
	dwWidth			= 0;
	dwHeight		= 0;
	fmt				= D3DFMT_UNKNOWN;
}
CRT::~CRT			()
{
	Destroy			();
}

void CRT::Create	(LPCSTR Name, u32 w, u32 h,	D3DFORMAT f)
{
	R_ASSERT	(HW.pDevice && Name && Name[0] && w && h);
	HRESULT		_hr;

	dwWidth		= w;
	dwHeight	= h;
	fmt			= f;

	// Get caps
	D3DCAPS9	caps;
	R_CHK		(HW.pDevice->GetDeviceCaps(&caps));

	// Pow2
	if (!btwIsPow2(w) || !btwIsPow2(h))
	{
		if (!HW.Caps.pixel.bNonPow2)	return;
	}

	// Check width-and-height of render target surface
	if (w>caps.MaxTextureWidth)			return;
	if (h>caps.MaxTextureHeight)		return;

	// Select usage
	u32 usage	= 0;
	if (D3DFMT_D24X8==fmt)				usage = D3DUSAGE_DEPTHSTENCIL;
	else								usage = D3DUSAGE_RENDERTARGET;

	// Validate render-target usage
	_hr = HW.pD3D->CheckDeviceFormat(
		D3DADAPTER_DEFAULT,
		HW.DevT,
		HW.Caps.fTarget,
		usage,
		D3DRTYPE_TEXTURE,
		f
		);
	if (FAILED(_hr))					return;

	// Try to create texture/surface
	Device.Shader.Evict					();
	_hr = HW.pDevice->CreateTexture		(w, h, 1, usage, f, D3DPOOL_DEFAULT, &pSurface,NULL);
	if (FAILED(_hr) || (0==pSurface))	return;

	// OK
	R_CHK		(pSurface->GetSurfaceLevel	(0,&pRT));
	pTexture	= Device.Shader._CreateTexture	(Name);
	pTexture->surface_set	(pSurface);
}

void CRT::Destroy	()
{
	pTexture->surface_set				(0);
	pTexture	= NULL;
	_RELEASE	(pRT		);
	_RELEASE	(pSurface	);
}

//////////////////////////////////////////////////////////////////////////
CRTC::CRTC			()
{
	pSurface									= NULL;
	pRT[0]=pRT[1]=pRT[2]=pRT[3]=pRT[4]=pRT[5]	= NULL;
	dwSize										= 0;
	fmt											= D3DFMT_UNKNOWN;
}
CRTC::~CRTC			()
{
	Destroy			();
}

void CRTC::Create	(LPCSTR Name, u32 size,	D3DFORMAT f)
{
	R_ASSERT	(HW.pDevice && Name && Name[0] && size && btwIsPow2(size));
	HRESULT		_hr;

	dwSize		= size;
	fmt			= f;

	// Get caps
	D3DCAPS9	caps;
	R_CHK		(HW.pDevice->GetDeviceCaps(&caps));

	// Check width-and-height of render target surface
	if (size>caps.MaxTextureWidth)		return;
	if (size>caps.MaxTextureHeight)		return;

	// Validate render-target usage
	_hr = HW.pD3D->CheckDeviceFormat(
		D3DADAPTER_DEFAULT,
		HW.DevT,
		HW.Caps.fTarget,
		D3DUSAGE_RENDERTARGET,
		D3DRTYPE_CUBETEXTURE,
		f
		);
	if (FAILED(_hr))					return;

	// Try to create texture/surface
	Device.Shader.Evict					();
	_hr = HW.pDevice->CreateCubeTexture	(size, 1, D3DUSAGE_RENDERTARGET, f, D3DPOOL_DEFAULT, &pSurface,NULL);
	if (FAILED(_hr) || (0==pSurface))	return;

	// OK
	for (u32 face=0; face<6; face++)
		R_CHK	(pSurface->GetCubeMapSurface	((D3DCUBEMAP_FACES)face, 0, pRT+face));
	pTexture	= Device.Shader._CreateTexture	(Name);
	pTexture->surface_set						(pSurface);
}

void CRTC::Destroy()
{
	pTexture->surface_set	(0);
	pTexture				= NULL;
	for (u32 face=0; face<6; face++)
		_RELEASE	(pRT[face]	);
	_RELEASE	(pSurface	);
}
