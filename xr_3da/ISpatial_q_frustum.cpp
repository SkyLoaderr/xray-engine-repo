#include "stdafx.h"
#include "ISpatial.h"
#include "frustum.h"

extern Fvector	c_spatial_offset[8];

class	walker
{
public:
	u32			mask;
	CFrustum*	F;
public:
	walker					(u32 _mask, const CFrustum* _F)
	{
		mask	= _mask;
		F		= (CFrustum*)_F;
	}
	void		walk		(ISpatial_NODE* N, Fvector& n_center, float n_radius, u32 fmask)
	{
		// box
		Fbox	BB;			BB.set	(n_center.x-n_radius, n_center.y-n_radius, n_center.z-n_radius, n_center.x+n_radius, n_center.y+n_radius, n_center.z+n_radius);
		if		(fcvNone==F->testAABB(BB.min,BB.max,fmask))	return;

		// test items
		xr_vector<ISpatial*>::iterator _it	=	N->items.begin	();
		xr_vector<ISpatial*>::iterator _end	=	N->items.end	();
		for (; _it!=_end; _it++)
		{
			ISpatial*		S	= *_it;
			if (0==(S->spatial.type&mask))	continue;

			Fvector&		sC		= S->spatial.center;
			float			sR		= S->spatial.radius;
			u32				tmask	= fmask;
			if (fcvNone==F->testSphere(sC,sR,tmask))	continue;

			g_SpatialSpace.q_result.push_back(S);
		}

		// recurse
		float	c_radius		= n_radius/2;
		for (u32 octant=0; octant<8; octant++)
		{
			if (0==N->children[octant])		continue;
			Fvector		c_center;			c_center.mad	(n_center,c_spatial_offset[octant],n_radius/4.f);
			walk							(N->children[octant],c_center,c_radius,fmask);
		}
	}
};

void	ISpatial_DB::q_frustum		(u32 _o, u32 _mask, const CFrustum& _frustum)	
{
	q_result.clear	();
	walker			W(_mask,&_frustum);	W.walk(m_root,m_center,m_bounds,_frustum.getMask()); 
}
