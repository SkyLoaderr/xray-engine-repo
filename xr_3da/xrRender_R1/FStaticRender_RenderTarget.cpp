#include "stdafx.h"
#include "fstaticrender_rendertarget.h"

static LPCSTR		RTname			= "$user$rendertarget";

CRenderTarget::CRenderTarget()
{
	bAvailable			= FALSE;
	RT					= 0;
	pTempZB				= 0;

	pShaderSet			= 0;
	pShaderGray			= 0;
	pGeom				= 0;
	pGeom2				= 0;

	param_blur			= 0.f;
	param_gray			= 0.f;
	param_noise			= 0.f;
	param_duality_h		= 0.f;
	param_duality_v		= 0.f;
	param_noise_fps		= 25.f;

	im_noise_time		= 1/100;
	im_noise_shift_w	= 0;
	im_noise_shift_h	= 0;
}

BOOL CRenderTarget::Create	()
{
	curWidth			= Device.dwWidth;
	curHeight			= Device.dwHeight;

	// Select mode to operate in
	switch (ps_r__Supersample)
	{
	case	1:		rtWidth = 1*Device.dwWidth;					rtHeight=1*Device.dwHeight;					break;
	case	2:		rtWidth = iFloor(1.414f*Device.dwWidth);	rtHeight=iFloor(1.414f*Device.dwHeight);	break;
	case	3:		rtWidth = iFloor(1.732f*Device.dwWidth);	rtHeight=iFloor(1.732f*Device.dwHeight);	break;
	case	4:		rtWidth = 2*Device.dwWidth;					rtHeight=2*Device.dwHeight;					break;
	case	5:		rtWidth = iFloor(2.236f*Device.dwWidth);	rtHeight=iFloor(2.236f*Device.dwHeight);	break;
	case	6:		rtWidth = iFloor(2.449f*Device.dwWidth);	rtHeight=iFloor(2.449f*Device.dwHeight);	break;
	default:		rtWidth	= Device.dwWidth;					rtHeight=Device.dwHeight;					return FALSE;
	}
	while (rtWidth%2)	rtWidth--;
	while (rtHeight%2)	rtHeight--;
	Msg				("* SSample: %dx%d",rtWidth,rtHeight);

	// Bufferts
	RT				= Device.Shader._CreateRT		(RTname,rtWidth,rtHeight,HW.Caps.fTarget);
	if ((rtHeight!=Device.dwHeight) || (rtWidth!=Device.dwWidth))
	{
		R_CHK		(HW.pDevice->CreateDepthStencilSurface	(rtWidth,rtHeight,HW.Caps.fDepth,D3DMULTISAMPLE_NONE,0,TRUE,&ZB,NULL));
	} else {
		ZB			= HW.pBaseZB;
		ZB->AddRef	();
	}

	// Temp ZB, used by some of the shadowing code
	R_CHK	(pDevice->CreateDepthStencilSurface	(512,512,fDepth,D3DMULTISAMPLE_NONE,0,TRUE,&pTempZB,NULL));

	// Shaders and stream
	string64	_rt_2_name;
	strconcat					(_rt_2_name,RTname,",",RTname);
	pGeom						= Device.Shader.CreateGeom		(FVF::F_TL,		RCache.Vertex.Buffer(), RCache.QuadIB);
	pGeom2						= Device.Shader.CreateGeom		(FVF::F_TL2uv,	RCache.Vertex.Buffer(), RCache.QuadIB);
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
	Msg							("* SSample: %s",bAvailable?"enabled":"disabled");
}

void CRenderTarget::OnDeviceDestroy	()
{
	_RELEASE					(pTempZB);
	_RELEASE					(ZB);
	Device.Shader.Delete		(pShaderNoise);
	Device.Shader.Delete		(pShaderDuality);
	Device.Shader.Delete		(pShaderBlend);
	Device.Shader.Delete		(pShaderGray);
	Device.Shader.Delete		(pShaderSet);
	Device.Shader._DeleteRT		(RT);
	Device.Shader.DeleteGeom	(pGeom);
	Device.Shader.DeleteGeom	(pGeom2);
}

void CRenderTarget::eff_load	(LPCSTR n)
{
	/*
	param_blur					= pSettings->r_float	(n,"blur");
	param_gray					= pSettings->r_float	(n,"gray");
	param_noise					= pSettings->r_float	(n,"noise");
	param_noise_scale			= pSettings->r_float	(n,"noise_scale");
	param_noise_color			= pSettings->r_color	(n,"noise_color");
	*/
}

void CRenderTarget::e_render_noise	()
{
	RCache.set_Shader				(pShaderNoise);
	
	CTexture*	T					= RCache.get_ActiveTexture	(0);
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

	Fcolor		c_gray, c_base, c_res;
	c_base.set	(param_noise_color);
	c_gray.set	(.502f,.502f,.502f,0);
	c_res.lerp	(c_gray,c_base,param_noise);

	s32			Cblend				= iFloor		((1.f-param_noise)*255.f);	clamp		(Cblend,0,255);
	u32			Cgray				= subst_alpha	(c_res.get(),u32(Cblend));

	// 
	u32			Offset;
	FVF::TL* pv			= (FVF::TL*) RCache.Vertex.Lock	(4,pGeom->vb_stride,Offset);
	pv->set(0,			float(_h),	.0001f,.9999f, Cgray, p0.x, p1.y);	pv++;
	pv->set(0,			0,			.0001f,.9999f, Cgray, p0.x, p0.y);	pv++;
	pv->set(float(_w),	float(_h),	.0001f,.9999f, Cgray, p1.x, p1.y);	pv++;
	pv->set(float(_w),	0,			.0001f,.9999f, Cgray, p1.x, p0.y);	pv++;
	RCache.Vertex.Unlock	(4,pGeom->vb_stride);

	// Draw Noise
	RCache.set_Geometry		(pGeom);
	RCache.Render			(D3DPT_TRIANGLELIST,Offset+0,0,4,0,2);
}

void CRenderTarget::e_render_duality()
{
	RCache.set_Shader		(pShaderDuality);

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
	FVF::TL2uv* pv					= (FVF::TL2uv*) RCache.Vertex.Lock	(4,pGeom2->vb_stride,Offset);
	pv->set(0,			float(_h),	.0001f,.9999f, C, r0.x, r1.y, l0.x, l1.y);	pv++;
	pv->set(0,			0,			.0001f,.9999f, C, r0.x, r0.y, l0.x, l0.y);	pv++;
	pv->set(float(_w),	float(_h),	.0001f,.9999f, C, r1.x, r1.y, l1.x, l1.y);	pv++;
	pv->set(float(_w),	0,			.0001f,.9999f, C, r1.x, r0.y, l1.x, l0.y);	pv++;
	RCache.Vertex.Unlock	(4,pGeom2->vb_stride);

	// Draw Noise
	RCache.set_Geometry		(pGeom2);
	RCache.Render			(D3DPT_TRIANGLELIST,Offset+0,0,4,0,2);
}

BOOL CRenderTarget::Perform		()
{
	return Available() && ( NeedPostProcess() || (ps_r__Supersample>1));
}

void CRenderTarget::Begin		()
{
	if (!Perform())	
	{
		// Base RT
		RCache.set_RT			(HW.pBaseRT);
		RCache.set_ZB			(HW.pBaseZB);
		curWidth				= Device.dwWidth;
		curHeight				= Device.dwHeight;
	} else {
		// Our 
		RCache.set_RT			(RT->pRT);
		RCache.set_ZB			(ZB);
		curWidth				= rtWidth;
		curHeight				= rtHeight;
	}
	Device.Clear				();
}

void CRenderTarget::End		()
{
	RCache.set_RT				(HW.pBaseRT);
	RCache.set_ZB				(HW.pBaseZB);
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
	if (rtHeight!=Device.dwHeight)	param_blur = 1.f;


	Fvector2			shift,p0,p1;
	shift.set			(.5f/tw, .5f/th);
	shift.mul			(param_blur);
	p0.set				(.5f/tw, .5f/th);
	p1.set				((tw+.5f)/tw, (th+.5f)/th );
	p0.add				(shift);
	p1.add				(shift);
	
	// Fill vertex buffer
	FVF::TL* pv			= (FVF::TL*) RCache.Vertex.Lock	(12,pGeom->vb_stride,Offset);
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

	RCache.Vertex.Unlock	(12,pGeom->vb_stride);

	// Actual rendering
	if (param_duality_h>0.001f || param_duality_v>0.001f)
	{
		e_render_duality	();
	} else 
	{
		if (param_gray>0.001f) 
		{
			// Draw GRAY
			RCache.set_Shader		(pShaderGray);
			RCache.set_Geometry		(pGeom);
			RCache.Render			(D3DPT_TRIANGLELIST,Offset+0,0,4,0,2);

			if (param_gray<0.999f) {
				// Blend COLOR
				RCache.set_Shader		(pShaderBlend);
				RCache.set_Geometry		(pGeom);
				RCache.Render			(D3DPT_TRIANGLELIST,Offset+4, 0,4,0,2);
			}
		} else {
			// Draw COLOR
			RCache.set_Shader		(pShaderSet);
			RCache.set_Geometry		(pGeom);
			RCache.Render			(D3DPT_TRIANGLELIST,Offset+4,0,4,0,2);
		}
	}

	if (param_noise>0.001f)	e_render_noise	();
}
