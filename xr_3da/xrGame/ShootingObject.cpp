//////////////////////////////////////////////////////////////////////
// ShootingObject.cpp:  ��������� ��� ��������� ���������� �������� 
//						(������ � ���������� �������) 	
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "ShootingObject.h"

#include "ParticlesObject.h"
#include "WeaponAmmo.h"

#include "actor.h"

#define HIT_POWER_EPSILON 0.05f
#define WALLMARK_SIZE 0.04f

CShootingObject::CShootingObject(void)
{
	fTimeToFire			= 0;
	iHitPower			= 0;
	m_fStartBulletSpeed = 1000.f;


	m_fCurrentHitPower = 0.0f;
	m_fCurrentHitImpulse = 0.0f;
	m_fCurrentFireDist = 0.0f;
	m_fCurrentWallmarkSize = WALLMARK_SIZE;
	m_pCurrentCartridge = NULL;
	m_vCurrentShootDir = Fvector().set(0,0,0);
	m_vCurrentShootPos = Fvector().set(0,0,0);
	m_vEndPoint = Fvector().set(0,0,0);
	m_iCurrentParentID = 0xFFFF;
	m_eCurrentHitType = ALife::eHitTypeFireWound;


	//lights
	light_render				= ::Render->light_create();
	light_render->set_shadow	(true);
	m_bShotLight = true;

	//particles
	m_sFlameParticlesCurrent = m_sFlameParticles = m_sFlameParticles2 = NULL;
	m_sSmokeParticlesCurrent = m_sSmokeParticles = NULL;
	m_sShellParticles = NULL;

}
CShootingObject::~CShootingObject(void)
{
	//lights
	::Render->light_destroy	(light_render);
}

void CShootingObject::reinit()
{
}

void CShootingObject::Load	(LPCSTR section)
{
	//������� ��������� ������
	fireDispersionBase	= pSettings->r_float		(section,"fire_dispersion_base"	);
	fireDispersionBase	= deg2rad					(fireDispersionBase);

	// tracer
	tracerHeadSpeed		= pSettings->r_float		(section,"tracer_head_speed"	);
	tracerTrailCoeff	= pSettings->r_float		(section,"tracer_trail_scale"	);
	tracerStartLength	= pSettings->r_float		(section,"tracer_start_length"	);
	tracerWidth			= pSettings->r_float		(section,"tracer_width"			);


	//����� ������������� �� �������
	fTimeToFire			= pSettings->r_float		(section,"rpm");
	VERIFY(fTimeToFire>0.f);
	fTimeToFire			= 60.f / fTimeToFire;

	LoadFireParams		(section, "");
	LoadLights			(section, "");
	LoadShellParticles	(section, "");
	LoadFlameParticles	(section, "");
}

void CShootingObject::LoadFireParams(LPCSTR section, LPCSTR prefix)
{
	string256 full_name;

	//���� �������� � ��� ���������
	iHitPower			= pSettings->r_s32		(section,strconcat(full_name, prefix, "hit_power"));
	fHitImpulse			= pSettings->r_float	(section,strconcat(full_name, prefix, "hit_impulse"));
	//������������ ���������� ������ ����
	fireDistance		= pSettings->r_float	(section,strconcat(full_name, prefix, "fire_distance"));
	//��������� �������� ����
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

void CShootingObject::Light_Render	(Fvector& P)
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

	strconcat(full_name, prefix, "flame_particles_2");
	if(pSettings->line_exist(section, full_name))
		m_sFlameParticles2 = pSettings->r_string(section, full_name);

	strconcat(full_name, prefix, "smoke_particles");
	if(pSettings->line_exist(section, full_name))
		m_sSmokeParticles = pSettings->r_string (section, full_name);

	//������� ��������
	m_sFlameParticlesCurrent = m_sFlameParticles;
	m_sSmokeParticlesCurrent = m_sSmokeParticles;
}


//////////////////////////////////////////////////////////////////////////
// Weapon particles
//////////////////////////////////////////////////////////////////////////
//�������� �����
void CShootingObject::OnShellDrop	(const Fvector& play_pos,
									 const Fvector& parent_vel)
{
	if(!m_sShellParticles) return;

	CParticlesObject* pShellParticles = xr_new<CParticlesObject>(*m_sShellParticles,Sector());

	Fmatrix particles_pos; 
	particles_pos.set(XFORM());
	particles_pos.c.set(play_pos);

	pShellParticles->UpdateParent(particles_pos, parent_vel); 
	pShellParticles->Play();
}



void CShootingObject::StartFlameParticles2	()
{
	StartParticles (m_pFlameParticles2, *m_sFlameParticles2, CurrentFirePoint2());
}
void CShootingObject::StopFlameParticles2	()
{
	StopParticles (m_pFlameParticles2);
}
void CShootingObject::UpdateFlameParticles2	()
{
	UpdateParticles (m_pFlameParticles2, CurrentFirePoint2());
}



//�������� ����
void CShootingObject::StartSmokeParticles	(const Fvector& play_pos,
											const Fvector& parent_vel)
{
	CParticlesObject* pSmokeParticles = NULL;
	StartParticles(pSmokeParticles, *m_sSmokeParticlesCurrent, play_pos, parent_vel, true);
}


void CShootingObject::StartFlameParticles	()
{
	if(!m_sFlameParticlesCurrent) return;

	//���� �������� �����������
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
	pos.set(XFORM()); 
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