// LightShadows.cpp: implementation of the CLightShadows class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LightShadows.h"
#include "xr_object.h"

const	float	S_distance	= 32;

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
	std::sort(id.begin(),id.end(),pred_casters(this));

}
