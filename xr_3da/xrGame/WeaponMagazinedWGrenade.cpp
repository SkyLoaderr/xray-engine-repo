#include "stdafx.h"
#include "weaponmagazinedwgrenade.h"
#include "WeaponHUD.h"
#include "entity.h"
#include "PSObject.h"
#include "ParticlesObject.h"
#include "GrenadeLauncher.h"
#include "WeaponFakeGrenade.h"
#include "xrserver_objects_alife_items.h"

//////////////////////////////////////////////////////////////////////////////////////////////////

CWeaponMagazinedWGrenade::CWeaponMagazinedWGrenade(LPCSTR name,ESoundTypes eSoundType) : CWeaponMagazined(name, eSoundType)
{
	m_ammoType2 = 0;

	m_pGrenade = NULL;
    m_bGrenadeMode = false;
	m_fGrenadeVel = 0.f;
}

CWeaponMagazinedWGrenade::~CWeaponMagazinedWGrenade(void)
{
}

void CWeaponMagazinedWGrenade::Load	(LPCSTR section)
{
	inherited::Load		(section);
	//// Sounds
	LoadSound(section,"snd_shoot_grenade"	, sndShotG		, TRUE, m_eSoundShot);
	LoadSound(section,"snd_reload_grenade"	, sndReloadG	, TRUE, m_eSoundReload);
	LoadSound(section,"snd_switch"			, sndSwitch		, TRUE, m_eSoundReload);
	

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
		m_fGrenadeVel = pSettings->r_float(section, "grenade_vel");
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

	// sounds
	DestroySound(sndShotG);
	DestroySound(sndReloadG);
	DestroySound(sndSwitch);
}


BOOL CWeaponMagazinedWGrenade::net_Spawn(LPVOID DC) 
{
	m_pGrenadePoint = &vLastFP2;
	BOOL l_res = inherited::net_Spawn(DC);
	 
	CKinematics* V = PKinematics(m_pHUD->Visual()); R_ASSERT(V);
	V->LL_GetBoneInstance(V->LL_BoneID(grenade_bone_name)).set_callback(GrenadeCallback, this);

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
		PlaySound(sndReloadG,vLastFP);

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
		PlaySound(sndShotG, vLastFP);
		
		AddShotEffector		();
		
		//партиклы огня вылета гранаты из подствольника
		StartFlameParticles2();
	} 
	else inherited::OnShot();
}
//переход в режим подствольника или выход из него
void CWeaponMagazinedWGrenade::SwitchMode() 
{
	if(!IsGrenadeLauncherAttached() || eIdle != STATE || IsPending())
	{
		return;
	}
	m_bPending = true;

	PerformSwitch();
	
	UpdateFP();
	PlaySound(sndSwitch,vLastFP);

	PlayAnimModeSwitch();
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
}

bool CWeaponMagazinedWGrenade::Action(s32 cmd, u32 flags) 
{
	if(inherited::Action(cmd, flags)) return true;
	
	switch(cmd) 
	{
	case kWPN_ZOOM: 
	case kWPN_FUNC: 
			{
                if(flags&CMD_START) SwitchMode();
				return true;
			}
	}
	return false;
}

void CWeaponMagazinedWGrenade::state_Fire(float dt) 
{
	//режим стрельбы подствольника
	if(m_bGrenadeMode)
	{
		UpdateFP				();
		fTime					-=dt;
		Fvector					p1, d; 
		p1.set(vLastFP); 
		d.set(vLastFD);
		
		if(H_Parent()) 
			dynamic_cast<CEntity*>	(H_Parent())->g_fireParams	(this, p1,d);
		else 
			return;
		
		while (fTime<0)
		{

			fTime			+=	fTimeToFire;

			++m_shotNum;
			OnShot			();
			
			// Ammo
			if(Local()) 
			{
				m_magazine.pop	();
				if(!(--iAmmoElapsed)) OnMagazineEmpty();
			}
		}
		UpdateSounds			();
		if(m_shotNum == m_queueSize) FireEnd();
	} 
	//режим стрельбы очередями
	else inherited::state_Fire(dt);
}

void CWeaponMagazinedWGrenade::SwitchState(u32 S) 
{
	inherited::SwitchState(S);
	
	//стрельнуть из подствольника
	if(m_bGrenadeMode && STATE == eIdle && S == eFire && m_pGrenade) 
	{
		Fvector						p1, d; 
		p1.set(vLastFP); 
		d.set(vLastFD);

		CEntity*					E = dynamic_cast<CEntity*>(H_Parent());
		
		if (E) E->g_fireParams		(this, p1,d);
		
		m_pGrenade->m_pos.set(p1);
		m_pGrenade->m_vel.set(d); 
		m_pGrenade->m_vel.y += .0f; 
		m_pGrenade->m_vel.mul(m_fGrenadeVel);
		m_pGrenade->m_pOwner = dynamic_cast<CGameObject*>(H_Parent());
		
		NET_Packet P;
		u_EventGen(P,GE_OWNERSHIP_REJECT,ID());
		P.w_u16(u16(m_pGrenade->ID()));
		u_EventSend(P);
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
				CWeaponFakeGrenade *l_pG = dynamic_cast<CWeaponFakeGrenade*>(Level().Objects.net_Find(id));
				VERIFY(l_pG);
				m_pGrenade = l_pG;
				m_pGrenade->m_iCurrentParentID = ID();
				l_pG->H_SetParent(this);
			}
			break;
		case GE_OWNERSHIP_REJECT :
			{
				P.r_u16(id);
				CWeaponFakeGrenade *l_pG = dynamic_cast<CWeaponFakeGrenade*>(Level().Objects.net_Find(id));
				m_pGrenade = NULL;
				l_pG->H_SetParent(0);
				break;
			}
	}
}

void CWeaponMagazinedWGrenade::ReloadMagazine() 
{
	inherited::ReloadMagazine();
	
	//перезарядка подствольного гранатомета
	if(iAmmoElapsed && !m_pGrenade && m_bGrenadeMode) 
	{
		ref_str fake_grenade_name = pSettings->r_string(*m_pAmmo->cNameSect(), "fake_grenade_name");
		SpawFakeGrenade(*fake_grenade_name);
	}
}

void __stdcall CWeaponMagazinedWGrenade::GrenadeCallback(CBoneInstance* B) 
{
	CWeaponMagazinedWGrenade* l_pW = dynamic_cast<CWeaponMagazinedWGrenade*>(
									 static_cast<CObject*>(B->Callback_Param)); 
	R_ASSERT(l_pW);
	if(l_pW->m_bHideGrenade) B->mTransform.scale(EPS, EPS, EPS);
}

void CWeaponMagazinedWGrenade::OnStateSwitch(u32 S) 
{
	inherited::OnStateSwitch(S);
	m_bHideGrenade = (!iAmmoElapsed && !(S == eReload));
}


void CWeaponMagazinedWGrenade::OnAnimationEnd()
{
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
	CGrenadeLauncher* pGrenadeLauncher = dynamic_cast<CGrenadeLauncher*>(pIItem);
	
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
	CGrenadeLauncher* pGrenadeLauncher = dynamic_cast<CGrenadeLauncher*>(pIItem);
	
	if(pGrenadeLauncher &&
	   CSE_ALifeItemWeapon::eAddonAttachable == m_eGrenadeLauncherStatus &&
	   0 == (m_flagsAddOnState&CSE_ALifeItemWeapon::eWeaponAddonGrenadeLauncher) &&
	   !xr_strcmp(*m_sGrenadeLauncherName, pIItem->cNameSect()))
	{
		m_flagsAddOnState |= CSE_ALifeItemWeapon::eWeaponAddonGrenadeLauncher;

		m_fGrenadeVel = pGrenadeLauncher->GetGrenadeVel();

 		//уничтожить подствольник из инвентаря
		pIItem->Drop();
		NET_Packet P;
		u_EventGen(P,GE_DESTROY,pIItem->ID());
		Msg					("ge_destroy: [%d] - %s",pIItem->ID(),*pIItem->cName());
		P.w_u16(u16(pIItem->ID()));
		u_EventSend(P);

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
			m_fGrenadeVel = pSettings->r_float(*m_sGrenadeLauncherName,"grenade_vel");

			if(m_bZoomEnabled && m_pHUD)
			{
				m_pHUD->SetZoomOffset(pSettings->r_fvector3(hud_sect, "grenade_zoom_offset"));
				m_pHUD->SetZoomRotateY(pSettings->r_float(hud_sect, "grenade_zoom_rotate_y"));
			}
		}
		else
		{	
			if(m_bZoomEnabled && m_pHUD)
			{
				m_pHUD->SetZoomOffset(pSettings->r_fvector3(hud_sect, "zoom_offset"));
				m_pHUD->SetZoomRotateY(pSettings->r_float(hud_sect, "zoom_rotate_y"));
			}
		}
	}
}


void CWeaponMagazinedWGrenade::SpawFakeGrenade(const char* grenade_section_name)
{
		CSE_Abstract*		D	= F_entity_Create(grenade_section_name);
		R_ASSERT			(D);
	
	
		// Fill
		strcpy				(D->s_name, grenade_section_name);
		strcpy				(D->s_name_replace,"");
		D->s_gameid			=	u8(GameID());
		D->s_RP				=	0xff;
		D->ID				=	0xffff;
		D->ID_Parent		=	(u16)ID();
		D->ID_Phantom		=	0xffff;
		D->s_flags.set		(M_SPAWN_OBJECT_LOCAL);
		D->RespawnTime		=	0;
		// Send
		NET_Packet			P;
		D->Spawn_Write		(P,TRUE);
		Level().Send		(P,net_flags(TRUE));
		// Destroy
		F_entity_Destroy	(D);
}


//виртуальные функции для проигрывания анимации HUD
void CWeaponMagazinedWGrenade::PlayAnimShow()
{
	if(IsGrenadeLauncherAttached())
	{
		if(!m_bGrenadeMode)
			m_pHUD->animPlay(mhud_show_w_gl[Random.randI(mhud_idle.size())],FALSE,this);
		else
			m_pHUD->animPlay(mhud_show_g[Random.randI(mhud_idle.size())],FALSE,this);
	}	
	else
		m_pHUD->animPlay(mhud_show[Random.randI(mhud_show.size())],FALSE,this);
}

void CWeaponMagazinedWGrenade::PlayAnimHide()
{
	if(IsGrenadeLauncherAttached())
		m_pHUD->animPlay(mhud_hide_w_gl[Random.randI(mhud_idle.size())],TRUE,this);
	else
		m_pHUD->animPlay (mhud_hide[Random.randI(mhud_hide.size())],TRUE,this);
}

void CWeaponMagazinedWGrenade::PlayAnimReload()
{
	if(IsGrenadeLauncherAttached())
		m_pHUD->animPlay(mhud_reload_w_gl[Random.randI(mhud_idle.size())],TRUE,this);
	else
		m_pHUD->animPlay(mhud_reload[Random.randI(mhud_reload.size())],TRUE,this);
}

void CWeaponMagazinedWGrenade::PlayAnimIdle()
{
	if(IsGrenadeLauncherAttached())
	{
		if(m_bGrenadeMode)
		{
			if(IsZoomed())
				m_pHUD->animPlay(mhud_idle_g_aim[Random.randI(mhud_idle.size())]);
			else
				m_pHUD->animPlay(mhud_idle_g[Random.randI(mhud_idle_g.size())]);
		}
		else
		{
			if(IsZoomed())
				m_pHUD->animPlay(mhud_idle_w_gl_aim[Random.randI(mhud_idle.size())]);	
			else
				m_pHUD->animPlay(mhud_idle_w_gl[Random.randI(mhud_idle.size())]);	
				
		}
	}
	else
	{
		if(IsZoomed())
			m_pHUD->animPlay(mhud_idle_aim[Random.randI(mhud_idle.size())]);
		else
			m_pHUD->animPlay(mhud_idle[Random.randI(mhud_idle.size())]);
	}
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
			m_pHUD->animPlay(mhud_shots_w_gl[Random.randI(mhud_idle.size())],TRUE,this);
		else
			m_pHUD->animPlay(mhud_shots[Random.randI(mhud_shots.size())],TRUE,this);
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