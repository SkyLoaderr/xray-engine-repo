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


void CWeapon::FireTrace		(const Fvector& P, const Fvector& D)
{
	VERIFY		(m_magazine.size());

	CCartridge &l_cartridge = m_magazine.top();

	//повысить изношенность оружия с учетом влияния конкретного патрона
	ChangeCondition(-conditionDecreasePerShot*l_cartridge.m_impair);

	
	float weapon_fire_disp = GetFireDispersion(false);
	Fvector dir_base_disp;
	dir_base_disp.random_dir(D, weapon_fire_disp, Random);

	float cartirdge_fire_disp = GetFireDispersion(true) - weapon_fire_disp;

	//выстерлить пулю (с учетом возможной стрельбы дробью)
	for(int i = 0; i < l_cartridge.m_buckShot; ++i) 
	{
		FireBullet(P, dir_base_disp, cartirdge_fire_disp,
					l_cartridge, H_Parent()->ID(), ID());
	}

	StartShotParticles	();
	if(m_bShotLight) Light_Start();

	
	// Ammo
	m_magazine.pop	();
	--iAmmoElapsed;

	if(iAmmoElapsed==0) 
		OnMagazineEmpty();

	//проверить не произошла ли осечка
	CheckForMisfire();

	VERIFY((u32)iAmmoElapsed == m_magazine.size());
}

void CWeapon::Fire2Start()				
{ 
	bWorking2=true;	
}
void CWeapon::Fire2End	()
{ 
	//принудительно останавливать зацикленные партиклы
	if(m_pFlameParticles2 && m_pFlameParticles2->IsLooped()) 
		StopFlameParticles2	();

	bWorking2=false;
}

void CWeapon::StopShooting		()
{
	m_bPending = true;

	//принудительно останавливать зацикленные партиклы
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