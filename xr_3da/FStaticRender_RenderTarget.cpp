#include "stdafx.h"
#include "fstaticrender_rendertarget.h"
#include "xr_ini.h"

static LPCSTR		RTname			= "$user$rendertarget";
int					psSupersample	= 0;

CRenderTarget::CRenderTarget()
{
	bAvailable		= FALSE;
	RT				= 0;

	pShaderSet		= 0;
	pShaderGray		= 0;
	pVS				= 0;

	param_blur		= 0.f;
	param_gray		= 0.f;
	param_noise		= 0.f;
	param_duality_h	= 0.f;
	param_duality_v	= 0.f;
	param_noise_fps	= 25.f;

	im_noise_time	= 1/100;
	im_noise_shift_w= 0;
	im_noise_shift_h= 0;
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
	string64	_rt_2_name;
	strconcat					(_rt_2_name,RTname,",",RTname);
	pVS							= Device.Shader._CreateVS		(FVF::F_TL);
	pVS2						= Device.Shader._CreateVS		(FVF::F_TL2uv);
	pShaderSet					= Device.Shader.Create			("effects\\screen_set",		RTname);
	pShaderGray					= Device.Shader.Create			("effects\\screen_gray",	RTname);
	pShaderBlend				= Device.Shader.Create			("effects\\screen_blend",	RTname);
	pShaderDuality				= Device.Shader.Create			("effects\\blur",			_rt_2_name);
	pShaderNoise				= Device.Shader.Create			("effects\\screen_noise",	"fx\\fx_noise2");
	return	RT->Valid	();
}

void CRenderTarget::OnDeviceCreate	()
{
	bAvailable					= Create	();
	eff_load					("postprocess_base");
}

void CRenderTarget::OnDeviceDestroy	()
{
	_RELEASE					(ZB);
	Device.Shader.Delete		(pShaderNoise);
	Device.Shader.Delete		(pShaderDuality);
	Device.Shader.Delete		(pShaderBlend);
	Device.Shader.Delete		(pShaderGray);
	Device.Shader.Delete		(pShaderSet);
	Device.Shader._DeleteRT		(RT);
	Device.Shader._DeleteVS		(pVS);
	Device.Shader._DeleteVS		(pVS2);
}

void CRenderTarget::eff_load	(LPCSTR n)
{
	/*
	param_blur					= pSettings->ReadFLOAT	(n,"blur");
	param_gray					= pSettings->ReadFLOAT	(n,"gray");
	param_noise					= pSettings->ReadFLOAT	(n,"noise");
	param_noise_scale			= pSettings->ReadFLOAT	(n,"noise_scale");
	param_noise_color			= pSettings->ReadCOLOR	(n,"noise_color");
	*/
}

void CRenderTarget::e_render_noise	()
{
	Device.Shader.set_Shader		(pShaderNoise);
	
	CTexture*	T					= Device.Shader.get_ActiveTexture	(0);
	u32			tw					= iFloor(float(T->get_Width	())*param_noise_scale+EPS_S);
	u32			th					= iFloor(float(T->get_Height())*param_noise_scale+EPS_S);

	// calculate shift from FPSes
	im_noise_time					-= Device.fTimeDelta;
	if (im_noise_time<0)
	{
		im_noise_shift_w			= ::Random.randI(tw);
		im_noise_shift_h			= ::Random.randI(tw);
		float	fps_time			= 1/param_noise_fps;
		while (im_noise_time<0)		im_noise_time += fps_time;
	}

	u32			shift_w				= im_noise_shift_w;
	u32			shift_h				= im_noise_shift_h;
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

	u32			Cblend				= iFloor		(param_noise*255.f);
	clamp		(Cblend,0u,255u);
	u32			Cgray				= param_noise_color | D3DCOLOR_RGBA	(0,0,0,Cblend);

	// 
	u32			Offset;
	FVF::TL* pv			= (FVF::TL*) Device.Streams.Vertex.Lock	(4,pVS->dwStride,Offset);
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

void CRenderTarget::e_render_duality()
{
	Device.Shader.set_Shader		(pShaderDuality);

	float		shift_u				= param_duality_h*.5f;
	float		shift_v				= param_duality_v*.5f;

	Fvector2	r0,r1,l0,l1;
	r0.set(0.f,0.f);					r1.set(1.f-shift_u,1.f-shift_v);
	l0.set(0.f+shift_u,0.f+shift_v);	l1.set(1.f,1.f);

	u32			_w					= Device.dwWidth;
	u32			_h					= Device.dwHeight;
	u32			C					= 0xffffffff;

	// 
	u32			Offset;
	FVF::TL2uv* pv					= (FVF::TL2uv*) Device.Streams.Vertex.Lock	(4,pVS2->dwStride,Offset);
	pv->set(0,			float(_h),	.0001f,.9999f, C, r0.x, r1.y, l0.x, l1.y);	pv++;
	pv->set(0,			0,			.0001f,.9999f, C, r0.x, r0.y, l0.x, l0.y);	pv++;
	pv->set(float(_w),	float(_h),	.0001f,.9999f, C, r1.x, r1.y, l1.x, l1.y);	pv++;
	pv->set(float(_w),	0,			.0001f,.9999f, C, r1.x, r0.y, l1.x, l0.y);	pv++;
	Device.Streams.Vertex.Unlock	(4,pVS2->dwStride);

	// Draw Noise
	Device.Primitive.setVertices	(pVS2->dwHandle, pVS2->dwStride, Device.Streams.Vertex.Buffer());
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
	
	if (!Perform())							return;
	if (!psDeviceFlags.test(rsPostprocess))	return;
	
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
	if (param_duality_h>0.001f || param_duality_v>0.001f)
	{
		e_render_duality	();
	} else 
	{
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

	if (param_noise>0.001f)	e_render_noise	();
}
