// LightTrack.h: interface for the CLightTrack class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LIGHTTRACK_H__89914D61_AC0B_4C7C_BA8C_D7D810738CE7__INCLUDED_)
#define AFX_LIGHTTRACK_H__89914D61_AC0B_4C7C_BA8C_D7D810738CE7__INCLUDED_
#pragma once

const	float				lt_inc			= 4.f;
const	float				lt_dec			= 2.f;
const	float				lt_smooth		= 4.f;

class	R1_light;

class	CLightTrack			: public IRender_ObjectSpecific
{
public:
	struct Item 
	{
		R1_light*			source;
		Collide::ray_cache	cache;
		float				test;			// note range: (-1[no]..1[yes])
		float				energy;
	};
	struct Light
	{
		Flight				L;
		float				energy;
	};
	xr_vector<Item>			track;
	xr_vector<Light>		lights;
	float					ambient;
	u32						dwFrame;
public:
	u32						Shadowed_dwFrame;
	int						Shadowed_Slot;

	void					add				(R1_light* L);
	void					remove			(R1_light* id);

	virtual ~CLightTrack	()	{};
};

#endif // !defined(AFX_LIGHTTRACK_H__89914D61_AC0B_4C7C_BA8C_D7D810738CE7__INCLUDED_)
