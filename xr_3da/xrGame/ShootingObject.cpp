//////////////////////////////////////////////////////////////////////
// ShootingObject.cpp:  интерфейс для семейства стреляющих объектов 
//						(оружие и осколочные гранаты) 	
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "ShootingObject.h"

#include "ParticlesObject.h"
#include "WeaponAmmo.h"

#include "actor.h"
#include "level.h"
#include "level_bullet_manager.h"

#define HIT_POWER_EPSILON 0.05f
#define WALLMARK_SIZE 0.04f

CShootingObject::CShootingObject(void)
{
	fTime				= 0;
 	fTimeToFire			= 0;
	iHitPower			= 0;
	m_fStartBulletSpeed = 1000.f;


	m_fCurrentHitPower = 0.0f;
	m_fCurrentHitImpulse = 0.0f;
	m_fCurrentFireDist = 0.0f;
	m_fCurrentWallmarkSize = WALLMARK_SIZE;
	m_vCurrentShootDir = Fvector().set(0,0,0);
	m_vCurrentShootPos = Fvector().set(0,0,0);
	m_iCurrentParentID = 0xFFFF;
	m_eCurrentHitType = ALife::eHitTypeFireWound;


	//particles
	m_sFlameParticlesCurrent = m_sFlameParticles = NULL;
	m_sSmokeParticlesCurrent = m_sSmokeParticles = NULL;
	m_sShellParticles = NULL;
	
	bWorking			= false;

	reinit();

}
CShootingObject::~CShootingObject(void)
{
}

void CShootingObject::reinit()
{
	m_pFlameParticles	= NULL;
}

void CShootingObject::Load	(LPCSTR section)
{
	//базовая дисперсия оружия
	fireDispersionBase	= pSettings->r_float		(section,"fire_dispersion_base"	);
	fireDispersionBase	= deg2rad					(fireDispersionBase);


	//время затрачиваемое на выстрел
	fTimeToFire			= pSettings->r_float		(section,"rpm");
	VERIFY(fTimeToFire>0.f);
	fTimeToFire			= 60.f / fTimeToFire;

	LoadFireParams		(section, "");
	LoadLights			(section, "");
	LoadShellParticles	(section, "");
	LoadFlameParticles	(section, "");
}

void CShootingObject::Light_Create		()
{
	//lights
	light_render				= ::Render->light_create();
	light_render->set_shadow	(true);
	m_bShotLight = true;
}
void CShootingObject::Light_Destroy		()
{
	//lights
	::Render->light_destroy	(light_render);
}


void CShootingObject::LoadFireParams(LPCSTR section, LPCSTR prefix)
{
	string256 full_name;

	//сила выстрела и его мощьность
	iHitPower			= pSettings->r_s32		(section,strconcat(full_name, prefix, "hit_power"));
	fHitImpulse			= pSettings->r_float	(section,strconcat(full_name, prefix, "hit_impulse"));
	//максимальное расстояние полета пули
	fireDistance		= pSettings->r_float	(section,strconcat(full_name, prefix, "fire_distance"));
	//начальная скорость пули
	m_fStartBulletSpeed = pSettings->r_float	(section,strconcat(full_name, prefix, "bullet_speed"));
}


void CShootingObject::LoadLights(LPCSTR section, LPCSTR prefix)
{
	string256 full_name;

	// light
	if(m_bShotLight) 
	{
		Fvector clr			= pSettings->r_fvector3		(section, strconcat(full_name, prefix, "light_color"));
		light_base_color.set(clr.x,clr.y,clr.z,1);
		light_base_range	= pSettings->r_float		(section, strconcat(full_name, prefix, "light_range")		);
		light_var_color		= pSettings->r_float		(section, strconcat(full_name, prefix, "light_var_color")	);
		light_var_range		= pSettings->r_float		(section, strconcat(full_name, prefix, "light_var_range")	);
		light_lifetime		= pSettings->r_float		(section, strconcat(full_name, prefix, "light_time")		);
		light_time			= -1.f;
	}
}


void CShootingObject::Light_Start	()
{
	if (Device.dwFrame	!= light_frame)
	{
		light_frame					= Device.dwFrame;
		light_time					= light_lifetime;
		
		light_build_color.set		(Random.randFs(light_var_color,light_base_color.r),Random.randFs(light_var_color,light_base_color.g),Random.randFs(light_var_color,light_base_color.b),1);
		light_build_range			= Random.randFs(light_var_range,light_base_range);

		light_render->set_active	(true);
	}
}

void CShootingObject::Light_Render	(const Fvector& P)
{
	float light_scale			= light_time/light_lifetime;
	light_render->set_position	(P);
	light_render->set_color		(light_build_color.r*light_scale,light_build_color.g*light_scale,light_build_color.b*light_scale);
	light_render->set_range		(light_build_range*light_scale);
}


//////////////////////////////////////////////////////////////////////////
// Particles
//////////////////////////////////////////////////////////////////////////

void CShootingObject::StartParticles (CParticlesObject*& pParticles, LPCSTR particles_name, 
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
void CShootingObject::StopParticles (CParticlesObject*&	pParticles)
{
	if(pParticles == NULL) return;

	pParticles->Stop();
	pParticles->PSI_destroy();
	pParticles = NULL;

}
void CShootingObject::UpdateParticles (CParticlesObject*& pParticles, 
							   const Fvector& pos, const Fvector& vel)
{
	if(!pParticles) return;

	Fmatrix particles_pos; 
	particles_pos.set(ParticlesXFORM());
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


void CShootingObject::LoadShellParticles (LPCSTR section, LPCSTR prefix)
{
	string256 full_name;
	strconcat(full_name, prefix, "shell_particles");

	if(pSettings->line_exist(section,full_name)) 
	{
		m_sShellParticles = pSettings->r_string (section,full_name);
		vShellPoint	= pSettings->r_fvector3	(section,strconcat(full_name, prefix, "shell_point"));
	}
}

void CShootingObject::LoadFlameParticles (LPCSTR section, LPCSTR prefix)
{
	string256 full_name;

	// flames
	strconcat(full_name, prefix, "flame_particles");
	if(pSettings->line_exist(section, full_name))
		m_sFlameParticles	= pSettings->r_string (section, full_name);

	strconcat(full_name, prefix, "smoke_particles");
	if(pSettings->line_exist(section, full_name))
		m_sSmokeParticles = pSettings->r_string (section, full_name);

	strconcat(full_name, prefix, "shot_particles");
	if(pSettings->line_exist(section, full_name))
		m_sShotParticles = pSettings->r_string (section, full_name);


	//текущие партиклы
	m_sFlameParticlesCurrent = m_sFlameParticles;
	m_sSmokeParticlesCurrent = m_sSmokeParticles;
}


//////////////////////////////////////////////////////////////////////////
// Weapon particles
//////////////////////////////////////////////////////////////////////////
//партиклы гильз
void CShootingObject::OnShellDrop	(const Fvector& play_pos,
									 const Fvector& parent_vel)
{
	if(!m_sShellParticles) return;

	CParticlesObject* pShellParticles = xr_new<CParticlesObject>(*m_sShellParticles,Sector());

	Fmatrix particles_pos; 
	particles_pos.set(ParticlesXFORM());
	particles_pos.c.set(play_pos);

	pShellParticles->UpdateParent(particles_pos, parent_vel); 
	pShellParticles->Play();
}


//партиклы дыма
void CShootingObject::StartSmokeParticles	(const Fvector& play_pos,
											const Fvector& parent_vel)
{
	CParticlesObject* pSmokeParticles = NULL;
	StartParticles(pSmokeParticles, *m_sSmokeParticlesCurrent, play_pos, parent_vel, true);
}


void CShootingObject::StartFlameParticles	()
{
	if(!m_sFlameParticlesCurrent) return;

	//если партиклы циклические
	if(m_pFlameParticles && m_pFlameParticles->IsLooped() && 
		m_pFlameParticles->IsPlaying()) 
	{
		UpdateFlameParticles();
		return;
	}

	StopFlameParticles();
	m_pFlameParticles = xr_new<CParticlesObject>(*m_sFlameParticlesCurrent,Sector(),false);
	UpdateFlameParticles();
	m_pFlameParticles->Play();

}
void CShootingObject::StopFlameParticles	()
{
	if(!m_sFlameParticlesCurrent) return;
	if(m_pFlameParticles == NULL) return;

	m_pFlameParticles->SetAutoRemove(true);
	m_pFlameParticles->Stop();
	m_pFlameParticles = NULL;
}

void CShootingObject::UpdateFlameParticles	()
{
	if(!m_sFlameParticlesCurrent) return;
	if(!m_pFlameParticles) return;

	Fmatrix pos; 
	pos.set(ParticlesXFORM()); 
	pos.c.set(CurrentFirePoint());

	m_pFlameParticles->SetXFORM(pos);


	if(!m_pFlameParticles->IsLooped() && 
		!m_pFlameParticles->IsPlaying() &&
		!m_pFlameParticles->PSI_alive())
	{
		m_pFlameParticles->Stop();
		m_pFlameParticles->PSI_destroy();
		m_pFlameParticles = NULL;
	}
}

//подсветка от выстрела
void CShootingObject::UpdateLight()
{
	if (m_bShotLight && light_time>0)		
	{
		light_time -= Device.fTimeDelta;
		if (light_time<=0) StopLight();
	}
}

void CShootingObject::StopLight			()
{
	light_render->set_active(false);
}

void CShootingObject::RenderLight()
{
	if (m_bShotLight && light_time>0) 
		Light_Render(CurrentFirePoint());
}

void CShootingObject::FireBullet(const Fvector& pos, 
								 const Fvector& shot_dir, 
								 float fire_disp,
								 const CCartridge& cartridge,
								 u16 parent_id,
								 u16 weapon_id)
{
	Fvector dir;
	dir.random_dir(shot_dir, fire_disp, Random);

	//инициализипровать текущие параметры выстрела перед запуском RayPick
	m_fCurrentHitPower	= float(iHitPower);
	m_fCurrentHitImpulse = fHitImpulse;
	m_fCurrentWallmarkSize = cartridge.fWallmarkSize;
	m_vCurrentShootDir = dir;
	m_vCurrentShootPos = pos;
	m_fCurrentFireDist = fireDistance;
	m_iCurrentParentID = parent_id;

	Level().BulletManager().AddBullet(	pos, dir, m_fStartBulletSpeed, float(iHitPower), 
										fHitImpulse, parent_id, weapon_id, 
										ALife::eHitTypeFireWound, fireDistance, cartridge);
}

void CShootingObject::FireStart	()
{
	bWorking=true;	
}
void CShootingObject::FireEnd	()				
{ 
	bWorking=false;	
}

void CShootingObject::StartShotParticles	()
{
	CParticlesObject* pSmokeParticles = NULL;
	StartParticles(pSmokeParticles, *m_sShotParticles, 
					m_vCurrentShootPos, m_vCurrentShootDir, true);
}