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
#pragma pack(1)
class NodePosition {
	u8	data[5];
	
	void xz	(u32 value)
	{
		Memory.mem_copy	(data,&value,3);
	}

	void y	(u16 value)
	{
		Memory.mem_copy	(data + 3,&value,2);
	}

public:
	u32	xz	() const
	{
		return			((*((u32*)data)) & 0x00ffffff);
	}
	u32	y	() const
	{
		return			(*((u16*)(data + 3)));
	}

	friend class CLevelGraph;
	friend struct CNodePositionCompressor;
	friend struct CNodePositionConverter;
};

class NodeCompressed {
//	u8				data[11];
//	
//	void link(u8 link_index, u32 value)
//	{
//		value			&= 0x001fffff;
//		switch (link_index) {
//			case 0 : {
//				value	|= (*(u32*)data) & 0xffe00000;
//				Memory.mem_copy(data, &value, sizeof(u32));
//				break;
//			}
//			case 1 : {
//				value	<<= 5;
//				value	|= (*(u32*)(data + 2)) & 0xfc00001f;
//				Memory.mem_copy(data + 2, &value, sizeof(u32));
//				break;
//			}
//			case 2 : {
//				value	<<= 2;
//				value	|= (*(u32*)(data + 5)) & 0xff800003;
//				Memory.mem_copy(data + 5, &value, sizeof(u32));
//				break;
//			}
//			case 3 : {
//				value	<<= 7;
//				value	|= (*(u32*)(data + 7)) & 0xf000007f;
//				Memory.mem_copy(data + 7, &value, sizeof(u32));
//				break;
//			}
//			default : NODEFAULT;
//		}
//	}
//	
//	void light(u8 value)
//	{
//		data[10]		|= value << 4;
//	}
	u32				link0:24;
	u32				link1:24;
	u32				link2:24;
	u32				link3:24;
	u8				_light;

	void link(u8 link_index, u32 value)
	{
		switch (link_index) {
			case 0 : {
				link0 = value;
				break;
			}
			case 1 : {
				link1 = value;
				break;
			}
			case 2 : {
				link2 = value;
				break;
			}
			case 3 : {
				link3 = value;
				break;
			}
			default : NODEFAULT;
		}
	}
	void light(u8 value)
	{
		_light		= value;
	}


public:
	u8				cover;
	u16				plane;
	NodePosition	p;

//	u32	link(u8 index) const
//	{
//		switch (index) {
//			case 0 :	return	((*(u32*)data) & 0x001fffff);
//			case 1 :	return	(((*(u32*)(data + 2)) >> 5) & 0x001fffff);
//			case 2 :	return	(((*(u32*)(data + 5)) >> 2) & 0x001fffff);
//			case 3 :	return	(((*(u32*)(data + 7)) >> 7) & 0x001fffff);
//			default :	NODEFAULT;
//		}
//#ifdef DEBUG
//		return			(0);
//#endif
//	}
//	
//	u8	light() const
//	{
//		return			(data[10] >> 4);
//	}
//	
	u32	link(u8 index) const
	{
		switch (index) {
			case 0 :	return	(link0);
			case 1 :	return	(link1);
			case 2 :	return	(link2);
			case 3 :	return	(link3);
			default :	NODEFAULT;
		}
#ifdef DEBUG
		return			(0);
#endif
	}
	
	u8	light() const
	{
		return			(_light);
	}
	

	friend class CLevelGraph;
	friend struct CNodeCompressed;
};									// 2+5+1+11 = 19b
#pragma pack	(pop)

const u32 XRCL_CURRENT_VERSION		=	16;	// input
const u32 XRCL_PRODUCTION_VERSION	=	12;	// output
const u32 CFORM_CURRENT_VERSION		=	4;
const u32 XRAI_CURRENT_VERSION		=	4;

#endif // xrLevelH
