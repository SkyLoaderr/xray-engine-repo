#include "stdafx.h"
#include "ISpatial.h"

template <bool b_first, bool b_nearest, bool b_ordered>
class	walker_ray
{
public:
	u32			mask;
	Fvector		start;
	Fvector		dir;
	float		range;
public:
	walker_ray				(u32 _mask, const Fvector& _start, const Fvector&	_dir, float _range)
	{
		mask	= _mask;
		start	= _start;
		dir		= _dir;
		range	= _range;
	}
	void		walk		(ISpatial_NODE* N)
	{
	}
};

void	ISpatial_DB::q_ray	(u32 _o, u32 _mask, const Fvector&	_start,  const Fvector&	_dir, float _range)
{
	if (_o & O_ONLYFIRST)
	{
		if (_o & O_ONLYNEAREST)
		{
		} else {
		}
	} else {
		if (_o & O_ONLYNEAREST)
		{
		} else {
		}
	}
}
