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
const	int		batch_size	= 128;

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
	LPCSTR	RTtemp	= "$user$temp";
	
	// 
	RT			= Device.Shader._CreateRT	(RTname,S_rt_size,S_rt_size);
	RT_temp		= Device.Shader._CreateRT	(RTtemp,S_rt_size,S_rt_size);
	sh_Texture	= Device.Shader.Create		("effects\\shadow_texture");
	sh_World	= Device.Shader.Create		("effects\\shadow_world",	RTname);
	sh_Blur		= Device.Shader.Create		("effects\\shadow_blur",	RTtemp);
	vs_World	= Device.Streams.Create		(FVF::F_LIT, 4*batch_size*3);

	// Debug
	vs_Screen	= Device.Streams.Create		(FVF::F_TL,4);
	sh_Screen	= Device.Shader.Create		("effects\\screen_set",RTname);
}

void CLightShadows::OnDeviceDestroy	()
{
	// Debug
	Device.Shader.Delete					(sh_Screen	);
	
	// 
	Device.Shader.Delete					(sh_Blur	);
	Device.Shader.Delete					(sh_World	);
	Device.Shader.Delete					(sh_Texture	);
	Device.Shader._DeleteRT					(RT_temp	);
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


IC float PLC_energy	(Fvector& P, Fvector& N, xrLIGHT* L, float E)
{
	Fvector Ldir;
	if (L->type==D3DLIGHT_DIRECTIONAL)
	{
		// Cos
		Ldir.invert	(L->direction);
		float D		= Ldir.dotproduct( N );
		if( D <=0 )						return 0;
		
		// Trace Light
		float A		= D*E;
		return A;
	} else {
		// Distance
		float sqD	= P.distance_to_sqr(L->position);
		if (sqD > (L->range*L->range))	return 0;
		
		// Dir
		Ldir.sub	(L->position,P);
		Ldir.normalize_safe();
		float D		= Ldir.dotproduct( N );
		if( D <=0 )						return 0;
		
		// Trace Light
		float R		= sqrtf(sqD);
		float A		= D*E / (L->attenuation0 + L->attenuation1*R + L->attenuation2*sqD);
		return A;
	}
}

IC DWORD PLC_calc	(Fvector& P, Fvector& N, xrLIGHT* L, float E)
{
	float	A		= 1.f-.5f*PLC_energy(P,N,L,E);
	int		iA		= iCeil(255.f*A);
	clamp	(iA,0,255);
	return	D3DCOLOR_RGBA	(iA,iA,iA,iA);
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

	Device.Statistic.TEST.Begin	();
	Device.Shader.set_RT		(RT_temp->pRT,0);
	HW.pDevice->Clear			(0,0,D3DCLEAR_TARGET,D3DCOLOR_XRGB(255,255,255),1,0);
	
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
			
			// calculate light center
			Fvector		Lpos	= L->position;
			float		Lrange	= L->range;
			if (L->type==D3DLIGHT_DIRECTIONAL)
			{
				Lpos.mul	(L->direction,-100);
				Lpos.add	(C.C);
				Lrange		= 120;
			}
			
			// calculate projection-matrix
			Fmatrix		mProject;
			float		p_dist	=	C.C.distance_to(Lpos);
			float		p_R		=	C.O->Radius();
			float		p_hat	=	p_R/p_dist;
			float		p_asp	=	1.f;
			float		p_near	=	p_dist-p_R-eps;	if (p_near<eps)			p_near	= eps;
			float		p_far	=	Lrange;			if (p_far<(p_near+eps))	p_far	= p_near+eps;
			mProject.build_projection_HAT	(p_hat,p_asp,p_near,p_far);
			Device.set_xform_project		(mProject);
			
			// calculate view-matrix
			Fmatrix		mView;
			Fvector		v_D,v_N,v_R;
			v_D.sub					(C.C,Lpos);;
			if(1-fabsf(v_D.y)<EPS)	v_N.set(1,0,0);
			else            		v_N.set(0,1,0);
			v_R.crossproduct		(v_N,v_D);
			v_N.crossproduct		(v_D,v_R);
			mView.build_camera		(Lpos,C.C,v_N);
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
				V->Render				(1.0f);
			}
			
			// register shadow and increment slot
			Fvector N; N.set	(0,1,0);
			shadows.push_back	(shadow());
			shadows.back().slot	=	slot_id;
			shadows.back().M	=	mCombine;
			shadows.back().L	=	L;
			shadows.back().C	=	PLC_calc(C.C,N,L,L->diffuse.magnitude_rgb());
			slot_id	++;
		}
	}
	casters.clear	();
	id.clear		();
	
	// Blur
	{
		float						dim				= S_rt_size;
		Fvector2					shift,p0,p1;
		shift.set					(.5f/dim, .5f/dim);
		p0.set						(.5f/dim, .5f/dim);
		p1.set						((dim+.5f)/dim, (dim+.5f)/dim);
		p0.add						(shift);
		p1.add						(shift);
		
		// Fill vertex buffer
		DWORD C						=	0xffffffff, Offset;
		FVF::TL* pv					=	(FVF::TL*) vs_Screen->Lock(4,Offset);
		pv->set						(0,		dim,	.0001f,.9999f, C, p0.x, p1.y);	pv++;
		pv->set						(0,		0,		.0001f,.9999f, C, p0.x, p0.y);	pv++;
		pv->set						(dim,	dim,	.0001f,.9999f, C, p1.x, p1.y);	pv++;
		pv->set						(dim,	0,		.0001f,.9999f, C, p1.x, p0.y);	pv++;
		vs_Screen->Unlock			(4);
		
		// Actual rendering
		Device.Shader.set_RT		(RT->pRT,	0);
		Device.Shader.set_Shader	(sh_Blur	);
		Device.Primitive.Draw		(vs_Screen, 4,2,Offset,Device.Streams_QuadIB);
	}
	
	// Finita la comedia
	Device.Statistic.TEST.End	();
	
	Device.set_xform_project	(Device.mProject);
	Device.set_xform_view		(Device.mView);
}

void CLightShadows::render	()
{
	// Gain access to collision-DB
	CDB::MODEL*		DB		= pCreator->ObjectSpace.GetStaticModel();
	CDB::TRI*		TRIS	= DB->get_tris();

	int slot_line	= S_rt_size/S_size;
	int slot_max	= slot_line*slot_line;
	
	// Projection and xform
	float _43			= Device.mProject._43;
	Device.mProject._43 -= 0.001f; 
	Device.set_xform_world	(Fidentity);
	Device.set_xform_project(Device.mProject);
	Fvector	View		= Device.vCameraPosition;
	
	// Render shadows
	Device.Shader.set_Shader	(sh_World);
	int batch					= 0;
	DWORD Offset				= 0;
	DWORD C						= 0x7f7f7f7f;
	FVF::LIT* pv				= (FVF::LIT*) vs_World->Lock(batch_size*3,Offset);
	for (int s_it=0; s_it<shadows.size(); s_it++)
	{
		shadow&		S			=	shadows[s_it];
		int			s_x			=	S.slot%slot_line;
		int			s_y			=	S.slot/slot_line;
		Fvector2	t_scale, t_offset;
		t_scale.set	(float(S_size)/float(S_rt_size),float(S_size)/float(S_rt_size));
		t_scale.mul (.5f);
		t_offset.set(float(s_x)/float(slot_line),float(s_y)/float(slot_line));
		t_offset.x	+= .5f/S_rt_size;
		t_offset.y	+= .5f/S_rt_size;
		
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
			// 
			CDB::TRI&	t		= TRIS[p->id];
			sPoly		A,B;
			A.push_back			(*t.verts[0]);
			A.push_back			(*t.verts[1]);
			A.push_back			(*t.verts[2]);

			// Calc plane
			Fplane		P;
			P.build				(A[0],A[1],A[2]);
			if (P.classify(View)<0)				continue;
			if (P.classify(S.L->position)<0)	continue;
			
			// Clip polygon
			sPoly*		clip	= F.ClipPoly(A,B);
			if (0==clip)		continue;
			
			// Triangulate poly 
			for (int v=2; v<clip->size(); v++)
			{
				Fvector& v1		= (*clip)[0];
				Fvector& v2		= (*clip)[v-1];
				Fvector& v3		= (*clip)[v];
				Fvector		T;
				
				S.M.transform(T,v1); pv->set(v1,S.C,(T.x+1)*t_scale.x+t_offset.x,(1-T.y)*t_scale.y+t_offset.y); pv++;
				S.M.transform(T,v2); pv->set(v2,S.C,(T.x+1)*t_scale.x+t_offset.x,(1-T.y)*t_scale.y+t_offset.y); pv++;
				S.M.transform(T,v3); pv->set(v3,S.C,(T.x+1)*t_scale.x+t_offset.x,(1-T.y)*t_scale.y+t_offset.y); pv++;
				batch++;
				if (batch==batch_size)	{
					// Flush
					vs_World->Unlock		(batch*3);
					Device.Primitive.Draw	(vs_World,batch,Offset);
					pv						= (FVF::LIT*) vs_World->Lock(batch_size*3,Offset);
					batch					= 0;
				}
			}
		}
	}

	// Flush if nessesary
	vs_World->Unlock		(batch*3);
	if (batch)				Device.Primitive.Draw	(vs_World,batch,Offset);
	
	// Clear all shadows
	shadows.clear			();

	// Debug
//*
	{
		// UV
		Fvector2				p0,p1;
		p0.set					(.5f/S_rt_size, .5f/S_rt_size);
		p1.set					((S_rt_size+.5f)/S_rt_size, (S_rt_size+.5f)/S_rt_size);
		
		// Fill vertex buffer
		C			=	0xffffffff;
		DWORD _w	=	S_rt_size/2, _h = S_rt_size/2;
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
//*/

	// Projection
	Device.mProject._43 = _43;
	Device.set_xform_project	(Device.mProject);
}
