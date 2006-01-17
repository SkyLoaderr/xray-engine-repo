// Skeleton.cpp: implementation of the CSkeleton class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Skeleton.h"
#include "Skeletonbody.h"
#include "Skeletonmotions.h"
#include "Skeleton_fmt.h"
#include "pose.h"

#include "x_ray.h"
#include "xr_smallfont.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSkeleton::CSkeleton( CStream* pStream )
{
	pCurrentMotion		= NULL;
	pSkeletonMotions	= NULL;
	pPose				= NULL;

	pSkeletonMotions = new CSkeletonMotions( pStream );

	// Create pose
	pPose	= gePose_Create(); VERIFY(pPose);
	int i, BoneCount, ParentBone, Index;
	BOOL bRes;
	const char *Name;
	geXForm3d Attachment;
	BoneCount = pSkeletonBody->BoneArray.size();
	for (i=0; i<BoneCount; i++){
		pSkeletonBody->GetBone( i, &Name,&Attachment, &ParentBone );
		bRes = gePose_AddJoint( pPose, ParentBone,Name,&Attachment,&Index);
		VERIFY(bRes);
	}
}

CSkeleton::~CSkeleton()
{
	_DELETE(pSkeletonBody);
	_DELETE(pSkeletonMotions);
	if (pPose)			gePose_Destroy	(&pPose);
}

BOOL CSkeleton::Attach( CSkeleton* pSkeleton, const char* SlaveBoneName, const char* MasterBoneName )
{
	VERIFY(pSkeleton);
	VERIFY(MasterBoneName);
	VERIFY(pSkeleton->pPose);
	VERIFY(pPose);

	int MasterBoneIndex, SlaveBoneIndex;
	BOOL bRes;
	geXForm3d Dummy;
	geXForm3d_SetIdentity(&Dummy);

	bRes = pSkeletonBody->GetBoneIndexByName(MasterBoneName,&MasterBoneIndex);
	VERIFY(bRes);
	if (SlaveBoneName){
		bRes = pSkeleton->pSkeletonBody->GetBoneIndexByName(SlaveBoneName,&SlaveBoneIndex);
		VERIFY(bRes);
	}else{
		SlaveBoneIndex = 0;
	}

	return gePose_Attach( pSkeleton->pPose, SlaveBoneIndex,	pPose, MasterBoneIndex, &Dummy);
}

void CSkeleton::Detach( CSkeleton* pSkeleton )
{
	VERIFY(pSkeleton->pPose);
	gePose_Detach( pSkeleton->pPose );
}

void CSkeleton::PlayMotion(int id, BOOL bLoop)
{
// если есть незаконченное действие положить его в Blend
	CMotion* new_motion = pSkeletonMotions->GetMotion(id);
	if (pCurrentMotion && pCurrentMotion->bActive){
		if (pCurrentMotion==new_motion){
			pCurrentMotion->bLoopPlay = bLoop;
			return;
		}
		pCurrentMotion->SetBlended();
		BlendedMotions.push_back(pCurrentMotion);
	}

	pCurrentMotion = new_motion;
	pCurrentMotion->Play(bLoop);

	vector<CMotion*>::iterator new_end;
	new_end = remove(BlendedMotions.begin(), BlendedMotions.end(),pCurrentMotion);
	BlendedMotions.erase(new_end,BlendedMotions.end());
}

void CSkeleton::PlayMotion(const char* name, BOOL bLoop)
{
	int id;
	id = pSkeletonMotions->GetMotionIdByName(name);
	VERIFY(id!=-1);
	PlayMotion(id, bLoop);
}

void CSkeleton::OnMove()
{
	float dt = Device.fTimeDelta;
	if (pCurrentMotion){
#ifdef DEBUG
		pApp->pFont->Size(0.025f);
		pApp->pFont->Out(-1,0,"Motion: %s",pCurrentMotion->motion->Name);
#endif
		pCurrentMotion->Step(dt);
		gePose_SetMotion( pPose,pCurrentMotion->motion,pCurrentMotion->start_time + pCurrentMotion->motion_time,0);
	}
	for (int i=0; i<BlendedMotions.size(); i++){
		CMotion* M = BlendedMotions[i];
		if (M->Step(dt)){
			gePose_BlendMotion	(pPose, M->motion, M->start_time + M->motion_time,
								NULL,	M->blend_amount, GE_POSE_BLEND_HERMITE);
		}else{
			vector<CMotion*>::iterator new_end;
			new_end = remove(BlendedMotions.begin(), BlendedMotions.end(),M);
			BlendedMotions.erase(new_end,BlendedMotions.end());
		}
	}
}

/*
			MotionIndex = GameMgr_GetMotionIndexDef(Client->GMgr, Player->MotionIndex);

			assert(MotionIndex);
			assert(MotionIndex->Active == GE_TRUE);

			// Set the base motion and transform
			if (MotionIndex->Active)
			{
				float FrameTime;

				if (Player->FrameTime > Player->OldFrameTime)
					FrameTime = Player->OldFrameTime + (Player->FrameTime - Player->OldFrameTime) * Ratio;
				else
					FrameTime = Player->FrameTime;

				MotionName = MotionIndex->MotionName;

				Motion = geActor_GetMotionByName(Player->ActorDef, MotionName);

				if (!Motion)
					GenVS_Error("Client_UpdateSinglePlayer:  geActor_GetMotionByName1 failed...");

				geActor_SetPose(Player->Actor, Motion, FrameTime, &XForm);
			}

		// Go through and blend in all the motion hacks...
		{
			int32				i;
			GPlayer_MotionData	*pMotionData;

			pMotionData = Player->MotionData;

			for (i=0; i< Player->NumMotionData; i++, pMotionData++)
			{
				MotionIndex = GameMgr_GetMotionIndexDef(Client->GMgr, pMotionData->MotionIndex);

				assert(MotionIndex->Active == GE_TRUE);

				if (!MotionIndex->Active)
					continue;

				Motion = geActor_GetMotionByName(Player->ActorDef, MotionIndex->MotionName);

				if (!Motion)
					GenVS_Error("Client_UpdateSinglePlayer:  geActor_GetMotionByName2 failed...");

				geActor_BlendPose(Player->Actor, Motion, pMotionData->MotionTime, &XForm, 1.0f);
			}
		}

		// Go through and set all the xform hacks for the bones...
		{
			int32				i;
			GPlayer_XFormData	*pXFormData;
			GameMgr_BoneIndex	*pBoneIndex;
			geXForm3d			BoneXForm;

			pXFormData = Player->XFormData;

			for (i=0; i< Player->NumXFormData; i++, pXFormData++)
			{
				pBoneIndex = GameMgr_GetBoneIndex(Client->GMgr, pXFormData->BoneIndex);

				assert(pBoneIndex->Active == GE_TRUE);

				if (!pBoneIndex->Active)
					continue;

				geActor_GetBoneAttachment(Player->Actor, pBoneIndex->BoneName, &BoneXForm);

				pXFormData->XForm.Translation = BoneXForm.Translation;

				geActor_SetBoneAttachment(Player->Actor, pBoneIndex->BoneName, &pXFormData->XForm);
			}
		}

	}

*/
