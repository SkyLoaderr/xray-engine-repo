#ifndef xrLevelH
#define xrLevelH
#pragma once

#define NODE_NEIGHBOUR_COUNT 4

enum fsL_Chunks {
	fsL_HEADER2			=1,		//*
	fsL_SHADERS			=2,		//*
	fsL_VISUALS			=3,		//*
	fsL_PORTALS			=4,		//*		- Portal polygons
	fsL_LIGHT_DYNAMIC	=6,		//*
	fsL_GLOWS			=7,		//*		- All glows inside level
	fsL_SECTORS			=8,		//*		- All sectors on level
	fsL_IB				=9,		//*
	fsL_VB				=10,	//*
    fsL_forcedword		= 0xFFFFFFFF
};
enum fsESectorChunks {
	fsP_Portals = 1,	// - portal polygons
	fsP_Root,			// - geometry root
	fsP_forcedword = u32(-1)
};
enum fsSLS_Chunks {
	fsSLS_Description	= 1,	// Name of level
	fsSLS_ServerState,
	fsSLS_UnitsSpecific,
	fsSLS_forcedword = u32(-1)
};

#pragma pack(push,8)
struct hdrLEVEL
{
	u32		XRLC_version;
};

struct hdrCFORM
{
	u32		version;
	u32		vertcount;
	u32		facecount;
	Fbox	aabb;
};

struct	hdrNODES
{
	u32		version;
	u32		count;
	float	size;
	float	size_y;
	Fbox	aabb;
};
#pragma pack(pop)
#pragma pack(push,1)
struct NodePosition
{
	u32				xz:24;
	u32				y:16;
};

struct NodeCompressed
{
	u16				plane;			// 2
	NodePosition	p;				// 2+2+2 = 6
	u8				cover;			// 1
	u64				light	:4;		// 1
	u64				link0	:21;
	u64				link1	:21;
	u64				link2	:21;
	u64				link3	:21;

	IC u32 get_link	(int i) const
	{
		switch (i) {
			case 0 :	return link0;
			case 1 :	return link1;
			case 2 :	return link2;
			case 3 :	return link3;
			default :	NODEFAULT;
		}
		return		(-1);
	}
};									// 2+5+1+11 = 19b
#pragma pack	(pop)

const u32 XRCL_CURRENT_VERSION		=	16;	// input
const u32 XRCL_PRODUCTION_VERSION	=	12;	// output
const u32 CFORM_CURRENT_VERSION		=	4;
const u32 XRAI_CURRENT_VERSION		=	4;

#endif // xrLevelH
