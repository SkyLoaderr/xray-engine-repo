// LightTrack.h: interface for the CLightTrack class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LIGHTTRACK_H__89914D61_AC0B_4C7C_BA8C_D7D810738CE7__INCLUDED_)
#define AFX_LIGHTTRACK_H__89914D61_AC0B_4C7C_BA8C_D7D810738CE7__INCLUDED_
#pragma once

#include "xr_collide_defs.h"

const float					lt_inc			= 1.f;
const float					lt_dec			= 1.f;

class ENGINE_API CLightTrack  
{
public:
	struct Item 
	{
		int					id;
		Collide::ray_cache	Cache;
		float				test;			// note range: (-1[no]..1[yes])
		float				energy;
	};
	vector<Item>			track;
public:
	void					add				(int id);
	void					remove			(int id);
};

#endif // !defined(AFX_LIGHTTRACK_H__89914D61_AC0B_4C7C_BA8C_D7D810738CE7__INCLUDED_)
