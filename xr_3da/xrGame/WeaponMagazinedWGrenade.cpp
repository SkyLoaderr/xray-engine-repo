#include "stdafx.h"
#include "weaponmagazinedwgrenade.h"
#include "WeaponHUD.h"
#include "entity.h"
#include "PSObject.h"
#include "ParticlesObject.h"
#include "GrenadeLauncher.h"
#include "xrserver_objects_alife_items.h"
#include "ExplosiveRocket.h"
#include "Actor_Flags.h"
#include "xr_level_controller.h"
#include "level.h"

//////////////////////////////////////////////////////////////////////////////////////////////////

CWeaponMagazinedWGrenade::CWeaponMagazinedWGrenade(LPCSTR name,ESoundTypes eSoundType) : CWeaponMagazined(name, eSoundType)
{
	m_ammoType2 = 0;
    m_bGrenadeMode = false;
}

CWeaponMagazinedWGrenade::~CWeaponMagazinedWGrenade(void)
{
	// sounds
	HUD_SOUND::DestroySound(sndShotG);
	HUD_SOUND::DestroySound(sndReloadG);
	HUD_SOUND::DestroySound(sndSwitch);
}

void CWeaponMagazinedWGrenade::StopHUDSounds		()
{
	HUD_SOUND::StopSound(sndShotG);
	HUD_SOUND::StopSound(sndReloadG);
	HUD_SOUND::StopSound(sndSwitch);

	inherited::StopHUDSounds();
}

void CWeaponMagazinedWGrenade::Load	(LPCSTR section)
{
	inherited::Load			(section);
	CRocketLauncher::Load	(section);
	
	
	//// Sounds
	HUD_SOUND::LoadSound(section,"snd_shoot_grenade"	, sndShotG		, TRUE, m_eSoundShot);
	HUD_SOUND::LoadSound(section,"snd_reload_grenade"	, sndReloadG	, TRUE, m_eSoundReload);
	HUD_SOUND::LoadSound(section,"snd_switch"			, sndSwitch		, TRUE, m_eSoundReload);
	

	m_sFlameParticles2 = pSettings->r_string(section, "grenade_flame_particles");

	
	// HUD :: Anims
	R_ASSERT			(m_pHUD);

	animGet				(mhud_idle_g,	pSettings->r_string(*hud_sect, "anim_idle_g"));
	animGet				(mhud_reload_g,	pSettings->r_string(*hud_sect, "anim_reload_g"));
	animGet				(mhud_shots_g,	pSettings->r_string(*hud_sect, "anim_shoot_g"));
	animGet				(mhud_switch_g,	pSettings->r_string(*hud_sect, "anim_switch_grenade_on"));
	animGet				(mhud_switch,	pSettings->r_string(*hud_sect, "anim_switch_grenade_off"));
	animGet				(mhud_show_g,	pSettings->r_string(*hud_sect, "anim_draw_g"));
	animGet				(mhud_hide_g,	pSettings->r_string(*hud_sect, "anim_holster_g"));

	animGet				(mhud_idle_w_gl,	pSettings->r_string(*hud_sect, "anim_idle_gl"));
	animGet				(mhud_reload_w_gl,	pSettings->r_string(*hud_sect, "anim_reload_gl"));
	animGet				(mhud_show_w_gl,	pSettings->r_string(*hud_sect, "anim_draw_gl"));
	animGet				(mhud_hide_w_gl,	pSettings->r_string(*hud_sect, "anim_holster_gl"));
	animGet				(mhud_shots_w_gl,	pSettings->r_string(*hud_sect, "anim_shoot_gl"));

	if(this->IsZoomEnabled())
	{
		animGet				(mhud_idle_g_aim,		pSettings->r_string(*hud_sect, "anim_idle_g_aim"));
		animGet				(mhud_idle_w_gl_aim,	pSettings->r_string(*hud_sect, "anim_idle_gl_aim"));
	}


	if(m_eGrenadeLauncherStatus == ALife::eAddonPermanent)
	{
		CRocketLauncher::m_fLaunchSpeed = pSettings->r_float(section, "grenade_vel");
	}

	grenade_bone_name = pSettings->r_string(*hud_sect, "grenade_bone");

	// load ammo classes SECOND (grenade_class)
	m_ammoTypes2.clear	(); 
	LPCSTR				S = pSettings->r_string(section,"grenade_class");
	if (S && S[0]) 
	{
		string128		_ammoItem;
		int				count		= _GetItemCount	(S);
		for (int it=0; it<count; ++it)	
		{
			_GetItem				(S,it,_ammoItem);
			m_ammoTypes2.push_back	(_ammoItem);
		}
		m_ammoName2 = pSettings->r_string(*m_ammoTypes2[0],"inv_name_short");
	}
	else
		m_ammoName2 = 0;

	iMagazineSize2 = iMagazineSize;
}

void CWeaponMagazinedWGrenade::net_Destroy()
{
	inherited::net_Destroy();
}


BOOL CWeaponMagazinedWGrenade::net_Spawn(LPVOID DC) 
{
	m_pGrenadePoint = &vLastFP2;
	BOOL l_res = inherited::net_Spawn(DC);
	 
	UpdateGrenadeVisibility(!!iAmmoElapsed);
	m_bPending = false;

	return l_res;
}
void CWeaponMagazinedWGrenade::switch2_Idle() 
{
	inherited::switch2_Idle();
}
void CWeaponMagazinedWGrenade::switch2_Reload()
{
	if(m_bGrenadeMode) 
	{
		UpdateFP();
		PlaySound(sndReloadG,vLastFP2);

		m_pHUD->animPlay(mhud_reload_g[Random.randI(mhud_reload_g.size())],FALSE,this);
		m_bPending = true;
	}
	else 
	     inherited::switch2_Reload();
}

void CWeaponMagazinedWGrenade::OnShot		()
{
	if(m_bGrenadeMode)
	{
		UpdateFP();
		PlaySound(sndShotG, vLastFP2);
		
		AddShotEffector		();
		
		//партиклы огня вылета гранаты из подствольника
		StartFlameParticles2();
	} 
	else inherited::OnShot();
}
//переход в режим подствольника или выход из него
//если мы в режиме стрельбы очередями, переключиться
//на одиночные, а уже потом на подствольник
void CWeaponMagazinedWGrenade::SwitchMode() 
{
	/*if(!SingleShotMode() || 
	   !IsGrenadeLauncherAttached() || 
	   eIdle != STATE || IsPending()) 
	{
		inherited::SwitchMode();
		return;
	}*/

	if(!IsGrenadeLauncherAttached() || eIdle != STATE || IsPending()) 
		return;

	m_bPending = true;

	PerformSwitch();
	
	UpdateFP	();
	PlaySound	(sndSwitch,vLastFP);

	PlayAnimModeSwitch();

	//после стрельбы из подствольника вернуться в стрельбу очередями
/*	if(!m_bGrenadeMode)
	{
		if(SingleShotMode())
			inherited::SwitchMode();
	}*/
	m_dwAmmoCurrentCalcFrame = 0;
}

void  CWeaponMagazinedWGrenade::PerformSwitch()
{
	m_bGrenadeMode = !m_bGrenadeMode;

	iMagazineSize = m_bGrenadeMode?1:iMagazineSize2;

	m_ammoTypes.swap(m_ammoTypes2);

	swap		(m_ammoType,m_ammoType2);
	swap		(m_ammoName,m_ammoName2);

	xr_stack<CCartridge> l_magazine;
	while(m_magazine.size()) { l_magazine.push(m_magazine.top()); m_magazine.pop(); }
	while(m_magazine2.size()) { m_magazine.push(m_magazine2.top()); m_magazine2.pop(); }
	while(l_magazine.size()) { m_magazine2.push(l_magazine.top()); l_magazine.pop(); }
	iAmmoElapsed = (int)m_magazine.size();

	if(m_bZoomEnabled && m_pHUD)
	{
		if(m_bGrenadeMode)
			LoadZoomOffset(*hud_sect, "grenade_");
		else 
		{
			if(GrenadeLauncherAttachable())
				LoadZoomOffset(*hud_sect, "grenade_normal_");
			else
				LoadZoomOffset(*hud_sect, "");
		}
	}
}

bool CWeaponMagazinedWGrenade::Action(s32 cmd, u32 flags) 
{
	if(inherited::Action(cmd, flags)) return true;
	
	switch(cmd) 
	{
	case kWPN_ZOOM: 
	case kWPN_FUNC: 
			{
                if(flags&CMD_START) 
					SwitchState(eSwitch);// SwitchMode();
				return true;
			}
	}
	return false;
}

void CWeaponMagazinedWGrenade::state_Fire(float dt) 
{
	VERIFY(fTimeToFire>0.f);

	//режим стрельбы подствольника
	if(m_bGrenadeMode)
	{
		UpdateFP				();
		fTime					-=dt;
		Fvector					p1, d; 
		p1.set(vLastFP2); 
		d.set(vLastFD);
		
		if(H_Parent()) 
			smart_cast<CEntity*>	(H_Parent())->g_fireParams	(this, p1,d);
		else 
			return;
		
		while (fTime<=0 && (IsWorking() || m_bFireSingleShot))
		{

			fTime			+=	fTimeToFire;

			++m_iShotNum;
			OnShot			();
			
			// Ammo
			if(Local()) 
			{
				m_magazine.pop	();
				--iAmmoElapsed;
				if(!iAmmoElapsed) 
					OnMagazineEmpty();
			}
		}
		UpdateSounds			();
		if(m_iShotNum == m_iQueueSize) FireEnd();
	} 
	//режим стрельбы очередями
	else inherited::state_Fire(dt);
}

void CWeaponMagazinedWGrenade::SwitchState(u32 S) 
{
	inherited::SwitchState(S);
	
	//стрельнуть из подствольника
	if(m_bGrenadeMode && STATE == eIdle && S == eFire && getRocketCount()/*m_pRocket*/) 
	{
		Fvector						p1, d; 
		p1.set(vLastFP2);
		d.set(vLastFD);

		CEntity*					E = smart_cast<CEntity*>(H_Parent());
				
		if (E)
			E->g_fireParams		(this, p1,d);

		p1.set(*m_pGrenadePoint);

		
		Fmatrix launch_matrix;
		launch_matrix.identity();
		launch_matrix.k.set(d);
		Fvector::generate_orthonormal_basis(launch_matrix.k,
											launch_matrix.j, launch_matrix.i);
		launch_matrix.c.set(p1);

		d.normalize();
		d.mul(CRocketLauncher::m_fLaunchSpeed);
		CRocketLauncher::LaunchRocket(launch_matrix, d, zero_vel);

		CExplosiveRocket* pGrenade = smart_cast<CExplosiveRocket*>(getCurrentRocket()/*m_pRocket*/);
		VERIFY(pGrenade);
		pGrenade->SetCurrentParentID(H_Parent()->ID());

		
		if (Local() && OnServer())
		{
			NET_Packet P;
			u_EventGen(P,GE_OWNERSHIP_REJECT,ID());
			P.w_u16(getCurrentRocket()->ID()/*m_pRocket->ID()*/);
			u_EventSend(P);
		};

	}
}

void CWeaponMagazinedWGrenade::OnEvent(NET_Packet& P, u16 type) 
{
	inherited::OnEvent(P,type);
	u16 id;
	switch (type) 
	{
		case GE_OWNERSHIP_TAKE: 
			{
				P.r_u16(id);
				CRocketLauncher::AttachRocket(id, this);
			}
			break;
		case GE_OWNERSHIP_REJECT :
			{
				P.r_u16(id);
				CRocketLauncher::DetachRocket(id);
				break;
			}
	}
}

void CWeaponMagazinedWGrenade::ReloadMagazine() 
{
	//fake чтобы на гранаты для подствольника не действовало UnlimitedAmmo
//	BOOL UnlimitedAmmo = (psActorFlags.is_any(AF_UNLIMITEDAMMO)) != 0;	
//	if (m_bGrenadeMode) psActorFlags.set(AF_UNLIMITEDAMMO, FALSE);

	inherited::ReloadMagazine();

//	psActorFlags.set(AF_UNLIMITEDAMMO, UnlimitedAmmo);
	
	//перезарядка подствольного гранатомета
	if(iAmmoElapsed && !getRocketCount()/*m_pRocket*/ && m_bGrenadeMode) 
	{
		shared_str fake_grenade_name = pSettings->r_string(*m_pAmmo->cNameSect(), "fake_grenade_name");
		CRocketLauncher::SpawnRocket(*fake_grenade_name, this);
	}
}


void CWeaponMagazinedWGrenade::OnStateSwitch(u32 S) 
{
	switch (S)
	{
	case eSwitch:
		{
			SwitchMode();
		}break;
	}
	inherited::OnStateSwitch(S);
	
	UpdateGrenadeVisibility(!!iAmmoElapsed || S == eReload);
}


void CWeaponMagazinedWGrenade::OnAnimationEnd()
{
	switch (STATE)
	{
	case eSwitch:
		{
			SwitchState(eIdle);
		}break;
	}
	inherited::OnAnimationEnd();
}

void CWeaponMagazinedWGrenade::OnH_B_Chield	()
{
	inherited::OnH_B_Chield();
}
void CWeaponMagazinedWGrenade::OnH_B_Independent()
{
	inherited::OnH_B_Independent();

	m_bPending		= false;
	if (m_bGrenadeMode) {
		STATE		= eIdle;
		SwitchMode	();
		m_bPending	= false;
	}
}

bool CWeaponMagazinedWGrenade::CanAttach(PIItem pIItem)
{
	CGrenadeLauncher* pGrenadeLauncher = smart_cast<CGrenadeLauncher*>(pIItem);
	
	if(pGrenadeLauncher &&
	   CSE_ALifeItemWeapon::eAddonAttachable == m_eGrenadeLauncherStatus &&
	   0 == (m_flagsAddOnState&CSE_ALifeItemWeapon::eWeaponAddonGrenadeLauncher) &&
	   !xr_strcmp(*m_sGrenadeLauncherName, pIItem->cNameSect()))
       return true;
	else
		return inherited::CanAttach(pIItem);
}

bool CWeaponMagazinedWGrenade::CanDetach(const char* item_section_name)
{
	if(CSE_ALifeItemWeapon::eAddonAttachable == m_eGrenadeLauncherStatus &&
	   0 != (m_flagsAddOnState&CSE_ALifeItemWeapon::eWeaponAddonGrenadeLauncher) &&
	   !xr_strcmp(*m_sGrenadeLauncherName, item_section_name))
	   return true;
	else
	   return inherited::CanDetach(item_section_name);
}

bool CWeaponMagazinedWGrenade::Attach(PIItem pIItem)
{
	CGrenadeLauncher* pGrenadeLauncher = smart_cast<CGrenadeLauncher*>(pIItem);
	
	if(pGrenadeLauncher &&
	   CSE_ALifeItemWeapon::eAddonAttachable == m_eGrenadeLauncherStatus &&
	   0 == (m_flagsAddOnState&CSE_ALifeItemWeapon::eWeaponAddonGrenadeLauncher) &&
	   !xr_strcmp(*m_sGrenadeLauncherName, pIItem->cNameSect()))
	{
		m_flagsAddOnState |= CSE_ALifeItemWeapon::eWeaponAddonGrenadeLauncher;

		CRocketLauncher::m_fLaunchSpeed = pGrenadeLauncher->GetGrenadeVel();

 		//уничтожить подствольник из инвентаря
		pIItem->Drop();
		if (OnServer()) 
		{
			NET_Packet P;
			u_EventGen(P,GE_DESTROY,pIItem->ID());
			//		Msg					("ge_destroy: [%d] - %s",pIItem->ID(),*pIItem->cName());
			P.w_u16(u16(pIItem->ID()));
			u_EventSend(P);
		};

		InitAddons();
		UpdateAddonsVisibility();
		return true;
	}
	else
        return inherited::Attach(pIItem);
}
bool CWeaponMagazinedWGrenade::Detach(const char* item_section_name)
{
	if (CSE_ALifeItemWeapon::eAddonAttachable == m_eGrenadeLauncherStatus &&
	   0 != (m_flagsAddOnState&CSE_ALifeItemWeapon::eWeaponAddonGrenadeLauncher) &&
	   !xr_strcmp(*m_sGrenadeLauncherName, item_section_name))
	{
		m_flagsAddOnState &= ~CSE_ALifeItemWeapon::eWeaponAddonGrenadeLauncher;
		if(m_bGrenadeMode)
		{
			UnloadMagazine();
			PerformSwitch();
		}

		UpdateAddonsVisibility();
		return CInventoryItem::Detach(item_section_name);
	}
	else
		return inherited::Detach(item_section_name);;
}




void CWeaponMagazinedWGrenade::InitAddons()
{	
	inherited::InitAddons();

	if(GrenadeLauncherAttachable())
	{
		if(IsGrenadeLauncherAttached())
		{
			CRocketLauncher::m_fLaunchSpeed = pSettings->r_float(*m_sGrenadeLauncherName,"grenade_vel");

			if(m_bZoomEnabled && m_pHUD)
				LoadZoomOffset(*hud_sect, "grenade_normal_");
		}
		else
		{	
			if(m_bZoomEnabled && m_pHUD)
				LoadZoomOffset(*hud_sect, "");
		}
	}
}




//виртуальные функции для проигрывания анимации HUD
void CWeaponMagazinedWGrenade::PlayAnimShow()
{
	if(IsGrenadeLauncherAttached())
	{
		if(!m_bGrenadeMode)
			m_pHUD->animPlay(mhud_show_w_gl[Random.randI(mhud_show_w_gl.size())],FALSE,this);
		else
			m_pHUD->animPlay(mhud_show_g[Random.randI(mhud_show_g.size())],FALSE,this);
	}	
	else
		m_pHUD->animPlay(mhud_show[Random.randI(mhud_show.size())],FALSE,this);
}

void CWeaponMagazinedWGrenade::PlayAnimHide()
{
	if(IsGrenadeLauncherAttached())
		m_pHUD->animPlay(mhud_hide_w_gl[Random.randI(mhud_hide_w_gl.size())],TRUE,this);
	else
		m_pHUD->animPlay (mhud_hide[Random.randI(mhud_hide.size())],TRUE,this);
}

void CWeaponMagazinedWGrenade::PlayAnimReload()
{
	if(IsGrenadeLauncherAttached())
		m_pHUD->animPlay(mhud_reload_w_gl[Random.randI(mhud_reload_w_gl.size())],TRUE,this);
	else
		inherited::PlayAnimReload();
}

void CWeaponMagazinedWGrenade::PlayAnimIdle()
{
	if(IsGrenadeLauncherAttached())
	{
		if(m_bGrenadeMode)
		{
			if(IsZoomed())
				m_pHUD->animPlay(mhud_idle_g_aim[Random.randI(mhud_idle_g_aim.size())], TRUE);
			else
				m_pHUD->animPlay(mhud_idle_g[Random.randI(mhud_idle_g.size())], TRUE);
		}
		else
		{
			if(IsZoomed())
				m_pHUD->animPlay(mhud_idle_w_gl_aim[Random.randI(mhud_idle_w_gl_aim.size())], TRUE);
			else
				m_pHUD->animPlay(mhud_idle_w_gl[Random.randI(mhud_idle_w_gl.size())], TRUE);
				
		}
	}
	else
		inherited::PlayAnimIdle();
}
void CWeaponMagazinedWGrenade::PlayAnimShoot()
{
	if(this->m_bGrenadeMode)
	{
		//анимация стрельбы из подствольника
		m_pHUD->animPlay(mhud_shots_g[Random.randI(mhud_shots_g.size())],TRUE,this);
	}
	else
	{
		if(IsGrenadeLauncherAttached())
			m_pHUD->animPlay(mhud_shots_w_gl[Random.randI(mhud_shots_w_gl.size())],TRUE,this);
		else
			inherited::PlayAnimShoot();
	}
}

void  CWeaponMagazinedWGrenade::PlayAnimModeSwitch()
{
	if(m_bGrenadeMode)
		m_pHUD->animPlay(mhud_switch_g[Random.randI(mhud_switch_g.size())],TRUE,this);
	else 
		m_pHUD->animPlay(mhud_switch[Random.randI(mhud_switch.size())],TRUE,this);
}


void CWeaponMagazinedWGrenade::UpdateSounds	()
{
	inherited::UpdateSounds();

	// ref_sound positions
	if (sndShotG.snd.feedback || 
		sndReloadG.snd.feedback || 
		sndSwitch.snd.feedback)
	{
		UpdateFP					();

		if (sndShotG.snd.feedback)		sndShotG.set_position		(vLastFP);
		if (sndReloadG.snd.feedback)	sndReloadG.set_position		(vLastFP);
		if (sndSwitch.snd.feedback)		sndSwitch.set_position		(vLastFP);
	}
}

void CWeaponMagazinedWGrenade::UpdateGrenadeVisibility(bool visibility)
{
	if (H_Parent() != Level().CurrentEntity()) return;
	CKinematics* pHudVisual = smart_cast<CKinematics*>(m_pHUD->Visual());
	VERIFY(pHudVisual);
	pHudVisual->LL_SetBoneVisible(pHudVisual->LL_BoneID(*grenade_bone_name),visibility,TRUE);
}
