#include "stdafx.h"

void CRenderTarget::shadow_direct	(u32 dls_phase)
{
	// *** assume accumulator setted up ***
	float			z_bias				= dls_phase?0.006f:0.003f;

	// texture adjustment matrix
	float			fTexelOffs			= (.5f / DSM_size);
	Fmatrix			m_TexelAdjust		= 
	{
		0.5f,				0.0f,				0.0f,			0.0f,
		0.0f,				-0.5f,				0.0f,			0.0f,
		0.0f,				0.0f,				1.0f,			0.0f,
		0.5f + fTexelOffs,	0.5f + fTexelOffs,	0.0f - z_bias,	1.0f
	};

	// compute xforms
	Fmatrix			xf_world;		xf_world.invert	(Device.mView);
	Fmatrix&		xf_view			= RImplementation.LR_Direct.L_view;
	Fmatrix			xf_project;		xf_project.mul	(m_TexelAdjust,RImplementation.LR_Direct.L_project);
	RCache.set_xform_world			(xf_world	);
	RCache.set_xform_view			(xf_view	);
	RCache.set_xform_project		(xf_project	);

	// Draw full-screen quad textured with our scene image
	{
		u32		Offset;
		u32		C					= D3DCOLOR_RGBA	(255,255,255,255);
		float	_w					= float(Device.dwWidth);
		float	_h					= float(Device.dwHeight);

		Fvector2					p0,p1;
		p0.set						(.5f/_w, .5f/_h);
		p1.set						((_w+.5f)/_w, (_h+.5f)/_h );

		// Analyze depth
		float	d_Z	= EPS_S, d_W = 1.f;
		if (0==dls_phase)
		{
			Fvector3				pt;
			Fvector4				pt_hpos;
			pt.mad					(Device.vCameraPosition,Device.vCameraDirection,DSM_distance_1);
			Device.mFullTransform.transform(pt_hpos,pt);
			d_Z						= pt_hpos.z;
			d_W						= pt_hpos.w;
		}

		// Fill vertex buffer
		FVF::TL* pv					= (FVF::TL*) RCache.Vertex.Lock	(4,g_combine->vb_stride,Offset);
		pv->set						(EPS,			float(_h+EPS),	d_Z,	d_W, C, p0.x, p1.y);	pv++;
		pv->set						(EPS,			EPS,			d_Z,	d_W, C, p0.x, p0.y);	pv++;
		pv->set						(float(_w+EPS),	float(_h+EPS),	d_Z,	d_W, C, p1.x, p1.y);	pv++;
		pv->set						(float(_w+EPS),	EPS,			d_Z,	d_W, C, p1.x, p0.y);	pv++;
		RCache.Vertex.Unlock		(4,g_combine->vb_stride);
		RCache.set_Geometry			(g_combine);

		// Shader + constants
		float circle				= ps_r2_ls_dsm_kernel / DSM_size;
		if (1)
		{
			Fvector4 J; float scale		= circle/11.f;

			// 1
			RCache.set_Element			(s_accum_direct->E[dls_phase]);
			R_constant* _C				= RCache.get_c			("jitter");
			J.set(11, 0,  0);			J.sub(11); J.mul(scale);	RCache.set_ca	(_C,0,J.x,J.y,-J.y,-J.x);
			J.set(19, 3,  0);			J.sub(11); J.mul(scale);	RCache.set_ca	(_C,1,J.x,J.y,-J.y,-J.x);
			J.set(22, 11, 0);			J.sub(11); J.mul(scale);	RCache.set_ca	(_C,2,J.x,J.y,-J.y,-J.x);
			J.set(19, 19, 0);			J.sub(11); J.mul(scale);	RCache.set_ca	(_C,3,J.x,J.y,-J.y,-J.x);
			J.set(9,  7,  15, 9);		J.sub(11); J.mul(scale);	RCache.set_ca	(_C,4,J.x,J.y,J.w,J.z);
			J.set(13, 15, 7,  13);		J.sub(11); J.mul(scale);	RCache.set_ca	(_C,5,J.x,J.y,J.w,J.z);
			CHK_DX						(HW.pDevice->SetRenderState	( D3DRS_STENCILREF,		0x02	));
			RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
		} else {
			Fvector4 J; float scale		= circle/27.f;

			// 1
			RCache.set_Element			(s_accum_direct->E[0]);
			R_constant* _C1				= RCache.get_c			("jitter");
			J.set(21, 2,  33, 2 );		J.sub(27); J.mul(scale); RCache.set_ca	(_C1,0,J.x,J.y,J.w,J.z);
			J.set(9,  9,  45, 9 );		J.sub(27); J.mul(scale); RCache.set_ca	(_C1,1,J.x,J.y,J.w,J.z);
			J.set(20, 12, 34, 12);		J.sub(27); J.mul(scale); RCache.set_ca	(_C1,2,J.x,J.y,J.w,J.z);
			J.set(12, 20, 27, 20);		J.sub(27); J.mul(scale); RCache.set_ca	(_C1,3,J.x,J.y,J.w,J.z);
			J.set(42, 20, 2,  21);		J.sub(27); J.mul(scale); RCache.set_ca	(_C1,4,J.x,J.y,J.w,J.z);
			J.set(52, 21, 20, 27);		J.sub(27); J.mul(scale); RCache.set_ca	(_C1,5,J.x,J.y,J.w,J.z);
			RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);

			// 2
			RCache.set_Element			(s_accum_direct->E[1]);
			R_constant* _D1				= RCache.get_c			("jitter");
			J.set(34, 27, 2,  33);	J.sub(27); J.mul(scale); RCache.set_ca	(_D1,0,J.x,J.y,J.w,J.z);
			J.set(52, 33, 12, 34);	J.sub(27); J.mul(scale); RCache.set_ca	(_D1,1,J.x,J.y,J.w,J.z);
			J.set(27, 34, 42, 34);	J.sub(27); J.mul(scale); RCache.set_ca	(_D1,2,J.x,J.y,J.w,J.z);
			J.set(20, 42, 34, 42);	J.sub(27); J.mul(scale); RCache.set_ca	(_D1,3,J.x,J.y,J.w,J.z);
			J.set(9,  45, 45, 45);	J.sub(27); J.mul(scale); RCache.set_ca	(_D1,4,J.x,J.y,J.w,J.z);
			J.set(21, 52, 33, 52);	J.sub(27); J.mul(scale); RCache.set_ca	(_D1,5,J.x,J.y,J.w,J.z);
			CHK_DX						(HW.pDevice->SetRenderState	( D3DRS_STENCILREF,		0x02	));
			RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
		}
	}
}

void CRenderTarget::accum_direct()
{
	u32		Offset;
	u32		C					= D3DCOLOR_RGBA	(255,255,255,255);
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
	L_clr.div					(RImplementation.Lights.sun_color, ps_r2_ls_dynamic_range);
	L_spec						= L_clr.magnitude()/_sqrt(3.f);
	Device.mView.transform_dir	(L_dir,RImplementation.Lights.sun_dir);
	L_dir.normalize				();
	RCache.set_c				("light_direction",	L_dir.x,L_dir.y,L_dir.z,0.f);
	RCache.set_c				("light_color",		L_clr.x,L_clr.y,L_clr.z,L_spec);

	// Render if stencil >= 0x2
	CHK_DX						(HW.pDevice->SetRenderState	( D3DRS_STENCILREF,		0x02	));
	RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
}
