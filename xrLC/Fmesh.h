#ifndef fmeshH
#define fmeshH
#pragma once

BOOL ValidateIndices		(u32 vCount, u32 iCount, u16* pIndices);

///////////////////////////////////////////////////////////////////////////////////////////////////////
// MESH as it is represented in file
enum MT {
	MT_NORMAL				=0,
	MT_HIERRARHY			=1,
	MT_PROGRESSIVE			=2,
	MT_SKELETON_ANIM		=4,
	MT_SKELETON_GEOMDEF_PM	=3,
	MT_SKELETON_GEOMDEF_ST	=7,
	MT_CACHED				=8,
	MT_PARTICLE_SYSTEM		=9,
	MT_LOD					=11,
	MT_TREE					=12,
	MT_PARTICLE_EFFECT		=13,
	MT_PARTICLE_GROUP		=14,
	MT_SKELETON_RIGID		=15,
};

enum OGF_Chuncks {
	OGF_HEADER		= 1,
	OGF_TEXTURE		= 2,
	OGF_MATERIAL	= 4,	// actors only
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
	OGF_IKDATA_obs	= 23,	// * For skeletons only (__obsolete__)
	OGF_USERDATA	= 24,	// * For skeletons only (Ini-file)
	OGF_IKDATA		= 25,	// * For skeletons only
	OGF_MOTIONS2	= 26,	// * For skeletons only
	OGF_DESC		= 27,	// * 
	OGF_IKDATA2		= 28,	// * For skeletons only
	OGF_MOTION_REFS	= 29,	// * For skeletons only
    OGF_forcedword	= 0xFFFFFFFF
};

const u16	xrOGF_SMParamsVersion	= 1;

// OGF_DESC
struct ogf_desc{
	ref_str	source_file;
    ref_str	build_name;
    time_t	build_time;
    ref_str	create_name;
    time_t	create_time;
    ref_str	modif_name;
    time_t	modif_time;
    		ogf_desc():build_time(0),create_time(0),modif_time(0){}
    void 	Load	(IReader& F);
    void 	Save	(IWriter& F);
};

// OGF_HEADER
const BYTE	xrOGF_FormatVersion		= 3;
struct ogf_header {
	u8		format_version;			// = xrOGF_FormatVersion
	u8		type;					// MT
	u16		shader_id;				// should not be ZERO
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
