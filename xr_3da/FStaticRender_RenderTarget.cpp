#include "stdafx.h"
#include "fstaticrender.h"
#include "fstaticrender_rendertarget.h"

LPCSTR	RTname	= "$user$rendertarget";

CRenderTarget::CRenderTarget()
{
	bAvailable	= FALSE;
	pSurface	= 0;
	pRT			= 0;
	pBaseRT		= 0;
	pBaseZB		= 0;

	pTexture	= 0;
	pShaderSet	= 0;
	pShaderGray	= 0;
	pStream		= 0;
}

BOOL CRenderTarget::Create	()
{
	R_ASSERT	(HW.pDevice);
	HRESULT		_hr;
	
	// Get caps
	D3DCAPS8	caps;
	R_CHK		(HW.pDevice->GetDeviceCaps(&caps));
	
	// Check nonpow2 conditional
	if (!HW.Caps.pixel.bNonPow2)										return FALSE;
	
	// Check width-and-height of render target surface
	if (Device.dwWidth>caps.MaxTextureWidth)							return FALSE;
	if (Device.dwHeight>caps.MaxTextureHeight)							return FALSE;
	
	// Validate render-target usage
	_hr = HW.pD3D->CheckDeviceFormat(
		D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		HW.Caps.fTarget,
		D3DUSAGE_RENDERTARGET,
		D3DRTYPE_TEXTURE,
		D3DFMT_R5G6B5
		);
	if (FAILED(_hr))													return FALSE;
	
	// Try to create texture/surface
	_hr = HW.pDevice->CreateTexture(Device.dwWidth,Device.dwHeight,1,D3DUSAGE_RENDERTARGET,D3DFMT_R5G6B5,D3DPOOL_DEFAULT,&pSurface);
	if (FAILED(_hr) || (0==pSurface))									return FALSE;
	
	// OK
	R_CHK	(pSurface->GetSurfaceLevel			(0,&pRT));
	R_CHK	(HW.pDevice->GetRenderTarget		(&pBaseRT));
	R_CHK	(HW.pDevice->GetDepthStencilSurface	(&pBaseZB));
	
	// Texture and shader
	pTexture	= Device.Shader._CreateTexture	(RTname);
	pShaderSet	= Device.Shader.Create			("screen_set",	RTname);
	pShaderGray	= Device.Shader.Create			("screen_gray",	RTname);
	pTexture->surface_set	(pSurface);
	return	TRUE;
}

void CRenderTarget::OnDeviceCreate	()
{
	bAvailable	= FALSE;
	if (psDeviceFlags&rsAntialiasBlurred)	bAvailable	= Create	();
}

void CRenderTarget::OnDeviceDestroy	()
{
	Device.Shader.Delete		(pShaderGray);
	Device.Shader.Delete		(pShaderSet);
	Device.Shader._DeleteTexture(pTexture);
	
	_RELEASE	(pBaseZB);
	_RELEASE	(pBaseRT);
	_RELEASE	(pRT);
	_RELEASE	(pSurface);
}

void CRenderTarget::Begin	()
{
	R_CHK		(HW.pDevice->SetRenderTarget	(pRT,		pBaseZB));
}

void CRenderTarget::End		(float blur)
{
	R_CHK		(HW.pDevice->SetRenderTarget	(pBaseRT,	pBaseZB));
	
	// Draw full-screen quad textured with our scene image
	DWORD	Offset;
	DWORD	C		= 0xffffffff;
	float	tw		= float(Device.dwWidth);
	float	th		= float(Device.dwHeight);
	DWORD	_w		= Device.dwWidth-1;
	DWORD	_h		= Device.dwHeight-1;
	
	// UV
	Fvector2		shift,p0,p1;
	shift.set		(.5f/tw, .5f/th);
	shift.mul		(blur);
	p0.set			(.5f/tw, .5f/th);
	p1.set			((tw-.5f)/tw, (th-.5f)/th );
	p0.add			(shift);
	p1.add			(shift);
	
	// Fill vertex buffer
	FVF::TL* pv = (FVF::TL*) pStream->Lock(4,Offset);
	pv->set(0,			float(_h),	1, 1, C, p0.x, p1.y);	pv++;
	pv->set(0,			0,			1, 1, C, p0.x, p0.y);	pv++;
	pv->set(float(_w),	float(_h),	1, 1, C, p1.x, p1.y);	pv++;
	pv->set(float(_w),	0,			1, 1, C, p1.x, p0.y);	pv++;
	pStream->Unlock			(4);

	// Actual rendering
	Device.Shader.set_Shader(pShader);
	Device.Primitive.Draw	(pStream,4,2,Offset,Device.Streams_QuadIB);
}
