// LightShadows.cpp: implementation of the CLightShadows class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LightShadows.h"
#include "xr_object.h"
#include "render.h"
#include "flightscontroller.h"
#include "xr_creator.h"

const	float	S_distance	= 32;
const	float	S_level		= .1f;
const	int		S_size		= 64;
const	int		S_rt_size	= 512;
const	int		S_polys		= 128;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLightShadows::CLightShadows()
{
	current	= 0;
	RT		= 0;
}

CLightShadows::~CLightShadows()
{
}

void CLightShadows::OnDeviceCreate	()
{
	LPCSTR	RTname	= "$user$shadow";
	
	// 
	RT			= Device.Shader._CreateRT	(RTname,S_rt_size,S_rt_size);
	sh_Texture	= Device.Shader.Create		("effects\\shadow_texture");
	sh_World	= Device.Shader.Create		("effects\\shadow_world",RTname);
	vs_World	= Device.Streams.Create		(FVF::F_L, S_polys*3);

	// Debug
	vs_Screen	= Device.Streams.Create		(FVF::F_TL,4);
	sh_Screen	= Device.Shader.Create		("effects\\screen_set",RTname);
}

void CLightShadows::OnDeviceDestroy	()
{
	// Debug
	Device.Shader.Delete					(sh_Screen	);
	
	// 
	Device.Shader.Delete					(sh_World	);
	Device.Shader.Delete					(sh_Texture	);
	Device.Shader._DeleteRT					(RT			);
}

void CLightShadows::set_object	(CObject* O)
{
	if (0==O)	current		= 0;
	else 
	{
		Fvector		C;
		O->clCenter	(C);
		float		D = C.distance_to(Device.vCameraPosition)+O->Radius();
		if (D < S_distance)		current	= O;
		else					current = 0;
		
		if (current)
		{
			id.push_back		(casters.size());
			casters.push_back	(caster());
			casters.back().O	= current;
			casters.back().C	= C;
			casters.back().D	= D;
		}
	}
}

void CLightShadows::add_element	(NODE* N)
{
	if (0==current)	return;
	casters.back().nodes.push_back	(*N);
}


//
class	pred_casters
{
	CLightShadows*		S;
public:
	pred_casters(CLightShadows* _S) : S(_S) {};
	IC bool operator () (int c1, int c2)
	{ return S->casters[c1].D<S->casters[c2].D; }
};
void CLightShadows::calculate	()
{
	if (id.empty())	return;

	Device.Shader.set_RT		(RT->pRT,0);
	Device.Statistic.TEST.Begin	();
	HW.pDevice->Clear			(0,0,D3DCLEAR_TARGET,D3DCOLOR_XRGB(0,0,0),1,0);
	
	// set shader
	Device.Shader.set_Shader	(sh_Texture);

	// sort by distance
	std::sort	(id.begin(),id.end(),pred_casters(this));
	
	// iterate on objects
	int	slot_id		= 0;
	int slot_line	= S_rt_size/S_size;
	int slot_max	= slot_line*slot_line;
	const float	eps = 2*EPS_L;
	for (int o_it=0; o_it<id.size(); o_it++)
	{
		caster&	C	= casters[id[o_it]];
		if (C.nodes.empty())	continue;
		
		// Select lights and calc importance
		lights.clear			();
		::Render->L_select		(C.C,C.O->Radius(),lights);
		
		// iterate on lights
		for (int l_it=0; (l_it<lights.size()) && (slot_id<slot_max); l_it++)
		{
			xrLIGHT*	L		=	lights[l_it];
			float	level		=	L->diffuse.magnitude_rgb	();
			if (level<S_level)	continue;
			
			// calculate projection-matrix
			Fmatrix		mProject;
			float		p_dist	=	C.C.distance_to(L->position);
			float		p_R		=	C.O->Radius();
			float		p_hat	=	p_R/p_dist;
			float		p_asp	=	1.f;
			float		p_near	=	p_dist-p_R-eps;	if (p_near<eps)			p_near	= eps;
			float		p_far	=	L->range;		if (p_far<(p_near+eps))	p_far	= p_near+eps;
			mProject.build_projection_HAT	(p_hat,p_asp,p_near,p_far);
			Device.set_xform_project		(mProject);
			
			// calculate view-matrix
			Fmatrix		mView;
			Fvector		v_D,v_N,v_R;
			v_D.sub					(C.C,L->position);;
			if(1-fabsf(v_D.y)<EPS)	v_N.set(1,0,0);
			else            		v_N.set(0,1,0);
			v_R.crossproduct		(v_N,v_D);
			v_N.crossproduct		(v_D,v_R);
			mView.build_camera		(L->position,C.C,v_N);
			Device.set_xform_view	(mView);
			
			// combine and build frustum
			Fmatrix		mCombine;
			mCombine.mul			(mProject,mView);
			
			// Select slot and set viewport
			int		s_x			=	slot_id%slot_line;
			int		s_y			=	slot_id/slot_line;
			D3DVIEWPORT8 VP		=	{s_x*S_size,s_y*S_size,S_size,S_size,0,1 };
			CHK_DX					(HW.pDevice->SetViewport(&VP));
			
			// Render object-parts
			for (int n_it=0; n_it<C.nodes.size(); n_it++)
			{
				NODE& N			=	C.nodes[n_it];
				CVisual *V		=	N.val.pVisual;
				Device.set_xform_world	(N.val.Matrix);
				V->Render				(.7f);
			}
			
			// register shadow and increment slot
			shadows.push_back	(shadow());
			shadows.back().slot	=	slot_id;
			shadows.back().M	=	mCombine;
			shadows.back().L	=	L;
			slot_id	++;
		}
	}
	casters.clear	();
	id.clear		();
	
	Device.Statistic.TEST.End	();

	Device.set_xform_project	(Device.mProject);
	Device.set_xform_view		(Device.mView);
}

void CLightShadows::render	()
{
	// Gain access to collision-DB
	CDB::MODEL*		DB		= pCreator->ObjectSpace.GetStaticModel();
	CDB::TRI*		TRIS	= DB->get_tris();
	
	// Render shadows
	Device.Shader.set_Shader(sh_World);
	for (int s_it=0; s_it<shadows.size(); s_it++)
	{
		shadow&		S			= shadows[s_it];
		
		// Frustum
		CFrustum	F;
		F.CreateFromMatrix		(S.M,FRUSTUM_P_ALL);

		// Query
		XRC.frustum_options		(0);
		XRC.frustum_query		(DB,F);
		if (0==XRC.r_count())	continue;

		// Clip polys by frustum
		for (CDB::RESULT* p = XRC.r_begin(); p!=XRC.r_end(); p++)
		{
			CDB::TRI&	t		= TRIS[p->id];
			sPoly		A,B;
			A.push_back			(*t.verts[0]);
			A.push_back			(*t.verts[1]);
			A.push_back			(*t.verts[2]);
			sPoly*		clip	= F.ClipPoly(A,B);
			if (0==clip)		continue;


		}

		
		// Rendering
		DWORD Offset, C=0;
		FVF::L* pv				= (FVF::L*) vs_World->Lock(XRC.r_count()*3,Offset);
		
		vs_World->Unlock		(XRC.r_count()*3);
	}
	shadows.clear	();
	
	// UV
	Fvector2		p0,p1;
	p0.set			(.5f/S_rt_size, .5f/S_rt_size);
	p1.set			((S_rt_size+.5f)/S_rt_size, (S_rt_size+.5f)/S_rt_size);
	
	// Fill vertex buffer
	DWORD Offset, C=0xffffffff;
	DWORD _w = S_rt_size/2, _h = S_rt_size/2;
	FVF::TL* pv = (FVF::TL*) vs_Screen->Lock(4,Offset);
	pv->set(0,			float(_h),	.0001f,.9999f, C, p0.x, p1.y);	pv++;
	pv->set(0,			0,			.0001f,.9999f, C, p0.x, p0.y);	pv++;
	pv->set(float(_w),	float(_h),	.0001f,.9999f, C, p1.x, p1.y);	pv++;
	pv->set(float(_w),	0,			.0001f,.9999f, C, p1.x, p0.y);	pv++;
	vs_Screen->Unlock			(4);
	
	// Actual rendering
	Device.Shader.set_Shader(sh_Screen);
	Device.Primitive.Draw	(vs_Screen,4,2,Offset,Device.Streams_QuadIB);
}
