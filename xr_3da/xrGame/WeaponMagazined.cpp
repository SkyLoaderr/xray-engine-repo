#include "stdafx.h"

#include "PSObject.h"
#include "hudmanager.h"
#include "WeaponHUD.h"
#include "WeaponMagazined.h"
#include "entity.h"
#include "actor.h"
#include "actor.h"
#include "ParticlesObject.h"
#include "scope.h"
#include "silencer.h"
#include "inventory.h"
#include "xrserver_objects_alife_items.h"

#include "ActorEffector.h"
#include "EffectorZoomInertion.h"



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CWeaponMagazined::CWeaponMagazined(LPCSTR name, ESoundTypes eSoundType) : CWeapon(name)
{
	m_eSoundShow		= ESoundTypes(SOUND_TYPE_ITEM_TAKING | eSoundType);
	m_eSoundHide		= ESoundTypes(SOUND_TYPE_ITEM_HIDING | eSoundType);
	m_eSoundShot		= ESoundTypes(SOUND_TYPE_WEAPON_SHOOTING | eSoundType);
	m_eSoundEmptyClick	= ESoundTypes(SOUND_TYPE_WEAPON_EMPTY_CLICKING | eSoundType);
	m_eSoundReload		= ESoundTypes(SOUND_TYPE_WEAPON_RECHARGING | eSoundType);
	
	fTime				= 0;
	m_queueSize = m_shotNum = 0;

	m_pSndShotCurrent = NULL;
	m_sSilencerFlameParticles = m_sSilencerSmokeParticles = NULL;

	m_bFireSingleShot = false;
}

CWeaponMagazined::~CWeaponMagazined()
{
}

void CWeaponMagazined::net_Destroy()
{
	inherited::net_Destroy();

	// sounds
	DestroySound(sndShow);
	DestroySound(sndHide);
	DestroySound(sndShot);
	DestroySound(sndEmptyClick);
	DestroySound(sndReload);
}


void CWeaponMagazined::Load	(LPCSTR section)
{
	inherited::Load		(section);
		
	// Sounds
	LoadSound(section,"snd_draw"	, sndShow		, TRUE, m_eSoundShow		);
	LoadSound(section,"snd_holster"	, sndHide		, TRUE, m_eSoundHide		);
	LoadSound(section,"snd_shoot"	, sndShot		, TRUE, m_eSoundShot		);
	LoadSound(section,"snd_empty"	, sndEmptyClick	, TRUE, m_eSoundEmptyClick	);
	LoadSound(section,"snd_reload"	, sndReload		, TRUE, m_eSoundReload		);
	
	m_pSndShotCurrent = &sndShot;
		
	
	// HUD :: Anims
	R_ASSERT			(m_pHUD);
	animGet				(mhud_idle,		pSettings->r_string(*hud_sect, "anim_idle"));
	animGet				(mhud_reload,	pSettings->r_string(*hud_sect, "anim_reload"));
	animGet				(mhud_show,		pSettings->r_string(*hud_sect, "anim_draw"));
	animGet				(mhud_hide,		pSettings->r_string(*hud_sect, "anim_holster"));
	animGet				(mhud_shots,	pSettings->r_string(*hud_sect, "anim_shoot"));
	
	if(IsZoomEnabled())
		animGet				(mhud_idle_aim,		pSettings->r_string(*hud_sect, "anim_idle_aim"));
	

	//����� � �������� ���������, ����� ����� ����
	if(m_eSilencerStatus == ALife::eAddonAttachable)
	{
		if(pSettings->line_exist(section, "silencer_flame_particles"))
			m_sSilencerFlameParticles = pSettings->r_string(section, "silencer_flame_particles");
		if(pSettings->line_exist(section, "silencer_smoke_particles"))
			m_sSilencerSmokeParticles = pSettings->r_string(section, "silencer_smoke_particles");
		LoadSound(section,"snd_silncer_shot", sndSilencerShot, TRUE, m_eSoundShot);
	}
}

void CWeaponMagazined::FireStart		()
{
	if(dynamic_cast<CActor*>(H_Parent())) m_queueSize = 0;
	
	if(IsValid() && !IsMisfire()) 
	{
		if(!IsWorking() || AllowFireWhileWorking())
		{
			if(STATE==eReload) return;
			if(STATE==eShowing) return;
			if(STATE==eHiding) return;
			if(STATE==eMisfire) return;

			inherited::FireStart();
			
			if (iAmmoElapsed == 0) 
				OnMagazineEmpty();
			else
				SwitchState(eFire);
		}
	} 
	else 
	{
		if(eReload!=STATE && eMisfire!=STATE) OnMagazineEmpty();
	}
}

void CWeaponMagazined::FireEnd() 
{
	m_shotNum = 0;

	inherited::FireEnd();

	CActor	*actor = dynamic_cast<CActor*>(H_Parent());
	if(!iAmmoElapsed && actor) Reload();
}

void CWeaponMagazined::Reload() 
{
	inherited::Reload();
	TryReload();
}

bool CWeaponMagazined::TryReload() 
{
	if(m_pInventory) 
	{
		m_pAmmo = dynamic_cast<CWeaponAmmo*>(m_pInventory->Get(*m_ammoTypes[m_ammoType],
											!dynamic_cast<CActor*>(H_Parent())));

		
		if(IsMisfire())
		{
			m_bPending = true;
			SwitchState(eReload); 
			return true;
		}

		if(m_pAmmo) 
		{
			m_bPending = true;
			SwitchState(eReload); 
			return true;
		} 
		else for(u32 i = 0; i < m_ammoTypes.size(); ++i) 
		{
			m_pAmmo = dynamic_cast<CWeaponAmmo*>(m_pInventory->Get(*m_ammoTypes[i],
																   !dynamic_cast<CActor*>(H_Parent())));
			if(m_pAmmo) 
			{ 
				m_ammoType = i; 
				m_bPending = true;
				SwitchState(eReload); 
				return true; 
			}
		}
	}
	SwitchState(eIdle);
	return false;
}

bool CWeaponMagazined::IsAmmoAvailable()
{
	if (dynamic_cast<CWeaponAmmo*>(m_pInventory->Get(*m_ammoTypes[m_ammoType],!dynamic_cast<CActor*>(H_Parent()))))
		return	(true);
	else
		for(u32 i = 0; i < m_ammoTypes.size(); ++i)
			if (dynamic_cast<CWeaponAmmo*>(m_pInventory->Get(*m_ammoTypes[i],!dynamic_cast<CActor*>(H_Parent()))))
				return	(true);
	return		(false);
}

void CWeaponMagazined::OnMagazineEmpty() 
{
	//FireEnd();

	SwitchState(eMagEmpty); 
	inherited::OnMagazineEmpty();
}

void CWeaponMagazined::UnloadMagazine() 
{
	xr_map<LPCSTR, u16> l_ammo;
	
	while(!m_magazine.empty()) 
	{
		CCartridge &l_cartridge = m_magazine.top();
		xr_map<LPCSTR, u16>::iterator l_it;
		for(l_it = l_ammo.begin(); l_ammo.end() != l_it; ++l_it) 
		{
            if(!xr_strcmp(*l_cartridge.m_ammoSect, l_it->first)) 
            { 
				 ++(l_it->second); 
				 break; 
			}
		}

		if(l_it == l_ammo.end()) l_ammo[*l_cartridge.m_ammoSect] = 1;
		m_magazine.pop(); 
		--iAmmoElapsed;
	}
	
	xr_map<LPCSTR, u16>::iterator l_it;
	for(l_it = l_ammo.begin(); l_ammo.end() != l_it; ++l_it) 
	{
		CWeaponAmmo *l_pA = dynamic_cast<CWeaponAmmo*>(m_pInventory->Get(l_it->first,
														!dynamic_cast<CActor*>(H_Parent())));
		if(l_pA) 
		{
			u16 l_free = l_pA->m_boxSize - l_pA->m_boxCurr;
			l_pA->m_boxCurr = l_pA->m_boxCurr + (l_free < l_it->second ? l_free : l_it->second);
			l_it->second = l_it->second - (l_free < l_it->second ? l_free : l_it->second);
		}
		if(l_it->second) SpawnAmmo(l_it->second, l_it->first);
	}
}

void CWeaponMagazined::ReloadMagazine() 
{
	SwitchState(eIdle);

	//��������� ������ ��� �����������
	if(IsMisfire())	bMisfire = false;
	
	//���������� ��������� �������������
	//������ ������ ����� ��������
	static bool l_lockType = false;
	if(!l_lockType) m_ammoName = NULL;
	
	if(m_pInventory) 
	{
		//���������� ����� � ��������� ������� �������� ���� 
		m_pAmmo = dynamic_cast<CWeaponAmmo*>(m_pInventory->Get(*m_ammoTypes[m_ammoType],
														   !dynamic_cast<CActor*>(H_Parent())));
		
		if(!m_pAmmo && !l_lockType) 
		{
			for(u32 i = 0; i < m_ammoTypes.size(); ++i) 
			{
				//��������� ������� ���� ���������� �����
				m_pAmmo = dynamic_cast<CWeaponAmmo*>(m_pInventory->Get(*m_ammoTypes[i],
													!dynamic_cast<CActor*>(H_Parent())));
				if(m_pAmmo) 
				{ 
					m_ammoType = i; 
					break; 
				}
			}
		}
	}

	//��� �������� ��� �����������
	if(!m_pAmmo) return;

	//��������� �������, ���� ��������� ��������� ������� ����
	if(!l_lockType && !m_magazine.empty() && 
		(!m_pAmmo || xr_strcmp(m_pAmmo->cNameSect(), 
					 *m_magazine.top().m_ammoSect)))
		UnloadMagazine();
	
	CCartridge l_cartridge;
	while(iAmmoElapsed < iMagazineSize && m_pAmmo->Get(l_cartridge)) 
	{
		++iAmmoElapsed;
		m_magazine.push(l_cartridge);
	}
	m_ammoName = m_pAmmo->m_nameShort;

	//�������� ������� ��������, ���� ��� ������
	if(!m_pAmmo->m_boxCurr) m_pAmmo->Drop();

	if(iMagazineSize > iAmmoElapsed) 
	{ 
		l_lockType = true; 
		ReloadMagazine(); 
		l_lockType = false; 
	}
}

void CWeaponMagazined::OnStateSwitch	(u32 S)
{
	switch (S)
	{
	case eIdle:
		switch2_Idle	();
		break;
	case eFire:
		switch2_Fire	();
		break;
	case eFire2:
		switch2_Fire2	();
		break;
	case eMisfire:
		break;
	case eMagEmpty:
		switch2_Empty	();
		break;
	case eReload:
		switch2_Reload	();
		break;
	case eShowing:
		switch2_Showing	();
		break;
	case eHiding:
		switch2_Hiding	();
		break;
	case eHidden:
		switch2_Hidden	();
		break;
	}

	inherited::OnStateSwitch(S);
}

void CWeaponMagazined::UpdateCL			()
{
	inherited::UpdateCL	();
	float dt = Device.fTimeDelta;

	
	// cycle update
	switch (STATE)
	{
	case eShowing:
	case eHiding:
	case eReload:
	case eIdle:
		fTime			-=	dt;
		if (fTime<0)	
			fTime = 0;
		break;
	case eFire:			
		state_Fire		(dt);	
        if(fTime<=0)
			StopShooting();
		break;
	case eMisfire:		state_Misfire	(dt);	break;
	case eMagEmpty:		state_MagEmpty	(dt);	break;
	case eHidden:		break;
	}

	UpdateSounds		();


}

void CWeaponMagazined::UpdateSounds	()
{
	if (Device.dwFrame == dwUpdateSounds_Frame)  
		return;
	
	dwUpdateSounds_Frame = Device.dwFrame;

	// ref_sound positions
	if (sndShow.snd.feedback || 
		sndHide.snd.feedback || 
		sndShot.snd.feedback || 
		sndReload.snd.feedback || 
		sndEmptyClick.snd.feedback)
	{
		UpdateFP					();

		if (sndShow.snd.feedback)		sndShow.set_position		(vLastFP);
		if (sndHide.snd.feedback)		sndHide.set_position		(vLastFP);
		if (sndShot.snd.feedback)		sndShot.set_position		(vLastFP);
		if (sndReload.snd.feedback)		sndReload.set_position		(vLastFP);
		if (sndEmptyClick.snd.feedback)	sndEmptyClick.set_position	(vLastFP);
	}
}

void CWeaponMagazined::state_Fire	(float dt)
{

	fTime					-=dt;

	UpdateFP				();


	Fvector					p1, d; 
	p1.set(vLastFP);
	d.set(vLastFD);

	if(H_Parent()) 
		dynamic_cast<CEntity*>	(H_Parent())->g_fireParams	(this, p1,d);
	else 
		return;
	
	while (fTime<=0 && (IsWorking() || m_bFireSingleShot) && !m_magazine.empty())
	{
		m_bFireSingleShot = false;

		fTime			+=	fTimeToFire;

		++m_shotNum;
		OnShot			();
		FireTrace		(p1,vLastFP,d);
	}

	UpdateSounds			();

	//��� �������� ��������� ���������, 
	//� ������� ������� �������� �����
	//if(m_shotNum == m_queueSize) FireEnd();
}

void CWeaponMagazined::state_Misfire	(float /**dt/**/)
{
	UpdateFP				();
	OnEmptyClick			();
	SwitchState				(eIdle);
	
	bMisfire = true;

	UpdateSounds			();
}

void CWeaponMagazined::state_MagEmpty	(float /**dt/**/)
{
/*	UpdateFP				();
	OnEmptyClick			();
	SwitchState				(eIdle);
	UpdateSounds			();*/
}

void CWeaponMagazined::renderable_Render	()
{
	inherited::renderable_Render();
	UpdateXForm();
}

void CWeaponMagazined::SetDefaults	()
{
	CWeapon::SetDefaults		();
}

void CWeaponMagazined::Hide		()
{
	// add shot effector
	//SwitchState(eHiding);
	SwitchState(eHidden);
}
void CWeaponMagazined::Show		()
{
	SwitchState(eShowing);
}


void CWeaponMagazined::OnShot		()
{
	// Sound
	UpdateFP();

	PlaySound(*m_pSndShotCurrent,vLastFP);

	// Camera
	AddShotEffector		();

	// Animation
	PlayAnimShoot		();
	
	// Shell Drop
	OnShellDrop			();
	
	// ����� �� ������
	StartFlameParticles	();

	//��� �� ������
	StartSmokeParticles	();
}


void CWeaponMagazined::OnEmptyClick	()
{
	UpdateFP();
	PlaySound(sndEmptyClick,vLastFP);
}
void CWeaponMagazined::OnAnimationEnd() 
{
	switch(STATE) 
	{
		case eReload:	ReloadMagazine();		break;	// End of reload animation
		case eHiding:	SwitchState(eHidden);   break;	// End of Hide
		case eShowing:	SwitchState(eIdle);		break;	// End of Show
		case eIdle:		switch2_Idle();			break;  // Keep showing idle
	}
}
void CWeaponMagazined::switch2_Idle	()
{
	m_bPending = false;
	PlayAnimIdle();
}


void CWeaponMagazined::switch2_Fire	()
{
	m_bFireSingleShot = true;
}
void CWeaponMagazined::switch2_Empty()
{
	OnZoomOut();
	if(!TryReload())
	{
		OnEmptyClick();
	}
	else
	{
		inherited::FireEnd();
	}
}
void CWeaponMagazined::switch2_Reload()
{
	UpdateFP();
	PlaySound(sndReload,vLastFP);
	
	PlayAnimReload();
	m_bPending = true;
}
void CWeaponMagazined::switch2_Hiding()
{
	CWeapon::FireEnd();
	
	UpdateFP();
	PlaySound(sndHide,vLastFP);

	PlayAnimHide();
	m_bPending = true;
}

void CWeaponMagazined::switch2_Hidden()
{
	signal_HideComplete		();
	RemoveShotEffector		();
}
void CWeaponMagazined::switch2_Showing()
{
	UpdateFP();
	PlaySound(sndShow,vLastFP);

	m_bPending = true;
	PlayAnimShow();
}

bool CWeaponMagazined::Action(s32 cmd, u32 flags) 
{
	if(inherited::Action(cmd, flags)) return true;
	
	//���� ������ ���-�� ������, �� ������ �� ������
	if(IsPending()) return false;
	
	switch(cmd) 
	{
	case kWPN_RELOAD:
		{
			if(flags&CMD_START) 
				if(iAmmoElapsed < iMagazineSize || IsMisfire()) 
					Reload();
		} 
		return true;
	}
	return false;
}

bool CWeaponMagazined::CanAttach(PIItem pIItem)
{
	CScope* pScope = dynamic_cast<CScope*>(pIItem);
	CSilencer* pSilencer = dynamic_cast<CSilencer*>(pIItem);
	
	if(pScope &&
	   m_eScopeStatus == ALife::eAddonAttachable &&
	   (m_flagsAddOnState&CSE_ALifeItemWeapon::eWeaponAddonScope) == 0 &&
	   !xr_strcmp(*m_sScopeName, pIItem->cNameSect()))
       return true;
	else if(pSilencer &&
	   m_eSilencerStatus == ALife::eAddonAttachable &&
	   (m_flagsAddOnState&CSE_ALifeItemWeapon::eWeaponAddonSilencer) == 0 &&
	   !xr_strcmp(*m_sSilencerName, pIItem->cNameSect()))
       return true;
	else
		return inherited::CanAttach(pIItem);
}

bool CWeaponMagazined::CanDetach(const char* item_section_name)
{
	if( m_eScopeStatus == CSE_ALifeItemWeapon::eAddonAttachable &&
	   0 != (m_flagsAddOnState&CSE_ALifeItemWeapon::eWeaponAddonScope) &&
	   xr_strcmp(*m_sScopeName, item_section_name))
       return true;
	else if(m_eSilencerStatus == CSE_ALifeItemWeapon::eAddonAttachable &&
	   0 != (m_flagsAddOnState&CSE_ALifeItemWeapon::eWeaponAddonSilencer) &&
	   xr_strcmp(*m_sSilencerName, item_section_name))
       return true;
	else
		return inherited::CanDetach(item_section_name);
}

bool CWeaponMagazined::Attach(PIItem pIItem)
{
	bool result = false;

	CScope* pScope = dynamic_cast<CScope*>(pIItem);
	CSilencer* pSilencer = dynamic_cast<CSilencer*>(pIItem);
	
	if(pScope &&
	   m_eScopeStatus == CSE_ALifeItemWeapon::eAddonAttachable &&
	   (m_flagsAddOnState&CSE_ALifeItemWeapon::eWeaponAddonScope) == 0 &&
	   !xr_strcmp(*m_sScopeName, pIItem->cNameSect()))
	{
		m_flagsAddOnState |= CSE_ALifeItemWeapon::eWeaponAddonScope;
		result = true;
	}
	else if(pSilencer &&
	   m_eSilencerStatus == CSE_ALifeItemWeapon::eAddonAttachable &&
	   (m_flagsAddOnState&CSE_ALifeItemWeapon::eWeaponAddonSilencer) == 0 &&
	   !xr_strcmp(*m_sSilencerName, pIItem->cNameSect()))
	{
		m_flagsAddOnState |= CSE_ALifeItemWeapon::eWeaponAddonSilencer;
		result = true;
	}

	if(result)
	{
		if (OnServer())
		{
			//���������� �������������� ���� �� ���������
			pIItem->Drop();
			NET_Packet P;
			u_EventGen(P,GE_DESTROY,pIItem->ID());
			Msg					("ge_destroy: [%d] - %s",pIItem->ID(),*pIItem->cName());
			P.w_u16(u16(pIItem->ID()));
			u_EventSend(P);
		};

		UpdateAddonsVisibility();
		InitAddons();

		return true;
	}
	else
        return inherited::Attach(pIItem);
}


bool CWeaponMagazined::Detach(const char* item_section_name)
{
	if(m_eScopeStatus == CSE_ALifeItemWeapon::eAddonAttachable &&
	   0 != (m_flagsAddOnState&CSE_ALifeItemWeapon::eWeaponAddonScope) &&
	   !xr_strcmp(*m_sScopeName, item_section_name))
	{
		m_flagsAddOnState &= ~CSE_ALifeItemWeapon::eWeaponAddonScope;
		
		UpdateAddonsVisibility();
		InitAddons();

		return CInventoryItem::Detach(item_section_name);
	}
	else if(m_eSilencerStatus == CSE_ALifeItemWeapon::eAddonAttachable &&
	   0 != (m_flagsAddOnState&CSE_ALifeItemWeapon::eWeaponAddonSilencer) &&
	   !xr_strcmp(*m_sSilencerName, item_section_name))
	{
		m_flagsAddOnState &= ~CSE_ALifeItemWeapon::eWeaponAddonSilencer;

		UpdateAddonsVisibility();
		InitAddons();
		return CInventoryItem::Detach(item_section_name);
	}
	else
		return inherited::Detach(item_section_name);;
}

void CWeaponMagazined::InitAddons()
{
	//////////////////////////////////////////////////////////////////////////
	// ������
	if(IsScopeAttached())
	{
		if(m_eScopeStatus == ALife::eAddonAttachable)
		{
			m_sScopeName = pSettings->r_string(cNameSect(), "scope_name");
			m_iScopeX	 = pSettings->r_s32(cNameSect(),"scope_x");
			m_iScopeY	 = pSettings->r_s32(cNameSect(),"scope_y");

			ref_str scope_tex_name;
			scope_tex_name = pSettings->r_string(*m_sScopeName, "scope_texture");
			m_fScopeZoomFactor = pSettings->r_float	(*m_sScopeName, "scope_zoom_factor");
			if(m_UIScope.GetShader())
				m_UIScope.GetShader().destroy();	
			m_UIScope.Init(*scope_tex_name, "hud\\default", 0, 0, alNone);
		}
		else if(m_eScopeStatus == ALife::eAddonPermanent)
		{
			m_fScopeZoomFactor = pSettings->r_float	(cNameSect(), "scope_zoom_factor");
			ref_str scope_tex_name;
			scope_tex_name = pSettings->r_string(cNameSect(), "scope_texture");

			if(m_UIScope.GetShader())
				m_UIScope.GetShader().destroy();	
			m_UIScope.Init(*scope_tex_name, "hud\\default", 0, 0, alNone);
		}
	}
	else
	{
		if(m_UIScope.GetShader())
			m_UIScope.GetShader().destroy();	
		
		if(IsZoomEnabled())
			m_fScopeZoomFactor = pSettings->r_float	(cNameSect(), "scope_zoom_factor");
	}

	

	//////////////////////////////////////////////////////////////////////////
	// ���������
	if(IsSilencerAttached() && SilencerAttachable())
	{		
		m_sFlameParticlesCurrent = m_sSilencerFlameParticles;
		m_sSmokeParticlesCurrent = m_sSilencerSmokeParticles;
		m_pSndShotCurrent = &sndSilencerShot;

		//���� ��������
		iHitPower			= pSettings->r_s32		(cNameSect(),"silencer_hit_power"		);
		fHitImpulse			= pSettings->r_float	(cNameSect(),"silencer_hit_impulse"		);
		fireDistance		= pSettings->r_float	(cNameSect(),"silencer_fire_distance"	);

		//��������� �� ��������
		if(m_bShotLight) 
		{
			Fvector clr			= pSettings->r_fvector3		(cNameSect(),"silencer_light_color"		);
			light_base_color.set(clr.x,clr.y,clr.z,1);
			light_base_range	= pSettings->r_float		(cNameSect(),"silencer_light_range"		);
			light_var_color		= pSettings->r_float		(cNameSect(),"silencer_light_var_color"	);
			light_var_range		= pSettings->r_float		(cNameSect(),"silencer_light_var_range"	);
			light_lifetime		= pSettings->r_float		(cNameSect(),"silencer_light_time"		);
			light_time			= -1.f;
		}
	}
	else
	{
		m_sFlameParticlesCurrent = m_sFlameParticles;
		m_sSmokeParticlesCurrent = m_sSmokeParticles;
		m_pSndShotCurrent = &sndShot;

		//���� ��������
		iHitPower			= pSettings->r_s32		(cNameSect(),"hit_power"		);
		fHitImpulse			= pSettings->r_float	(cNameSect(),"hit_impulse"		);
		fireDistance		= pSettings->r_float	(cNameSect(),"fire_distance"	);

		//��������� �� ��������
		if(m_bShotLight) 
		{
			Fvector clr			= pSettings->r_fvector3		(cNameSect(),"light_color"		);
			light_base_color.set(clr.x,clr.y,clr.z,1);
			light_base_range	= pSettings->r_float		(cNameSect(),"light_range"		);
			light_var_color		= pSettings->r_float		(cNameSect(),"light_var_color"	);
			light_var_range		= pSettings->r_float		(cNameSect(),"light_var_range"	);
			light_lifetime		= pSettings->r_float		(cNameSect(),"light_time"		);
			light_time			= -1.f;
		}
	}

	inherited::InitAddons();
}


//����������� ������� ��� ������������ �������� HUD
void CWeaponMagazined::PlayAnimShow()
{
	m_pHUD->animPlay(mhud_show[Random.randI(mhud_show.size())],FALSE,this);
}

void CWeaponMagazined::PlayAnimHide()
{
	m_pHUD->animPlay (mhud_hide[Random.randI(mhud_hide.size())],TRUE,this);
}

void CWeaponMagazined::PlayAnimReload()
{
	m_pHUD->animPlay(mhud_reload[Random.randI(mhud_reload.size())],TRUE,this);
}

void CWeaponMagazined::PlayAnimIdle()
{
	if(IsZoomed())
		m_pHUD->animPlay(mhud_idle_aim[Random.randI(mhud_idle_aim.size())], TRUE);
	else
		m_pHUD->animPlay(mhud_idle[Random.randI(mhud_idle.size())], TRUE);
}
void CWeaponMagazined::PlayAnimShoot()
{
	m_pHUD->animPlay(mhud_shots[Random.randI(mhud_shots.size())],TRUE,this);
}


void CWeaponMagazined::OnZoomIn			()
{
	inherited::OnZoomIn();

	if(STATE == eIdle)
		PlayAnimIdle();


	CActor* pActor = dynamic_cast<CActor*>(H_Parent());
	if(pActor)
	{
		CEffectorZoomInertion* S = dynamic_cast<CEffectorZoomInertion*>	(pActor->EffectorManager().GetEffector(eCEZoom));
		if (!S)	
			S = (CEffectorZoomInertion*)pActor->EffectorManager().AddEffector(xr_new<CEffectorZoomInertion> ());
		S->SetRndSeed(pActor->GetZoomRndSeed());
		R_ASSERT				(S);
	}
}
void CWeaponMagazined::OnZoomOut		()
{
	inherited::OnZoomOut();

	if(STATE == eIdle)
		PlayAnimIdle();

	CActor* pActor = dynamic_cast<CActor*>(H_Parent());
	if(pActor)
		pActor->EffectorManager().RemoveEffector	(eCEZoom);

}