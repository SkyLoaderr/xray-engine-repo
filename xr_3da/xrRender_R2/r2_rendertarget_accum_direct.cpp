#include "stdafx.h"

void CRenderTarget::accum_direct	()
{
	// *** assume accumulator setted up ***

	// texture adjustment matrix
	float			fTexelOffs			= (.5f / DSM_size);
	Fmatrix			m_TexelAdjust		= 
	{
		0.5f,				0.0f,				0.0f,			0.0f,
		0.0f,				-0.5f,				0.0f,			0.0f,
		0.0f,				0.0f,				1.0f,			0.0f,
		0.5f + fTexelOffs,	0.5f + fTexelOffs,	0.0f - .004f,	1.0f
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

		// Fill vertex buffer
		FVF::TL* pv					= (FVF::TL*) RCache.Vertex.Lock	(4,g_combine->vb_stride,Offset);
		pv->set						(EPS,			float(_h+EPS),	EPS,	1.f, C, p0.x, p1.y);	pv++;
		pv->set						(EPS,			EPS,			EPS,	1.f, C, p0.x, p0.y);	pv++;
		pv->set						(float(_w+EPS),	float(_h+EPS),	EPS,	1.f, C, p1.x, p1.y);	pv++;
		pv->set						(float(_w+EPS),	EPS,			EPS,	1.f, C, p1.x, p0.y);	pv++;
		RCache.Vertex.Unlock		(4,g_combine->vb_stride);

		// Shader + constants
		RCache.set_Shader			(s_accum_direct);
		Fvector4 J; float scale		= (3.f / DSM_size)/11.f;
		R_constant* C				= RCache.set_c			("jitter");
		J.set(11, 0,  0);		J.sub(11); J.mul(scale);	RCache.set_ca	(C,0,J.x,J.y,-J.y,-J.x);
		J.set(19, 3,  0);		J.sub(11); J.mul(scale);	RCache.set_ca	(C,1,J.x,J.y,-J.y,-J.x);
		J.set(22, 11, 0);		J.sub(11); J.mul(scale);	RCache.set_ca	(C,2,J.x,J.y,-J.y,-J.x);
		J.set(19, 19, 0);		J.sub(11); J.mul(scale);	RCache.set_ca	(C,3,J.x,J.y,-J.y,-J.x);
		J.set(9,  7,  15, 9);	J.sub(11); J.mul(scale);	RCache.set_ca	(C,4,J.x,J.y,J.w,J.z);
		J.set(13, 15, 7,  13);	J.sub(11); J.mul(scale);	RCache.set_ca	(C,5,J.x,J.y,J.w,J.z);

		// Render
		RCache.set_Geometry			(g_combine);
		RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
	}
}
