// SkeletonBody.h: interface for the CSkeletonBody class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SKELETONBODY_H__8435254F_2AFC_42A8_B855_9DE6C14B7F20__INCLUDED_)
#define AFX_SKELETONBODY_H__8435254F_2AFC_42A8_B855_9DE6C14B7F20__INCLUDED_
#pragma once

#include "xform3d.h"

#define UNDEFINE_BONE -1

typedef signed long    int32;
typedef signed short   int16;
typedef signed char    int8 ;
typedef unsigned long  uint32;
typedef unsigned short uint16;
typedef unsigned char  uint8 ;

typedef struct Body_XSkinVertex
{
	Fvector			XPoint;
	float			XU,XV;
	int8			fake;
	int16			BoneIndex;
} Body_XSkinVertex;

typedef struct Body_Normal
{
	Fvector			Normal;
	int8			fake;
	int16			BoneIndex;
} Body_Normal;

typedef struct Body_Bone
{
	Fvector			BoundingBoxMin;
	Fvector			BoundingBoxMax;
	geXForm3d       AttachmentMatrix;
	int16			ParentBoneIndex;
} Body_Bone;

typedef struct Body_Triangle
{
	int16			VtxIndex[3];
	int16			NormalIndex[3];
	int16			MaterialIndex;
} Body_Triangle;

typedef struct Body_Material
{
	int				Bitmap;
	float			Red,Green,Blue;
} Body_Material;

class CStream;

class ENGINE_API CSkeletonBody
{
	friend class				CBodyInstance;
	friend class				CSkeleton;

	int16						XSkinVertexCount;
	vector<Body_XSkinVertex>	XSkinVertexArray;		// Sorted by BoneIndex

	vector<Body_Normal>			SkinNormalArray;

	vector<Body_Bone>			BoneArray;
	vector<char*>				BoneNames;

	vector<Body_Material>		MaterialArray;
	vector<char*>				MaterialNames;

	vector<Body_Triangle>		FaceArray;				// Sorted by MaterialIndex
public:
						CSkeletonBody		( CStream* pStream );
	virtual				~CSkeletonBody		();
//	BOOL				AddBone				( int ParentBoneIndex, const char *BoneName,
//											const geXForm3d *AttachmentMatrix, int *BoneIndex);
	void				GetBone				( int BoneIndex, const char **BoneName,
											geXForm3d *Attachment, int *ParentBoneIndex);
	BOOL				GetBoneIndexByName	( const char* BoneName, int* pBoneIndex );
//	BOOL				GetBoneByName		( const char* BoneName,	int* pBoneIndex,
//											geXForm3d* Attachment,	int* pParentBoneIndex);
};

#endif // !defined(AFX_SKELETONBODY_H__8435254F_2AFC_42A8_B855_9DE6C14B7F20__INCLUDED_)
