// WeaponMGun.cpp: implementation of the CWeaponMGun class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WeaponMGun.h"
#include "..\..\xr_creator.h"
#include "..\..\cameramanager.h"
#include "..\..\effectornoise.h"
#include "..\..\xr_cursor.h"
#include "..\..\environment.h"
#include "..\..\xr_area.h"
#include "..\..\collide\cl_obb.h"
#include "..\..\fstaticrender.h"
#include "..\..\FShotMarkVisual.h"
#include "..\..\customactor.h"
#include "..\..\std_classes.h"
#include "..\..\xr_sndman.h"
#include "..\..\xr_mac.h"

#define TIME_TO_FIRE		.15f
#define HIT_POWER			26
#define MGUN_SPRITE_SIZE	.15f

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWeaponMGun::CWeaponMGun(void *p) : CWeapon(p) //,CFastSprite("STD:mgun")
{
// sprite
//	FSprite S;
//	S.pos.set(0,0,0);
//	S.alpha = .0f;
//	S.size	= MGUN_SPRITE_SIZE;
//	sprites.push_back(S);
//	sprites.push_back(S);
//	CFastSprite::mTransform.identity();

	hSndFire = pSounds->Create3D("weapons\\mgun_fire.wav");
}

CWeaponMGun::~CWeaponMGun()
{
	EXEC_SND(hSndFire,pSounds->Delete3D(hSndFire));
}

void CWeaponMGun::FireStart()
{
	if (isFound() && !isWorking() && iAmmoElapsed) {
		CWeapon::FireStart();
		VERIFY(pParent);
		if (pParent->bLocal)
			pCreator->Cameras.AddEffector(new CEffectorNoise(0.06f),1e10);
		fTime = 0;
	}
}

void CWeaponMGun::FireEnd()
{
	if (isWorking()) {
		CWeapon::FireEnd();
		VERIFY(pParent);
		if (pParent->bLocal)
			pCreator->Cameras.AddEffector(NULL,0);
	}
//	sprites[0].alpha = sprites[1].alpha = 0.f;
}

void CWeaponMGun::OnMove()
{
	CWeapon::OnMove();
	if (isWorking()) {
		fTime-=Device.fTimeDelta;
		if (fTime<0) {
			VERIFY(pParent);
			fTime+=TIME_TO_FIRE;
			// real fire
			iAmmoElapsed--;
			Fvector p1,d;
			pParent->GetFireParams(p1,d);
			float	dist=pCreator->Environment.fViewportFar;

		// sprite
			Fvector fPos;
			fPos.direct(p1,d,.5f);
//			CFastSprite::mTransform.translate_over(fPos);
//			fRadius = 100.f;
//			fTTL = fTime;
//			sprites[0].alpha = sprites[1].alpha = 1.f;
//			sprites[0].angle = rnd()*PI_MUL_2;
//			sprites[1].angle = rnd()*PI_MUL_2;

		//	sounds
			Fvector r_pos; r_pos.mul(d, 1); r_pos.add(fPos);
			EXEC_SND(hSndFire, pSounds->Play3DAtPos(hSndFire,r_pos));
		//
			pParent->bEnabled = false;
			if (pCreator->ObjectSpace.RayPick( p1, d, &dist )) {
				if ( dist>0.5f) {
					AddShotmark(p1,d,dist-0.5f);
					HitIfActor();
				}
			}
			pParent->bEnabled = true;

			if (iAmmoElapsed==0) {
				pParent->FireEnd();
			}
		}else{
//			sprites[0].alpha -= fTime/TIME_TO_FIRE;
//			sprites[1].alpha -= sprites[0].alpha*0.75f;
		}
	}
	for (int i=0; i<shotmarks.size(); i++) 
	{
		if (shotmarks[i]->isReadyForDestroy()) {
//			pRender->ModelUnregister(shotmarks[i]);
			_DELETE(shotmarks[i]);
			shotmarks.erase(shotmarks.begin()+i);
			i--;
		}
	}
}

void CWeaponMGun::SetDefaults()
{
	CWeapon::SetDefaults();
	iAmmoElapsed = 100;
	bFound = true;
}

void CWeaponMGun::AddShotmark(Fvector &p1, Fvector &d, float dist)
{
	// Add to container
	/*
	FBasicVisual	*v = pCreator->Render.pVisuals->Get("shotmark.ogf");
	VERIFY			(v->Type=MT_SHOTMARK);
	FShotMarkVisual *p = (FShotMarkVisual *)v->CreateInstance();
	p->Copy			(v);
	pCreator->Render.AddDynamic(
		p,&(p->mSelfTransform),&(p->vPosition));
	

	// Calc matrix & center
	Fvector		norm,dir;
	
	p->vPosition.direct(p1,d,dist);
	norm.set(0,1,0);
//	norm.set(pCreator->ObjectSpace.tris_list[0].N);
	dir.sub	(pCreator->ObjectSpace.tris_list[0].p1,pCreator->ObjectSpace.tris_list[0].p2);
	dir.normalize();
	
	p->mSelfTransform.rotation			(dir,norm);
	p->mSelfTransform.translate_over	(p->vPosition);
	
	shotmarks.push_back(p);
	*/
}

void CWeaponMGun::HitIfActor()
{
	CObject *pObject = pCreator->ObjectSpace.collide_list[0]->GetOwner();
	if (pObject) {
		if (pObject->SUB_CLS_ID==CLSID_OBJECT_ACTOR) {
			CCustomActor *pActor = (CCustomActor *)pObject;
			pActor->Hit(HIT_POWER);
		}
	}
}
