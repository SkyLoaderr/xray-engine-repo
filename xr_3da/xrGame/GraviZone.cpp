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

	m_dwThrowInTime = 5000;
	m_fThrowInImpulse = 800.f;
	m_fBlowoutRadiusPercent = 0.3f;

	m_fTeleHeight = 1.5f;
    m_dwTimeToTele = 7000;
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

		if(m_dwTeleTime> m_dwTimeToTele + 1000)
		{

			m_dwTeleTime = 0;

			xr_set<CObject*>::iterator it;
			for(it = m_inZone.begin(); m_inZone.end() != it; ++it) 
			{
				CGameObject* GO = dynamic_cast<CGameObject*>(*it);

				if(GO && GO->m_pPhysicsShell && !TTelekinesis::is_active_object(GO))
				{
					TTelekinesis::activate(GO, 0.1f, m_fTeleHeight, m_dwTimeToTele);
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
	CGameObject* GO = dynamic_cast<CGameObject*>(O);
	if(!GO) return;
	CEntityAlive* EA = dynamic_cast<CEntityAlive*>(GO);

	//////////////////////////////////////////////////////////////////////////
	//	зат€гиваем объет по направлению к центру зоны

	Fvector throw_in_dir;
	Fvector zone_center;
	Center(zone_center);
	throw_in_dir.sub(zone_center, GO->Position());

	float dist = throw_in_dir.magnitude();
	float dist_to_radius = dist/Radius();
	
	if(dist_to_radius>m_fBlowoutRadiusPercent)
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
		else if(GO && GO->m_pPhysicsShell)
		{
			GO->m_pPhysicsShell->applyImpulse(throw_in_dir, m_fThrowInImpulse*GO->GetMass()/500.f);
		}
	}
	else
	{
		//////////////////////////////////////////////////////////////////////////
		// выброс аномалии

		//если врем€ выброса еще не пришло
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

		
		//статистика по объекту
		m_ObjectInfoMap[O].total_damage += power;
		m_ObjectInfoMap[O].hit_num++;



		if(power > 0.01f) 
		{
			m_dwDeltaTime = 0;
			position_in_bone_space.set(0.f,0.f,0.f);

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


			PlayHitParticles(GO);
		}
	}
}