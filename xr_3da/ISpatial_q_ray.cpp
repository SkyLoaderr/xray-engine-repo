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
	void		walk		(ISpatial_NODE* N, Fvector& n_center, float n_radius)
	{
		// box
		Fvector	coord;
		Fbox	BB;			BB.set	(n_center.x-n_radius, n_center.y-n_radius, n_center.z-n_radius, n_center.x+n_radius, n_center.y+n_radius, n_center.z+n_radius);
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
		float	c_radius		= n_radius/2;
		for (u32 octant=0; octant<8; octant++)
		{
			if (0==N->children[octant])		continue;
			Fvector		c_center;			c_center.mad	(n_center,c_spatial_offset[octant],n_radius/4.f);
			walk							(N->children[octant],c_center,c_radius);
			if (b_first && !g_SpatialSpace.q_result.empty())	return;
		}
	}
};

void	ISpatial_DB::q_ray	(u32 _o, u32 _mask, const Fvector&	_start,  const Fvector&	_dir, float _range)
{
	q_result.clear	();
	if (_o & O_ONLYFIRST)
	{
		if (_o & O_ONLYNEAREST)		{ walker<true,true>		W(_mask,_start,_dir,_range);	W.walk(m_root,m_center,m_bounds); } 
		else						{ walker<true,false>	W(_mask,_start,_dir,_range);	W.walk(m_root,m_center,m_bounds); } 
	} else {
		if (_o & O_ONLYNEAREST)		{ walker<false,true>	W(_mask,_start,_dir,_range);	W.walk(m_root,m_center,m_bounds); } 
		else						{ walker<false,false>	W(_mask,_start,_dir,_range);	W.walk(m_root,m_center,m_bounds); } 
	}
}
