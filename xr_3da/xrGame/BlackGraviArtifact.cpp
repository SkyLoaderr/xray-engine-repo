///////////////////////////////////////////////////////////////
// BlackGraviArtifact.cpp
// BlackGraviArtefact - гравитационный артефакт, 
// такой же как и обычный, но при получении хита
///////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BlackGraviArtifact.h"
#include "PhysicsShell.h"
#include "entity_alive.h"
#include "ParticlesObject.h"
#include "phmovementcontrol.h"
#include "xrmessages.h"
#include "physicsshellholder.h"
#include "explosive.h"
#include "net_utils.h"

CBlackGraviArtefact::CBlackGraviArtefact(void) 
{
	m_fImpulseThreshold = 10.f;
	m_fRadius = 10.f;
	m_fStrikeImpulse = 50.f;

	m_bStrike = false;
}

CBlackGraviArtefact::~CBlackGraviArtefact(void) 
{
	m_GameObjectList.clear();
}



void CBlackGraviArtefact::Load(LPCSTR section) 
{
	inherited::Load(section);

	m_fImpulseThreshold = pSettings->r_float(section,"impulse_threshold");
	m_fRadius = pSettings->r_float(section,"radius");
	m_fStrikeImpulse = pSettings->r_float(section,"strike_impulse");
	m_sParticleName = pSettings->r_string(section,"particle");
}

BOOL CBlackGraviArtefact::net_Spawn(LPVOID DC)
{
	if(!inherited::net_Spawn(DC)) return FALSE;



	CParticlesObject* pStaticPG;
	pStaticPG = xr_new<CParticlesObject>("anomaly\\galantine",Sector(),false);
	Fmatrix pos;
	//pos.rotateY(1.57);
	//pos.mulA(pos);
	pos.scale(0.7f,0.7f,0.7f);
	pos.translate_over(XFORM().c);
	
	Fvector vel;
	vel.set(0,0,0);
	pStaticPG->UpdateParent(pos, vel); 
	pStaticPG->Play();



	return TRUE;
}

void CBlackGraviArtefact::UpdateCL() 
{
	inherited::UpdateCL();

	if (getVisible() && m_pPhysicsShell) {
		if (m_bStrike) {
			Fvector	P; 
			P.set(Position());
			feel_touch_update(P,m_fRadius);

			GraviStrike();

			CParticlesObject* pStaticPG;
			pStaticPG = xr_new<CParticlesObject>(*m_sParticleName,Sector());
			Fmatrix pos; 
			pos.set(XFORM()); 
			Fvector vel; 
			//vel.sub(Position(),ps_Element(0).vPosition); 
			//vel.div((Level().timeServer()-ps_Element(0).dwTime)/1000.f);
			vel.set(0,0,0);
			pStaticPG->UpdateParent(pos, vel); 
			pStaticPG->Play();

			m_bStrike = false;
		}
	}
}

void CBlackGraviArtefact::Hit(float P, Fvector &dir,
						CObject* who, s16 element,
						Fvector position_in_object_space,
						float impulse,
						ALife::EHitType hit_type)
{
	if(impulse>m_fImpulseThreshold)
	{
		m_bStrike = true;
		//чтоб выстрел не повлиял на траекторию полета артефакта
		impulse = 0;
	}
	
	inherited::Hit(P, dir, who, element, position_in_object_space, impulse, hit_type);
}

void CBlackGraviArtefact::feel_touch_new(CObject* O) 
{
	CPhysicsShellHolder* pGameObject = smart_cast<CPhysicsShellHolder*>(O);
	CArtefact* pArtefact = smart_cast<CArtefact*>(O);

	if(pGameObject && !pArtefact) 
	{
		m_GameObjectList.push_back(pGameObject);
	}
}

void CBlackGraviArtefact::feel_touch_delete(CObject* O) 
{
	CGameObject* pGameObject = static_cast<CGameObject*>(O);
	CArtefact* pArtefact = smart_cast<CArtefact*>(O);

	if(pGameObject && !pArtefact)
	{
		m_GameObjectList.erase(std::find(m_GameObjectList.begin(), 
										 m_GameObjectList.end(), 
										 pGameObject));
	}
}

BOOL CBlackGraviArtefact::feel_touch_contact(CObject* O) 
{
	CGameObject* pGameObject = static_cast<CGameObject*>(O);

	if(pGameObject)
		return TRUE;
	else
		return FALSE;
}

void CBlackGraviArtefact::GraviStrike()
{
	xr_list<s16>		elements_list;
	xr_list<Fvector>	bone_position_list;

	Fvector object_pos; 
	Fvector strike_dir;

	for(GAME_OBJECT_LIST_it it = m_GameObjectList.begin(); 
						    m_GameObjectList.end() != it;
							++it)
	{
		CPhysicsShellHolder* pGameObject = *it;

		if(pGameObject->Visual()) 
			pGameObject->Center(object_pos); 
		else 
			object_pos.set(pGameObject->Position());

		strike_dir.sub(object_pos, Position()); 
		float distance = strike_dir.magnitude(); 

		float impulse = 100.f*m_fStrikeImpulse * (1.f - (distance/m_fRadius)*
										   (distance/m_fRadius));
						
		if(impulse > .001f) 
		{
			setEnabled(false);
			impulse *= CExplosive::ExplosionEffect(pGameObject, Position(), m_fRadius, 
											  elements_list, 
											  bone_position_list);
			setEnabled(true);
		}

		float hit_power;
		CEntityAlive* pEntityAlive = smart_cast<CEntityAlive*>(pGameObject);
		if(pGameObject->m_pPhysicsShell) 
			hit_power = 0;
		else if(pEntityAlive && pEntityAlive->g_Alive() && 
				pEntityAlive->m_PhysicMovementControl->CharacterExist())
			hit_power = 0;
		else
			hit_power = impulse;

		
		if(impulse > .001f) 
		{
			while(!elements_list.empty()) 
			{
				s16 element = elements_list.front();
				Fvector bone_pos = bone_position_list.front();
				
				NET_Packet		P;
				u_EventGen		(P,GE_HIT, pGameObject->ID());
				P.w_u16			(ID());
				P.w_u16			(ID());
				P.w_dir			(strike_dir);
				P.w_float		(hit_power);
				P.w_s16			(element);
				P.w_vec3		(bone_pos);
				P.w_float		(impulse);
				P.w_u16			(u16(ALife::eHitTypeWound));
				u_EventSend		(P);
				elements_list.pop_front();
				bone_position_list.pop_front();
			}
		}
	}
}