#include "stdafx.h"
#include "ISpatial.h"

extern Fvector	c_spatial_offset[8];

template <bool b_first, bool b_nearest>
class	walker
{
public:
	u32			mask;
	Fvector		start;
	Fvector		dir;
	float		range;
	float		range2;
public:
	walker					(u32 _mask, const Fvector& _start, const Fvector&	_dir, float _range)
	{
		mask	= _mask;
		start	= _start;
		dir		= _dir;
		range	= _range;
		range2	= _range*_range;
	}
	void		walk		(ISpatial_NODE* N, Fvector& n_C, float n_R)
	{
		// box
		Fvector	coord;
		float	n_vR	=		2*n_R;
		Fbox	BB;		BB.set	(n_C.x-n_vR, n_C.y-n_vR, n_C.z-n_vR, n_C.x+n_vR, n_C.y+n_vR, n_C.z+n_vR);
		if		(!BB.Pick2	(start,dir,coord))			return;
		if		(coord.distance_to_sqr(start)>range2)	return;

		// test items
		xr_vector<ISpatial*>::iterator _it	=	N->items.begin	();
		xr_vector<ISpatial*>::iterator _end	=	N->items.end	();
		for (; _it!=_end; _it++)
		{
			ISpatial*		S	= *_it;
			if (0==(S->spatial.type&mask))	continue;

			Fvector&		sC	= S->spatial.center;
			float			sR	= S->spatial.radius;

			Fvector Q;			Q.sub(sC,start);
			float	c_sqr		= Q.square_magnitude();
			float	v			= Q.dotproduct		(dir);
			float	d			= sR*sR - (c_sqr - v*v);
			if (d > 0)
			{
				// sphere intersected (assume object intersects too)
				if (b_nearest && d<range)		{ range=d; range2=d*d; }
				g_SpatialSpace.q_result.push_back(S);
				if (b_first)					return;
			}
		}

		// recurse
		float	c_R		= n_R/2;
		for (u32 octant=0; octant<8; octant++)
		{
			if (0==N->children[octant])	continue;
			Fvector		c_C;			c_C.mad	(n_C,c_spatial_offset[octant],c_R);
			walk						(N->children[octant],c_C,c_R);
			if (b_first && !g_SpatialSpace.q_result.empty())	return;
		}
	}
};

void	ISpatial_DB::q_ray	(u32 _o, u32 _mask, const Fvector&	_start,  const Fvector&	_dir, float _range)
{
	VERIFY		(!lock);
	lock		= TRUE;

	q_result.clear	();
	if (_o & O_ONLYFIRST)
	{
		if (_o & O_ONLYNEAREST)		{ walker<true,true>		W(_mask,_start,_dir,_range);	W.walk(m_root,m_center,m_bounds); } 
		else						{ walker<true,false>	W(_mask,_start,_dir,_range);	W.walk(m_root,m_center,m_bounds); } 
	} else {
		if (_o & O_ONLYNEAREST)		{ walker<false,true>	W(_mask,_start,_dir,_range);	W.walk(m_root,m_center,m_bounds); } 
		else						{ walker<false,false>	W(_mask,_start,_dir,_range);	W.walk(m_root,m_center,m_bounds); } 
	}

	lock		= FALSE;
}
