// WeaponFire.cpp: implementation of the CWeapon class.
// function responsible for firing with CWeapon
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Weapon.h"
#include "WeaponHUD.h"
#include "ParticlesObject.h"
#include "HUDManager.h"
#include "entity.h"
#include "actor.h"

#include "actoreffector.h"
#include "effectorshot.h"

#include "level_bullet_manager.h"

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

	//�������� ������������ ������ � ������ ������� ����������� �������
	ChangeCondition(-conditionDecreasePerShot*l_cartridge.m_impair);


	BOOL bResult = false;


	for(int i = 0; i < l_cartridge.m_buckShot; ++i) 
	{
		//dir1.random_dir(dir, fireDispersionBase * l_cartridge.m_kDisp, Random);
		//D = dir1;
		//������� �������� ����������� ��������� �������
		float fire_disp_with_ammo = fire_disp+fireDispersionBase * l_cartridge.m_kDisp;

/*		dir = D;
		dir.x += RandNormal(0.f, fire_disp_with_ammo);
		dir.y += RandNormal(0.f, fire_disp_with_ammo);
		dir.z += RandNormal(0.f, fire_disp_with_ammo);*/

		dir.random_dir(D, fire_disp_with_ammo, Random);
		D = dir;

		//����������������� ������� ��������� �������� ����� �������� RayPick
		m_fCurrentHitPower	= float(iHitPower);
		m_fCurrentHitImpulse = fHitImpulse;
		m_fCurrentWallmarkSize = l_cartridge.fWallmarkSize;
		m_pCurrentCartridge = &l_cartridge;
		m_vCurrentShootDir = D;
		m_vCurrentShootPos = P;
		m_fCurrentFireDist = fireDistance;
		m_iCurrentParentID = H_Parent()->ID();

		/*
		*	const Fvector& position,
		const Fvector& dir,
		float speed
		float power,
		float impulse,
		u16	sender_id,
		ALife::EHitType e_hit_type,
		const CCartridge& cartridge
		 */
		Level().BulletManager().AddBullet(xr_new<SBullet>(P, D , m_fStartBulletSpeed,
													float(iHitPower),
													fHitImpulse,
													H_Parent()->ID(),
													ALife::eHitTypeFireWound,
													l_cartridge));
		/*
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
		}*/
	}

	// light
	if(m_bShotLight) Light_Start();
	
	// Ammo
	if(Local()) 
	{
		m_magazine.pop	();
		--iAmmoElapsed;

		if(iAmmoElapsed==0) 
			OnMagazineEmpty();

		//��������� �� ��������� �� ������
		CheckForMisfire();
	}

	
	return				bResult;
}

void CWeapon::FireStart	()
{
	bWorking=true;	
}
void CWeapon::FireEnd	()				
{ 
	//������������� ������������� ����������� ��������
	if(m_pFlameParticles && m_pFlameParticles->IsLooped()) 
		StopFlameParticles	();

	bWorking=false;	
}
void CWeapon::Fire2Start()				
{ 
	bWorking2=true;	
}
void CWeapon::Fire2End	()
{ 
	//������������� ������������� ����������� ��������
	if(m_pFlameParticles2 && m_pFlameParticles2->IsLooped()) 
		StopFlameParticles2	();

	bWorking2=false;
}


void CWeapon::StartFlameParticles	()
{
	if(!m_sFlameParticlesCurrent) return;

	//���� �������� �����������
	if(m_pFlameParticles && m_pFlameParticles->IsLooped() && 
	   m_pFlameParticles->IsPlaying()) 
	{
		UpdateFlameParticles();
		return;
	}

	
	//m_pFlameParticles = m_pFlameParticlesCache[m_iNextParticle];
	//if(m_pFlameParticles->IsLooped()) m_iNextParticle++;
	//if(m_iNextParticle>=PARTICLES_CACHE_SIZE) m_iNextParticle = 0;

	StopFlameParticles();
	m_pFlameParticles = xr_new<CParticlesObject>(m_sFlameParticlesCurrent,Sector(),false);
	UpdateFlameParticles();
	m_pFlameParticles->Play();
	
}
void CWeapon::StopFlameParticles	()
{
	if(!m_sFlameParticlesCurrent) return;
	if(m_pFlameParticles == NULL) return;

	m_pFlameParticles->SetAutoRemove(true);
	m_pFlameParticles->Stop();
	m_pFlameParticles = NULL;
}

void CWeapon::UpdateFlameParticles	()
{
	if(!m_sFlameParticlesCurrent) return;
	if(!m_pFlameParticles) return;

	Fmatrix pos; 
	pos.set(XFORM()); 
	pos.c.set(vLastFP);

	//m_pFlameParticles->UpdateParent(pos, zero_vel);
	m_pFlameParticles->SetXFORM(pos);

	
	if(!m_pFlameParticles->IsLooped() && 
		!m_pFlameParticles->IsPlaying() &&
		!m_pFlameParticles->PSI_alive())
	{
		m_pFlameParticles->Stop();
		//m_pFlameParticles->SetAutoRemove(true);
		m_pFlameParticles->PSI_destroy();
		m_pFlameParticles = NULL;
	}

	

/*	for(int i=0; i<PARTICLES_CACHE_SIZE;i++)
	{
		if(m_pFlameParticlesCache[i])
		{
			m_pFlameParticlesCache[i]->SetXFORM(pos);
			
			if(!m_pFlameParticlesCache[i]->IsLooped() && 
				m_pFlameParticlesCache[i]->IsPlaying() &&
			   !m_pFlameParticlesCache[i]->PSI_alive())
			{
				m_pFlameParticlesCache[i]->Stop();
				//m_pFlameParticles->PSI_destroy();
				//m_pFlameParticles = NULL;
			}
		}
	}*/
}


//�������� ����
void CWeapon::StartSmokeParticles	()
{
	CParticlesObject* pSmokeParticles = NULL;

	Fvector vel; 
	PHGetLinearVell(vel);

	StartParticles(pSmokeParticles, m_sSmokeParticlesCurrent, vLastFP, vel, true);
}

//�������� �����
void CWeapon::OnShellDrop	()
{
	if(!m_sShellParticles) return;

	CParticlesObject* pShellParticles = xr_new<CParticlesObject>(m_sShellParticles,Sector());

	Fmatrix particles_pos; 
	particles_pos.set(XFORM());
	particles_pos.c.set(vLastSP);

	Fvector vel; 
	PHGetLinearVell(vel);

	pShellParticles->UpdateParent(particles_pos, vel); 
	pShellParticles->Play();
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

void CWeapon::StartFlameParticles2	()
{
	StartParticles (m_pFlameParticles2, m_sFlameParticles2, vLastFP2);
}
void CWeapon::StopFlameParticles2	()
{
	StopParticles (m_pFlameParticles2);
}
void CWeapon::UpdateFlameParticles2	()
{
	UpdateParticles (m_pFlameParticles2, vLastFP2);
}

//////////////////////////////////////////////////////////////////////////
// ��� ������� ������ ������
void CWeapon::AddShotEffector		()
{
	CActor* pActor = dynamic_cast<CActor*>(H_Parent());
	if(pActor)
	{
		CEffectorShot* S		= dynamic_cast<CEffectorShot*>	(pActor->EffectorManager().GetEffector(cefShot)); 
		if (!S)	S				= (CEffectorShot*)pActor->EffectorManager().AddEffector(xr_new<CEffectorShot> (camMaxAngle,camRelaxSpeed));
		R_ASSERT				(S);
		S->Shot					(camDispersion);
	}
}

void  CWeapon::RemoveShotEffector	()
{
	CActor* pActor = dynamic_cast<CActor*>(H_Parent());
   	if(pActor)
		pActor->EffectorManager().RemoveEffector	(cefShot);
}

const Fvector& CWeapon::GetRecoilDeltaAngle()
{
	CActor* pActor		= dynamic_cast<CActor*>(H_Parent());
	
	CEffectorShot* S = NULL;
	if(pActor)
		S = dynamic_cast<CEffectorShot*>(pActor->EffectorManager().GetEffector(cefShot)); 
	
	if(S)
	{
		S->GetDeltaAngle(m_vRecoilDeltaAngle);
		return m_vRecoilDeltaAngle;
	}
	else
	{
		m_vRecoilDeltaAngle.set(0.f,0.f,0.f);
		return m_vRecoilDeltaAngle;
	}
}