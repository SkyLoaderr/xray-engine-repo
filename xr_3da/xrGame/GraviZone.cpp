//////////////////////////////////////////////////////////////////////////
// GraviZone.cpp:	гравитационна€ аномали€
//////////////////////////////////////////////////////////////////////////
//					состоит как бы из 2х зон
//					одна зат€гивает объект, друга€ взрывает и 
//					все неживые объекты (предметы и трупы)
//					поднимает в воздух и качает там какое-то
//					врем€
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "gravizone.h"

#include "PhysicsShell.h"
#include "entity_alive.h"
#include "phmovementcontrol.h"
#include "xrmessages.h"
#include "PhysicsShellHolder.h"
#include "Level.h"

CBaseGraviZone ::CBaseGraviZone (void)
{
	m_dwTeleTime = 0;
}
CBaseGraviZone ::~CBaseGraviZone (void)
{
	
}

void CBaseGraviZone ::Load(LPCSTR section)
{
	inherited::Load(section);

	
	m_fThrowInImpulse = pSettings->r_float(section,	"throw_in_impulse");//800.f;
	m_fThrowInImpulseAlive = pSettings->r_float(section,	"throw_in_impulse_alive");//800.f;
	m_fThrowInAtten = pSettings->r_float(section,	"throw_in_atten");
	m_fBlowoutRadiusPercent = pSettings->r_float(section,"blowout_radius_percent");//0.3f;

	m_fTeleHeight = pSettings->r_float(section,		 "tele_height");//1.5f;
    m_dwTimeToTele = pSettings->r_u32(section,		"time_to_tele");//7000;
	m_dwTelePause = pSettings->r_u32(section,		"tele_pause");//1000

	if(pSettings->line_exist(section,	"tele_particles_big"))
		m_sTeleParticlesBig = pSettings->r_string(section,	"tele_particles_big");
	else
		m_sTeleParticlesBig = NULL;
	
	if(pSettings->line_exist(section, "tele_particles_small"))
		m_sTeleParticlesSmall = pSettings->r_string(section, "tele_particles_small");
	else
		m_sTeleParticlesSmall = NULL;
}

BOOL CBaseGraviZone ::net_Spawn(LPVOID DC)
{
	return inherited::net_Spawn(DC);
}
void CBaseGraviZone ::net_Destroy()
{
	Telekinesis().Deactivate();
	inherited::net_Destroy();
}



void CBaseGraviZone ::shedule_Update		(u32 dt)
{
	inherited::shedule_Update(dt);
	Telekinesis().schedule_update();
}


bool  CBaseGraviZone ::BlowoutState()
{
	bool result = inherited::BlowoutState();

//	if(!result)
	{
		UpdateBlowout();
		AffectObjects();
	}

	return result;
}


bool CBaseGraviZone ::IdleState()
{
	bool result = inherited::IdleState();

	m_dwTeleTime += Device.dwTimeDelta;

	if(!result)
	{
		if(m_dwTeleTime> m_dwTimeToTele)
		{
			xr_set<CObject*>::iterator it;
			for(it = m_inZone.begin(); m_inZone.end() != it; ++it) 
			{
				CPhysicsShellHolder * GO = smart_cast<CPhysicsShellHolder *>(*it);

				if(GO && GO->PPhysicsShell() && Telekinesis().is_active_object(GO))
				{
					Telekinesis().deactivate(GO);
					StopTeleParticles(GO);
				}
			}
		}
		if(m_dwTeleTime> m_dwTimeToTele + m_dwTelePause)
		{
			m_dwTeleTime = 0;

			xr_set<CObject *>::iterator it;
			for(it = m_inZone.begin(); m_inZone.end() != it; ++it) 
			{
				CPhysicsShellHolder * GO = smart_cast<CPhysicsShellHolder *>(*it);

				if(GO && GO->PPhysicsShell() && !Telekinesis().is_active_object(GO))
				{
					Telekinesis().activate(GO, 0.1f, m_fTeleHeight, m_dwTimeToTele);
					PlayTeleParticles(GO);
				}
			}
		}
	}
	else
		Telekinesis().deactivate();


	return result;
}

void CBaseGraviZone ::Affect(CObject* O) 
{
	CPhysicsShellHolder* GO = smart_cast<CPhysicsShellHolder*>(O);
	if(!GO) return;


	//////////////////////////////////////////////////////////////////////////
	//	зат€гиваем объет по направлению к центру зоны

	Fvector throw_in_dir;
	Fvector zone_center;
	ThrowInCenter(zone_center);
	throw_in_dir.sub(zone_center, GO->Position());

	float dist = throw_in_dir.magnitude();
	float dist_to_radius = dist/Radius();

	if(!fis_zero(dist))
	{
		throw_in_dir.mul(1.f/dist);
	}
	else throw_in_dir.set(0.f,1.f,0.f);
	//---------------------------------------------------------
	bool CanApplyPhisImpulse = GO->Local() == TRUE;
/*	if (EA && EA->g_Alive())
	{
		CanApplyPhisImpulse &= (Level().CurrentControlEntity() && Level().CurrentControlEntity() == EA);
	};*/
	//---------------------------------------------------------	
	if(dist_to_radius>m_fBlowoutRadiusPercent && CanApplyPhisImpulse)
	{
		AffectPull(GO,throw_in_dir,dist);
	}
	else
	{
		//////////////////////////////////////////////////////////////////////////
		// выброс аномалии
		
		//если врем€ выброса еще не пришло
		if(m_dwBlowoutExplosionTime<(u32)m_iPreviousStateTime ||
			m_dwBlowoutExplosionTime>=(u32)m_iStateTime) return;
		AffectThrow(GO,throw_in_dir,dist);
			
	}
}

void CBaseGraviZone ::  ThrowInCenter(Fvector& C)
{
	Center(C);
}
void CBaseGraviZone ::	AffectPull(CPhysicsShellHolder* GO,const Fvector& throw_in_dir,float dist)
{
	CEntityAlive* EA = smart_cast<CEntityAlive*>(GO);	
	if(EA && EA->g_Alive())
	{
		AffectPullAlife(EA,throw_in_dir,dist);
	}
	else if(GO && GO->PPhysicsShell())
	{
		AffectPullDead(GO,throw_in_dir,dist);
	}
}
void CBaseGraviZone ::	AffectPullAlife(CEntityAlive* EA,const Fvector& throw_in_dir,float dist)
{
			float rel_power = RelativePower(dist);
			float throw_power = m_fThrowInImpulseAlive*rel_power*rel_power*rel_power*rel_power*rel_power;
			//throw_in_dir.normalize();

			Fvector vel;
			vel.set(throw_in_dir);
			vel.mul(throw_power);
			EA->m_PhysicMovementControl->AddControlVel(vel);
}
void CBaseGraviZone ::	AffectPullDead(CPhysicsShellHolder* GO,const Fvector& throw_in_dir,float dist)
{
			GO->PPhysicsShell()->applyImpulse(throw_in_dir,dist * m_fThrowInImpulse*GO->GetMass()/100.f);
}
void CBaseGraviZone ::	AffectThrow(CPhysicsShellHolder* GO,const Fvector& throw_in_dir,float dist)
{

	Fvector position_in_bone_space;

	float power = Power(dist);//Power(GO->Position().distance_to(zone_center));
	float impulse = m_fHitImpulseScale*power*GO->GetMass();

	//if(fis_zero(dist))
	//{
	//	impulse = 0.f;
	//	throw_in_dir.set(0,1,0);
	//}
	//else
	//	throw_in_dir.normalize();


	//статистика по объекту
	m_ObjectInfoMap[smart_cast<CObject*>(GO)].total_damage += power;
	m_ObjectInfoMap[smart_cast<CObject*>(GO)].hit_num++;

	if(power > 0.01f) 
	{
		m_dwDeltaTime = 0;
		position_in_bone_space.set(0.f,0.f,0.f);

		if (OnServer())
		{
			NET_Packet	l_P;
			u_EventGen	(l_P,GE_HIT, GO->ID());
			l_P.w_u16	(u16(GO->ID()));
			l_P.w_u16	(ID());
			l_P.w_dir	(throw_in_dir);
			l_P.w_float	(power);
			l_P.w_s16	(0/*(s16)BI_NONE*/);
			l_P.w_vec3	(position_in_bone_space);
			l_P.w_float	(impulse);
			l_P.w_u16	((u16)m_eHitTypeBlowout);
			u_EventSend	(l_P);
		};


		PlayHitParticles(GO);
	}
}


void CBaseGraviZone ::PlayTeleParticles(CGameObject* pObject)
{
	CParticlesPlayer* PP = smart_cast<CParticlesPlayer*>(pObject);
	if(!PP) return;

	shared_str particle_str = NULL;

	//разные партиклы дл€ объектов разного размера
	if(pObject->Radius()<SMALL_OBJECT_RADIUS)
	{
		if(!m_sTeleParticlesSmall) return;
		particle_str = m_sTeleParticlesSmall;
	}
	else
	{
		if(!m_sTeleParticlesBig) return;
		particle_str = m_sTeleParticlesBig;
	}

	PP->StartParticles(*particle_str, Fvector().set(0,1,0), ID());
}
void CBaseGraviZone ::StopTeleParticles(CGameObject* pObject)
{
	CParticlesPlayer* PP = smart_cast<CParticlesPlayer*>(pObject);
	if(!PP) return;

	shared_str particle_str = NULL;

	//разные партиклы дл€ объектов разного размера
	if(pObject->Radius()<SMALL_OBJECT_RADIUS)
	{
		if(!m_sTeleParticlesSmall) return;
		particle_str = m_sTeleParticlesSmall;
	}
	else
	{
		if(!m_sTeleParticlesBig) return;
		particle_str = m_sTeleParticlesBig;
	}

	OBJECT_INFO_MAP_IT it	= m_ObjectInfoMap.find(pObject);
	if(m_ObjectInfoMap.end() == it) return;

	//остановить партиклы
	PP->StopParticles	(*particle_str);
}

float CBaseGraviZone ::RelativePower(float dist)
{
	float radius = Radius();
//	if(dist>radius*m_fBlowoutRadiusPercent) return 0.f;

	radius = Radius()*m_fThrowInAtten;
	float power = radius < dist ? 0 : (1.f - m_fAttenuation*(dist/radius)*(dist/radius));
	return power < 0 ? 0 : power;
}

