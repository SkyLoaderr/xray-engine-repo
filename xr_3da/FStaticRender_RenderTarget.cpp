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

	param_blur	= 0;
	param_gray	= 0;
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
		HW.Caps.fTarget
		);
	if (FAILED(_hr))													return FALSE;
	
	// Try to create texture/surface
	_hr = HW.pDevice->CreateTexture(Device.dwWidth,Device.dwHeight,1,D3DUSAGE_RENDERTARGET,HW.Caps.fTarget,D3DPOOL_DEFAULT,&pSurface);
	if (FAILED(_hr) || (0==pSurface))									return FALSE;
	
	// OK
	R_CHK	(pSurface->GetSurfaceLevel			(0,&pRT));
	R_CHK	(HW.pDevice->GetRenderTarget		(&pBaseRT));
	R_CHK	(HW.pDevice->GetDepthStencilSurface	(&pBaseZB));
	
	// Texture and shader
	pStream		= Device.Streams.Create			(FVF::F_TL,6);
	pTexture	= Device.Shader._CreateTexture	(RTname);
	pShaderSet	= Device.Shader.Create			("effects\\screen_set",		RTname);
	pShaderGray	= Device.Shader.Create			("effects\\screen_gray",	RTname);
	pTexture->surface_set	(pSurface);
	return	TRUE;
}

void CRenderTarget::OnDeviceCreate	()
{
	bAvailable	= FALSE;
	bAvailable	= Create	();
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
	if (!Available() || !NeedPostProcess())	return;

	Device.Statistic.TEST.Begin		();
	R_CHK		(HW.pDevice->SetRenderTarget	(pRT,		pBaseZB));
	if (psDeviceFlags&rsClearBB) CHK_DX(HW.pDevice->Clear(0,0,D3DCLEAR_TARGET,D3DCOLOR_XRGB(0,255,0),1,0));
	Device.Statistic.TEST.End		();
}

void CRenderTarget::End		()
{
	if (!Available() || !NeedPostProcess())	return;
	
	Device.Statistic.TEST.Begin		();
	R_CHK			(HW.pDevice->SetRenderTarget	(pBaseRT,	pBaseZB));
	Device.Statistic.TEST.End		();
	
	// Draw full-screen quad textured with our scene image
	DWORD	Offset;
	DWORD	C		= D3DCOLOR_RGBA	(120,120,120,0);
	// DWORD	C		= 0xffffffff;
	float	tw		= float(Device.dwWidth);
	float	th		= float(Device.dwHeight);
	DWORD	_w		= Device.dwWidth;
	DWORD	_h		= Device.dwHeight;
	
	// UV
	Fvector2		shift,p0,p1;
	shift.set		(.5f/tw, .5f/th);
	shift.mul		(blur);
	p0.set			(.5f/tw, .5f/th);
	p1.set			((tw+.5f)/tw, (th+.5f)/th );
	p0.add			(shift);
	p1.add			(shift);
	
	// Fill vertex buffer
	FVF::TL* pv = (FVF::TL*) pStream->Lock(4,Offset);
	pv->set(0,			float(_h),	.0001f,.9999f, C, p0.x, p1.y);	pv++;
	pv->set(0,			0,			.0001f,.9999f, C, p0.x, p0.y);	pv++;
	pv->set(float(_w),	float(_h),	.0001f,.9999f, C, p1.x, p1.y);	pv++;
	pv->set(float(_w),	0,			.0001f,.9999f, C, p1.x, p0.y);	pv++;
	pStream->Unlock			(4);

	// Actual rendering
	Device.Shader.set_Shader(pShaderGray);
	Device.Primitive.Draw	(pStream,4,2,Offset,Device.Streams_QuadIB);
}
