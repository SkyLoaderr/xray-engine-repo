// WeaponRail.cpp: implementation of the CWeaponRail class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "..\cameramanager.h"
#include "..\effectorrecoil.h"
#include "..\environment.h"
#include "..\xr_area.h"
#include "..\collide\cl_defs.h"
#include "..\fastspriterail.h"

#include "entity.h"
#include "WeaponRail.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWeaponRail::CWeaponRail() : CWeapon("rail")
{
	pTrail			= NULL;
	pSounds->Create3D(sndFire,"weapons\\rail_fire.wav");
	bCanFire		= true;
	iAmmoElapsed	= 10;
}

CWeaponRail::~CWeaponRail()
{
	pSounds->Delete3D(sndFire);
}

void CWeaponRail::FireStart()
{
	if (!IsWorking() && IsValid() && bCanFire) {
		bCanFire = false;
		CWeapon::FireStart();
		fTime = 0;
	}
}

void CWeaponRail::FireEnd()
{
	if (IsWorking()) {
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
//	CWeapon::OnMove();
	if (IsWorking()) {
		fTime-=Device.fTimeDelta;
		if (fTime<0) {
//			VERIFY(pParent);
			fTime+=iHitPower;
			bCanFire = false;

			// real fire
			iAmmoElapsed--;
			if (iAmmoElapsed==0) m_pParent->g_fireEnd();

//			BOOL bLocal = pParent->SUB_CLS_ID==CLSID_OBJECT_ACTOR;

//			if (bLocal)
//				pCreator->Cameras.AddEffector(
//					new CEffectorRecoil(.20f),Recoil_TIME);

			Fvector p1,d;
			m_pParent->g_fireParams(p1,d);
			float	dist=pCreator->Environment.Current.Far;

		//	sounds
			Fvector r_pos; r_pos.direct(p1,d,2);
			pSounds->Play3DAtPos(sndFire,r_pos);

			m_pParent->bEnabled = false;
			Collide::ray_query RQ;
			if (pCreator->ObjectSpace.RayPick( p1, d, dist, RQ )) {
				if ( RQ.range>0.3f) 
				{
					SetRail		(p1,d,RQ,TRUE);
					if (RQ.O)	{
						if (RQ.O->CLS_ID == CLSID_ENTITY)
							((CEntity*)RQ.O)->Hit(iHitPower,d,m_pParent);
					}
				}
			} else {
				RQ.range	= pCreator->Environment.Current.Far;
				SetRail		(p1,d,RQ,FALSE);
			}
			m_pParent->bEnabled = true;
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

void CWeaponRail::SetRail(Fvector &p1, Fvector &d, Collide::ray_query& R, BOOL bWallmark)
{
	float dist = R.range;
	if (dist>50.f) dist=50.f;
	Fvector vStart,vEnd;
	vStart.direct(p1,d,.2f);
	vEnd.direct(p1,d,dist);
	_DELETE(pTrail);

	if (bWallmark) AddShotmark(d,vEnd,R);
}

