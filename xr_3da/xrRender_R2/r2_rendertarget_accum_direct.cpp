#include "stdafx.h"

void CRenderTarget::accum_direct		()
{
	RImplementation.stats.l_visible		++;

	// *** assume accumulator setted up ***
	light*			fuckingsun			= RImplementation.Lights.sun	;

	// texture adjustment matrix
	float			fTexelOffs			= (.5f / float(RImplementation.o.smapsize));
	float			fRange				= float(1.f)*ps_r2_ls_depth_scale;
	float			fBias				= ps_r2_ls_depth_bias;
	Fmatrix			m_TexelAdjust		= 
	{
		0.5f,				0.0f,				0.0f,			0.0f,
		0.0f,				-0.5f,				0.0f,			0.0f,
		0.0f,				0.0f,				fRange,			0.0f,
		0.5f + fTexelOffs,	0.5f + fTexelOffs,	fBias,			1.0f
	};

	// compute xforms
	Fmatrix				m_shadow;
	{
		Fmatrix			xf_invview;		xf_invview.invert	(Device.mView)	;
		Fmatrix			xf_project;		xf_project.mul		(m_TexelAdjust,fuckingsun->X.D.combine);
		m_shadow.mul	(xf_project,	xf_invview);
	}

	// Perform lighting
	// Draw full-screen quad textured with our scene image
	{
		u32		Offset;
		u32		C					= color_rgba	(255,255,255,255);
		float	_w					= float			(Device.dwWidth);
		float	_h					= float			(Device.dwHeight);

		Fvector2					p0,p1;
		p0.set						(.5f/_w, .5f/_h);
		p1.set						((_w+.5f)/_w, (_h+.5f)/_h );

		// Analyze depth
		float	d_Z	= EPS_S, d_W = 1.f;

		// Fill vertex buffer
		FVF::TL* pv					= (FVF::TL*) RCache.Vertex.Lock	(4,g_combine->vb_stride,Offset);
		pv->set						(EPS,			float(_h+EPS),	d_Z,	d_W, C, p0.x, p1.y);	pv++;
		pv->set						(EPS,			EPS,			d_Z,	d_W, C, p0.x, p0.y);	pv++;
		pv->set						(float(_w+EPS),	float(_h+EPS),	d_Z,	d_W, C, p1.x, p1.y);	pv++;
		pv->set						(float(_w+EPS),	EPS,			d_Z,	d_W, C, p1.x, p0.y);	pv++;
		RCache.Vertex.Unlock		(4,g_combine->vb_stride);
		RCache.set_Geometry			(g_combine);

		// Common constants
		Fvector		L_dir,L_clr;	float L_spec;
		L_clr.set					(fuckingsun->color.r,fuckingsun->color.g,fuckingsun->color.b);
		L_spec						= u_diffuse2s	(L_clr)/ps_r2_ls_dynamic_range;
		L_clr.div					(ps_r2_ls_dynamic_range		);
		Device.mView.transform_dir	(L_dir,fuckingsun->direction);
		L_dir.normalize				();

		// setup
		RCache.set_Element			(s_accum_direct->E[0]);
		RCache.set_c				("Ldynamic_dir",		L_dir.x,L_dir.y,L_dir.z,0		);
		RCache.set_c				("Ldynamic_color",		L_clr.x,L_clr.y,L_clr.z,L_spec	);
		RCache.set_c				("m_shadow",			m_shadow						);

		RCache.set_Stencil			(TRUE,D3DCMP_LESSEQUAL,	0x01,0xff,0x00);
		RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
	}
}

/*
void CRenderTarget::shadow_direct	(light* L, u32 dls_phase)
{
	// Draw full-screen quad textured with our scene image
	{
		u32		Offset;
		u32		C					= color_rgba	(255,255,255,255);
		float	_w					= float(Device.dwWidth);
		float	_h					= float(Device.dwHeight);

		Fvector2					p0,p1;
		p0.set						(.5f/_w, .5f/_h);
		p1.set						((_w+.5f)/_w, (_h+.5f)/_h );

		// Analyze depth
		float	d_Z	= EPS_S, d_W = 1.f;

		// Fill vertex buffer
		FVF::TL* pv					= (FVF::TL*) RCache.Vertex.Lock	(4,g_combine->vb_stride,Offset);
		pv->set						(EPS,			float(_h+EPS),	d_Z,	d_W, C, p0.x, p1.y);	pv++;
		pv->set						(EPS,			EPS,			d_Z,	d_W, C, p0.x, p0.y);	pv++;
		pv->set						(float(_w+EPS),	float(_h+EPS),	d_Z,	d_W, C, p1.x, p1.y);	pv++;
		pv->set						(float(_w+EPS),	EPS,			d_Z,	d_W, C, p1.x, p0.y);	pv++;
		RCache.Vertex.Unlock		(4,g_combine->vb_stride);
		RCache.set_Geometry			(g_combine);

		// Shader + constants
		float circle				= ps_r2_ls_dsm_kernel / float(RImplementation.o.smapsize);
		Fvector4 J; float scale		= circle/11.f;

		// 1
		RCache.set_Element			(s_accum_direct->E[dls_phase]);
		R_constant* _C				= RCache.get_c			("J_direct");
		if (C)
		{
			J.set(11, 0,  0);		J.sub(11); J.mul(scale);	RCache.set_ca	(_C,0,J.x,J.y,-J.y,-J.x);
			J.set(19, 3,  0);		J.sub(11); J.mul(scale);	RCache.set_ca	(_C,1,J.x,J.y,-J.y,-J.x);
			J.set(22, 11, 0);		J.sub(11); J.mul(scale);	RCache.set_ca	(_C,2,J.x,J.y,-J.y,-J.x);
			J.set(19, 19, 0);		J.sub(11); J.mul(scale);	RCache.set_ca	(_C,3,J.x,J.y,-J.y,-J.x);
			J.set(9,  7,  15, 9);	J.sub(11); J.mul(scale);	RCache.set_ca	(_C,4,J.x,J.y,J.w,J.z);
			J.set(13, 15, 7,  13);	J.sub(11); J.mul(scale);	RCache.set_ca	(_C,5,J.x,J.y,J.w,J.z);
		}
		RCache.set_Stencil			(TRUE,D3DCMP_LESSEQUAL,0x02,0xff,0x00);
		RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
	}
}

void CRenderTarget::accum_direct()
{
	// Common
	u32		Offset						= 0;
	float	d_Z							= EPS_S;
	float	d_W							= 1.f;
	Fvector2							p0,p1;

	// ***** Assuming next usage will be for directional light *****
	// Stencil	- draw only where stencil >= 0x1
	// Stencil	- increment value if both (stencil,aref) pass
	// Stencil	- result -> 0x2 where pixel can be potentialy lighted/shadowed
	if (0 && ps_r2_ls_flags.test(R2FLAG_SUN))
	{
		// Restore targets
		u_setrt						(rt_Accumulator,NULL,NULL,HW.pBaseZB);
		RCache.set_Stencil			(TRUE,D3DCMP_LESSEQUAL,0x03,0x01,0x02,D3DSTENCILOP_KEEP,D3DSTENCILOP_REPLACE,D3DSTENCILOP_KEEP);

		// Assuming next usage will be for directional light
		u32		C					= color_rgba	(255,255,255,255);
		float	_w					= float			(Device.dwWidth);
		float	_h					= float			(Device.dwHeight);

		p0.set						(.5f/_w, .5f/_h);
		p1.set						((_w+.5f)/_w, (_h+.5f)/_h );

		// Fill vertex buffer
		FVF::TL* pv					= (FVF::TL*) RCache.Vertex.Lock	(4,g_combine->vb_stride,Offset);
		pv->set						(EPS,			float(_h+EPS),	d_Z,	d_W, C, p0.x, p1.y);	pv++;
		pv->set						(EPS,			EPS,			d_Z,	d_W, C, p0.x, p0.y);	pv++;
		pv->set						(float(_w+EPS),	float(_h+EPS),	d_Z,	d_W, C, p1.x, p1.y);	pv++;
		pv->set						(float(_w+EPS),	EPS,			d_Z,	d_W, C, p1.x, p0.y);	pv++;
		RCache.Vertex.Unlock		(4,g_combine->vb_stride);
		RCache.set_Geometry			(g_combine);
		RCache.set_Element			(s_accum_mask->E[2]);

		// Calculate light-brightness
		Fcolor		L_sunc			= RImplementation.Lights.sun->color; 
		Fvector		L_clr;			L_clr.set(L_sunc.r,L_sunc.g,L_sunc.b).div(ps_r2_ls_dynamic_range);
		float		L_max			= _max(_max(L_clr.x,L_clr.y),L_clr.z);
		float		L_mag			= L_clr.magnitude()/_sqrt(3.f);
		float		L_gray			= (L_clr.x + L_clr.y + L_clr.z)/3.f;
		float		L_brightness	= (L_max+L_mag+L_gray)/3.f;		// maximal brightness at dot(L,N)=1

		// Constants
		Fvector		L_dir;
		Device.mView.transform_dir	(L_dir,RImplementation.Lights.sun->direction);
		L_dir.invert				();
		L_dir.set_length			(L_brightness);
		// RCache.set_c				("light_direction",	L_dir.x,L_dir.y,L_dir.z,0.f);

		// Render
		RCache.set_ColorWriteEnable	(FALSE);
		RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
		RCache.set_ColorWriteEnable	();
	}

	u32		Offset;
	u32		C					= color_rgba	(255,255,255,255);
	float	_w					= float(Device.dwWidth);
	float	_h					= float(Device.dwHeight);

	Fvector2					p0,p1;
	p0.set						(.5f/_w, .5f/_h);
	p1.set						((_w+.5f)/_w, (_h+.5f)/_h );

	// Fill vertex buffer
	float	d_Z	= EPS_S, d_W = 1.f;
	FVF::TL* pv					= (FVF::TL*) RCache.Vertex.Lock	(4,g_combine->vb_stride,Offset);
	pv->set						(EPS,			float(_h+EPS),	d_Z,	d_W, C, p0.x, p1.y);	pv++;
	pv->set						(EPS,			EPS,			d_Z,	d_W, C, p0.x, p0.y);	pv++;
	pv->set						(float(_w+EPS),	float(_h+EPS),	d_Z,	d_W, C, p1.x, p1.y);	pv++;
	pv->set						(float(_w+EPS),	EPS,			d_Z,	d_W, C, p1.x, p0.y);	pv++;
	RCache.Vertex.Unlock		(4,g_combine->vb_stride);
	RCache.set_Geometry			(g_combine);
	RCache.set_Element			(s_accum_direct->E[2]);

	// Constants
	Fvector		L_dir,L_clr;	float L_spec;
	Fcolor		L_sunc			= RImplementation.Lights.sun->color;
	L_clr.set					(L_sunc.r,L_sunc.g,L_sunc.b);
	L_spec						= u_diffuse2s	(L_clr)/ps_r2_ls_dynamic_range;
	L_clr.div					(ps_r2_ls_dynamic_range);
	Device.mView.transform_dir	(L_dir,RImplementation.Lights.sun->direction);
	L_dir.normalize				();
	RCache.set_c				("Ldynamic_color",	L_clr.x,L_clr.y,L_clr.z,L_spec);

	// Render if stencil >= 0x2
	RCache.set_Stencil			(TRUE,D3DCMP_LESSEQUAL,0x02,0xff,0x00);
	RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
}
*/