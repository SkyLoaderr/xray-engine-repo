//////////////////////////////////////////////////////////////////////
// CustomRocket.cpp:	ракета, которой стреляет RocketLauncher 
//						(умеет лететь, светиться и отыгрывать партиклы)
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "customrocket.h"
#include "ParticlesObject.h"
#include "PhysicsShell.h"
#include "extendedgeom.h"
#include "level.h"
#include "xrMessages.h"
#include "gamemtllib.h"
#include "tri-colliderknoopc/dTriList.h"

CCustomRocket::CCustomRocket() 
{
	m_eState = eInactive;
	m_bEnginePresent = false;
	m_bStopLightsWithEngine = true;
	m_bLightsEnabled = false;


	m_vPrevVel.set(0,0,0);

	m_pTrailLight = NULL;
}

CCustomRocket::~CCustomRocket	()
{
	if (m_pTrailLight) ::Render->light_destroy(m_pTrailLight);
}


void CCustomRocket::reinit		()
{
	inherited::reinit();

	if (m_pTrailLight) ::Render->light_destroy(m_pTrailLight);
	m_pTrailLight = ::Render->light_create();
	m_pTrailLight->set_shadow(true);

	m_pEngineParticles = NULL;
	m_pFlyParticles = NULL;

	m_pOwner = NULL;

	m_vPrevVel.set(0,0,0);
}


BOOL CCustomRocket::net_Spawn(LPVOID DC) 
{
	BOOL result = inherited::net_Spawn(DC);
	return result;
}

void CCustomRocket::net_Destroy() 
{
	inherited::net_Destroy();
	CPHUpdateObject::Deactivate();

	StopEngine();
	StopFlying();
}


void CCustomRocket::SetLaunchParams (const Fmatrix& xform, 
									 const Fvector& vel,
									 const Fvector& angular_vel)
{
	m_LaunchXForm = xform;
	m_vLaunchVelocity = vel;
	m_vLaunchAngularVelocity = angular_vel;
}


void CCustomRocket::activate_physic_shell	()
{
	VERIFY(H_Parent());

	if( m_pPhysicsShell->bActive )
		return;

	m_pPhysicsShell->Activate(m_LaunchXForm, m_vLaunchVelocity, m_vLaunchAngularVelocity);
	m_pPhysicsShell->Update	();

	XFORM().set(m_pPhysicsShell->mXFORM);
	Position().set(m_pPhysicsShell->mXFORM.c);
	m_pPhysicsShell->set_PhysicsRefObject(this);
	m_pPhysicsShell->set_ObjectContactCallback(ObjectContactCallback);
	m_pPhysicsShell->set_ContactCallback(NULL);
	m_pPhysicsShell->SetAirResistance(0.f,0.f);
}

void CCustomRocket::create_physic_shell	()
{
	create_box2sphere_physic_shell();
}

//////////////////////////////////////////////////////////////////////////
// Rocket specific functions
//////////////////////////////////////////////////////////////////////////


void __stdcall CCustomRocket::ObjectContactCallback(bool& do_colide,dContact& c ,SGameMtl * material_1,SGameMtl * material_2) 
{
	do_colide = false;


	dxGeomUserData *l_pUD1 = NULL;
	dxGeomUserData *l_pUD2 = NULL;
	l_pUD1 = retrieveGeomUserData(c.geom.g1);
	l_pUD2 = retrieveGeomUserData(c.geom.g2);

	SGameMtl* material=0;
	CCustomRocket *l_this = l_pUD1 ? smart_cast<CCustomRocket*>(l_pUD1->ph_ref_object) : NULL;
	Fvector vUp;
	if(!l_this){
		l_this = l_pUD2 ? smart_cast<CCustomRocket*>(l_pUD2->ph_ref_object) : NULL;
		vUp.invert(*(Fvector*)&c.geom.normal);

		//if(dGeomGetClass(c.geom.g1)==dTriListClass)
		//	material=GMLib.GetMaterialByIdx((u16)c.surface.mode);
		//else
		//	material=GMLib.GetMaterialByIdx(l_pUD2->material);
		material=material_2;

	}else{
		vUp.set(*(Fvector*)&c.geom.normal);	

		//if(dGeomGetClass(c.geom.g2)==dTriListClass)
		//	material=GMLib.GetMaterialByIdx((u16)c.surface.mode);
		//else
		//	material=GMLib.GetMaterialByIdx(l_pUD1->material);
		material=material_1;

	}
	VERIFY(material);
	if(material->Flags.is(SGameMtl::flPassable)) return;

	if(!l_this) return;

	CGameObject *l_pOwner = l_pUD1 ? smart_cast<CGameObject*>(l_pUD1->ph_ref_object) : NULL;
	if(!l_pOwner || l_pOwner == (CGameObject*)l_this) l_pOwner = l_pUD2 ? smart_cast<CGameObject*>(l_pUD2->ph_ref_object) : NULL;
	if(!l_pOwner || l_pOwner != l_this->m_pOwner) 
	{
		if(l_this->m_pOwner) 
		{
			Fvector l_pos; l_pos.set(l_this->Position());
			if(!l_pUD1||!l_pUD2) 
			{
				dxGeomUserData *&l_pUD = l_pUD1?l_pUD1:l_pUD2;
				if(l_pUD->pushing_neg) 
				{
					Fvector velocity;
					l_this->PHGetLinearVell(velocity);
					if (velocity.square_magnitude() > EPS)	
					{	//. desync?
						velocity.normalize();
						float cosinus=velocity.dotproduct(*((Fvector*)l_pUD->neg_tri.norm));
						float dist=l_pUD->neg_tri.dist/cosinus;
						velocity.mul(dist);
						l_pos.sub(velocity);
					}
				}
			}
			l_this->Contact(l_pos, vUp);
		}
	} else {}
}

void CCustomRocket::Load(LPCSTR section) 
{
	inherited::Load	(section);

	reload(section);
}

void  CCustomRocket::reload		(LPCSTR section)
{
	m_eState = eInactive;

	m_bEnginePresent = !!pSettings->r_bool(section, "engine_present");
	if(m_bEnginePresent)
	{
		m_dwEngineWorkTime = pSettings->r_u32(section, "engine_work_time");
		m_fEngineImpulse		 = pSettings->r_float(section, "engine_impulse");
		m_fEngineImpulseUp		 = pSettings->r_float(section, "engine_impulse_up");
	}


	m_bLightsEnabled = !!pSettings->r_bool(section, "lights_enabled");
	if(m_bLightsEnabled)
	{
		sscanf(pSettings->r_string(section,"trail_light_color"), "%f,%f,%f", 
			&m_TrailLightColor.r, &m_TrailLightColor.g, &m_TrailLightColor.b);
		m_fTrailLightRange	= pSettings->r_float(section,"trail_light_range");
	}

	if(pSettings->line_exist(section,"engine_particles")) 
		m_sEngineParticles	= pSettings->r_string(section,"engine_particles");
	if(pSettings->line_exist(section,"fly_particles")) 
		m_sFlyParticles	= pSettings->r_string(section,"fly_particles");
}


void CCustomRocket::Contact(const Fvector &pos, const Fvector &normal)
{
	if(eCollide == m_eState) return;

	StopEngine();
	StopFlying();


	m_eState = eCollide;

	//дективировать физическую оболочку,чтоб ракета не летела дальше
	if(m_pPhysicsShell)
	{
		m_pPhysicsShell->set_LinearVel(zero_vel);
		m_pPhysicsShell->set_AngularVel(zero_vel);
		m_pPhysicsShell->set_ObjectContactCallback(NULL);
	}
//	if (OnClient()) return;

	Position().set(pos);
}


void CCustomRocket::OnH_B_Independent() 
{
	inherited::OnH_B_Independent();
	//-------------------------------------------
	m_pOwner = H_Parent() ? smart_cast<CGameObject*>(H_Parent()->H_Root()) : NULL;
	//-------------------------------------------
}


void CCustomRocket::OnH_A_Independent() 
{
	inherited::OnH_A_Independent();

	setVisible(true);
	StartFlying();
	StartEngine();
}


void CCustomRocket::UpdateCL()
{
	inherited::UpdateCL();
	
	switch (m_eState)
	{
	case eInactive:
		break;
	//состояния eEngine и eFlying отличаются, тем
	//что вызывается UpdateEngine у eEngine, остальные
	//функции общие
	case eEngine:
		UpdateEngine();
	case eFlying:
		UpdateLights();
		UpdateParticles();
		break;
	}
}

void CCustomRocket::StartEngine				()
{
	VERIFY(NULL == H_Parent());

	if(!m_bEnginePresent)
	{
		m_eState = eFlying;
		return;
	}

	m_eState = eEngine;
	m_dwEngineTime = m_dwEngineWorkTime;

	StartEngineParticles();

	CPHUpdateObject::Activate();
}

void CCustomRocket::StopEngine				()
{
	m_eState = eFlying;

	m_dwEngineTime = 0;

	if(m_bStopLightsWithEngine)
		StopLights();

	StopEngineParticles();

	CPHUpdateObject::Deactivate();
}


void CCustomRocket::UpdateEnginePh			()
{
	if (Level().In_NetCorrectionPrediction()) return;
	float force = m_fEngineImpulse*fixed_step;// * Device.fTimeDelta;
	float k_back=1.f;
	Fvector l_pos, l_dir; 
	l_pos.set(0, 0,-2.f);
	l_dir.set(XFORM().k);
	
	l_dir.normalize();
	m_pPhysicsShell->applyImpulse(l_dir,(1.f+k_back)*force);
	m_pPhysicsShell->get_LinearVel(l_dir);
	l_dir.normalize_safe();
	l_dir.invert();
	m_pPhysicsShell->applyImpulseTrace(l_pos, l_dir, force);
	l_dir.set(0, 1.f, 0);
	force = m_fEngineImpulseUp*fixed_step;// * Device.fTimeDelta;
	m_pPhysicsShell->applyImpulse(l_dir, force);

	//m_pPhysicsShell->set_AngularVel()
}


void CCustomRocket::UpdateEngine				()
{
	VERIFY(getVisible() && m_pPhysicsShell);

	if (m_dwEngineTime <= 0) 
	{
		StopEngine();
		return;
	}

	m_dwEngineTime -= Device.dwTimeDelta;
}



//////////////////////////////////////////////////////////////////////////
//	Lights
//////////////////////////////////////////////////////////////////////////
void CCustomRocket::StartLights()
{
	if(!m_bLightsEnabled) return;

	//включить световую подсветку от двигателя
	m_pTrailLight->set_color(m_TrailLightColor.r, 
							 m_TrailLightColor.g, 
							 m_TrailLightColor.b);

	m_pTrailLight->set_range(m_fTrailLightRange);
	m_pTrailLight->set_position(Position()); 
	m_pTrailLight->set_active(true);
}

void CCustomRocket::StopLights()
{
	if(!m_bLightsEnabled) return;
	m_pTrailLight->set_active(false);
}

void CCustomRocket::UpdateLights()
{
	if(!m_bLightsEnabled || !m_pTrailLight->get_active()) return;
	m_pTrailLight->set_position(Position());
}


void CCustomRocket::PhDataUpdate			(float step)
{
	
}
void CCustomRocket::PhTune					(float step)
{
	UpdateEnginePh							();
}



//////////////////////////////////////////////////////////////////////////
//	Particles
//////////////////////////////////////////////////////////////////////////

void CCustomRocket::UpdateParticles()
{
	if(!m_pEngineParticles && !m_pFlyParticles) return;

	Fvector vel;
	PHGetLinearVell(vel);

	vel.add(m_vPrevVel,vel);
	vel.mul(0.5f);
	m_vPrevVel.set(vel);

	Fmatrix particles_xform;
	particles_xform.identity();
	particles_xform.k.set(XFORM().k);
	particles_xform.k.mul(-1.f);
	Fvector::generate_orthonormal_basis(particles_xform.k, 
										particles_xform.j, 
										particles_xform.i);
    particles_xform.c.set(XFORM().c);


	if(m_pEngineParticles)
		m_pEngineParticles->UpdateParent(particles_xform, vel);
	
	if(m_pFlyParticles)
		m_pFlyParticles->UpdateParent(particles_xform, vel);
}

void CCustomRocket::StartEngineParticles()
{
	VERIFY(m_pEngineParticles == NULL);
	if(!m_sEngineParticles) return;
	m_pEngineParticles = xr_new<CParticlesObject>(*m_sEngineParticles, Sector(),false);

	UpdateParticles();
	m_pEngineParticles->Play();

	VERIFY(m_pEngineParticles);
	VERIFY3(m_pEngineParticles->IsLooped(), "must be a looped particle system for rocket engine: %s", *m_sEngineParticles);
}
void CCustomRocket::StopEngineParticles()
{
	if(m_pEngineParticles == NULL) return;
	m_pEngineParticles->Stop();
	m_pEngineParticles->SetAutoRemove(true);
	m_pEngineParticles = NULL;
}
void CCustomRocket::StartFlyParticles()
{
	VERIFY(m_pFlyParticles == NULL);

	if(!m_sFlyParticles) return;
	m_pFlyParticles = xr_new<CParticlesObject>(*m_sFlyParticles, Sector(),false);
	
	UpdateParticles();
	m_pFlyParticles->Play();
	
	VERIFY(m_pFlyParticles);
	VERIFY3(m_pFlyParticles->IsLooped(), "must be a looped particle system for rocket fly: %s", *m_sFlyParticles);
}
void CCustomRocket::StopFlyParticles()
{
	if(m_pFlyParticles == NULL) return;
	m_pFlyParticles->Stop();
	m_pFlyParticles->SetAutoRemove(true);
	m_pFlyParticles = NULL;
}

void CCustomRocket::StartFlying				()
{
	StartFlyParticles();
	StartLights();
}
void CCustomRocket::StopFlying				()
{
	StopFlyParticles();
	StopLights();
}

void	CCustomRocket::OnEvent(NET_Packet& P, u16 type)
{
	switch (type)
	{
	case GE_GRENADE_EXPLODE:
		{
			if (m_eState != eCollide && OnClient())
			{
				CCustomRocket::Contact(Position(), Direction());
			};
		}break;
	}
	inherited::OnEvent(P,type);
};