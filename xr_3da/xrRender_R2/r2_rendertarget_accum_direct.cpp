#include "stdafx.h"

void CRenderTarget::shadow_direct	(light* L, u32 dls_phase)
{
	RImplementation.stats.l_visible	++;

	// *** assume accumulator setted up ***
	// texture adjustment matrix
	float			fTexelOffs			= (.5f / float(RImplementation.o.smapsize));
	u32				uRange				= 1; 
	float			fRange				= float(uRange);
	float			fBias				= -0.0035f*fRange;
	Fmatrix			m_TexelAdjust		= 
	{
		0.5f,				0.0f,				0.0f,			0.0f,
		0.0f,				-0.5f,				0.0f,			0.0f,
		0.0f,				0.0f,				fRange,			0.0f,
		0.5f + fTexelOffs,	0.5f + fTexelOffs,	fBias,			1.0f
	};

	// compute xforms
	Fmatrix			xf_world;		xf_world.invert	(Device.mView);
	Fmatrix&		xf_view			= L->X.D.view;
	Fmatrix			xf_project;		xf_project.mul	(m_TexelAdjust,L->X.D.project);
	RCache.set_xform_world			(xf_world	);
	RCache.set_xform_view			(xf_view	);
	RCache.set_xform_project		(xf_project	);

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
