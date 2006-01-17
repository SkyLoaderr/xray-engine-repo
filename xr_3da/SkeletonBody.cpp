// SkeletonBody.cpp: implementation of the CSkeletonBody class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SkeletonBody.h"
#include "Skeleton_fmt.h"
#include "ffileops.h"

char* GetTextureName(char* src)
{
	char* dest = strrchr(src, ':');
	if (dest!=NULL){
		dest++;
		return dest;
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CSkeletonBody::CSkeletonBody( CStream* pStream )
{
	axf_header	axf_hdr;
	int i, cnt;
	char name[256];

	if (!pStream->ReadChunk(AXF_HEADER, &axf_hdr)) THROW;

	VERIFY((axf_hdr.format_version==SKELETON_VERSION));

	if (axf_hdr.has_body){
// read vertex
		if (pStream->FindChunk(AXF_BODY_VERTEX)){
			pStream->Read(&cnt, sizeof(cnt));
			XSkinVertexArray.resize(cnt);
			pStream->Read(XSkinVertexArray.begin(), sizeof(Body_XSkinVertex)*cnt);
		}
// read normals
		if (pStream->FindChunk(AXF_BODY_NORMAL)){
			pStream->Read(&cnt, sizeof(cnt));
			SkinNormalArray.resize(cnt);
			pStream->Read(SkinNormalArray.begin(), sizeof(Body_Normal)*cnt);
		}
// read bones
		if (pStream->FindChunk(AXF_BODY_BONES)){
			pStream->Read(&cnt, sizeof(cnt));
			BoneArray.resize(cnt);
			BoneNames.resize(cnt);
			pStream->Read(BoneArray.begin(), sizeof(Body_Bone)*cnt);
			for (i=0; i<cnt; i++){
				pStream->ReadStringZ(name);
				BoneNames[i] = strdup(name);
			}
		}
// read material
		if (pStream->FindChunk(AXF_BODY_MATERIAL)){
			pStream->Read(&cnt, sizeof(cnt));
			MaterialArray.resize(cnt);
			MaterialNames.resize(cnt);
			pStream->Read(MaterialArray.begin(), sizeof(Body_Material)*cnt);
			for (i=0; i<cnt; i++){
				pStream->ReadStringZ(name);
				char* tname = GetTextureName(name);
				VERIFY(tname);
				MaterialNames[i] = strdup(tname);
			}
		}
// read faces
		if (pStream->FindChunk(AXF_BODY_FACES)){
			pStream->Read(&cnt, sizeof(cnt));
			FaceArray.resize(cnt);
			pStream->Read(FaceArray.begin(), sizeof(Body_Triangle)*cnt);
		}
	}
}

CSkeletonBody::~CSkeletonBody()
{
	int i;
	for( i=0; i<BoneNames.size(); i++ )		_FREE(BoneNames[i]);
	for( i=0; i<MaterialNames.size(); i++ )	_FREE(MaterialNames[i]);
}

void CSkeletonBody::GetBone( int BoneIndex, const char **BoneName,
							geXForm3d *Attachment, int *ParentBoneIndex)
{
	VERIFY( Attachment != NULL );
	VERIFY( ParentBoneIndex != NULL );
	VERIFY( BoneName != NULL );

	VERIFY( BoneIndex >=0 );
	VERIFY( BoneIndex < BoneArray.size() );
	*Attachment = BoneArray[BoneIndex].AttachmentMatrix;
	*ParentBoneIndex = BoneArray[BoneIndex].ParentBoneIndex;
	*BoneName = BoneNames[BoneIndex];
}

BOOL CSkeletonBody::GetBoneIndexByName	( const char* BoneName, int* pBoneIndex )
{
	VERIFY(pBoneIndex);
	VERIFY( BoneName );

	for (int i=0; i<BoneNames.size(); i++)
		if (stricmp(BoneNames[i], BoneName)==0) {
			*pBoneIndex = i; return TRUE;
		}
	*pBoneIndex = UNDEFINE_BONE;
	return FALSE;
}
