#include "stdafx.h"
#include "CustomZone.h"
#include "ZoneVisual.h"
#include "PHObject.h"
#include "PHMovementControl.h"
#include "AmebaZone.h"
#include "hudmanager.h"
#include "level.h"
#include "entity_alive.h"

CAmebaZone::CAmebaZone()
{
	m_fVelocityLimit=1.f;
}

CAmebaZone::~CAmebaZone()
{

}
void CAmebaZone::Load(LPCSTR section)
{
	inherited::Load(section);
	m_fVelocityLimit= pSettings->r_float(section,"max_velocity_in_zone");
}
bool CAmebaZone::BlowoutState()
{
	bool result = inherited::BlowoutState();
	if(!result) UpdateBlowout();
	xr_set<CObject*>::iterator it;
	for(it = m_inZone.begin(); m_inZone.end() != it; ++it) Affect(*it);
	return result;
}

void  CAmebaZone::Affect(CObject* O) 
{
	CPhysicsShellHolder *pGameObject = smart_cast<CPhysicsShellHolder*>(O);
	if(!pGameObject) return;

	if(m_ObjectInfoMap[O].zone_ignore) return;

#ifdef DEBUG
	char l_pow[255]; 
	sprintf(l_pow, "zone hit. %.1f", Power(distance_to_center(O)));
	if(bDebug) HUD().outMessage(0xffffffff,pGameObject->cName(), l_pow);
#endif
	Fvector hit_dir; 
	hit_dir.set(::Random.randF(-.5f,.5f), 
		::Random.randF(.0f,1.f), 
		::Random.randF(-.5f,.5f)); 
	hit_dir.normalize();


	Fvector position_in_bone_space;

	float power = Power(distance_to_center(O));
	float impulse = m_fHitImpulseScale*power*pGameObject->GetMass();

	//статистика по объекту
	m_ObjectInfoMap[O].total_damage += power;
	m_ObjectInfoMap[O].hit_num++;

	if(power > 0.01f) 
	{
		m_dwDeltaTime = 0;
		position_in_bone_space.set(0.f,0.f,0.f);

		if (OnServer())
		{
			NET_Packet	l_P;
			u_EventGen	(l_P,GE_HIT, pGameObject->ID());
			l_P.w_u16	(ID());
			l_P.w_u16	(ID());
			l_P.w_dir	(hit_dir);
			l_P.w_float	(power);
			l_P.w_s16	(/*(s16)BI_NONE*/0);
			l_P.w_vec3	(position_in_bone_space);
			l_P.w_float	(impulse);
			l_P.w_u16	((u16)m_eHitTypeBlowout);
			u_EventSend	(l_P);
		};


		PlayHitParticles(pGameObject);
	}
}

void CAmebaZone::PhTune(dReal step)
{
	xr_set<CObject*>::iterator it;
	for(it = m_inZone.begin(); m_inZone.end() != it; ++it) 
	{
		CEntityAlive	*EA=smart_cast<CEntityAlive*>(*it);
		if(EA)
		{
			CPHMovementControl* mc=EA->movement_control();
			if(mc)
			{
				//Fvector vel;
				//mc->GetCharacterVelocity(vel);
				//vel.invert();
				//vel.mul(mc->GetMass());
				if(distance_to_center(*it)<effective_radius())
								mc->SetVelocityLimit(m_fVelocityLimit);
			}
		}
		
	}
}

void CAmebaZone::SwitchZoneState(EZoneState new_state)
{
	if(new_state==eZoneStateBlowout&&m_eZoneState!=eZoneStateBlowout)
	{
		CPHUpdateObject::Activate();
	}

	if(new_state!=eZoneStateBlowout&&m_eZoneState==eZoneStateBlowout)
	{
		CPHUpdateObject::Deactivate();
	}
	inherited::SwitchZoneState(new_state);
}

float CAmebaZone::distance_to_center(CObject* O)
{
	Fvector P; 
	XFORM().transform_tiny(P,CFORM()->getSphere().P);
	Fvector OP;OP.set(O->Position());
	return _sqrt((P.x-OP.x)*(P.x-OP.x)+(P.x-OP.x)*(P.x-OP.x));
}