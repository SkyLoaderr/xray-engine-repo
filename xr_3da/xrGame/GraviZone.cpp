//////////////////////////////////////////////////////////////////////////
// GraviZone.cpp:	�������������� ��������
//////////////////////////////////////////////////////////////////////////
//					������� ��� �� �� 2� ���
//					���� ���������� ������, ������ �������� � 
//					��� ������� ������� (�������� � �����)
//					��������� � ������ � ������ ��� �����-��
//					�����
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "gravizone.h"

#include "PhysicsShell.h"
#include "entity_alive.h"
#include "phmovementcontrol.h"
#include "xrmessages.h"
#include "PhysicsShellHolder.h"
#include "Level.h"

CGraviZone::CGraviZone(void)
{
	m_dwTeleTime = 0;
}
CGraviZone::~CGraviZone(void)
{
}

void CGraviZone::Load(LPCSTR section)
{
	inherited::Load(section);

	m_fThrowInImpulse = pSettings->r_float(section,	"throw_in_impulse");//800.f;
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

BOOL CGraviZone::net_Spawn(LPVOID DC)
{
	return inherited::net_Spawn(DC);
}
void CGraviZone::net_Destroy()
{
	TTelekinesis::Deactivate();
	inherited::net_Destroy();
}



void CGraviZone::shedule_Update		(u32 dt)
{
	inherited::shedule_Update(dt);
	TTelekinesis::schedule_update();
}


bool  CGraviZone::BlowoutState()
{
	bool result = inherited::BlowoutState();

	if(!result)
	{
		UpdateBlowout();
		AffectObjects();
	}

	return result;
}


bool CGraviZone::IdleState()
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
				CPhysicsShellHolder * GO = dynamic_cast<CPhysicsShellHolder *>(*it);

				if(GO && GO->PPhysicsShell() && TTelekinesis::is_active_object(GO))
				{
					TTelekinesis::deactivate(GO);
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
				CPhysicsShellHolder * GO = dynamic_cast<CPhysicsShellHolder *>(*it);

				if(GO && GO->PPhysicsShell() && !TTelekinesis::is_active_object(GO))
				{
					TTelekinesis::activate(GO, 0.1f, m_fTeleHeight, m_dwTimeToTele);
					PlayTeleParticles(GO);
				}
			}
		}
	}
	else
		TTelekinesis::deactivate();


	return result;
}

void CGraviZone::Affect(CObject* O) 
{
	CPhysicsShellHolder* GO = dynamic_cast<CPhysicsShellHolder*>(O);
	if(!GO) return;
	CEntityAlive* EA = dynamic_cast<CEntityAlive*>(GO);	

	//////////////////////////////////////////////////////////////////////////
	//	���������� ����� �� ����������� � ������ ����

	Fvector throw_in_dir;
	Fvector zone_center;
	Center(zone_center);
	throw_in_dir.sub(zone_center, GO->Position());

	float dist = throw_in_dir.magnitude();
	float dist_to_radius = dist/Radius();
	//---------------------------------------------------------
	bool CanApplyPhisImpulse = GO->Local() == TRUE;
	if (EA && EA->g_Alive())
	{
		CanApplyPhisImpulse &= (Level().CurrentControlEntity() && Level().CurrentControlEntity() == EA);
	};
	//---------------------------------------------------------	
	if(dist_to_radius>m_fBlowoutRadiusPercent && CanApplyPhisImpulse)
	{
		if(EA && EA->g_Alive())
		{
			float rel_power = RelativePower(dist);
			float throw_power = m_fThrowInImpulse*rel_power*rel_power*rel_power*rel_power*rel_power;
			throw_in_dir.normalize();

			Fvector vel;
			vel.set(throw_in_dir);
			vel.mul(throw_power);
			EA->m_PhysicMovementControl->AddControlVel(vel);
		}
		else if(GO && GO->PPhysicsShell())
		{
			GO->PPhysicsShell()->applyImpulse(throw_in_dir, m_fThrowInImpulse*GO->GetMass()/500.f);
		}
	}
	else
	{
		//////////////////////////////////////////////////////////////////////////
		// ������ ��������

		//���� ����� ������� ��� �� ������
		if(m_dwBlowoutExplosionTime<(u32)m_iPreviousStateTime ||
			m_dwBlowoutExplosionTime>=(u32)m_iStateTime) return;

		Fvector position_in_bone_space;

		float power = Power(GO->Position().distance_to(zone_center));
		float impulse = m_fHitImpulseScale*power*GO->GetMass();

		if(fis_zero(dist))
		{
			impulse = 0.f;
			throw_in_dir.set(0,1,0);
		}
		else
			throw_in_dir.normalize();

		
		//���������� �� �������
		m_ObjectInfoMap[O].total_damage += power;
		m_ObjectInfoMap[O].hit_num++;

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
}


void CGraviZone::PlayTeleParticles(CGameObject* pObject)
{
	ref_str particle_str = NULL;

	//������ �������� ��� �������� ������� �������
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

	pObject->StartParticles(*particle_str, Fvector().set(0,1,0), ID());
}
void CGraviZone::StopTeleParticles(CGameObject* pObject)
{
	ref_str particle_str = NULL;

	//������ �������� ��� �������� ������� �������
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

	//���������� ��������
	pObject->StopParticles	(*particle_str);
}

