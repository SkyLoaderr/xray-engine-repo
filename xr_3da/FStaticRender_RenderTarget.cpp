#include "stdafx.h"
#include "fstaticrender_rendertarget.h"

static LPCSTR		RTname			= "$user$rendertarget";
int					psSupersample	= 0;

CRenderTarget::CRenderTarget()
{
	bAvailable		= FALSE;
	RT				= 0;

	pShaderSet		= 0;
	pShaderGray		= 0;
	pVS				= 0;

	param_blur		= 0;
	param_gray		= 0;
	param_noise		= 0;
}

BOOL CRenderTarget::Create	()
{
	// Select mode to operate in
	switch (psSupersample)
	{
	case	1:		rtWidth = 1*Device.dwWidth;					rtHeight=1*Device.dwHeight;					break;
	case	2:		rtWidth = iFloor(1.414f*Device.dwWidth);	rtHeight=iFloor(1.414f*Device.dwHeight);	break;
	case	3:		rtWidth = iFloor(1.732f*Device.dwWidth);	rtHeight=iFloor(1.732f*Device.dwHeight);	break;
	case	4:		rtWidth = 2*Device.dwWidth;					rtHeight=2*Device.dwHeight;					break;
	default:		rtWidth	= Device.dwWidth;					rtHeight=Device.dwHeight;					return FALSE;
	}
	while (rtWidth%2)	rtWidth--;
	while (rtHeight%2)	rtHeight--;

	// Bufferts
	RT				= Device.Shader._CreateRT		(RTname,rtWidth,rtHeight);
	if ((rtHeight!=Device.dwHeight) || (rtWidth!=Device.dwWidth))
	{
		R_CHK		(HW.pDevice->CreateDepthStencilSurface	(rtWidth,rtHeight,HW.Caps.fDepth,D3DMULTISAMPLE_NONE,&ZB));
	} else {
		ZB			= HW.pBaseZB;
		ZB->AddRef	();
	}
	
	// Shaders and stream
	pVS							= Device.Shader._CreateVS		(FVF::F_TL);
	pShaderSet					= Device.Shader.Create			("effects\\screen_set",		RTname);
	pShaderGray					= Device.Shader.Create			("effects\\screen_gray",	RTname);
	pShaderBlend				= Device.Shader.Create			("effects\\screen_blend",	RTname);
	pShaderNoise				= Device.Shader.Create			("effects\\screen_noise",	"fx\\fx_noise");
	return	RT->Valid	();
}

void CRenderTarget::OnDeviceCreate	()
{
	bAvailable					= Create	();
	set_blur					(1.f);
}

void CRenderTarget::OnDeviceDestroy	()
{
	_RELEASE					(ZB);
	Device.Shader.Delete		(pShaderNoise);
	Device.Shader.Delete		(pShaderBlend);
	Device.Shader.Delete		(pShaderGray);
	Device.Shader.Delete		(pShaderSet);
	Device.Shader._DeleteRT		(RT);
	Device.Shader._DeleteVS		(pVS);
}

void CRenderTarget::e_render_noise	()
{
	Device.Shader.set_Shader		(pShaderNoise);

	CTexture*	T					= Device.Shader.get_ActiveTexture	(0);
	u32			tw					= T->get_Width	();
	u32			th					= T->get_Height	();
	u32			shift_w				= ::Random.randI(tw);
	u32			shift_h				= ::Random.randI(th);
	float		start_u				= (float(shift_w)+.5f)/(tw);
	float		start_v				= (float(shift_h)+.5f)/(th);
	u32			_w					= Device.dwWidth;
	u32			_h					= Device.dwHeight;
	u32			cnt_w				= _w / tw;
	u32			cnt_h				= _h / th;
	float		end_u				= start_u + float(cnt_w) + 1;
	float		end_v				= start_v + float(cnt_h) + 1;

	Fvector2	p0,p1;
	p0.set		(start_u,	start_v	);
	p1.set		(end_u,		end_v	);

	DWORD		Cgray				= D3DCOLOR_RGBA	(127,127,127,0);

	// 
	u32			Offset;
	FVF::TL* pv			= (FVF::TL*) Device.Streams.Vertex.Lock	(12,pVS->dwStride,Offset);
	pv->set(0,			float(_h),	.0001f,.9999f, Cgray, p0.x, p1.y);	pv++;
	pv->set(0,			0,			.0001f,.9999f, Cgray, p0.x, p0.y);	pv++;
	pv->set(float(_w),	float(_h),	.0001f,.9999f, Cgray, p1.x, p1.y);	pv++;
	pv->set(float(_w),	0,			.0001f,.9999f, Cgray, p1.x, p0.y);	pv++;
	Device.Streams.Vertex.Unlock	(4,pVS->dwStride);

	// Draw Noise
	Device.Primitive.setVertices	(pVS->dwHandle,pVS->dwStride,Device.Streams.Vertex.Buffer());
	Device.Primitive.setIndices		(Offset+0,Device.Streams.QuadIB);
	Device.Primitive.Render			(D3DPT_TRIANGLELIST,0,4,0,2);
}

BOOL CRenderTarget::Perform		()
{
	return Available() && ( NeedPostProcess() || (psSupersample>1));
}

void CRenderTarget::Begin		()
{
	if (!Perform())	
	{
		// Base RT
		Device.Shader.set_RT	(HW.pBaseRT,HW.pBaseZB);
		curWidth				= Device.dwWidth;
		curHeight				= Device.dwHeight;
	} else {
		// Our 
		Device.Shader.set_RT	(RT->pRT,ZB);
		curWidth				= rtWidth;
		curHeight				= rtHeight;
	}
	Device.Clear				();
}

void CRenderTarget::End		()
{
	Device.Shader.set_RT		(HW.pBaseRT,HW.pBaseZB);
	curWidth					= Device.dwWidth;
	curHeight					= Device.dwHeight;
	
	if (!Perform())		return;
	
	// Draw full-screen quad textured with our scene image
	u32	Offset;
	u32	Cgray			= D3DCOLOR_RGBA	(90,90,90,0);
	int		A			= iFloor		((1-param_gray)*255.f); clamp(A,0,255);
	u32	Calpha			= D3DCOLOR_RGBA	(255,255,255,A);
	float	tw			= float(rtWidth);
	float	th			= float(rtHeight);
	float	_w			= float(Device.dwWidth);
	float	_h			= float(Device.dwHeight);
	u32	xW				= 64;
	u32	xH				= 64;
	
	// UV
	Fvector2			shift,p0,p1;
	shift.set			(.5f/tw, .5f/th);
	shift.mul			(param_blur);
	p0.set				(.5f/tw, .5f/th);
	p1.set				((tw+.5f)/tw, (th+.5f)/th );
	p0.add				(shift);
	p1.add				(shift);
	
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
		if (param_gray>0.001f) 
		{
			// Draw GRAY
			Device.Shader.set_Shader		(pShaderGray);
			Device.Primitive.setVertices	(pVS->dwHandle,pVS->dwStride,Device.Streams.Vertex.Buffer());
			Device.Primitive.setIndices		(Offset+0,Device.Streams.QuadIB);
			Device.Primitive.Render			(D3DPT_TRIANGLELIST,0,4,0,2);

			if (param_gray<0.999f) {
				// Blend COLOR
				Device.Shader.set_Shader		(pShaderBlend);
				Device.Primitive.setVertices	(pVS->dwHandle, pVS->dwStride,Device.Streams.Vertex.Buffer());
				Device.Primitive.setIndices		(Offset+4, Device.Streams.QuadIB);
				Device.Primitive.Render			(D3DPT_TRIANGLELIST,0,4,0,2);
			}
		} else {
			// Draw COLOR
			Device.Shader.set_Shader		(pShaderSet);
			Device.Primitive.setVertices	(pVS->dwHandle,pVS->dwStride,Device.Streams.Vertex.Buffer());
			Device.Primitive.setIndices		(Offset+4,Device.Streams.QuadIB);
			Device.Primitive.Render			(D3DPT_TRIANGLELIST,0,4,0,2);
		}
	}

	if (param_noise>0.01f)	e_render_noise();
}
