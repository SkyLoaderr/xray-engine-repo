#include "stdafx.h"
#include "Light_Package.h"

void	light_Package::clear	()
{
	v_point.clear		();
	v_point_s.clear		();
	v_spot.clear		();
	v_spot_s.clear		();
}

IC	bool	pred_light_cmp	(light*	_1, light* _2)
{
	if	(_1->vis.pending)
	{
		if (_2->vis.pending)	return	_1->vis.query_order > _2->vis.query_order;	// q-order
		else					return	false;										// _2 should be first
	} else {
		if (_2->vis.pending)	return	true;										// _1 should be first 
		else					return	_1->range > _2->range;						// sort by range
	}
}

void	light_Package::sort		()
{
	// resort lights (pending -> at the end), maintain stable order
	std::stable_sort	(Lights.v_point_s.begin(),	Lights.v_point_s.end(),	pred_light_cmp);
	std::stable_sort	(Lights.v_point.begin(),	Lights.v_point.end(),	pred_light_cmp);
	std::stable_sort	(Lights.v_spot_s.begin(),	Lights.v_spot_s.end(),	pred_light_cmp);
	std::stable_sort	(Lights.v_spot.begin(),		Lights.v_spot.end(),	pred_light_cmp);
}
