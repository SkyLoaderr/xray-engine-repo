#pragma once

enum fsL_Chunks {
	fsL_HEADER=1,		//*
	fsL_TEXTURES,		//*
	fsL_SHADERS,		//*
	fsL_VISUALS,		//*
	fsL_VBUFFERS,		//*
	fsL_CFORM,			//*
	fsL_PORTALS,		//*		- Portal polygons
	fsL_LIGHTS,			//*
	fsL_LIGHT_KEYS,		//*		- keyframes
	fsL_GLOWS,			//*		- All glows inside level
	fsL_SECTORS,		//*		- All sectors on level
	fsL_VISIBILITY,		//*		- Relevance map
    fsL_forcedword	= 0xFFFFFFFF
};
enum fsEVisChunks {
	fsV_HEADER = 1,
	fsV_NODES,
	fsV_LIGHTS,
	fsV_GLOWS,
	fsV_OCCLUDERS,
	fsV_MAP,
	fsV_forcedword = DWORD(-1)
};
enum fsESectorChunks {
	fsP_Portals = 1,	// - portal polygons
	fsP_Root,			// - geometry root
	fsP_Occluders,		// - occluders inside sector
	fsP_Glows,			// - glows inside sector
	fsP_Lights,			// - lights, which touches sector
	fsP_forcedword = DWORD(-1)
};

#pragma pack(push,8)
struct hdrLEVEL
{
	DWORD	XRLC_version;
	char	name[124];
};
struct hdrCFORM 
{
	DWORD	version;
	DWORD	vertcount;
	DWORD	facecount;
	Fbox	aabb;
};
struct	hdrNODES
{
	DWORD	version;
	DWORD	count;
	float	size;
	float	size_y;
	Fbox	aabb;
};
#pragma pack(pop)
#pragma pack(push,1)
#pragma warning(disable:4200)
typedef BYTE		NodeLink[3];
typedef short		NodePosition[3];
struct NodeCompressed		
{
	WORD			plane;			// 2
	NodePosition	p0;				// 2+2+2 = 6
	NodePosition	p1;				// 2+2+2 = 6
	BYTE			sector;			// 1
	BYTE			light;			// 1
	BYTE			cover[4];		// 4
	BYTE			link_count;		// 1		
};								// 2+6+6+1+1+4+1 = 21b + links
#pragma warning(default:4200)
#pragma pack(pop)

#define XRCL_CURRENT_VERSION	10
#define XRCL_PRODUCTION_VERSION	5
#define CFORM_CURRENT_VERSION	2
#define XRAI_CURRENT_VERSION	2
