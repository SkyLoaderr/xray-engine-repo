#include "stdafx.h"
#include "weaponshotgun.h"
#include "WeaponHUD.h"
#include "entity.h"
#include "effectorshot.h"
#include "ParticlesObject.h"

CWeaponShotgun::CWeaponShotgun(void) : CWeaponCustomPistol("TOZ34")
{
    m_eSoundShotBoth	= ESoundTypes(SOUND_TYPE_WEAPON_SHOOTING /*| eSoundType*/);
	
	m_pFlameParticles2 = NULL;
}

CWeaponShotgun::~CWeaponShotgun(void)
{
}

void CWeaponShotgun::net_Destroy()
{
	inherited::net_Destroy();

	// sounds
	SoundDestroy		(sndShot		);
}

void CWeaponShotgun::Load	(LPCSTR section)
{
	inherited::Load		(section);

	// ���� � �������� ��� �������
	SoundCreate			(sndShotBoth,		"shoot_both"   ,m_eSoundShotBoth);
	animGet				(mhud_shot_boths,	pSettings->r_string(*hud_sect,"anim_shoot_both"));
}


#define FLAME_TIME 0.05f
void CWeaponShotgun::OnDrawFlame	()
{
	return;
	if(eFire2 != STATE) inherited::OnDrawFlame();
	else if (fFlameTime>0)	
	{
		// fire flash
#pragma todo("Oles to Yura: replace '::Render->add_Patch' with particles")
		/*
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

		std::swap(m_pHUD->vFirePoint, m_pHUD->vFirePoint2);
		std::swap(vFirePoint, vFirePoint2);
		UpdateFP();

		// fire flash 2
#pragma todo("Oles to Yura: replace '::Render->add_Patch' with particles")
		/*
		P = vLastFP;
		k=fFlameTime/FLAME_TIME;
		D; D.mul(vLastFD,::Random.randF(fFlameLength*k)/float(iFlameDiv));
		f = fFlameSize;
		for (int i=0; i<iFlameDiv; ++i)
		{
			f		*= 0.9f;
			float	S = f+f*::Random.randF	();
			float	A = ::Random.randF		(PI_MUL_2);
			::Render->add_Patch				(hFlames[Random.randI(hFlames.size())],P,S,A,hud_mode);
			P.add(D);
		}
		fFlameTime -= Device.fTimeDelta;
		*/
	}
}


void CWeaponShotgun::renderable_Render	()
{
	inherited::renderable_Render();
	if(STATE == eFire2) OnDrawFlame();
}



void CWeaponShotgun::OnShot () 
{
	std::swap(m_pHUD->vFirePoint, m_pHUD->vFirePoint2);
	std::swap(vFirePoint, vFirePoint2);
	std::swap(m_pFlameParticles, m_pFlameParticles2);

	UpdateFP();
	inherited::OnShot();
}

void CWeaponShotgun::Fire2Start () 
{
	if(bPending) return;

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
/*	if (IsWorking())
	{
		CWeapon::FireEnd	();
	}
	if (!iAmmoElapsed)	
		TryReload	();
	else 
		SwitchState (eIdle);*/
}


void CWeaponShotgun::OnShotBoth()
{
	//���� �������� ������, ��� 2 
	if(iAmmoElapsed < iMagazineSize) 
	{ 
		OnShot(); 
		return; 
	}

	//���� �������� ��������
	Sound->play_at_pos			(sndShotBoth,H_Root(),vLastFP);

	// Camera
	if (hud_mode)
	{
		CEffectorShot* S		= dynamic_cast<CEffectorShot*>	(Level().Cameras.GetEffector(cefShot)); 
		if (!S)	S				= (CEffectorShot*)Level().Cameras.AddEffector(xr_new<CEffectorShot> (camMaxAngle,camRelaxSpeed));
		R_ASSERT				(S);
		S->Shot					(camDispersion);
	}
	
	// �������� �������
	m_pHUD->animPlay			(mhud_shot_boths[Random.randI(mhud_shot_boths.size())],FALSE,this);
	
	// Flames
	fFlameTime					= .1f;
	
	// Shell Drop
	OnShellDrop					();

	//����� �� 2� �������
	StartFlameParticles			();
	StartFlameParticles2		();

	//��� �� 2� �������
	CParticlesObject* pSmokeParticles = NULL;
	StartParticles(pSmokeParticles, m_sSmokeParticles, vLastFP,  zero_vel, true);
	pSmokeParticles = NULL;
	StartParticles(pSmokeParticles, m_sSmokeParticles, vLastFP2, zero_vel, true);

}

void CWeaponShotgun::switch2_Fire	()
{
	inherited::switch2_Fire	();
/*	if (fTime<=0)
	{
		UpdateFP					();

		// Fire
		Fvector						p1, d; 
		p1.set(vLastFP); 
		d.set(vLastFD);

		CEntity*					E = dynamic_cast<CEntity*>(H_Parent());
		if (E) E->g_fireParams		(p1,d);
		bFlame						=	TRUE;
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
	if (fTime<=0)
	{
		UpdateFP					();

		// Fire
		Fvector						p1, d; 
		p1.set(vLastFP); 
		d.set(vLastFD);

		CEntity*					E = dynamic_cast<CEntity*>(H_Parent());
		if (E) E->g_fireParams		(p1,d);
		bFlame						=	TRUE;
		
		OnShotBoth						();

		//������� �� ����� �������
		FireTrace					(p1,vLastFP,d);
		FireTrace					(p1,vLastFP2,d);
		fTime						+= fTimeToFire*2.f;

		// Patch for "previous frame position" :)))
		dwFP_Frame					= 0xffffffff;
		dwXF_Frame					= 0xffffffff;
	}
}

void CWeaponShotgun::UpdateSounds()
{
	inherited::UpdateSounds();

	UpdateFP();
	if (sndShotBoth.feedback)		sndShotBoth.set_position		(vLastFP);
}


bool CWeaponShotgun::Action(s32 cmd, u32 flags) 
{
	if(inherited::Action(cmd, flags)) return true;
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

void CWeaponShotgun::StartFlameParticles2	()
{
	StartParticles (m_pFlameParticles2, m_sFlameParticles, vLastFP2);
}
void CWeaponShotgun::StopFlameParticles2	()
{
	StopParticles (m_pFlameParticles2);
}
void CWeaponShotgun::UpdateFlameParticles2	()
{
	UpdateParticles (m_pFlameParticles2, vLastFP2);
}

void  CWeaponShotgun::UpdateCL	()
{
	inherited::UpdateCL();

	if(m_pFlameParticles2) UpdateFlameParticles2	();
}