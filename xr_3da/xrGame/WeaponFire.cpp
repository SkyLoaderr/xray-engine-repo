// WeaponFire.cpp: implementation of the CWeapon class.
// function responsible for firing with CWeapon
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "effectorshot.h"
#include "Weapon.h"
#include "WeaponHUD.h"

#include "ParticlesObject.h"

#include "HUDManager.h"
#include "entity.h"
#include "actor.h"


#define FLAME_TIME 0.05f


BOOL CWeapon::FireTrace		(const Fvector& P, const Fvector& Peff, Fvector& D)
{
	R_ASSERT(m_magazine.size());

	CCartridge &l_cartridge = m_magazine.top();

	// direct it by dispersion factor
	Fvector dir;
	//dir.random_dir(D,(fireDispersionBase+fireDispersion*fireDispersion_Current)*GetPrecision(),Random);
	//dir.random_dir(D,(fireDispersion*fireDispersion_Current)*
	//				  GetPrecision()*
	//				  GetConditionDispersionFactor(),Random);
	float fire_disp = fireDispersion*fireDispersion_Current*
							GetPrecision()*GetConditionDispersionFactor();

	// increase dispersion
	fireDispersion_Current += fireDispersion_Inc;
	clamp(fireDispersion_Current,0.f,1.f);

	//повысить изношенность оружия
	ChangeCondition(-conditionDecreasePerShot);


	BOOL bResult = false;


	for(int i = 0; i < l_cartridge.m_buckShot; ++i) 
	{
		//dir1.random_dir(dir, fireDispersionBase * l_cartridge.m_kDisp, Random);
		//D = dir1;
		//разброс учитывая коеффициент дисперсии патрона
		float fire_disp_with_ammo = fire_disp+fireDispersionBase * l_cartridge.m_kDisp;

/*		dir = D;
		dir.x += RandNormal(0.f, fire_disp_with_ammo);
		dir.y += RandNormal(0.f, fire_disp_with_ammo);
		dir.z += RandNormal(0.f, fire_disp_with_ammo);*/

		dir.random_dir(D, fire_disp_with_ammo, Random);
		D = dir;

		//инициализипровать текущие параметры выстрела перед запуском RayPick
		m_fCurrentHitPower	= float(iHitPower);
		m_fCurrentHitImpulse = fHitImpulse;
		m_pCurrentCartridge = &l_cartridge;
		m_vCurrentShootDir = D;
		m_vCurrentShootPos = P;
		m_fCurrentFireDist = fireDistance;
		m_iCurrentParentID = H_Parent()->ID();

		// ...and trace line
		m_vEndPoint.mad(m_vCurrentShootPos,	m_vCurrentShootDir,fireDistance*l_cartridge.m_kDist);

		Collide::ray_defs RD(P, D, fireDistance*l_cartridge.m_kDist,0,Collide::rqtBoth);
		H_Parent()->setEnabled(false);
		bResult |= Level().ObjectSpace.RayQuery( RD, firetrace_callback, dynamic_cast<CShootingObject*>(this));
		H_Parent()->setEnabled(true);

		// tracer
		if(l_cartridge.m_tracer && tracerFrame != Device.dwFrame) 
		{
			tracerFrame = Device.dwFrame;
			Level().Tracers.Add	(Peff,m_vEndPoint,tracerHeadSpeed,
								 tracerTrailCoeff,tracerStartLength,tracerWidth);
		}
	}

	// light
	if(m_shotLight) Light_Start();
	
	// Ammo
	if(Local()) 
	{
		m_abrasion		= _max(0.f, m_abrasion - l_cartridge.m_impair);
		m_magazine.pop	();
		--iAmmoElapsed;
		if(iAmmoElapsed==0) 
			OnMagazineEmpty();

		//проверить не произошла ли осечка
		CheckForMisfire();
	}

	
	return				bResult;
}

//нарисовать вспышку пламени из оружия
void CWeapon::OnDrawFlame	()
{
	if (fFlameTime>0)	
	{
#pragma todo("Oles to Yura: replace '::Render->add_Patch' with particles")
		/*
		// fire flash
		Fvector P = vLastFP;
		float k=fFlameTime/FLAME_TIME;
		Fvector D; D.mul(vLastFD,::Random.randF(fFlameLength*k)/float(iFlameDiv));
		float f = fFlameSize;
		for (int i=0; i<iFlameDiv; ++i)
		{
			f		*= 0.9f;
			float	S = f+f*::Random.randF	();
			float	A = ::Random.randF		(PI_MUL_2);
			::Render->add_Patch				(hFlames[Random.randI(hFlames.size())],P,S,A,hud_mode);
			P.add(D);
		}
		*/

		
/*		CParticlesObject* pStaticPG;
		pStaticPG = xr_new<CParticlesObject>(m_sFlameParticles,Sector(),false);

*/
		/*if(m_pFlameParticles && m_pFlameParticles->PSI_alive())
		{
			UpdateFlameParticles();
		}*/
		
		


		fFlameTime -= Device.fTimeDelta;


	}
}

void CWeapon::FireStart	()
{
	bWorking=true;	
}
void CWeapon::FireEnd	()				
{ 
	bWorking=false;	
	StopFlameParticles	();
}
void CWeapon::Fire2Start()				
{ 
	bWorking2=true;	
}
void CWeapon::Fire2End	()
{ 
	bWorking2=false;
	StopFlameParticles	();
}


void CWeapon::StartFlameParticles	()
{
	//StartParticles(m_pFlameParticles, m_sFlameParticles, vLastFP);
	if(!m_sFlameParticles) return;

	if(m_pFlameParticles != NULL) 
	{
		UpdateFlameParticles();
		return;
	}

	m_pFlameParticles = xr_new<CParticlesObject>(m_sFlameParticles,Sector(),false);

	UpdateFlameParticles();
	m_pFlameParticles->Play();
}
void CWeapon::StopFlameParticles	()
{
//	StopParticles(m_pFlameParticles);
	if(!m_sFlameParticles) return;

	if(m_pFlameParticles == NULL) return;

	m_pFlameParticles->Stop();
	m_pFlameParticles->PSI_destroy();
	m_pFlameParticles = NULL;
}

void CWeapon::UpdateFlameParticles	()
{
//	UpdateParticles	(m_pFlameParticles, vLastFP);
	if(!m_sFlameParticles) return;

	Fmatrix pos; 
	pos.set(XFORM()); 
	pos.c.set(vLastFP);
	m_pFlameParticles->SetXFORM(pos);

	
	if(!m_pFlameParticles->IsLooped() && !m_pFlameParticles->PSI_alive())
	{
		m_pFlameParticles->Stop();
		m_pFlameParticles->PSI_destroy();
		m_pFlameParticles = NULL;
	}
}

/*	Fvector vel; 
	vel.sub(Position(),ps_Element(0).vPosition); 
	vel.div((Level().timeServer()-ps_Element(0).dwTime)/1000.f);
	m_pFlameParticles->UpdateParent(pos, vel); 
*/

//партиклы дыма
void CWeapon::StartSmokeParticles	()
{
	CParticlesObject* pSmokeParticles = NULL;
	StartParticles(pSmokeParticles, m_sSmokeParticles, vLastFP, zero_vel, true);
	/*if(!m_sSmokeParticles) return;

	CParticlesObject* pSmokeParticles = xr_new<CParticlesObject>(m_sSmokeParticles,Sector());

	Fvector vel; 
	vel.sub(Position(),ps_Element(0).vPosition); 
	vel.div((Level().timeServer()-ps_Element(0).dwTime)/1000.f);

	Fmatrix pos; 
	pos.set(XFORM()); 
	pos.c.set(vLastFP);

	pSmokeParticles->UpdateParent(pos, vel); 
	pSmokeParticles->Play();*/
}



void CWeapon::StartParticles (CParticlesObject*& pParticles, LPCSTR particles_name, 
							  const Fvector& pos, const  Fvector& vel, bool auto_remove_flag)
{
	if(!particles_name) return;

	if(pParticles != NULL) 
	{
		UpdateParticles(pParticles, pos, vel);
		return;
	}

	pParticles = xr_new<CParticlesObject>(particles_name,Sector(),auto_remove_flag);

	UpdateParticles(pParticles, pos, vel);
	pParticles->Play();

}
void CWeapon::StopParticles (CParticlesObject*&	pParticles)
{
	if(pParticles == NULL) return;

	pParticles->Stop();
	pParticles->PSI_destroy();
	pParticles = NULL;

}
void CWeapon::UpdateParticles (CParticlesObject*& pParticles, 
							   const Fvector& pos, const Fvector& vel)
{
	if(!pParticles) return;

	Fmatrix particles_pos; 
	particles_pos.set(XFORM());
	particles_pos.c.set(pos);
	//pParticles->UpdateParent(particles_pos,vel);
	pParticles->SetXFORM(particles_pos);

	
	if(!pParticles->IsAutoRemove() && !pParticles->IsLooped() 
		&& !pParticles->PSI_alive())
	{
		pParticles->Stop();
		pParticles->PSI_destroy();
		pParticles = NULL;
	}
}