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


void CWeapon::FireTrace		(const Fvector& P, Fvector& D)
{
	R_ASSERT(m_magazine.size());

	CCartridge &l_cartridge = m_magazine.top();

	//�������� ������������ ������ � ������ ������� ����������� �������
	ChangeCondition(-conditionDecreasePerShot*l_cartridge.m_impair);

	//���������� ����
	FireBullet(P,D, GetFireDispersion(l_cartridge.m_kDisp),
				l_cartridge, H_Parent()->ID(), ID());
	
	// Ammo
	m_magazine.pop	();
	--iAmmoElapsed;

	if(iAmmoElapsed==0) 
		OnMagazineEmpty();

	//��������� �� ��������� �� ������
	CheckForMisfire();

	VERIFY((u32)iAmmoElapsed == m_magazine.size());
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

void CWeapon::StopShooting		()
{
	m_bPending = true;

	//������������� ������������� ����������� ��������
	if(m_pFlameParticles && m_pFlameParticles->IsLooped())
		StopFlameParticles	();

	SwitchState(eIdle);

	bWorking = false;
	//if(IsWorking()) FireEnd();
}


void CWeapon::StartFlameParticles2	()
{
	CShootingObject::StartParticles (m_pFlameParticles2, *m_sFlameParticles2, vLastFP2);
}
void CWeapon::StopFlameParticles2	()
{
	CShootingObject::StopParticles (m_pFlameParticles2);
}
void CWeapon::UpdateFlameParticles2	()
{
	CShootingObject::UpdateParticles (m_pFlameParticles2, vLastFP2);
}