// WeaponRail.cpp: implementation of the CWeaponRail class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WeaponRail.h"
#include "..\..\xr_creator.h"
#include "..\..\cameramanager.h"
#include "..\..\effectorrecoil.h"
#include "..\..\xr_cursor.h"
#include "..\..\environment.h"
#include "..\..\xr_area.h"
#include "..\..\collide\cl_defs.h"
#include "..\..\fstaticrender.h"
#include "..\..\fastspriterail.h"
#include "..\..\customactor.h"
#include "..\..\std_classes.h"
#include "..\..\xr_sndman.h"

#define TIME_TO_FIRE	0.1f
#define HIT_POWER		100

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWeaponRail::CWeaponRail(void *p) : CWeapon(p)
{
	pTrail			= NULL;
	hSndFire		= pSounds->Create3D("weapons\\rail_fire.wav");
	bCanFire		= true;
}

CWeaponRail::~CWeaponRail()
{
	Device.Texture.Delete(hWallmark);

	EXEC_SND(hSndFire,pSounds->Delete3D(hSndFire));
}

void CWeaponRail::FireStart()
{
	if (isFound() && !isWorking() && iAmmoElapsed && bCanFire) {
		bCanFire = false;
		CWeapon::FireStart();
		fTime = 0;
	}
}

void CWeaponRail::FireEnd()
{
	if (isWorking()) {
		CWeapon::FireEnd();
		/*
		VERIFY(pParent);
		if (pParent->bLocal)
			pCreator->Cameras.AddEffector(NULL,0);
		*/
	}
}

void CWeaponRail::OnMove()
{
	CWeapon::OnMove();
	if (isWorking()) {
		fTime-=Device.fTimeDelta;
		if (fTime<0) {
			VERIFY(pParent);
			fTime+=TIME_TO_FIRE;
			bCanFire = false;

			// real fire
			iAmmoElapsed--;
			if (iAmmoElapsed==0) pParent->FireEnd();

			BOOL bLocal = pParent->SUB_CLS_ID==CLSID_OBJECT_ACTOR;

//			if (bLocal)
//				pCreator->Cameras.AddEffector(
//					new CEffectorRecoil(.20f),Recoil_TIME);

			Fvector p1,d;
			pParent->GetFireParams(p1,d);
			float	dist=pCreator->Environment.fViewportFar;

		//	sounds
			Fvector r_pos; r_pos.direct(p1,d,2);
			EXEC_SND(hSndFire, pSounds->Play3DAtPos(hSndFire,r_pos));

			pParent->bEnabled = false;
			if (pCreator->ObjectSpace.RayPick( p1, d, &dist )) {
				if ( dist>0.5f) {
					SetRail		(p1,d,dist,TRUE);
					HitIfActor	();
				}
			} else {
				SetRail		(p1,d,pCreator->Environment.fViewportFar,FALSE);
			}
			pParent->bEnabled = true;
		}
	} else {
		fTime-=Device.fTimeDelta;
		if (fTime<0) {
			bCanFire = true;
		}
	}
	if (pTrail)
	{
		if (pTrail->isReadyForDestroy())	{_DELETE(pTrail);}
		else								pTrail->Update();
	}
}

void CWeaponRail::SetDefaults()
{
	CWeapon::SetDefaults();
	bCanFire		= true;
	iAmmoElapsed	= 1000;
}

void CWeaponRail::SetRail(Fvector &p1, Fvector &d, float dist, BOOL bWallmark)
{
	if (dist>50.f) dist=50.f;
	Fvector vStart,vEnd;
	vStart.direct(p1,d,.2f);
	vEnd.direct(p1,d,dist);
	_DELETE(pTrail);
	pTrail = new CFastSpriteRail(p1,vEnd);

	if (bWallmark) AddWallmark(vEnd);
}

void CWeaponRail::HitIfActor()
{
	CObject *pObject = pCreator->ObjectSpace.collide_list[0]->GetOwner();
	if (pObject) {
		if (pObject->SUB_CLS_ID==CLSID_OBJECT_ACTOR) {
			CCustomActor *pActor = (CCustomActor *)pObject;
			pParent->HitAnother(pActor,HIT_POWER);
		}
	}
}
