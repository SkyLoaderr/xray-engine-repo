#include "stdafx.h"

void	CRenderTarget::phase_decompress		()
{
	// Common
	RCache.set_CullMode			(CULL_NONE);
	RCache.set_Stencil			(TRUE,D3DCMP_LESSEQUAL,0x01,0xff,0x00);

	Fvector2					p0,p1;
	u32		Offset;
	u32		C					= D3DCOLOR_RGBA	(255,255,255,255);
	float	_w					= float(Device.dwWidth);
	float	_h					= float(Device.dwHeight);
	float	d_Z					= EPS_S;
	float	d_W					= 1.f;
	p0.set						(.5f/_w, .5f/_h);
	p1.set						((_w+.5f)/_w, (_h+.5f)/_h );

	if (RImplementation.b_nv3x)
	{
		// Fill vertex buffer
		FVF::TL* pv					= (FVF::TL*) RCache.Vertex.Lock	(4,g_decompress->vb_stride,Offset);
		pv->set						(EPS,			float(_h+EPS),	d_Z,	d_W, C, p0.x, p1.y);	pv++;
		pv->set						(EPS,			EPS,			d_Z,	d_W, C, p0.x, p0.y);	pv++;
		pv->set						(float(_w+EPS),	float(_h+EPS),	d_Z,	d_W, C, p1.x, p1.y);	pv++;
		pv->set						(float(_w+EPS),	EPS,			d_Z,	d_W, C, p1.x, p0.y);	pv++;
		RCache.Vertex.Unlock		(4,g_decompress->vb_stride);
		RCache.set_Geometry			(g_decompress);

		// Position
		u_setrt						(rt_Position,NULL,NULL,HW.pBaseZB);
		RCache.set_Element			(s_decompress->E[0]);
		RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);

		// Normal
		u_setrt						(rt_Normal,NULL,NULL,HW.pBaseZB);
		RCache.set_Element			(s_decompress->E[1]);
		RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);

		// Color
		u_setrt						(rt_Color,NULL,NULL,HW.pBaseZB);
		RCache.set_Element			(s_decompress->E[2]);
		RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
	} else {
		// Msg						("----------------");
		struct v_dc		{
			Fvector4	p;
			Fvector2	uv0;
			Fvector4	uv1;
			void		set			(Fmatrix& mUnwarp, float x, float y, float u0, float v0, float u1, float v1)	{
				p.set	(x,y,EPS_S,1);
				uv0.set	(u0,v0);
				uv1.set	(u1*mUnwarp._11*VIEWPORT_NEAR,	v1*mUnwarp._22*VIEWPORT_NEAR,	VIEWPORT_NEAR,	0);
				// Msg		("[%+1.3f]-[%+1.3f] : dx(%f),dy(%f),dz(%f)",u1,v1,uv1.x,uv1.y,uv1.z);
			}                                                                                                                                                                                                           
		};
		VERIFY						(g_decompress->vb_stride == sizeof(v_dc));
		float			w1			= -1.f/Device.dwWidth;
		float			h1			= -1.f/Device.dwHeight;
		float			ws			= 1.0f;	//float(Device.dwWidth-1)	/float(Device.dwWidth);
		float			hs			= 1.0f;	//float(Device.dwHeight-1)	/float(Device.dwHeight);

		// Fill vertex buffer
		Fmatrix						mUnwarp;
		D3DXMatrixInverse			((D3DXMATRIX*)&mUnwarp,0,(D3DXMATRIX*)&Device.mProject);
		// Msg						("w=%f,h=%f",mUnwarp._11*VIEWPORT_NEAR,mUnwarp._22*VIEWPORT_NEAR);
		v_dc* pv					= (v_dc*) RCache.Vertex.Lock	(4,g_decompress->vb_stride,Offset);
		pv->set						(mUnwarp,	EPS_S,				float(_h+EPS_S),	p0.x, p1.y, -1*ws+w1,	-1*hs+h1);	pv++;
		pv->set						(mUnwarp,	EPS_S,				EPS_S,				p0.x, p0.y, -1*ws+w1,	+1*hs+h1);	pv++;
		pv->set						(mUnwarp,	float(_w+EPS_S),	float(_h+EPS_S),	p1.x, p1.y, +1*ws+w1,	-1*hs+h1);	pv++;
		pv->set						(mUnwarp,	float(_w+EPS_S),	EPS_S,				p1.x, p0.y, +1*ws+w1,	+1*hs+h1);	pv++;
		RCache.Vertex.Unlock		(4,g_decompress->vb_stride);
		RCache.set_Geometry			(g_decompress);
		
		/*
		Fvector			test;	
		test.set		(-0.589866,0.491592,0.640621);	test.mul(VIEWPORT_NEAR/test.z);			
		Msg				("[0,0]      : dx(%f),dy(%f),dz(%f)",test.x,test.y,test.z);
		test.set		(0.589143,0.491897,0.641052);	test.mul(VIEWPORT_NEAR/test.z);
		Msg				("[1023,0]   : dx(%f),dy(%f),dz(%f)",test.x,test.y,test.z);
		test.set		(-0.590263,-0.490583,0.641029);	test.mul(VIEWPORT_NEAR/test.z);
		Msg				("[0,767]    : dx(%f),dy(%f),dz(%f)",test.x,test.y,test.z);
		test.set		(0.589506,-0.490919,0.641468);	test.mul(VIEWPORT_NEAR/test.z);
		Msg				("[1023,767] : dx(%f),dy(%f),dz(%f)",test.x,test.y,test.z);
		*/

		// Decompress
		u_setrt						(rt_Position,rt_Normal,NULL,HW.pBaseZB);
		RCache.set_Shader			(s_decompress);
		RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
	}
}
