// LightTrack.h: interface for the CLightTrack class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LIGHTTRACK_H__89914D61_AC0B_4C7C_BA8C_D7D810738CE7__INCLUDED_)
#define AFX_LIGHTTRACK_H__89914D61_AC0B_4C7C_BA8C_D7D810738CE7__INCLUDED_
#pragma once

const	float				lt_inc			= 4.f;
const	float				lt_dec			= 2.f;
const	float				lt_smooth		= 4.f;

/*
---		Not implemented

class	CShadowGeomCache
{
public:
	struct	Item
	{
		Fvector		Lpos;
		void*		data;
		u32			frame;
	};
public:
	Fvector					cached_pos;
	xr_map<light*,Item>		cache;
};
*/
class	CLightTrack			: public IRender_ObjectSpecific
{
public:
	struct	Item 
	{
		u32					frame_touched;
		light*				source;
		Collide::ray_cache	cache;
		float				test;			// note range: (-1[no]..1[yes])
		float				energy;
	};
	struct	Light
	{
		light*				source;
		float				energy;
		Fcolor				color;
	};
	xr_vector<Item>			track;
	xr_vector<Light>		lights;
	float					ambient;
	Fvector					approximate;
	u32						dwFrame;
public:
	//CShadowGeomCache		ShadowGeomCache;			
	u32						Shadowgen_dwFrame;

	u32						Shadowed_dwFrame;
	int						Shadowed_Slot;

	void					add				(light*			L);
	void					ltrack			(IRenderable*	O);

	CLightTrack				();
	virtual ~CLightTrack	()	{};
};

#endif // !defined(AFX_LIGHTTRACK_H__89914D61_AC0B_4C7C_BA8C_D7D810738CE7__INCLUDED_)
