#pragma once

#ifndef _DETAIL_FORMAT_H_
#define _DETAIL_FORMAT_H_
#pragma pack(push,1)

#define DETAIL_VERSION		1
#define DETAIL_SLOT_SIZE	4.f

//	int s_x	= iFloor			(EYE.x/slot_size+.5f)+offs_x;		// [0...size_x)
//	int s_z	= iFloor			(EYE.z/slot_size+.5f)+offs_z;		// [0...size_z)


/*
0 - Header(version,obj_count(max255),size_x,size_z,min_x,min_z)
1 - Objects
	0
	1
	2
	..
	obj_count-1
2 - slots
*/

struct ENGINE_API DetailHeader
{
	DWORD		version;
	DWORD		object_count;
	int			offs_x,	offs_z;
	DWORD		size_x,	size_z;
};

struct ENGINE_API DetailPalette
{
	u16		a0:4;
	u16		a1:4;
	u16		a2:4;
	u16		a3:4;
};
struct ENGINE_API DetailItem
{
	u8				id;		// 0xff - empty
	DetailPalette	palette;
};
struct ENGINE_API DetailSlot
{
	float			y_min,y_max;
	u32				r_yaw;
	u32 			r_scale;
	DetailItem		items[4];
	u16				color;
};

#pragma pack(pop)
#endif // _DEBUG
