//---------------------------------------------------------------------------
#include 	"stdafx.h"
#pragma hdrstop

#include 	"SkeletonCustom.h"

extern int	psSkeletonUpdate;

void CKinematics::CalculateBones			(BOOL bForceExact)
{
	// early out.
	// check if the info is still relevant
	// skip all the computations - assume nothing changes in a small period of time :)
	if		(Device.dwTimeGlobal == UCalc_Time)										return;	// early out for "fast" update
	UCalc_mtlock	lock	;
	OnCalculateBones		();
	if		(!bForceExact && (Device.dwTimeGlobal < (UCalc_Time + UCalc_Interval)))	return;	// early out for "slow" update
	if		(Update_Visibility)									Visibility_Update	();

	_DBG_SINGLE_USE_MARKER;
	// here we have either:
	//	1:	timeout elapsed
	//	2:	exact computation required
	UCalc_Time			= Device.dwTimeGlobal;

	// exact computation
	// Calculate bones
#ifdef DEBUG
	Device.Statistic->Animation.Begin();
#endif
	Bone_Calculate					(bones->at(iRoot),&Fidentity);
#ifdef DEBUG
	Device.Statistic->Animation.End	();
#endif

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

void CKinematics::Bone_Calculate	(CBoneData* bd, Fmatrix *parent)
{
	u16 SelfID						= bd->GetSelfID();
    if (LL_GetBoneVisible(SelfID)){
		CBoneInstance& INST			= LL_GetBoneInstance(SelfID);
        if (INST.Callback_overwrite){
			if (INST.Callback)		INST.Callback(&INST);
        } else {
            // Build matrix
            INST.mTransform.mul_43	(*parent,bd->bind_transform);
            if (INST.Callback)		INST.Callback(&INST);
        }
        INST.mRenderTransform.mul_43(INST.mTransform,bd->m2b_transform);

        // Calculate children
        for (xr_vector<CBoneData*>::iterator C=bd->children.begin(); C!=bd->children.end(); C++)
			Bone_Calculate			(*C,&INST.mTransform);
	}
}
