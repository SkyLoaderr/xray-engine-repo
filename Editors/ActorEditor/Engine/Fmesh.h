#ifndef fmeshH
#define fmeshH
#pragma once

/*
#define D3DFVF_RESERVED0        0x001	-	0000 0001
#define D3DFVF_POSITION_MASK    0x00E	-	0000 1110

#define D3DFVF_XYZ              0x002	-	0000 001	0
#define D3DFVF_XYZRHW           0x004	-	0000 010	0
#define D3DFVF_XYZB1            0x006	-	0000 011	0
#define D3DFVF_XYZB2            0x008	-	0000 100	0
#define D3DFVF_XYZB3            0x00a	-	0000 101	0
#define D3DFVF_XYZB4            0x00c	-	0000 110	0
#define D3DFVF_XYZB5            0x00e	-	0000 111	0

#define D3DFVF_NORMAL           0x010	-	0001 0000
#define D3DFVF_PSIZE            0x020	-	0010 0000
#define D3DFVF_DIFFUSE          0x040	-	0100 0000
#define D3DFVF_SPECULAR         0x080	-	1000 0000

#define D3DFVF_TEXCOUNT_MASK    0xf00
#define D3DFVF_TEXCOUNT_SHIFT   8
#define D3DFVF_TEX0             0x000
#define D3DFVF_TEX1             0x100
#define D3DFVF_TEX2             0x200
#define D3DFVF_TEX3             0x300
#define D3DFVF_TEX4             0x400
#define D3DFVF_TEX5             0x500
#define D3DFVF_TEX6             0x600
#define D3DFVF_TEX7             0x700
#define D3DFVF_TEX8             0x800

#define D3DFVF_LASTBETA_UBYTE4  0x1000

#define D3DFVF_RESERVED2        0xE000  // 4 reserved bits

#define D3DFVF_VERTEX ( D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1 )
#define D3DFVF_LVERTEX ( D3DFVF_XYZ | D3DFVF_RESERVED1 | D3DFVF_DIFFUSE | \
                         D3DFVF_SPECULAR | D3DFVF_TEX1 )
#define D3DFVF_TLVERTEX ( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | \
                          D3DFVF_TEX1 )
*/

BOOL ValidateIndices(u32 vCount, u32 iCount, u16* pIndices);

///////////////////////////////////////////////////////////////////////////////////////////////////////
// MESH as it is represented in file
enum MT {
	MT_NORMAL=0,
	MT_HIERRARHY,
	MT_PROGRESSIVE,
	MT_SKELETON_PART,
	MT_SKELETON,
	MT_SPRITE,
	MT_DETAIL_PATCH,
	MT_SKELETON_PART_STRIPPED,
	MT_CACHED,
	MT_PARTICLE_SYSTEM,
	MT_PROGRESSIVE_STRIPS,
	MT_LOD,
	MT_TREE,
	MT_PARTICLE_EFFECT,
	MT_PARTICLE_GROUP,
};

enum OGF_Chuncks {
	OGF_HEADER		= 1,
	OGF_TEXTURE		= 2,
	OGF_TEXTURE_L	= 3,	// Really - Texture link for static data
	OGF_MATERIAL	= 4,	// Actors only
	OGF_CHIELDS		= 5,
	OGF_BBOX		= 6,
	OGF_VERTICES	= 7,
	OGF_INDICES		= 8,
	OGF_P_MAP		= 9,
	OGF_VCONTAINER	= 10,
	OGF_BSPHERE		= 11,
	OGF_CHIELDS_L	= 12,	// Link to child visuals
	OGF_BONE_NAMES	= 13,	// * For skeletons only
	OGF_MOTIONS		= 14,	// * For skeletons only
	OGF_DPATCH		= 15,	// --- DPatches only
	OGF_P_LODS		= 16,
    OGF_CHILDREN	= 17,	// * For skeletons only
	OGF_SMPARAMS	= 18,	// * For skeletons only
	OGF_ICONTAINER	= 19,
	OGF_SMPARAMS2	= 20,	// * For skeletons only
	OGF_LODDEF2		= 21,	// + 5 channel data
	OGF_TREEDEF2	= 22,	// + 5 channel data
	OGF_IKDATA		= 23,	// * For skeletons only
	OGF_USERDATA	= 24,	// * For skeletons only (Ini-file)
    OGF_forcedword	= 0xFFFFFFFF
};

const u16	xrOGF_SMParamsVersion	= 1;

// OGF_HEADER
const BYTE	xrOGF_FormatVersion		= 3;
struct ogf_header {
	BYTE format_version;			// = xrOGF_FormatVersion
	BYTE type;						// MT
	u16	 flags;						// =0
};

// OGF_BBOX
struct ogf_bbox {
	Fvector min;
	Fvector max;
};

// OGF_BSPHERE
struct ogf_bsphere {
	Fvector c;
	float	r;
};

// OGF_TEXTURE1
//  Z-String - name
//  Z-String - shader

// OGF_TEXTURE_L
//	u32 T_link
//  u32 S_link

// OGF_MATERIAL
//  Fmaterial

// OGF_CHIELDS
//	u32 Count
//  Z-String [Count] - names

// OGF_CHIELDS_L
//	u32	Count
//	u32	idx[0...Count-1]

// OGF_VERTICES
//	u32		dwVertType;			// Type of vertices it contains
//	u32		dwVertCount;		// Count of vertices
//	..vertices itself

// OGF_INDICES
//	u32		Count
//  ..indices itself (u16[Count]);

// OGF_P_MAP
//		PMAP_HEADER = 0x1
//			u32		MinVertCount
//			u32		I_Current
//		PMAP_VSPLIT	= 0x2
//			VSPLIT[dwVertCount-MinVertCount]
//		PMAP_FACES	= 0x3
//			u32 dwCount
//			u16[dwCount]

// OGF_SF_LINK
//  Z-String - name of visual

// OGF_VCONTAINER
// u32		CID;		// Container ID
// u32		Offset;		// Add for every IDX and use as the start vertex
// u32		Count;		// Number of vertices

// OGF_BONES
// BYTE			Count;
// BYTE			Indices[...]

// OGF_TREEDEF
// xform : matrix4x4
// scale : vec4
// bias  : vec4

#endif // fmeshH
