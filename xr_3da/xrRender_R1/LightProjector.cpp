// LightProjector.cpp: implementation of the CLightProjector class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LightProjector.h"
#include "..\xr_object.h"
#include "lighttrack.h"

const	int			P_rt_size	= 512;
const	int			P_o_size	= 51;
const	int			P_o_line	= P_rt_size/P_o_size;
const	int			P_o_count	= P_o_line*P_o_line;
const	float		P_distance	= 48;					// switch distance between LOW-q light and HIGH-q light
const	float		P_distance2	= P_distance*P_distance;
const	float		P_cam_dist	= 200;
const	float		P_cam_range = 7.f;
const	D3DFORMAT	P_rtf		= D3DFMT_A8R8G8B8;
const	float		P_blur_kernel	= .5f;

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
	sh_BlurTR.create	("blur4",		RTtemp2);
	sh_BlurRT.create	("blur4",		RTname2);
	geom_Blur.create	(FVF::F_TL4uv,	RCache.Vertex.Buffer(), RCache.QuadIB);

	// Debug
	sh_Screen.create	("effects\\screen_set",RTname);
	geom_Screen.create	(FVF::F_TL,		RCache.Vertex.Buffer(), RCache.QuadIB);

	// ref-str for faster const-search
	c_xform				= "m_plmap_xform";
	c_clamp				= "m_plmap_clamp";
	c_factor			= "m_plmap_factor";
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
		if (!O->renderable_ShadowReceive() || RImplementation.val_bInvisible )	
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
	float			dist		= receivers[id].C.distance_to	(Device.vCameraPosition)+receivers[id].O->renderable.visual->vis.sphere.R;
	float			factor		= _sqr(dist/P_distance);
	RCache.set_c	(c_xform,	receivers[id].UVgen);
	RCache.set_c	(c_factor,	factor,factor,factor,factor);
	RCache.set_ca	(c_clamp,	0,receivers[id].UVclamp_min);
	RCache.set_ca	(c_clamp,	1,receivers[id].UVclamp_max);
}

/*
static Fvector down_vec ={0.f,-1.f,0.f};
static Fvector left_vec ={-1.f,0.f,0.f};
static Fvector right_vec={1.f,0.f,0.f};
static Fvector fwd_vec  ={0.f,0.f,1.f};
static Fvector back_vec ={0.f,0.f,-1.f};

void EDetailManager::UpdateSlotBBox(int sx, int sz, DetailSlot& slot)
{
        Fbox bbox;
    Frect rect;
    GetSlotRect                 (rect,sx,sz);
    bbox.min.set                (rect.x1, m_BBox.min.y, rect.y1);
    bbox.max.set                (rect.x2, m_BBox.max.y, rect.y2);

    SBoxPickInfoVec pinf;
    XRC.box_options(0);
    if (Scene.BoxPickObjects(bbox,pinf,&m_SnapObjects)){
                bbox.grow               (EPS_L_VAR);
        Fplane                  frustum_planes[4];
                frustum_planes[0].build(bbox.min,left_vec);
                frustum_planes[1].build(bbox.min,back_vec);
                frustum_planes[2].build(bbox.max,right_vec);
                frustum_planes[3].build(bbox.max,fwd_vec);

        CFrustum frustum;
        frustum.CreateFromPlanes(frustum_planes,4);

        float y_min             = flt_max;
        float y_max             = flt_min;
                for (SBoxPickInfoIt it=pinf.begin(); it!=pinf.end(); it++){
                for (int k=0; k<(int)it->inf.size(); k++){
                float range;
                Fvector verts[3];
                it->s_obj->GetFaceWorld(it->e_mesh,it->inf[k].id,verts);
                sPoly sSrc      (verts,3);
                sPoly sDest;
                sPoly* sRes = frustum.ClipPoly(sSrc, sDest);
                if (sRes){
                    for (u32 k=0; k<sRes->size(); k++){
                        float H = (*sRes)[k].y;
                        if (H>y_max) y_max = H+0.03f;
                        if (H<y_min) y_min = H-0.03f;
                    }
                    slot.w_y    (y_min,y_max-y_min);
                }
            }
            }
    }else{
        ZeroMemory(&slot,sizeof(DetailSlot));
        slot.w_id(0,DetailSlot::ID_Empty);
        slot.w_id(1,DetailSlot::ID_Empty);
        slot.w_id(2,DetailSlot::ID_Empty);
        slot.w_id(3,DetailSlot::ID_Empty);
    }
}
*/

//
void CLightProjector::calculate	()
{
	if (receivers.empty())		return;

	Device.Statistic.RenderDUMP_Pcalc.Begin	();
	RCache.set_RT				(RT_temp->pRT);
	RCache.set_ZB				(RImplementation.Target->pTempZB);
	CHK_DX(HW.pDevice->Clear	(0,0, D3DCLEAR_ZBUFFER | (HW.Caps.bStencil?D3DCLEAR_STENCIL:0), 0,1,0 ));
	RCache.set_xform_world		(Fidentity);

	// iterate on objects
	// Msg	("---------------------------------");
	for (u32 o_it=0; o_it<receivers.size(); o_it++)
	{
		recv&	C	= receivers	[o_it];

		// Msg					("[%f,%f,%f]-%f",C.C.x,C.C.y,C.C.z,C.O->renderable.visual->vis.sphere.R);
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

		// Select slot, set viewport
		int		s_x			=	o_it%P_o_line;
		int		s_y			=	o_it/P_o_line;
		D3DVIEWPORT9 VP		=	{s_x*P_o_size,s_y*P_o_size,P_o_size,P_o_size,0,1 };
		CHK_DX					(HW.pDevice->SetViewport(&VP));

		// calculate uv-gen matrix and clamper
		Fmatrix				mCombine;		mCombine.mul	(mProject,mView);
		Fmatrix				mTemp;
		float				fSlotSize		= float(P_o_size)/float(P_rt_size);
		float				fSlotX			= float(s_x*P_o_size)/float(P_rt_size);
		float				fSlotY			= float(s_y*P_o_size)/float(P_rt_size);
		float				fTexelOffs		= (.5f / P_rt_size);
		Fmatrix				m_TexelAdjust	= 
		{
			0.5f/*x-scale*/,	0.0f,							0.0f,				0.0f,
			0.0f,				-0.5f/*y-scale*/,				0.0f,				0.0f,
			0.0f,				0.0f,							1.0f/*z-range*/,	0.0f,
			0.5f/*x-bias*/,		0.5f + fTexelOffs/*y-bias*/,	0.0f/*z-bias*/,		1.0f
		};
		C.UVgen.mul			(m_TexelAdjust,mCombine);
		mTemp.scale			(fSlotSize,fSlotSize,1);
		C.UVgen.mulA		(mTemp);
		mTemp.translate		(fSlotX+fTexelOffs,fSlotY+fTexelOffs,0);
		C.UVgen.mulA		(mTemp);

		// Clear color to ambience
		/*
		float	c_a			=	((CLightTrack*)C.O->renderable.ROS)->ambient;
		int		c_i			=	iFloor(c_a)/2;
		CHK_DX				(HW.pDevice->Clear(0,0, D3DCLEAR_TARGET, color_rgba(c_i,c_i,c_i,c_i), 1, 0 ));
		*/
		
		// Build bbox and render
		Fvector				min,max;
		Fbox				BB;
		min.set				(C.C.x-p_R,	C.C.y-(p_R+P_cam_range),	C.C.z-p_R);
		max.set				(C.C.x+p_R,	C.C.y+0,					C.C.z+p_R);
		BB.set				(min,max);
		C.UVclamp_min.set	(min);
		C.UVclamp_max.set	(max);
		ISpatial*	spatial	= dynamic_cast<ISpatial*>	(C.O);
		if (spatial)		RImplementation.r_dsgraph_render_R1_box		(spatial->spatial.sector,BB,SE_R1_LMODELS);
		//if (spatial)		RImplementation.r_dsgraph_render_subspace	(spatial->spatial.sector,mCombine,v_C,FALSE);
	}

	// Blur
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
