#include "stdafx.h"
#include "fstaticrender_rendertarget.h"

static LPCSTR		RTname			= "$user$rendertarget";
static LPCSTR		RTname_distort	= "$user$distort";

CRenderTarget::CRenderTarget()
{
	bAvailable			= FALSE;
	RT					= 0;
	pTempZB				= 0;

	param_blur			= 0.f;
	param_gray			= 0.f;
	param_noise			= 0.f;
	param_duality_h		= 0.f;
	param_duality_v		= 0.f;
	param_noise_fps		= 25.f;
	param_noise_scale	= 1.f;

	im_noise_time		= 1/100;
	im_noise_shift_w	= 0;
	im_noise_shift_h	= 0;

	param_color_base	= color_rgba(127,127,127,	0);
	param_color_gray	= color_rgba(85,85,85,		0);
	param_color_add		= color_rgba(0,0,0,			0);

	bAvailable			= Create	();
	Msg					("* SSample: %s",bAvailable?"enabled":"disabled");
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
	RT.create			(RTname,			rtWidth,rtHeight,HW.Caps.fTarget);
	RT_distort.create	(RTname_distort,	rtWidth,rtHeight,HW.Caps.fTarget);
	if ((rtHeight!=Device.dwHeight) || (rtWidth!=Device.dwWidth))	{
		R_CHK		(HW.pDevice->CreateDepthStencilSurface	(rtWidth,rtHeight,HW.Caps.fDepth,D3DMULTISAMPLE_NONE,0,TRUE,&ZB,NULL));
	} else {
		ZB			= HW.pBaseZB;
		ZB->AddRef	();
	}

	// Temp ZB, used by some of the shadowing code
	R_CHK	(HW.pDevice->CreateDepthStencilSurface	(512,512,HW.Caps.fDepth,D3DMULTISAMPLE_NONE,0,TRUE,&pTempZB,NULL));

	// Shaders and stream
	s_postprocess.create				("postprocess");
	if (RImplementation.b_distortion)	s_postprocess_D.create("postprocess_d");
	g_postprocess.create				(D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_SPECULAR|D3DFVF_TEX3,RCache.Vertex.Buffer(),RCache.QuadIB);
	return	RT->valid() && RT_distort->valid();
}

CRenderTarget::~CRenderTarget	()
{
	_RELEASE					(pTempZB);
	_RELEASE					(ZB);
	s_postprocess_D.destroy		();
	s_postprocess.destroy		();
	g_postprocess.destroy		();
	RT_distort.destroy			();
	RT.destroy					();
}

void	CRenderTarget::calc_tc_noise		(Fvector2& p0, Fvector2& p1)
{
	CTexture*	T					= RCache.get_ActiveTexture	(2);
	VERIFY2		(T, "Texture #3 in noise shader should be setted up");
	u32			tw					= iCeil(float(T->get_Width	())*param_noise_scale+EPS_S);
	u32			th					= iCeil(float(T->get_Height ())*param_noise_scale+EPS_S);
	VERIFY2		(tw && th, "Noise scale can't be zero in any way");

	// calculate shift from FPSes
	im_noise_time					-= Device.fTimeDelta;
	if (im_noise_time<0)			{
		im_noise_shift_w			= ::Random.randI(tw?tw:1);
		im_noise_shift_h			= ::Random.randI(th?th:1);
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
 
	p0.set		(start_u,	start_v	);
	p1.set		(end_u,		end_v	);
}

void CRenderTarget::calc_tc_duality_ss	(Fvector2& r0, Fvector2& r1, Fvector2& l0, Fvector2& l1)
{
	// Calculate ordinaty TCs from blur and SS
	float	tw			= float(rtWidth);
	float	th			= float(rtHeight);
	if (rtHeight!=Device.dwHeight)	param_blur = 1.f;
	Fvector2			shift,p0,p1;
	shift.set			(.5f/tw, .5f/th);
	shift.mul			(param_blur);
	p0.set				(.5f/tw, .5f/th).add			(shift);
	p1.set				((tw+.5f)/tw, (th+.5f)/th ).add	(shift);

	// Calculate Duality TC
	float	shift_u		= param_duality_h*.5f;
	float	shift_v		= param_duality_v*.5f;

	r0.set(p0.x,p0.y);					r1.set(p1.x-shift_u,p1.y-shift_v);
	l0.set(p0.x+shift_u,p0.y+shift_v);	l1.set(p1.x,p1.y);
}

BOOL CRenderTarget::Perform		()
{
	return Available() && ( NeedPostProcess() || (ps_r__Supersample>1) || (frame_distort==(Device.dwFrame-1)));
}

#include <dinput.h>
#define SHOW(a)	Log(#a,a);
void CRenderTarget::Begin		()
{
	//.
	if (g_pGameLevel->IR_GetKeyState(DIK_LSHIFT))	
	{
		Msg					("[%5d]------------------------",Device.dwFrame);
		SHOW				(param_blur)
		SHOW				(param_gray)
		SHOW				(param_duality_h)
		SHOW				(param_duality_v)
		SHOW				(param_noise)
		SHOW				(param_noise_scale)
		SHOW				(param_noise_fps)

		SHOW				(param_color_base)
		SHOW				(param_color_gray)
		SHOW				(param_color_add)
	}

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

struct TL_2c3uv {
	Fvector4	p;
	u32			color0;
	u32			color1;
	Fvector2	uv	[3];
	IC void	set	(float x, float y, u32 c0, u32 c1, float u0, float v0, float u1, float v1, float u2, float v2)	{	
		p.set	(x,y,EPS_S,1.f); 
		color0 = c0; 
		color1 = c1;
		uv[0].set(u0,v0); 
		uv[1].set(u1,v1); 
		uv[2].set(u2,v2); 
	}
};

void CRenderTarget::End		()
{
	// find if distortion is needed at all
	BOOL	bPerform	= Perform				();
	BOOL	bDistort	= RImplementation.b_distortion;
	if (0==RImplementation.mapDistort.size())	bDistort	= FALSE;
	if (bDistort)		phase_distortion		();

	// combination/postprocess
	RCache.set_RT		(HW.pBaseRT);
	RCache.set_ZB		(HW.pBaseZB);
	curWidth			= Device.dwWidth;
	curHeight			= Device.dwHeight;
	
	if (!bPerform)		return;
	RCache.set_Shader	(bDistort ? s_postprocess_D : s_postprocess );

	int		gblend		= clampr		(iFloor((1-param_gray)*255.f),0,255);
	int		nblend		= clampr		(iFloor((1-param_noise)*255.f),0,255);
	u32					p_color			= subst_alpha		(param_color_base,nblend);
	u32					p_gray			= subst_alpha		(param_color_gray,gblend);
	u32					p_brightness	= param_color_add	;
	// Msg				("param_gray:%f(%d),param_noise:%f(%d)",param_gray,gblend,param_noise,nblend);
	// Msg				("base: %d,%d,%d",	color_get_R(p_color),		color_get_G(p_color),		color_get_B(p_color));
	// Msg				("gray: %d,%d,%d",	color_get_R(p_gray),		color_get_G(p_gray),		color_get_B(p_gray));
	// Msg				("add:  %d,%d,%d",	color_get_R(p_brightness),	color_get_G(p_brightness),	color_get_B(p_brightness));
	
	// Draw full-screen quad textured with our scene image
	u32		Offset;
	float	_w			= float(Device.dwWidth);
	float	_h			= float(Device.dwHeight);
	
	Fvector2			n0,n1,r0,r1,l0,l1;
	calc_tc_duality_ss	(r0,r1,l0,l1);
	calc_tc_noise		(n0,n1);

	// Fill vertex buffer
	TL_2c3uv* pv		= (TL_2c3uv*) RCache.Vertex.Lock	(4,g_postprocess.stride(),Offset);
	pv->set(0,			float(_h),	p_color, p_gray, r0.x, r1.y, l0.x, l1.y, n0.x, n1.y);	pv++;
	pv->set(0,			0,			p_color, p_gray, r0.x, r0.y, l0.x, l0.y, n0.x, n0.y);	pv++;
	pv->set(float(_w),	float(_h),	p_color, p_gray, r1.x, r1.y, l1.x, l1.y, n1.x, n1.y);	pv++;
	pv->set(float(_w),	0,			p_color, p_gray, r1.x, r0.y, l1.x, l0.y, n1.x, n0.y);	pv++;
	RCache.Vertex.Unlock									(4,g_postprocess.stride());

	// Actual rendering
	RCache.set_c		("c_brightness",color_get_R(p_brightness)/255.f,color_get_G(p_brightness)/255.f,color_get_B(p_brightness)/255.f,0);
	RCache.set_Geometry	(g_postprocess);
	RCache.Render		(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
}

void	CRenderTarget::phase_distortion	()
{
	frame_distort								= Device.dwFrame;
	RCache.set_RT								(RT_distort->pRT);
	RCache.set_ZB								(ZB);
	RCache.set_CullMode							(CULL_CCW);
	RCache.set_ColorWriteEnable					( );
	CHK_DX(HW.pDevice->Clear					( 0L, NULL, D3DCLEAR_TARGET, color_rgba(127,127,127,127), 1.0f, 0L));
	RImplementation.r_dsgraph_render_distort	( );
}
