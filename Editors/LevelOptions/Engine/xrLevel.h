#pragma once

enum fsL_Chunks {
	fsL_HEADER=1,		//*
	fsL_STRINGS,		//*
	fsL_VISUALS,		//*
	fsL_VBUFFERS,		//*
	fsL_CFORM,			//*
	fsL_PORTALS,		//*		- Portal polygons
	fsL_LIGHT_CONTROL,	//*
	fsL_LIGHT_DYNAMIC,	//*
	fsL_GLOWS,			//*		- All glows inside level
	fsL_SECTORS,		//*		- All sectors on level
	fsL_IBUFFERS,		//*
	fsL_VBUFFERS_DX9,	//*
    fsL_forcedword	= 0xFFFFFFFF
};
enum fsESectorChunks {
	fsP_Portals = 1,	// - portal polygons
	fsP_Root,			// - geometry root
	fsP_Occluders,		// - occluders inside sector
	fsP_Glows,			// - glows inside sector
	fsP_Lights,			// - lights, which touches sector
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
	char	name[124];
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
#pragma warning(disable:4200)
typedef BYTE		NodeLink[3];
struct NodePosition
{
	s16				x;
	u16				y;
	s16				z;
};
struct NodeCompressed
{
	WORD			plane;			// 2
	NodePosition	p0;				// 2+2+2 = 6
	NodePosition	p1;				// 2+2+2 = 6
	BYTE			light;			// 1

	u8				cover	[4];	// 4
	u8				links	: 6;	// 1
	u8				flag0	: 1;	// .
	u8				flag1	: 1;	// .
};									// 2+6+6+1+4+1 = 20b + links
#pragma warning	(default:4200)
#pragma pack	(pop)

const u32 XRCL_CURRENT_VERSION		=	14;
const u32 XRCL_PRODUCTION_VERSION	=	9;
const u32 CFORM_CURRENT_VERSION		=	3;
const u32 XRAI_CURRENT_VERSION		=	3;
