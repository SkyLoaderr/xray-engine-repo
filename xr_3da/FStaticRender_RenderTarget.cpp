#include "stdafx.h"
#include "fstaticrender.h"
#include "fstaticrender_rendertarget.h"

LPCSTR	RTname	= "$user$rendertarget";

CRenderTarget::CRenderTarget()
{
	bAvailable	= FALSE;
	RT			= 0;

	pShaderSet	= 0;
	pShaderGray	= 0;
	pStream		= 0;

	param_blur	= 0;
	param_gray	= 0;
}

BOOL CRenderTarget::Create	()
{
	RT			= Device.Shader._CreateRT		(RTname,Device.dwWidth,Device.dwHeight);
	
	// Shaders and stream
	pStream		= Device.Streams.Create			(FVF::F_TL,8);
	pShaderSet	= Device.Shader.Create			("effects\\screen_set",		RTname);
	pShaderGray	= Device.Shader.Create			("effects\\screen_gray",	RTname);
	pShaderBlend= Device.Shader.Create			("effects\\screen_blend",	RTname);
	return	RT->Valid();
}

void CRenderTarget::OnDeviceCreate	()
{
	bAvailable	= Create	();
}

void CRenderTarget::OnDeviceDestroy	()
{
	Device.Shader.Delete		(pShaderBlend);
	Device.Shader.Delete		(pShaderGray);
	Device.Shader.Delete		(pShaderSet);
	Device.Shader._DeleteRT		(RT);
}

void CRenderTarget::Begin	()
{
	if (!Available() || !NeedPostProcess())	return;

	R_CHK		(HW.pDevice->SetRenderTarget	(RT->pRT, HW.pBaseZB));
	if (psDeviceFlags&rsClearBB) CHK_DX(HW.pDevice->Clear(0,0,D3DCLEAR_TARGET,D3DCOLOR_XRGB(0,255,0),1,0));
}

void CRenderTarget::End		()
{
	if (!Available() || !NeedPostProcess())	return;
	
	R_CHK			(HW.pDevice->SetRenderTarget	(pBaseRT,	pBaseZB));
	
	// Draw full-screen quad textured with our scene image
	DWORD	Offset;
	DWORD	Cgray	= D3DCOLOR_RGBA	(90,90,90,0);
	int		A		= 127; //iFloor		((1-param_gray)*255.f); clamp(A,0,255);
	DWORD	Calpha	= D3DCOLOR_RGBA	(255,255,255,A);
	float	tw		= float(Device.dwWidth);
	float	th		= float(Device.dwHeight);
	DWORD	_w		= Device.dwWidth;
	DWORD	_h		= Device.dwHeight;
	
	// UV
	Fvector2		shift,p0,p1;
	shift.set		(.5f/tw, .5f/th);
	shift.mul		(param_blur);
	p0.set			(.5f/tw, .5f/th);
	p1.set			((tw+.5f)/tw, (th+.5f)/th );
	p0.add			(shift);
	p1.add			(shift);
	
	// Fill vertex buffer
	FVF::TL* pv = (FVF::TL*) pStream->Lock(8,Offset);
	pv->set(0,			float(_h),	.0001f,.9999f, Cgray, p0.x, p1.y);	pv++;
	pv->set(0,			0,			.0001f,.9999f, Cgray, p0.x, p0.y);	pv++;
	pv->set(float(_w),	float(_h),	.0001f,.9999f, Cgray, p1.x, p1.y);	pv++;
	pv->set(float(_w),	0,			.0001f,.9999f, Cgray, p1.x, p0.y);	pv++;
	pv->set(0,			float(_h),	.0001f,.9999f, Calpha,p0.x, p1.y);	pv++;
	pv->set(0,			0,			.0001f,.9999f, Calpha,p0.x, p0.y);	pv++;
	pv->set(float(_w),	float(_h),	.0001f,.9999f, Calpha,p1.x, p1.y);	pv++;
	pv->set(float(_w),	0,			.0001f,.9999f, Calpha,p1.x, p0.y);	pv++;
	pStream->Unlock			(8);

	// Actual rendering
	if (param_gray>0.001f) {
		// Draw GRAY
		Device.Shader.set_Shader(pShaderGray);
		Device.Primitive.Draw	(pStream,4,2,Offset+0,Device.Streams_QuadIB);
		if (param_gray<0.999f) {
			// Blend COLOR
			Device.Shader.set_Shader		(pShaderBlend);
			Device.Primitive.setVerticesUC	(pStream->getFVF(), pStream->getStride(), pStream->getBuffer());
			Device.Primitive.setIndicesUC	(Offset+4, Device.Streams_QuadIB);
			Device.Primitive.Render			(D3DPT_TRIANGLELIST,0,4,0,2);
		}
	} else {
		// Draw COLOR
		Device.Shader.set_Shader(pShaderSet);
		Device.Primitive.Draw	(pStream,4,2,Offset+4,Device.Streams_QuadIB);
	}
}
