// DynamicHeightMap.h: interface for the CDynamicHeightMap class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DYNAMICHEIGHTMAP_H__5A5BD0B8_1FC7_4067_A5A4_D40422E8B6D1__INCLUDED_)
#define AFX_DYNAMICHEIGHTMAP_H__5A5BD0B8_1FC7_4067_A5A4_D40422E8B6D1__INCLUDED_
#pragma once

const int	dhm_matrix		= 9;		// 8x8 array
const float dhm_size		= 4.f;		// 4m per slot
const int	dhm_precision	= 32;		// 32x32 subdivs per slot
const int	dhm_total		= dhm_matrix*dhm_matrix;

class CHM_Static
{
	struct	Slot
	{
		BOOL		bReady;
		float		data	[dhm_precision][dhm_precision];
	};
	Slot*						data	[dhm_matrix][dhm_matrix];	// database
	int							c_x,c_z;							// center of heighmap
	svector<Slot*,dhm_total>	task;
public: 
	void						Update		();

	CHM_Static					();
	~CHM_Static					();
};

class CHM_Dynamic
{
public:
	void						Update		();

	CHM_Dynamic					();
	~CHM_Dynamic				();
};

class CHeightMap
{
	CHM_Static		hm_static;
	CHM_Dynamic		hm_dynamic;
public:
	void			Initialize	();
	void			Update		();

	float			Query		(Fvector2& pos);				// 2D query
	Fvector			Query		(Fvector& pos, Fvector& dir);	// 3D ray-query
};

#endif // !defined(AFX_DYNAMICHEIGHTMAP_H__5A5BD0B8_1FC7_4067_A5A4_D40422E8B6D1__INCLUDED_)
