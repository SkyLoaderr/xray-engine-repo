// LightShadows.cpp: implementation of the CLightShadows class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LightShadows.h"
#include "xr_object.h"

const	float	S_distance	= 32;
const	float	S_level		= .1f;
const	int		S_size		= 64;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLightShadows::CLightShadows()
{
	current	= 0;
}

CLightShadows::~CLightShadows()
{
	
}

void CLightShadows::set_object	(CObject* O)
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

void CLightShadows::add_element	(NODE* N)
{
	if (0==current)	return;
	casters.back().nodes.push_back	(N);
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
	// sort by distance
	std::sort	(id.begin(),id.end(),pred_casters(this));
	
	// iterate on objects
	int	slot_id		= 0;
	int slot_line	= 512/S_size;
	int slot_max	= slot_line*slot_line;
	for (int o_it=0; o_it<id.size(); o_it++)
	{
		caster&	C	= casters[id[o_it]];
		if (C.nodes.empty())	continue;
		
		// Select lights and calc importance
		lights.clear		();
		::Render.Lights.Select	(C.C,C.O->Radius(),lights);
		
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
			float		p_near	=	p_dist-p_R-EPS_L; if (p_near<EPS_L)	p_near=EPS_L;
			float		p_far	=	p_dist+p_R+EPS_L;
			mProject.build_projection_HAT	(p_hat,p_asp,p_near,p_far);
			
			// calculate view-matrix
			Fmatrix		mView;
			Fvector		v_D,v_N,v_R;
			v_D.sub					(C.C,L->position);;
			if(1-fabsf(v_D.y)<EPS)	v_N.set(1,0,0);
			else            		v_N.set(0,1,0);
			v_R.crossproduct		(v_N,v_D);
			v_N.crossproduct		(v_D,v_R);
			mView.build_camera		(L->position,C.C,v_N);
			
			// combine and build frustum
			Fmatrix		mCombine;
			mCombine.mul			(mProject,mView);
			CFrustum	F;
			F.CreateFromMatrix		(mCombine,FRUSTUM_P_ALL);

			// Render object
			int		s_x		= slot_id%slot_line;
			int		s_y		= slot_id/slot_line;
		}
	}
}
