#include "stdafx.h"
#include "effectornoise.h"
#include "..\fstaticrender.h"
#include "..\bodyinstance.h"
#include "..\xr_tokens.h"
#include "..\3DSound.h"
#include "..\PSObject.h"
#include "..\PSVisual.h"
#include "..\xr_trims.h"
#include "hudmanager.h"

#include "WeaponHUD.h"
#include "WeaponProtecta.h"
#include "entity.h"
#include "xr_weapon_list.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CWeaponProtecta::CWeaponProtecta() : CWeapon("PROTECTA")
{
	pSounds->Create3D(sndShoot,		 "weapons\\protecta_fire");
	pSounds->Create3D(sndRicochet[0],"weapons\\ric1");
	pSounds->Create3D(sndRicochet[1],"weapons\\ric2");
	pSounds->Create3D(sndRicochet[2],"weapons\\ric3");
	pSounds->Create3D(sndRicochet[3],"weapons\\ric4");
	pSounds->Create3D(sndRicochet[4],"weapons\\ric5");

	iShotCount		= 0;

	vLastFP.set		(0,0,0);
	vLastFD.set		(0,0,0);

	fTime			= 0;
}

CWeaponProtecta::~CWeaponProtecta()
{
	// sounds
	pSounds->Delete3D(sndShoot);
	for (int i=0; i<SND_RIC_COUNT; i++) pSounds->Delete3D(sndRicochet[i]);
	
	_DELETE			(m_pShootPS);
}

void CWeaponProtecta::Load(CInifile* ini, const char* section){
	inherited::Load(ini, section);
	R_ASSERT(m_pHUD);
	
	m_pShootPS		= new CPSObject("protecta_smoke");

	vFirePoint		= ini->ReadVECTOR(section,"fire_point");
	iShotCount		= ini->ReadINT(section,"shot_count");
}

void CWeaponProtecta::FireStart()
{
	if (!IsWorking() && IsValid()){ 
		CWeapon::FireStart();
		st_target	= eShoot;
	}
}

void CWeaponProtecta::FireEnd(){
	if (IsWorking()){
		CWeapon::FireEnd();
		m_pHUD->FireEnd();
		st_target = eIdle;
	}
}


void CWeaponProtecta::UpdateXForm(BOOL bHUDView)
{
	if (Device.dwFrame!=dwXF_Frame)
	{
		dwXF_Frame = Device.dwFrame;

		if (bHUDView) {
			if (m_pHUD)	{
				Fmatrix			trans;
				trans.set(
					Device.vCameraRight,
					Device.vCameraTop,
					Device.vCameraDirection,
					Device.vCameraPosition
					);
				m_pHUD->UpdatePosition(trans);
			}
		} else {
			Fmatrix mRes;
			PKinematics V	= PKinematics(m_pParent->Visual());
			V->Calculate	();
			Fmatrix& mL		= V->LL_GetTransform(m_pContainer->m_iACTboneL);
			Fmatrix& mR		= V->LL_GetTransform(m_pContainer->m_iACTboneR);
			
			Fvector			R,D,N;
			D.sub			(mL.c,mR.c);	D.normalize_safe();
			R.crossproduct	(mR.j,D);	R.normalize_safe();
			N.crossproduct	(D,R);	N.normalize_safe();
			mRes.set		(R,N,D,mR.c);
			mRes.mul2		(m_pParent->clTransform);
			UpdatePosition	(mRes);
		}
	}
}

void CWeaponProtecta::UpdateFP(BOOL bHUDView)
{
	if (Device.dwFrame!=dwFP_Frame) 
	{
		dwFP_Frame = Device.dwFrame;

		// update animation
		PKinematics V			= bHUDView?PKinematics(m_pHUD->Visual()):0;
		if (V) V->Calculate		();

		// fire point&direction
		UpdateXForm				(bHUDView);
		Fmatrix& fire_mat		= bHUDView?V->LL_GetTransform(m_pHUD->iFireBone):precalc_identity;
		Fmatrix& parent			= bHUDView?m_pHUD->Transform():clTransform;
		Fvector& fp				= bHUDView?m_pHUD->vFirePoint:vFirePoint;
		fire_mat.transform_tiny	(vLastFP,fp);
		parent.transform_tiny	(vLastFP);
		vLastFD.set				(0.f,0.f,1.f);
		parent.transform_dir	(vLastFD);
	}
}

void CWeaponProtecta::Update(float dt, BOOL bHUDView)
{
	BOOL bShot = false;

	// on state change
	if (st_target!=st_current)
	{
		if (st_target == eShoot) UpdateFP(bHUDView);

		switch(st_target){
		case eIdle:
			break;
		case eShoot:
			pSounds->Play3DAtPos(sndShoot,vLastFP,true);
			break;
		}
		st_current=st_target;
	}

	// cycle update
	if (st_current == eShoot) UpdateFP(bHUDView);

	switch (st_current)
	{
	case eIdle:
		fTime	-= dt;
		if (fTime<0)	fTime = 0;
		break;
	case eShoot:
		{
			fTime-=dt;
			Fvector p1_base, d_base;
			m_pParent->g_fireParams(p1_base,d_base);
			
			while (fTime<0)
			{
				fTime			+=fTimeToFire;
				UpdateFP		(bHUDView);

				// play smoke
				m_pShootPS->Stop		();
				m_pShootPS->m_Emitter.m_ConeDirection.set	(vLastFD);
				m_pShootPS->PlayAtPos	(vLastFP);

				BOOL			bHit = FALSE;
				for (int i=0; i<iShotCount; i++)
				{
					Fvector p1=p1_base, d=d_base;
					bHit |=		FireTrace(p1,vLastFP,d);
				}
//				if (bHit)		pSounds->Play3DAtPos(sndRicochet[Random.randI(SND_RIC_COUNT)], vEnd,false);
				iAmmoElapsed	--;
		 		if (iAmmoElapsed==0) { m_pParent->g_fireEnd(); break; }
				m_pHUD->Shoot	();
			}

			// sound fire loop
			if (sndShoot.feedback) sndShoot.feedback->SetPosition(vLastFP);
		}
		break;
	}

	m_pHUD->UpdateAnimation();
}

void CWeaponProtecta::Render(BOOL bHUDView)
{
	UpdateXForm	(bHUDView);
	if (bHUDView)
	{ 
		// Interpolate lighting
		float l_f	= Device.fTimeDelta*fLightSmoothFactor;
		float l_i	= 1.f-l_f;
		float&	LL	= m_pParent->AI_Lighting;
		NodeCompressed*	NODE = m_pParent->AI_Node;
		float	CL	= NODE?float(NODE->light):255;
		LL			= l_i*LL + l_f*CL; 

		// HUD render
		if (m_pHUD){ 
			Level().HUD()->RenderModel(m_pHUD->Visual(),m_pHUD->Transform(),iFloor(LL));
			// Render PS Shoot
			Level().HUD()->RenderModel(m_pShootPS->Visual(),precalc_identity,255);
		}
	}
	else
	{
		// Actor render
		::Render.set_Transform		(&svTransform);
		::Render.set_LightLevel		(iFloor(m_pParent->AI_Lighting));
		::Render.add_leafs_Dynamic	(Visual());

		// Render PS Shoot
		::Render.add_leafs_Dynamic	(m_pShootPS->Visual());
	}
}

void CWeaponProtecta::SetDefaults()
{
	CWeapon::SetDefaults();
	iAmmoElapsed = 0;
}

void CWeaponProtecta::Hide		()
{
	inherited::Hide				();
	signal_HideComplete			();
}
void CWeaponProtecta::Show		()
{
	inherited::Show				();
}

void CWeaponProtecta::FireShotmark(const Fvector &vDir, const Fvector &vEnd, Collide::ray_query& R) 
{
	inherited::FireShotmark	(vDir, vEnd, R);

	// particles
	Fvector N,D;
	RAPID::tri* pTri	= pCreator->ObjectSpace.GetStaticTris()+R.element;
	N.mknormal			(pTri->V(0),pTri->V(1),pTri->V(2));
	D.reflect			(vDir,N);

	CSector* S			= ::Render.getSector(pTri->sector);

	// stones
	CPSObject* PS		= new CPSObject("stones",S,true);
	PS->m_Emitter.m_ConeDirection.set(D);
	PS->PlayAtPos		(vEnd);

	// smoke
	PS					= new CPSObject("smokepuffs_1",S,true);
	PS->m_Emitter.m_ConeDirection.set(D);
	PS->PlayAtPos		(vEnd);
}
