//---------------------------------------------------------------------------
#include 	"stdafx.h"
#pragma hdrstop

#include 	"SkeletonRigid.h"
#include	"SkeletonX.h"
#include	"fmesh.h"

extern int	psSkeletonUpdate;

//////////////////////////////////////////////////////////////////////////
// BoneInstance methods
void CBoneInstanceRigid::construct	()
{
	ZeroMemory					(this,sizeof(*this));
	mTransform.identity			();
	mRenderTransform.identity	();
	Callback_overwrite			= FALSE;
}

void CSkeletonRigid::CalculateBones			(BOOL bForceExact)
{
	// early out.
	// check if the info is still relevant
	// skip all the computations - assume nothing changes in a small period of time :)
	if		(Device.dwTimeGlobal == UCalc_Time)										return;	// early out for "fast" update
	if		(!bForceExact && (Device.dwTimeGlobal < (UCalc_Time + UCalc_Interval)))	return;	// early out for "slow" update
	if		(Update_Visibility)									Visibility_Update	();

	// here we have either:
	//	1:	timeout elapsed
	//	2:	exact computation required
	UCalc_Time			= Device.dwTimeGlobal;

	// exact computation
	// Calculate bones
	Device.Statistic.Animation.Begin();
	((CBoneDataRigid*)(*bones)[iRoot])->Calculate		(this,&Fidentity);
	Device.Statistic.Animation.End	();

	// Calculate BOXes/Spheres if needed
	UCalc_Visibox++; 
	if (UCalc_Visibox>=psSkeletonUpdate) 
	{
		// mark
		UCalc_Visibox		= -(::Random.randI(16));

		// the update itself
		Fbox	Box; Box.invalidate();
		for (u32 b=0; b<bones->size(); b++)
		{
			if			(!LL_GetBoneVisible(u16(b)))		continue;
			Fobb&		obb		= (*bones)[b]->obb;
			Fmatrix&	Mbone	= bone_instances[b].mTransform;
			Fmatrix		Mbox;	obb.xform_get(Mbox);
			Fmatrix		X;		X.mul_43(Mbone,Mbox);
			Fvector&	S		= obb.m_halfsize;

			Fvector			P,A;
			A.set( -S.x,	-S.y,	-S.z ); X.transform_tiny(P,A); Box.modify(P);
			A.set( -S.x,	-S.y,	 S.z ); X.transform_tiny(P,A); Box.modify(P);
			A.set(  S.x,	-S.y,	 S.z ); X.transform_tiny(P,A); Box.modify(P);
			A.set(  S.x,	-S.y,	-S.z ); X.transform_tiny(P,A); Box.modify(P);
			A.set( -S.x,	 S.y,	-S.z ); X.transform_tiny(P,A); Box.modify(P);
			A.set( -S.x,	 S.y,	 S.z ); X.transform_tiny(P,A); Box.modify(P);
			A.set(  S.x, 	 S.y,	 S.z ); X.transform_tiny(P,A); Box.modify(P);
			A.set(  S.x, 	 S.y,	-S.z ); X.transform_tiny(P,A); Box.modify(P);
		}

		// previous frame we have updated box - update sphere
		vis.box.min.average	(Box.min);
		vis.box.max.average	(Box.max);
		vis.box.getsphere	(vis.sphere.P,vis.sphere.R);
	}

	//
	if (Update_Callback)	Update_Callback(this);
}

void CBoneDataRigid::Calculate	(CKinematics* K, Fmatrix *parent)
{
    if (K->LL_GetBoneVisible(SelfID)){
		CBoneInstance& INST			= (CBoneInstance&)K->LL_GetBoneInstance(SelfID);
        if (INST.Callback_overwrite){
			if (INST.Callback)		INST.Callback(&INST);
        } else {
            // Build matrix
            INST.mTransform.mul_43	(*parent,bind_transform);
            if (INST.Callback)		INST.Callback(&INST);
        }
        INST.mRenderTransform.mul_43(INST.mTransform,m2b_transform);

        // Calculate children
        for (xr_vector<CBoneData*>::iterator C=children.begin(); C!=children.end(); C++)
            ((CBoneDataRigid*)(*C))->Calculate(K,&INST.mTransform);
	}
}
