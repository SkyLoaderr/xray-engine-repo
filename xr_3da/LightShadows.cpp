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
		
		// Select lights and calc importance
		lights.clear		();
		::Render.Lights.Select	(C.C,C.O->Radius(),lights);
		
		// iterate on lights
		for (int l_it=0; (l_it<lights.size()) && (slot_id<slot_max); l_it++)
		{
			xrLIGHT*	L	= lights[l_it];
			float	level	= L->diffuse.magnitude_rgb	();
			if (level<S_level)	continue;

			// calculate center-rect



			int		s_x		= slot_id%slot_line;
			int		s_y		= slot_id/slot_line;
		}
	}
}
