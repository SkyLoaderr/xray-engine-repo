// WeaponRail.cpp: implementation of the CWeaponRail class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WeaponRocket.h"
#include "..\..\xr_creator.h"
#include "..\..\cameramanager.h"
#include "..\..\effectorrecoil.h"
#include "..\..\xr_cursor.h"
#include "..\..\environment.h"
#include "..\..\xr_area.h"
#include "..\..\collide\cl_obb.h"
#include "..\..\fstaticrender.h"
#include "..\..\customactor.h"
#include "..\..\std_classes.h"
#include "..\..\xr_sndman.h"
#include "..\..\xr_mac.h"

#define TIME_TO_FIRE		1.f
#define HIT_POWER			45
#define ROCKET_LIFETIME		5.f		// c
#define ROCKET_SPEED		20.f	// м/с
#define ROCKET_RECOIL_DIST	.1f		// m
#define ROCKET_RECOIL_TIME	.1f		// c
#define ROCKET_TRAILTIME	.03f	// c
#define TRAIL_SPRITE_SIZE	.35f	// м
#define TRAIL_SPRITE_HIDE_SPEED	1.f	// ед/с
#define LIGHT_BASE			5.f
#define EXPLODE_TIME		1.f
#define EXPLODE_LIGHT_SCALE	3.f
#define EXPLODE_LIGHT_ATT	1.f
#define EXPLODE_RADIUS		1.f
#define EXPLODE_ADDSPRITE	.03f
#define EXPLODE_SPRITE_SIZE	1.55f	// м

CRocket::CRocket(CWeaponRocket* owner,Fvector &dir,Fvector &pos) // : CFastSprite("STD:trail")
{
	R_ASSERT(0);
	/*
	VERIFY(owner);
	pOwner = owner;
	mSelfTransform.identity();
	fLifeTime = ROCKET_LIFETIME;
	fTrailTime= ROCKET_TRAILTIME*5.f;
	fDir.set(dir);
	fPos.direct(pos,dir,1.f);
	Fvector norm;
	norm.set(0,1,0);
#pragma message("set correct normal for rocket")	
	
	mSelfTransform.rotation(fDir, norm);
	mSelfTransform.translate_over(fPos);
	state = rcFly;
	pVisual = pRender->LoadModel("rocket.ogf");
	pVisual->bVisible = true;
	
	mTransform.identity();
	fRadius = 100;
	fTTL = ROCKET_LIFETIME;
	
	light.type		= D3DLIGHT_POINT;
	light.diffuse.set(3.f, -.1f, -.1f, 0.f); 
	light.specular.set(3.f, -.1f, -.1f, 0.f); 
	light.ambient.set(1,0,0,0); 
    light.range		= LIGHT_BASE;
    light.attenuation0	= 1.0f;
    light.attenuation1	= 0.1f;
    light.attenuation2	= 0.1f;
	
//	Device.Lights.AddDynamic(&light);
	*/
}

CRocket::~CRocket()
{	
//	Device.Lights.RemoveDynamic(&light);
//	pRender->UnregisterDynamic(this);
}

void CRocket::OnMove()
{
	R_ASSERT(0);
	/*
	switch (state){
	case rcFly:
		{
			fLifeTime -= Device.fTimeDelta;
			if (fLifeTime<=0){
				Explode();
				break;
			}
			fDir.set(pOwner->ray_dir);
			Fvector motion;
			motion.mul(fDir,ROCKET_SPEED*Device.fTimeDelta);
			
			float	dist=motion.magnitude();
			if (pOwner->pParent->cfModel) pOwner->pParent->cfModel->Enable(false);
			if (pCreator->ObjectSpace.RayPick( fPos, fDir, &dist, true )){
				Explode		();
				HitIfActor	();
			}
			if (pOwner->pParent->cfModel) pOwner->pParent->cfModel->EnableRollback();
			
			fPos.add(motion);
			mSelfTransform.translate_over(fPos);
			light.position.set(fPos);
			
			// delete invisible trail ???
			for(int i=sprites.size()-1; i>=0; i--){
				FSprite* s = &(sprites[i]);
				s->alpha -= TRAIL_SPRITE_HIDE_SPEED*Device.fTimeDelta;
				//			if (s->alpha<=0) s->alpha=0;
				if (s->alpha<=0) sprites.erase(sprites.begin()+i);
			}
			fTrailTime -= Device.fTimeDelta;
			if (fTrailTime<=0) AddTrail();
		}
		break;
	case rcExplosion: 
		{
			fExplodeTime -= Device.fTimeDelta;
			if (fExplodeTime<0)	state = rcWaitingDestroy; 

			// delete invisible trail ???
			for(int i=sprites.size()-1; i>=0; i--){
				FSprite* s = &(sprites[i]);
				s->alpha -= TRAIL_SPRITE_HIDE_SPEED*Device.fTimeDelta;
				//			if (s->alpha<=0) s->alpha=0;
				if (s->alpha<=0) sprites.erase(sprites.begin()+i);
			}
			light.range = LIGHT_BASE + (1.f-fExplodeTime/EXPLODE_TIME)*EXPLODE_LIGHT_SCALE;
			light.attenuation0 += (1.f-fExplodeTime/EXPLODE_TIME)*EXPLODE_LIGHT_ATT;

			fExplodeAddS -= Device.fTimeDelta;
			if (fExplodeAddS<0) {
				// Add explode sprite
				fExplodeAddS += EXPLODE_ADDSPRITE;
				Fvector sdir;
				sdir.set(rnd(),rnd(),rnd());
				sdir.mul(2.f-fExplodeTime/EXPLODE_TIME);

				FSprite S;
				S.pos.direct(fPos,sdir,EXPLODE_RADIUS);
				S.alpha = 1.0f;
				S.angle = rnd()*PI_MUL_2;
				S.size	= (1+rnd()*.1f)*EXPLODE_SPRITE_SIZE; //*(1-sdir.magnitude());
				sprites.push_back(S);
			}

		}
	case rcWaitingDestroy:
		{
			// delete invisible trail ???
			for(int i=sprites.size()-1; i>=0; i--){
				FSprite* s = &(sprites[i]);
				s->alpha -= TRAIL_SPRITE_HIDE_SPEED*Device.fTimeDelta;
				//			if (s->alpha<=0) s->alpha=0;
				if (s->alpha<=0) sprites.erase(sprites.begin()+i);
			}
			if (sprites.size()==0) state = rcDestroy;
		}
		break;
	}
	*/
}

void CRocket::AddTrail()
{
	/*
	fTrailTime+= ROCKET_TRAILTIME;
	FSprite S;
	S.pos.set(fPos);
	S.alpha = 1.0f;
	S.angle = rnd()*PI_MUL_2;
	S.size	= TRAIL_SPRITE_SIZE;
	sprites.push_back(S);
	*/
}

void CRocket::Explode()
{
	state = rcExplosion;
	fExplodeTime = EXPLODE_TIME;
	fExplodeAddS = EXPLODE_ADDSPRITE;
	//	sounds
	EXEC_SND(pOwner->hSndExpl, pSounds->Play3DAtPos(pOwner->hSndExpl,fPos));
}

void CRocket::HitIfActor()
{
	CObject *pObject = _COLLIDE_LIST[0]->GetOwner();
	if (pObject) {
		if (pObject->SUB_CLS_ID==CLSID_OBJECT_ACTOR) {
			CCustomActor *pActor = (CCustomActor *)pObject;
			pActor->Hit(HIT_POWER);
		}
	}
}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWeaponRocket::CWeaponRocket(void *p) : CWeapon(p)
{
	hSndFire = pSounds->Create3D("weapons\\rocket_fire.wav");
	hSndExpl = pSounds->Create3D("weapons\\rocket_expl.wav");
}

CWeaponRocket::~CWeaponRocket()
{
	EXEC_SND(hSndFire,pSounds->Delete3D(hSndFire));
	EXEC_SND(hSndExpl,pSounds->Delete3D(hSndExpl));
	while (Rockets.size()){
		CRocket* r = Rockets[0];
		Rockets.erase(Rockets.begin());;
		_DELETE(r);
	}
}

void CWeaponRocket::FireStart()
{
	if (!isWorking() && iAmmoElapsed) {
		CWeapon::FireStart();
		fTime = 0;
	}
}

void CWeaponRocket::FireEnd()
{
	if (isWorking())
		CWeapon::FireEnd();
}

void CWeaponRocket::OnMove()
{
	CWeapon::OnMove();
	if (isWorking()) {
		fTime-=Device.fTimeDelta;
		if (fTime<0) {
			VERIFY(pParent);
			fTime+=TIME_TO_FIRE;
			
			// real fire
			iAmmoElapsed--;
			if (iAmmoElapsed==0) pParent->FireEnd();
			
			if (pParent->bLocal)
				pCreator->Cameras.AddEffector(
				new CEffectorRecoil(ROCKET_RECOIL_DIST),ROCKET_RECOIL_TIME);
			
			Fvector pos,dir;
			pParent->GetFireParams(pos,dir);
			
			AddRocket(dir,pos);
			
			//	sounds
			Fvector r_pos; r_pos.mul(dir, 2); r_pos.add(pos);
			EXEC_SND(hSndFire, pSounds->Play3DAtPos(hSndFire,r_pos));
		}
	}
	Fvector p;
	pParent->GetFireParams(p,ray_dir);
	// delete unused rocket
	for(int i=0; i<Rockets.size(); i++){
		CRocket* r = Rockets[i];
		r->OnMove();
		if (r->state==rcDestroy){
			Rockets.erase(Rockets.begin()+i);
			_DELETE(r);
		}
	}
}

void CWeaponRocket::SetDefaults()
{
	CWeapon::SetDefaults();
	iAmmoElapsed=100;
}

void CWeaponRocket::AddRocket	(Fvector &p, Fvector &d)
{
//	Rockets.push_back(new CRocket(this,p,d));
}
