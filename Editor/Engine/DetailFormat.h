#pragma once

#ifndef _DETAIL_FORMAT_H_
#define _DETAIL_FORMAT_H_
#pragma pack(push,1)

#define DETAIL_VERSION		2
#define DETAIL_SLOT_SIZE	2.f

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

	CFS_Memory F;
    m_Header.object_count=m_Objects.size();
	// header
	F.write_chunk		(DETMGR_CHUNK_HEADER,&m_Header,sizeof(DetailHeader));
    // objects
	F.open_chunk		(DETMGR_CHUNK_OBJECTS);
    for (DOIt it=m_Objects.begin(); it!=m_Objects.end(); it++){
		F.open_chunk	(it-m_Objects.begin());
        (*it)->Export	(F);
	    F.close_chunk	();
    }
    F.close_chunk		();
    // slots
	F.open_chunk		(DETMGR_CHUNK_SLOTS);
	F.write				(m_Slots.begin(),m_Slots.size()*sizeof(DetailSlot));
    F.close_chunk		();

    F.SaveTo			(fn,0);
*/
/*
// detail object
	char*			shader;
	char*			texture;

	DWORD			flag;
	float			min_scale;
	float	 		max_scale;

	DWORD			vert_count;
	DWORD			index_count;

	fvfVertexIn*	vertices;
	WORD*			indices;
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
struct ENGINE_API DetailSlot// 4+4+3*4+2 = 22b
{
	float			y_min,y_max;
	DetailItem		items[4];
	u16				color;
};

#pragma pack(pop)
#endif // _DEBUG
