#include "stdafx.h"
#include "fstaticrender_rendertarget.h"

static LPCSTR		RTname	= "$user$rendertarget";
static LPCSTR		RTtemp	= "$user$temp_small";
static CPS*			hPS		= 0;
static CTexture*	hTex	= 0;

CRenderTarget::CRenderTarget()
{
	bAvailable	= FALSE;
	RT			= 0;

	pShaderSet	= 0;
	pShaderGray	= 0;
	pVS			= 0;

	param_blur	= 0;
	param_gray	= 0;
}

BOOL CRenderTarget::Create	()
{
	// 
	RT			= Device.Shader._CreateRT		(RTname,Device.dwWidth,Device.dwHeight);
	RT_temp		= Device.Shader._CreateRT		(RTtemp,64,64);
	
	// Shaders and stream
	pVS			= Device.Shader._CreateVS		(FVF::F_TL);
	pShaderSet	= Device.Shader.Create			("effects\\screen_set",		RTname);
	pShaderGray	= Device.Shader.Create			("effects\\screen_gray",	RTname);
	pShaderBlend= Device.Shader.Create			("effects\\screen_blend",	RTname);
	pShaderAdd	= Device.Shader.Create			("effects\\screen_add",		RTtemp);
	return	RT->Valid() && RT_temp->Valid();
}

void CRenderTarget::OnDeviceCreate	()
{
	// hPS			= Device.Shader._CreatePS		("hdr");
	// hTex		= Device.Shader._CreateTexture	("transfer3");

	bAvailable	= Create	();
}

void CRenderTarget::OnDeviceDestroy	()
{
	Device.Shader.Delete		(pShaderAdd);
	Device.Shader.Delete		(pShaderBlend);
	Device.Shader.Delete		(pShaderGray);
	Device.Shader.Delete		(pShaderSet);
	Device.Shader._DeleteRT		(RT_temp);
	Device.Shader._DeleteRT		(RT);
	Device.Shader._DeleteVS		(pVS);

	// Device.Shader._DeletePS		(hPS);
	// Device.Shader._DeleteTexture(hTex);
}

void CRenderTarget::Begin	()
{
	if (!Available() || !NeedPostProcess())	
	{
		Device.Shader.set_RT	(HW.pBaseRT,HW.pBaseZB);
	} else {
		Device.Shader.set_RT	(RT->pRT,HW.pBaseZB);
		if (psDeviceFlags&rsClearBB) CHK_DX(HW.pDevice->Clear(0,0,D3DCLEAR_TARGET,D3DCOLOR_XRGB(0,255,0),1,0));
	}
}

void CRenderTarget::End		()
{
	Device.Shader.set_RT		(HW.pBaseRT,HW.pBaseZB);
	
	if (!Available() || !NeedPostProcess())	return;
	
	// Draw full-screen quad textured with our scene image
	DWORD	Offset;
	DWORD	Cgray	= D3DCOLOR_RGBA	(90,90,90,0);
	int		A		= iFloor		((1-param_gray)*255.f); clamp(A,0,255);
	DWORD	Calpha	= D3DCOLOR_RGBA	(255,255,255,A);
	float	tw		= float(Device.dwWidth);
	float	th		= float(Device.dwHeight);
	DWORD	_w		= Device.dwWidth;
	DWORD	_h		= Device.dwHeight;
	DWORD	xW		= 64;
	DWORD	xH		= 64;
	
	// UV
	Fvector2		shift,p0,p1;
	shift.set		(.5f/tw, .5f/th);
	shift.mul		(param_blur);
	p0.set			(.5f/tw, .5f/th);
	p1.set			((tw+.5f)/tw, (th+.5f)/th );
	p0.add			(shift);
	p1.add			(shift);
	
	// Fill vertex buffer
	FVF::TL* pv			= (FVF::TL*) Device.Streams.Vertex.Lock	(12,pVS->dwStride,Offset);
	pv->set(0,			float(_h),	.0001f,.9999f, Cgray, p0.x, p1.y);	pv++;
	pv->set(0,			0,			.0001f,.9999f, Cgray, p0.x, p0.y);	pv++;
	pv->set(float(_w),	float(_h),	.0001f,.9999f, Cgray, p1.x, p1.y);	pv++;
	pv->set(float(_w),	0,			.0001f,.9999f, Cgray, p1.x, p0.y);	pv++;
	pv->set(0,			float(_h),	.0001f,.9999f, Calpha,p0.x, p1.y);	pv++;
	pv->set(0,			0,			.0001f,.9999f, Calpha,p0.x, p0.y);	pv++;
	pv->set(float(_w),	float(_h),	.0001f,.9999f, Calpha,p1.x, p1.y);	pv++;
	pv->set(float(_w),	0,			.0001f,.9999f, Calpha,p1.x, p0.y);	pv++;

	pv->set(0,			float(xH),	.0001f,.9999f, Calpha,p0.x, p1.y);	pv++;
	pv->set(0,			0,			.0001f,.9999f, Calpha,p0.x, p0.y);	pv++;
	pv->set(float(xW),	float(xH),	.0001f,.9999f, Calpha,p1.x, p1.y);	pv++;
	pv->set(float(xW),	0,			.0001f,.9999f, Calpha,p1.x, p0.y);	pv++;

	Device.Streams.Vertex.Unlock	(12,pVS->dwStride);

	// Actual rendering
	if (FALSE)	
	{
		/*
		// Render to temporary buffer (PS)
		Device.Shader.set_RT				(RT_temp->pRT,HW.pTempZB);
		Device.Shader.set_Shader			(pShaderSet);
		HW.pDevice->SetPixelShader			(hPS->dwHandle);
		Device.Primitive.Draw				(pStream,4,2,Offset+8,Device.Streams_QuadIB);
		HW.pDevice->SetPixelShader			(0);

		// Render to screen
		Device.Shader.set_RT				(HW.pBaseRT,HW.pBaseZB);
		Device.Shader.set_Shader			(pShaderSet);
		Device.Primitive.Draw				(pStream,4,2,Offset+4,Device.Streams_QuadIB);

		// Add to screen
		Device.Shader.set_Shader			(pShaderAdd);
		Device.Primitive.Draw				(pStream,4,2,Offset+4,Device.Streams_QuadIB);
		*/
	} else {
		if (param_gray>0.001f) {
			// Draw GRAY
			Device.Shader.set_Shader		(pShaderGray);
			Device.Primitive.setVertices	(pVS->dwHandle,pVS->dwStride,Device.Streams.Vertex.Buffer());
			Device.Primitive.setIndices		(Offset+0,Device.Streams.QuadIB);
			Device.Primitive.Render			(D3DPT_TRIANGLELIST,0,4,0,2);
			UPDATEC							(4,2,1);

			if (param_gray<0.999f) {
				// Blend COLOR
				Device.Shader.set_Shader		(pShaderBlend);
				Device.Primitive.setVertices	(pVS->dwHandle, pVS->dwStride,Device.Streams.Vertex.Buffer());
				Device.Primitive.setIndices		(Offset+4, Device.Streams.QuadIB);
				Device.Primitive.Render			(D3DPT_TRIANGLELIST,0,4,0,2);
				UPDATEC							(4,2,1);
			}
		} else {
			// Draw COLOR
			Device.Shader.set_Shader		(pShaderSet);
			Device.Primitive.setVertices	(pVS->dwHandle,pVS->dwStride,Device.Streams.Vertex.Buffer());
			Device.Primitive.setIndices		(Offset+4,Device.Streams.QuadIB);
			Device.Primitive.Render			(D3DPT_TRIANGLELIST,0,4,0,2);
			UPDATEC							(4,2,1);
		}
	}
}
