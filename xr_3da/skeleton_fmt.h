#ifndef SKELETON_FMT_H
#define SKELETON_FMT_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	AXF_HEADER			=100,
	AXF_BODY_VERTEX		=101,
	AXF_BODY_NORMAL		=102,
	AXF_BODY_BONES		=103,
	AXF_BODY_MATERIAL	=104,
	AXF_BODY_FACES		=105,
	AXF_MOTION			=110, // AXF_MOTION + <number> -> 90 reserved
    AXF_forcedword	= 0xFFFFFFFF
} AXF_Chuncks;

#define SKELETON_VERSION 1

#pragma pack(push)
#pragma pack(1)
struct axf_header{
	unsigned char	format_version;	// =1
	unsigned char	has_body;
	unsigned short	motion_count;
};
#pragma pack(pop)

// AXF_BODY_VERTEX
//  int						XSkinVertexCount
//  ..geBody_XSkinVertex	XSkinVertexArray

// AXF_BODY_NORMAL
//  int						SkinNormalCount
//  ..geBody_Normal			SkinNormalArray

// AXF_BODY_BONES
//	int						BoneCount
//	..geBody_Bone			BoneArray
//	..BoneNames - Z-String

// AXF_BODY_MATERIAL
//	int						MaterialCount
//	..geBody_Material		MaterialArray
//	..MaterialName - Z-String

// AXF_BODY_FACES
//	int						FacesCount
//	..geBody_Triangle		FaceArray

// AXF_MOTIONS<number>
// ..MOT_FILE

#ifdef __cplusplus
}
#endif

#endif

