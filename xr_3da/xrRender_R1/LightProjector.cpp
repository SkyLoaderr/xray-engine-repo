// LightProjector.cpp: implementation of the CLightProjector class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LightProjector.h"
#include "..\xr_object.h"
#include "lighttrack.h"

const	int			P_rt_size	= 512;
const	int			P_o_size	= 56;
const	int			P_o_line	= P_rt_size/P_o_size;
const	int			P_o_count	= P_o_line*P_o_line;
const	float		P_distance	= 48;
const	float		P_distance2	= P_distance*P_distance;
const	float		P_cam_dist	= 200;
const	float		P_cam_range = 5.f;
const	D3DFORMAT	P_rtf		= D3DFMT_A8R8G8B8;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLightProjector::CLightProjector()
{
	current	= 0;
	RT		= 0;

	LPCSTR	RTname		= "$user$projector";
	LPCSTR	RTtemp		= "$user$temp";
	string128 RTname2;	strconcat(RTname2,RTname,",",RTname);
	string128 RTtemp2;	strconcat(RTtemp2,RTtemp,",",RTtemp);

	// 
	RT.create			(RTname,P_rt_size,P_rt_size,P_rtf);
	RT_temp.create		(RTtemp,P_rt_size,P_rt_size,P_rtf);
	sh_BlurTR.create	("effects\\blur",			RTtemp2);
	sh_BlurRT.create	("effects\\blur",			RTname2);
	geom_Blur.create	(FVF::F_TL2uv,	RCache.Vertex.Buffer(), RCache.QuadIB);

	// Debug
	sh_Screen.create	("effects\\screen_set",RTname);
	geom_Screen.create	(FVF::F_TL,		RCache.Vertex.Buffer(), RCache.QuadIB);
}

CLightProjector::~CLightProjector()
{
	// Debug
	sh_Screen.destroy	();
	geom_Screen.destroy	();

	// 
	sh_BlurRT.destroy	();
	sh_BlurTR.destroy	();
	geom_Screen.destroy	();
	RT_temp.destroy		();
	RT.destroy			();
}

void CLightProjector::set_object	(IRenderable* O)
{
	if ((0==O) || (receivers.size()>=P_o_count))	current		= 0;
	else  
	{
		if (!O->renderable_ShadowReceive())	
		{
			current		= 0;
			return;
		}

		Fvector		C;	O->renderable.xform.transform_tiny		(C,O->renderable.visual->vis.sphere.P);
		float		R	= O->renderable.visual->vis.sphere.R;
		float		D	= C.distance_to(Device.vCameraPosition)+R;
		if (D < P_distance)		current	= O;
		else					current = 0;
		
		if (current)
		{
			CLightTrack*	LT		= (CLightTrack*)current->renderable.ROS;
			LT->Shadowed_dwFrame	= Device.dwFrame;
			LT->Shadowed_Slot		= receivers.size();
			receivers.push_back		(recv());
			receivers.back().O		= current;
			receivers.back().C		= C;
			receivers.back().D		= D;
		}
	}
}

// 
void CLightProjector::setup		(int id)
{
	if (id>=int(receivers.size()))
	{
		Log		("! CLightProjector::setup - ID out of range");
		return;
	}
	RCache.set_c	("m_plmap_xform",receivers[id].UVgen);
}

//
void CLightProjector::calculate	()
{
	if (receivers.empty())		return;

	Device.Statistic.RenderDUMP_Pcalc.Begin	();
	RCache.set_RT				(RT_temp->pRT);
	RCache.set_ZB				(RImplementation.Target->pTempZB);
	CHK_DX(HW.pDevice->Clear	(0,0, D3DCLEAR_ZBUFFER | (HW.Caps.bStencil?D3DCLEAR_STENCIL:0), 0,1,0 ));
	RCache.set_xform_world		(Fidentity);

	Fmatrix	mInvView,mXform2UV,mTemp;
	float	t_s					= float(P_o_size)/float(P_rt_size);
	mInvView.invert				(Device.mView);
	mXform2UV.translate			(+1,		-1,			0);
	mTemp.scale					(.5f*t_s,	-.5f*t_s,	0);
	mXform2UV.mulA_43			(mTemp);
	
	// iterate on objects
	for (u32 o_it=0; o_it<receivers.size(); o_it++)
	{
		recv&	C	= receivers	[o_it];

		// calculate projection-matrix
		Fmatrix		mProject;
		float		p_R		=	C.O->renderable.visual->vis.sphere.R;
		float		p_hat	=	p_R/P_cam_dist;
		float		p_asp	=	1.f;
		float		p_near	=	P_cam_dist-EPS_L;									
		float		p_far	=	P_cam_dist+p_R+P_cam_range;	
		mProject.build_projection_HAT	(p_hat,p_asp,p_near,p_far);
		RCache.set_xform_project		(mProject);
		
		// calculate view-matrix
		Fmatrix		mView;
		Fvector		v_C, v_N;
		v_C.set					(C.C);
		v_C.y					+=	P_cam_dist;
		v_N.set					(0,0,1);
		mView.build_camera		(v_C,C.C,v_N);
		RCache.set_xform_view	(mView);

		// combine and build frustum
		Fmatrix		mCombine;	mCombine.mul	(mProject,mView);
		
		// Select slot, set viewport
		int		s_x			=	o_it%P_o_line;
		int		s_y			=	o_it/P_o_line;
		D3DVIEWPORT9 VP		=	{s_x*P_o_size,s_y*P_o_size,P_o_size,P_o_size,0,1 };
		CHK_DX					(HW.pDevice->SetViewport(&VP));

		// calculate uv-gen matrix
		C.UVgen.set				(mView);
		mTemp.scale				(1/p_R,1/p_R,0);
		C.UVgen.mulA_43			(mTemp);
		C.UVgen.mulA_43			(mXform2UV);
		mTemp.translate			(float(s_x*P_o_size)/float(P_rt_size), float(s_y*P_o_size)/float(P_rt_size), 0);
		C.UVgen.mulA_43			(mTemp);

		// Clear color to ambience
		float	c_a				=	((CLightTrack*)C.O->renderable.ROS)->ambient;
		int		c_i				=	iFloor(c_a)/2;
		CHK_DX					(HW.pDevice->Clear(0,0, D3DCLEAR_TARGET, color_rgba(c_i,c_i,c_i,c_i), 1, 0 ));
		
		// Build bbox and render
		Fvector	min,max;
		Fbox	BB;
		min.set					(C.C.x-p_R,	C.C.y-(p_R+P_cam_range),	C.C.z-p_R);
		max.set					(C.C.x+p_R,	C.C.y+0,					C.C.z+p_R);
		BB.set					(min,max);
		ISpatial*	spatial		= dynamic_cast<ISpatial*>(C.O);
		if (spatial)			RImplementation.RenderBox	(spatial->spatial.sector,BB,2);
	}

	// Blur
	{
		float						dim				= P_rt_size;
		Fvector2					shift,p0,p1,a0,a1,b0,b1,c0,c1,d0,d1;
		p0.set						(.5f/dim, .5f/dim);
		p1.set						((dim+.5f)/dim, (dim+.5f)/dim);
		shift.set(.5f/dim, .5f/dim); a0.add(p0,shift); a1.add(p1,shift); b0.sub(p0,shift); b1.sub(p1,shift);
		shift.set(.5f/dim,-.5f/dim); c0.add(p0,shift); c1.add(p1,shift); d0.sub(p0,shift); d1.sub(p1,shift);
		
		// Fill VB
		u32 C					=	0xffffffff, Offset;
		FVF::TL2uv* pv			=	(FVF::TL2uv*) RCache.Vertex.Lock	(8,geom_Blur->vb_stride,Offset);
		pv->set					(0.f,	dim,	C, a0.x, a1.y, b0.x, b1.y);	pv++;
		pv->set					(0.f,	0.f,	C, a0.x, a0.y, b0.x, b0.y);	pv++;
		pv->set					(dim,	dim,	C, a1.x, a1.y, b1.x, b1.y);	pv++;
		pv->set					(dim,	0.f,	C, a1.x, a0.y, b1.x, b0.y);	pv++;
		
		pv->set					(0.f,	dim,	C, c0.x, c1.y, d0.x, d1.y);	pv++;
		pv->set					(0.f,	0.f,	C, c0.x, c0.y, d0.x, d0.y);	pv++;
		pv->set					(dim,	dim,	C, c1.x, c1.y, d1.x, d1.y);	pv++;
		pv->set					(dim,	0.f,	C, c1.x, c0.y, d1.x, d0.y);	pv++;
		RCache.Vertex.Unlock	(8,geom_Blur->vb_stride);
		
		// Actual rendering (pass0, temp2real)
		RCache.set_RT			(RT->pRT);
		RCache.set_ZB			(NULL);
		RCache.set_Shader		(sh_BlurTR	);
		RCache.set_Geometry		(geom_Blur	);
		RCache.Render			(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
	}

	// Finita la comedia
	Device.Statistic.RenderDUMP_Pcalc.End	();
	
	RCache.set_xform_project	(Device.mProject);
	RCache.set_xform_view		(Device.mView);
}

void CLightProjector::render	()
{
	/*
	#define CLS(a) color_rgba(a,a,a,a)
	RCache.set_xform_world		(Fidentity);
	Device.Resources->OnFrameEnd	();
	for (u32 it=0; it<boxes.size(); it++)
	{
		Fvector C,D; boxes[it].get_CD	(C,D);
		RCache.dbg_DrawAABB	(C,D.x,D.y,D.z,0xffffffff);
	}
	boxes.clear();

	// Debug
	{
		// UV
		Fvector2				p0,p1;
		p0.set					(.5f/P_rt_size, .5f/P_rt_size);
		p1.set					((P_rt_size+.5f)/P_rt_size, (P_rt_size+.5f)/P_rt_size);
		
		// Fill vertex buffer
		u32 C			=	0xffffffff, Offset;
		u32 _w		=	P_rt_size/2, _h = P_rt_size/2;
		FVF::TL* pv		=	(FVF::TL*) geom_Screen->Lock(4,Offset);
		pv->set(0,			float(_h),	.0001f,.9999f, C, p0.x, p1.y);	pv++;
		pv->set(0,			0,			.0001f,.9999f, C, p0.x, p0.y);	pv++;
		pv->set(float(_w),	float(_h),	.0001f,.9999f, C, p1.x, p1.y);	pv++;
		pv->set(float(_w),	0,			.0001f,.9999f, C, p1.x, p0.y);	pv++;
		geom_Screen->Unlock			(4);
		
		// Actual rendering
		RCache.set_Shader(sh_Screen);
		RCache.Draw	(geom_Screen,4,2,Offset,Device.Streams_QuadIB);
	}
	*/
}
