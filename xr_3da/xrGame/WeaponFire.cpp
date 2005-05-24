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


float _nrand(float sigma)
{
#define ONE_OVER_SIGMA_EXP (1.0f / 0.7975f)

	if(sigma == 0) return 0;

	float y;
	do{
		y = -logf(Random.randF());
	}while(Random.randF() > expf(-_sqr(y - 1.0f)*0.5f));
	if(rand() & 0x1)	return y * sigma * ONE_OVER_SIGMA_EXP;
	else				return -y * sigma * ONE_OVER_SIGMA_EXP;
}

void random_dir(Fvector& tgt_dir, const Fvector& src_dir, float dispersion)
{
	float sigma			= dispersion/3.f;
	float alpha			= clampr		(_nrand(sigma),-dispersion,dispersion);
	float theta			= Random.randF	(0,PI);
	float r 			= tan			(alpha);
	Fvector 			U,V,T;
	Fvector::generate_orthonormal_basis	(src_dir,U,V);
	U.mul				(r*_sin(theta));
	V.mul				(r*_cos(theta));
	T.add				(U,V);
	tgt_dir.add			(src_dir,T).normalize();
}

void CWeapon::FireTrace		(const Fvector& P, const Fvector& D)
{
	VERIFY		(m_magazine.size());

	CCartridge &l_cartridge = m_magazine.top();
	VERIFY		(u16(-1) != l_cartridge.bullet_material_idx);

	//�������� ������������ ������ � ������ ������� ����������� �������
	ChangeCondition(-conditionDecreasePerShot*l_cartridge.m_impair);

	
	float	weapon_fire_disp	= GetFireDispersion(false);
	Fvector dir_base_disp;
	random_dir					(dir_base_disp,D,weapon_fire_disp);
//.	dir_base_disp.random_dir	(D, weapon_fire_disp, Random);

	float fire_disp				= GetFireDispersion(true);
	float cartirdge_fire_disp = fire_disp - weapon_fire_disp;

#ifdef DEBUG
	if(bDebug){
		const CInventoryOwner* pOwner	=	smart_cast<const CInventoryOwner*>(H_Parent());
		VERIFY (pOwner);
		float parent_disp = pOwner->GetWeaponAccuracy();

		Msg("disp_without cart=%f",rad2deg(weapon_fire_disp-parent_disp));
		Msg("disp_with cart=%f",rad2deg(fire_disp-parent_disp));
		
		Msg("disp_owner=%f",rad2deg(parent_disp));
		
		Fvector ddd, dd;
		dd = dir_base_disp;
//.		ddd.random_dir(dd, cartirdge_fire_disp, Random);
		random_dir(ddd,dd,cartirdge_fire_disp);

		float diff;
		Fvector _D = D;
		_D.normalize_safe();
		ddd.normalize_safe();
		
		diff = acos(_D.dotproduct(ddd));


		Msg("total angle diff=%f", rad2deg(diff));
	}

#endif

	bool SendHit = SendHitAllowed(H_Parent());
	//���������� ���� (� ������ ��������� �������� ������)
	for(int i = 0; i < l_cartridge.m_buckShot; ++i) 
	{
		FireBullet(P, dir_base_disp, cartirdge_fire_disp,
					l_cartridge, H_Parent()->ID(), ID(), SendHit);
	}

	StartShotParticles	();
	if(m_bShotLight) Light_Start();

	
	// Ammo
	m_magazine.pop	();
	--iAmmoElapsed;
/*
	if(iAmmoElapsed==0) 
		OnMagazineEmpty();
*/
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
	CShootingObject::StartParticles (m_pFlameParticles2, *m_sFlameParticles2, get_LastFP2());
}
void CWeapon::StopFlameParticles2	()
{
	CShootingObject::StopParticles (m_pFlameParticles2);
}
void CWeapon::UpdateFlameParticles2	()
{
	if (m_pFlameParticles2)			CShootingObject::UpdateParticles (m_pFlameParticles2, get_LastFP2());
}
