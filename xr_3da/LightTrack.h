// LightTrack.h: interface for the CLightTrack class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LIGHTTRACK_H__89914D61_AC0B_4C7C_BA8C_D7D810738CE7__INCLUDED_)
#define AFX_LIGHTTRACK_H__89914D61_AC0B_4C7C_BA8C_D7D810738CE7__INCLUDED_
#pragma once

class ENGINE_API CLightTrack  
{
public:
	struct Item 
	{
		int					id;
		Collide::ray_cache	Cache;
		float				fuzzy;			// note range: (-1[no]..1[yes])
		float				fuzzy_smooth;
		DWORD				mark;
	};
	vector<Item>			track;
public:
	CLightTrack();
	~CLightTrack();
};

#endif // !defined(AFX_LIGHTTRACK_H__89914D61_AC0B_4C7C_BA8C_D7D810738CE7__INCLUDED_)
