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
	void		walk		(ISpatial_NODE* N, Fvector& n_C, float n_R, u32 fmask)
	{
		// box
		float	n_vR	=		2*n_R;
		Fbox	BB;		BB.set	(n_C.x-n_vR, n_C.y-n_vR, n_C.z-n_vR, n_C.x+n_vR, n_C.y+n_vR, n_C.z+n_vR);
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
		float	c_R		= n_R/2;
		for (u32 octant=0; octant<8; octant++)
		{
			if (0==N->children[octant])	continue;
			Fvector		c_C;			c_C.mad	(n_C,c_spatial_offset[octant],c_R);
			walk						(N->children[octant],c_C,c_R,fmask);
		}
	}
};

void	ISpatial_DB::q_frustum		(u32 _o, u32 _mask, const CFrustum& _frustum)	
{
	q_result.clear	();
	walker			W(_mask,&_frustum);	W.walk(m_root,m_center,m_bounds,_frustum.getMask()); 
}
