#include "stdafx.h"
#include "weaponshotgun.h"
#include "WeaponHUD.h"
#include "entity.h"
#include "ParticlesObject.h"
#include "xr_level_controller.h"

CWeaponShotgun::CWeaponShotgun(void) : CWeaponCustomPistol("TOZ34")
{
    m_eSoundShotBoth	= ESoundTypes(SOUND_TYPE_WEAPON_SHOOTING);
}

CWeaponShotgun::~CWeaponShotgun(void)
{
	// sounds
	HUD_SOUND::DestroySound(sndShotBoth);
}

void CWeaponShotgun::net_Destroy()
{
	inherited::net_Destroy();
}

void CWeaponShotgun::Load	(LPCSTR section)
{
	inherited::Load		(section);

	// Звук и анимация для выстрела дуплетом
	HUD_SOUND::LoadSound(section, "snd_shoot_duplet", sndShotBoth, TRUE, m_eSoundShotBoth);
	animGet	(mhud_shot_boths,	pSettings->r_string(*hud_sect,"anim_shoot_both"));
}


void CWeaponShotgun::renderable_Render	()
{
	inherited::renderable_Render();
}



void CWeaponShotgun::OnShot () 
{
	std::swap(m_pHUD->FirePoint(), m_pHUD->FirePoint2());
	std::swap(vFirePoint, vFirePoint2);
	//std::swap(m_pFlameParticles, m_pFlameParticles2);

	///UpdateFP();
	inherited::OnShot();
}

void CWeaponShotgun::Fire2Start () 
{
	if(m_bPending) return;

	inherited::Fire2Start();

	if (IsValid())
	{
		if (!IsWorking())
		{
			if (STATE==eReload)			return;
			if (STATE==eShowing)		return;
			if (STATE==eHiding)			return;

			if (!iAmmoElapsed)	
			{
				CWeapon::FireStart	();
				SwitchState			(eMagEmpty);
			}
			else					
			{
				CWeapon::FireStart	();
				SwitchState			((iAmmoElapsed < iMagazineSize)?eFire:eFire2);
			}
		}
	}else{
		if (!iAmmoElapsed)	
			SwitchState			(eMagEmpty);
	}
}

void CWeaponShotgun::Fire2End () 
{
	inherited::Fire2End();
	FireEnd();
}


void CWeaponShotgun::OnShotBoth()
{
	//если патронов меньше, чем 2 
	if(iAmmoElapsed < iMagazineSize) 
	{ 
		OnShot(); 
		return; 
	}

	//звук выстрела дуплетом
	//UpdateFP();
	PlaySound(sndShotBoth,vLastFP);
	
	// Camera
	AddShotEffector		();
	
	// анимация дуплета
	m_pHUD->animPlay			(mhud_shot_boths[Random.randI(mhud_shot_boths.size())],FALSE,this);
	
	// Shell Drop
	Fvector vel; 
	PHGetLinearVell		(vel);
	OnShellDrop			(vLastSP, vel);

	//огонь из 2х стволов
	StartFlameParticles			();
	StartFlameParticles2		();

	//дым из 2х стволов
	CParticlesObject* pSmokeParticles = NULL;
	CShootingObject::StartParticles(pSmokeParticles, *m_sSmokeParticlesCurrent, vLastFP,  zero_vel, true);
	pSmokeParticles = NULL;
	CShootingObject::StartParticles(pSmokeParticles, *m_sSmokeParticlesCurrent, vLastFP2, zero_vel, true);

}

void CWeaponShotgun::switch2_Fire	()
{
	inherited::switch2_Fire	();
	bWorking = false;
/*	if (fTime<=0)
	{
		UpdateFP					();

		// Fire
		Fvector						p1, d; 
		p1.set(vLastFP); 
		d.set(vLastFD);

		CEntity*					E = smart_cast<CEntity*>(H_Parent());
		if (E) E->g_fireParams		(p1,d);
		OnShot						();
		FireTrace					(p1,vLastFP,d);
		fTime						+= fTimeToFire;

		// Patch for "previous frame position" :)))
		dwFP_Frame					= 0xffffffff;
		dwXF_Frame					= 0xffffffff;
	}*/
}

void CWeaponShotgun::switch2_Fire2	()
{
	VERIFY(fTimeToFire>0.f);

	if (fTime<=0)
	{
		//UpdateFP					();

		// Fire
		Fvector						p1, d; 
		p1.set(vLastFP); 
		d.set(vLastFD);

		CEntity*					E = smart_cast<CEntity*>(H_Parent());
		if (E) E->g_fireParams		(this, p1,d);
		
		OnShotBoth						();

		//выстрел из обоих стволов
		FireTrace					(p1,d);
		FireTrace					(p1,d);
		fTime						+= fTimeToFire*2.f;

		// Patch for "previous frame position" :)))
		dwFP_Frame					= 0xffffffff;
		dwXF_Frame					= 0xffffffff;
	}
}

void CWeaponShotgun::UpdateSounds()
{
	inherited::UpdateSounds();

	//UpdateFP();
	if (sndShotBoth.snd.feedback)		sndShotBoth.set_position		(vLastFP);
}


bool CWeaponShotgun::Action(s32 cmd, u32 flags) 
{
	if(inherited::Action(cmd, flags)) return true;
	
	//если оружие чем-то занято, то ничего не делать
	if(IsPending()) return false;

	switch(cmd) 
	{
		case kWPN_ZOOM : 
			{
				if(flags&CMD_START) Fire2Start();
				else Fire2End();
			}
			return true;
	}
	return false;
}

void  CWeaponShotgun::UpdateCL	()
{
	inherited::UpdateCL();
}