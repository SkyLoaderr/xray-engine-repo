// LightProjector.cpp: implementation of the CLightProjector class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LightProjector.h"
#include "..\xr_object.h"
#include "lighttrack.h"

// tir2.xrdemo		-> 45.2
// tir2.xrdemo		-> 61.8

const	float		P_distance		= 48;					// switch distance between LOW-q light and HIGH-q light
const	float		P_distance2		= P_distance*P_distance;
const	float		P_cam_dist		= 200;
const	float		P_cam_range		= 7.f;
const	D3DFORMAT	P_rtf			= D3DFMT_A8R8G8B8;
const	float		P_blur_kernel	= .5f;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLightProjector::CLightProjector()
{
	current				= 0;
	RT					= 0;

	// 
	RT.create			("$user$projector",P_rt_size,P_rt_size,P_rtf);

	// ref-str for faster const-search
	c_xform				= "m_plmap_xform";
	c_clamp				= "m_plmap_clamp";
	c_factor			= "m_plmap_factor";

	cache.resize		(P_o_count);
}

CLightProjector::~CLightProjector()
{
	RT.destroy			();
}

void CLightProjector::set_object	(IRenderable* O)
{
	if ((0==O) || (receivers.size()>=P_o_count))	current		= 0;
	else
	{
		if (!O->renderable_ShadowReceive() || RImplementation.val_bInvisible || ((CLightTrack*)O->renderable.ROS)->Shadowed_dwFrame==Device.dwFrame)	
		{
			current		= 0;
			return;
		}

		Fvector		C;	O->renderable.xform.transform_tiny		(C,O->renderable.visual->vis.sphere.P);
		float		R	= O->renderable.visual->vis.sphere.R;
		float		D	= C.distance_to(Device.vCameraPosition)+R;
		if (D < P_distance)		current	= O;
		else					current = 0;
		
		if (current)				{
			CLightTrack*	LT		= (CLightTrack*)current->renderable.ROS;
			LT->Shadowed_dwFrame	= Device.dwFrame;
			receivers.push_back		(current);
		}
	}
}

// 
void CLightProjector::setup		(int id)
{
	if (id>=int(cache.size()) || id<0)	{
		Log		("! CLightProjector::setup - ID out of range");
		return;
	}
	recv&			R			= cache[id];
	float			dist		= R.C.distance_to	(Device.vCameraPosition)+R.O->renderable.visual->vis.sphere.R;
	float			factor		= _sqr(dist/P_distance);
	RCache.set_c	(c_xform,	R.UVgen);
	Fvector&	m	= R.UVclamp_min;
	RCache.set_ca	(c_clamp,	0,m.x,m.y,m.z,factor);
	Fvector&	M	= R.UVclamp_max;
	RCache.set_ca	(c_clamp,	1,M.x,M.y,M.z,0);
}

//
void CLightProjector::calculate	()
{
	if (receivers.empty())		return;

	// perform validate / markup
	for (u32 r_it=0; r_it<receivers.size(); r_it++)
	{
		// validate
		BOOL				bValid	= TRUE;
		IRenderable*		O		= receivers[r_it];
		CLightTrack*		LT		= (CLightTrack*)O->renderable.ROS;
		int					slot	= LT->Shadowed_Slot;
		if (slot<0 || slot>=P_o_count)		bValid = FALSE;	// invalid slot
		else if (cache[slot].O!=O)			bValid = FALSE;	// not the same object
		else {
			Fbox				bb;
			bb.xform			(O->renderable.visual->vis.box,O->renderable.xform);
			if (!cache[slot].BB.contains(bb))	bValid	= FALSE;
		}

		// 
		if (bValid)			{
			// Ok, use cached version
			cache[slot].dwFrame	= Device.dwFrame;
		} else {
			taskid.push_back	(r_it);
		}
	}
	if (taskid.empty())			return;

	// Begin
	Device.Statistic.RenderDUMP_Pcalc.Begin	();
	RCache.set_RT				(RT->pRT);
	RCache.set_ZB				(RImplementation.Target->pTempZB);
	CHK_DX(HW.pDevice->Clear	(0,0, D3DCLEAR_ZBUFFER | (HW.Caps.bStencil?D3DCLEAR_STENCIL:0), 0,1,0 ));
	RCache.set_xform_world		(Fidentity);

	// reallocate/reassociate structures + perform all the work
	for (u32 c_it=0; c_it<cache.size(); c_it++)
	{
		if (taskid.empty())							break;
		if (Device.dwFrame==cache[c_it].dwFrame)	continue;

		// found not used slot
		int				tid		= taskid.back();	taskid.pop_back();
		recv&			R		= cache		[c_it];
		IRenderable*	O		= receivers	[tid];
		CLightTrack*	LT		= (CLightTrack*)O->renderable.ROS;
		Fvector			C;		O->renderable.xform.transform_tiny		(C,O->renderable.visual->vis.sphere.P);
		R.O						= O;
		R.C						= C;
		R.BB.xform				(O->renderable.visual->vis.box,O->renderable.xform).scale(0.1f);
		LT->Shadowed_Slot		= c_it; 

		// Msg					("[%f,%f,%f]-%f",C.C.x,C.C.y,C.C.z,C.O->renderable.visual->vis.sphere.R);
		// calculate projection-matrix
		Fmatrix		mProject;
		float		p_R			=	R.O->renderable.visual->vis.sphere.R * 1.1f;
		float		p_hat		=	p_R/P_cam_dist;
		float		p_asp		=	1.f;
		float		p_near		=	P_cam_dist-EPS_L;									
		float		p_far		=	P_cam_dist+p_R+P_cam_range;	
		mProject.build_projection_HAT	(p_hat,p_asp,p_near,p_far);
		RCache.set_xform_project		(mProject);
		
		// calculate view-matrix
		Fmatrix		mView;
		Fvector		v_C, v_N;
		v_C.set					(R.C);
		v_C.y					+=	P_cam_dist;
		v_N.set					(0,0,1);
		mView.build_camera		(v_C,R.C,v_N);
		RCache.set_xform_view	(mView);

		// Select slot, set viewport
		int		s_x				=	c_it%P_o_line;
		int		s_y				=	c_it/P_o_line;
		D3DVIEWPORT9 VP			=	{s_x*P_o_size,s_y*P_o_size,P_o_size,P_o_size,0,1 };
		CHK_DX					(HW.pDevice->SetViewport(&VP));

		// Clear color to ambience
		float	c_a				=	LT->ambient;
		int		c_i				=	iFloor(c_a)/2;
		CHK_DX					(HW.pDevice->Clear(0,0, D3DCLEAR_TARGET, color_rgba(c_i,c_i,c_i,c_i), 1, 0 ));

		// calculate uv-gen matrix and clamper
		Fmatrix					mCombine;		mCombine.mul	(mProject,mView);
		Fmatrix					mTemp;
		float					fSlotSize		= float(P_o_size)/float(P_rt_size);
		float					fSlotX			= float(s_x*P_o_size)/float(P_rt_size);
		float					fSlotY			= float(s_y*P_o_size)/float(P_rt_size);
		float					fTexelOffs		= (.5f / P_rt_size);
		Fmatrix					m_TexelAdjust	= 
		{
			0.5f/*x-scale*/,	0.0f,							0.0f,				0.0f,
			0.0f,				-0.5f/*y-scale*/,				0.0f,				0.0f,
			0.0f,				0.0f,							1.0f/*z-range*/,	0.0f,
			0.5f/*x-bias*/,		0.5f + fTexelOffs/*y-bias*/,	0.0f/*z-bias*/,		1.0f
		};
		R.UVgen.mul				(m_TexelAdjust,mCombine);
		mTemp.scale				(fSlotSize,fSlotSize,1);
		R.UVgen.mulA			(mTemp);
		mTemp.translate			(fSlotX+fTexelOffs,fSlotY+fTexelOffs,0);
		R.UVgen.mulA			(mTemp);

		// Build bbox and render
		Fvector					min,max;
		Fbox					BB;
		min.set					(R.C.x-p_R,	R.C.y-(p_R+P_cam_range),	R.C.z-p_R);
		max.set					(R.C.x+p_R,	R.C.y+0,					R.C.z+p_R);
		BB.set					(min,max);
		R.UVclamp_min.set		(min);
		R.UVclamp_max.set		(max);
		ISpatial*	spatial		= dynamic_cast<ISpatial*>	(O);
		if (spatial)			RImplementation.r_dsgraph_render_R1_box		(spatial->spatial.sector,BB,SE_R1_LMODELS);
		//if (spatial)		RImplementation.r_dsgraph_render_subspace	(spatial->spatial.sector,mCombine,v_C,FALSE);
	}

	// Blur
	/*
	{
		// Fill vertex buffer
		u32							Offset;
		FVF::TL4uv* pv				= (FVF::TL4uv*) RCache.Vertex.Lock	(4,geom_Blur.stride(),Offset);
		RImplementation.ApplyBlur4	(pv,P_rt_size,P_rt_size,P_blur_kernel);
		RCache.Vertex.Unlock		(4,geom_Blur.stride());

		// Actual rendering (pass0, temp2real)
		RCache.set_RT				(RT->pRT);
		RCache.set_ZB				(NULL);
		RCache.set_Shader			(sh_BlurTR	);
		RCache.set_Geometry			(geom_Blur	);
		RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
	}
	*/

	// Finita la comedia
	Device.Statistic.RenderDUMP_Pcalc.End	();
	
	RCache.set_xform_project	(Device.mProject);
	RCache.set_xform_view		(Device.mView);
}

#ifdef DEBUG
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
#endif
