#pragma once

struct	SMAP_Rect
{
	Ivector2				min,max;
	bool					intersect	(SMAP_Rect& R)					{
		if( max.x < R.min.x )	return false;
		if( max.y < R.min.y )	return false;
		if( min.x > R.max.x )	return false;
		if( min.y > R.max.y )	return false;
		return true;
	}
	bool					valid		()								{
		if (min.x==max.x)		return false;
		if (min.y==max.y)		return false;
		return	true;
	}
	bool					setup		(Ivector2& p, u32 size)			{
		min	= max =	p;
		max.add		(size-1);
	}
	void					get_cp		(Ivector2& p0, Ivector2& p1)	{
		p0.set	(max.x+1,min.y);		// right
		p1.set	(min.x,max.y+1);		// down
	}
};

class	SMAP_Allocator
{
	u32						psize;		// 
	xr_vector<SMAP_Rect>	stack;		// 
	xr_vector<Ivector2>		cpoint;		// critical points
public:
	BOOL			push	(u32	_size)
	{
	}
};
