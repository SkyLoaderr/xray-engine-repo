// LightProjector.cpp: implementation of the CLightProjector class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LightProjector.h"
#include "xr_object.h"
#include "render.h"
#include "lighttrack.h"

const	int		P_rt_size	= 512;
const	int		P_o_size	= 61;
const	int		P_o_line	= P_rt_size/P_o_size;
const	int		P_o_count	= P_o_line*P_o_line;
const	float	P_distance	= 48;
const	float	P_distance2	= P_distance*P_distance;
const	float	P_cam_dist	= 200;
const	float	P_cam_range = 5.f;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLightProjector::CLightProjector()
{
	current	= 0;
	RT		= 0;
}

CLightProjector::~CLightProjector()
{
}

void CLightProjector::OnDeviceCreate	()
{
	LPCSTR	RTname		= "$user$projector";
	LPCSTR	RTtemp		= "$user$temp";
	string128 RTname2;	strconcat(RTname2,RTname,",",RTname);
	string128 RTtemp2;	strconcat(RTtemp2,RTtemp,",",RTtemp);
	
	// 
	RT			= Device.Shader._CreateRT	(RTname,P_rt_size,P_rt_size);
	RT_temp		= Device.Shader._CreateRT	(RTtemp,P_rt_size,P_rt_size);
	sh_BlurTR	= Device.Shader.Create		("effects\\blur",			RTtemp2);
	sh_BlurRT	= Device.Shader.Create		("effects\\blur",			RTname2);
	vs_Blur		= Device.Streams.Create		(FVF::F_TL2uv, 8);

	// Debug
	sh_Screen	= Device.Shader.Create		("effects\\screen_set",RTname);
	vs_Screen	= Device.Streams.Create		(FVF::F_TL,4);
}

void CLightProjector::OnDeviceDestroy	()
{
	// Debug
	Device.Shader.Delete					(sh_Screen	);
	
	// 
	Device.Shader.Delete					(sh_BlurRT	);
	Device.Shader.Delete					(sh_BlurTR	);
	Device.Shader._DeleteRT					(RT_temp	);
	Device.Shader._DeleteRT					(RT			);
}

void CLightProjector::set_object	(CObject* O)
{
	if ((0==O) || (receivers.size()>=P_o_count))	current		= 0;
	else 
	{
		Fvector		C;
		O->clCenter	(C);
		float		D = C.distance_to(Device.vCameraPosition)+O->Radius();
		if (D < P_distance)		current	= O;
		else					current = 0;
		
		if (current)
		{
			current->Lights()->Shadowed_dwFrame	= Device.dwFrame;
			current->Lights()->Shadowed_Slot	= receivers.size();
			receivers.push_back			(recv());
			receivers.back().O			= current;
			receivers.back().C			= C;
			receivers.back().D			= D;
		}
	}
}
/*
class mat_tex2d : public D3DXMATRIX
{
public:
	mat_tex2d(const D3DMATRIX &rhs)
		: D3DXMATRIX(rhs._11, rhs._12, rhs._14, 0,
		rhs._21, rhs._22, rhs._24, 0,
		rhs._41, rhs._42, rhs._44, 0,
		0, 0, 0, 1)
	{}
	~mat_tex2d() {}
};

class mat_tex1d : public D3DXMATRIX
{
public:
	mat_tex1d(const D3DXMATRIX &rhs)
		: D3DXMATRIX(rhs._11, rhs._14, 0, 0,
		rhs._41, rhs._44, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1)
	{}
	~mat_tex1d() {}
};
*/

// 
void CLightProjector::setup		(int id)
{
	VERIFY				(id<int(receivers.size()));
	Device.set_xform	(D3DTS_TEXTURE0,receivers[id].UVgen);
}

//
void CLightProjector::calculate	()
{
	if (receivers.empty())		return;
	
	Device.Statistic.RenderDUMP_Pcalc.Begin	();
	Device.Shader.set_RT		(RT_temp->pRT,HW.pTempZB);
	CHK_DX(HW.pDevice->Clear	(0,0, D3DCLEAR_ZBUFFER | (HW.Caps.bStencil?D3DCLEAR_STENCIL:0), 0,1,0 ));
	Device.set_xform_world		(Fidentity);

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
		float		p_R		=	C.O->Radius();
		float		p_hat	=	p_R/P_cam_dist;
		float		p_asp	=	1.f;
		float		p_near	=	P_cam_dist-EPS_L;									
		float		p_far	=	P_cam_dist+p_R+P_cam_range;	
		mProject.build_projection_HAT	(p_hat,p_asp,p_near,p_far);
		Device.set_xform_project		(mProject);
		
		// calculate view-matrix
		Fmatrix		mView;
		Fvector		v_C, v_N;
		v_C.set					(C.C);
		v_C.y					+=	P_cam_dist;
		v_N.set					(0,0,1);
		mView.build_camera		(v_C,C.C,v_N);
		Device.set_xform_view	(mView);

		// combine and build frustum
		Fmatrix		mCombine;
		mCombine.mul			(mProject,mView);
		
		// Select slot, set viewport
		int		s_x			=	o_it%P_o_line;
		int		s_y			=	o_it/P_o_line;
		D3DVIEWPORT8 VP		=	{s_x*P_o_size,s_y*P_o_size,P_o_size,P_o_size,0,1 };
		CHK_DX					(HW.pDevice->SetViewport(&VP));

		// calculate uv-gen matrix
		Msg	("*** %s: [%d][%d]",C.O->cName(),s_x,s_y);
		C.UVgen.mul_43			(mView,mInvView);
		C.UVgen.mulA_43			(mXform2UV);
		mTemp.translate			(float(s_x*P_o_size)/float(P_rt_size), float(s_y*P_o_size)/float(P_rt_size), 0);
		C.UVgen.mulA_43			(mTemp);

		// Clear color to ambience
		float	c_a			=	C.O->Lights()->ambient;
		int		c_i			=	iFloor(c_a)/2;
		CHK_DX					(HW.pDevice->Clear(0,0, D3DCLEAR_TARGET, D3DCOLOR_RGBA(c_i,c_i,c_i,c_i), 1, 0 ));
		
		// Build bbox and render
		Fvector	min,max;
		Fbox	BB;
		min.set					(C.C.x-p_R,	C.C.y-(p_R+P_cam_range),	C.C.z-p_R);
		max.set					(C.C.x+p_R,	C.C.y+0,					C.C.z+p_R);
		BB.set					(min,max);
		::Render->RenderBox		(C.O->Sector(),BB,2);
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
		DWORD C						=	0xffffffff, Offset;
		FVF::TL2uv* pv				=	(FVF::TL2uv*) vs_Blur->Lock(8,Offset);
		pv->set						(0.f,	dim,	C, a0.x, a1.y, b0.x, b1.y);	pv++;
		pv->set						(0.f,	0.f,	C, a0.x, a0.y, b0.x, b0.y);	pv++;
		pv->set						(dim,	dim,	C, a1.x, a1.y, b1.x, b1.y);	pv++;
		pv->set						(dim,	0.f,	C, a1.x, a0.y, b1.x, b0.y);	pv++;
		
		pv->set						(0.f,	dim,	C, c0.x, c1.y, d0.x, d1.y);	pv++;
		pv->set						(0.f,	0.f,	C, c0.x, c0.y, d0.x, d0.y);	pv++;
		pv->set						(dim,	dim,	C, c1.x, c1.y, d1.x, d1.y);	pv++;
		pv->set						(dim,	0.f,	C, c1.x, c0.y, d1.x, d0.y);	pv++;
		vs_Blur->Unlock				(8);
		
		// Actual rendering (pass0, temp2real)
		Device.Shader.set_RT		(RT->pRT,	0);
		Device.Shader.set_Shader	(sh_BlurTR	);
		Device.Primitive.Draw		(vs_Blur,	4, 2, Offset,	Device.Streams_QuadIB);
		
/*
		for (int it=0; it<1; it++)	
		{
			// Actual rendering (pass1, real2temp)
			Device.Shader.set_RT		(RT_temp->pRT,	0);
			Device.Shader.set_Shader	(sh_BlurRT	);
			Device.Primitive.Draw		(vs_Blur,	4, 2, Offset+4,	Device.Streams_QuadIB);
			
			// Actual rendering (pass2, temp2real)
			Device.Shader.set_RT		(RT->pRT,	0);
			Device.Shader.set_Shader	(sh_BlurTR	);
			Device.Primitive.Draw		(vs_Blur,	4, 2, Offset,	Device.Streams_QuadIB);
		}
*/
	}
	
	// Finita la comedia
	Device.Statistic.RenderDUMP_Pcalc.End	();
	
	Device.set_xform_project	(Device.mProject);
	Device.set_xform_view		(Device.mView);
}

#define CLS(a) D3DCOLOR_RGBA(a,a,a,a)
void CLightProjector::render	()
{
/*
	Device.set_xform_world		(Fidentity);
	Device.Shader.OnFrameEnd	();
	for (u32 it=0; it<boxes.size(); it++)
	{
		Fvector C,D; boxes[it].get_CD	(C,D);
		Device.Primitive.dbg_DrawAABB	(C,D.x,D.y,D.z,0xffffffff);
	}
	boxes.clear();
*/

	// Debug
	{
		// UV
		Fvector2				p0,p1;
		p0.set					(.5f/P_rt_size, .5f/P_rt_size);
		p1.set					((P_rt_size+.5f)/P_rt_size, (P_rt_size+.5f)/P_rt_size);
		
		// Fill vertex buffer
		DWORD C			=	0xffffffff, Offset;
		DWORD _w	=	P_rt_size/2, _h = P_rt_size/2;
		FVF::TL* pv =	(FVF::TL*) vs_Screen->Lock(4,Offset);
		pv->set(0,			float(_h),	.0001f,.9999f, C, p0.x, p1.y);	pv++;
		pv->set(0,			0,			.0001f,.9999f, C, p0.x, p0.y);	pv++;
		pv->set(float(_w),	float(_h),	.0001f,.9999f, C, p1.x, p1.y);	pv++;
		pv->set(float(_w),	0,			.0001f,.9999f, C, p1.x, p0.y);	pv++;
		vs_Screen->Unlock			(4);
		
		// Actual rendering
		Device.Shader.set_Shader(sh_Screen);
		Device.Primitive.Draw	(vs_Screen,4,2,Offset,Device.Streams_QuadIB);
	}
}
