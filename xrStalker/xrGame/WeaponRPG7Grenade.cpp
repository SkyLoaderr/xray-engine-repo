// WeaponRPG7Grenade.h: объект для эмулирования гранаты РПГ7
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "WeaponRPG7Grenade.h"
#include "WeaponRPG7.h"

#include "entity.h"

#include "PhysicsShell.h"
#include "ExtendedGeom.h"


#include "ParticlesObject.h"



void __stdcall CWeaponRPG7Grenade::ObjectContactCallback(bool& do_colide,dContact& c) 
{
	do_colide = false;
	dxGeomUserData *l_pUD1 = NULL;
	dxGeomUserData *l_pUD2 = NULL;
	l_pUD1 = retrieveGeomUserData(c.geom.g1);
	l_pUD2 = retrieveGeomUserData(c.geom.g2);
	CWeaponRPG7Grenade *l_this = l_pUD1 ? dynamic_cast<CWeaponRPG7Grenade*>(l_pUD1->ph_ref_object) : NULL;
	if(!l_this) l_this = l_pUD2 ? dynamic_cast<CWeaponRPG7Grenade*>(l_pUD2->ph_ref_object) : NULL;
	
	if(!l_this) return;
	CGameObject *l_pOwner = l_pUD1 ? dynamic_cast<CGameObject*>(l_pUD1->ph_ref_object) : NULL;
	if(!l_pOwner || l_pOwner == (CGameObject*)l_this) l_pOwner = l_pUD2 ? dynamic_cast<CGameObject*>(l_pUD2->ph_ref_object) : NULL;
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
					velocity.normalize();
					float cosinus=velocity.dotproduct(*((Fvector*)l_pUD->neg_tri.norm));
					float dist=l_pUD->neg_tri.dist/cosinus;
					velocity.mul(dist);
					l_pos.sub(velocity);
				}
			}
			l_this->Explode(l_pos, *(Fvector*)&c.geom.normal);
		}
	} 
	else 
	{
	}
}

CWeaponRPG7Grenade::CWeaponRPG7Grenade() 
{
	m_state = stInactive;
	m_pos.set(0, 0, 0); m_vel.set(0, 0, 0);
	m_pOwner = NULL;
	m_eSoundExplode = ESoundTypes(SOUND_TYPE_WEAPON_SHOOTING);
	
	m_pLight = ::Render->light_create();
	m_pLight->set_shadow(true);
	m_explodeTime = m_dwEngineTime = m_flashTime = 0;

	m_pTrailLight = ::Render->light_create();
	m_pTrailLight->set_shadow(true);
}

CWeaponRPG7Grenade::~CWeaponRPG7Grenade() 
{
	::Render->light_destroy(m_pTrailLight);
}


void CWeaponRPG7Grenade::Load(LPCSTR section) 
{
	inherited::Load(section);

	m_mass = pSettings->r_float(section,"ph_mass");
	m_engine_f = pSettings->r_float(section,"engine_f");
	m_engine_u = pSettings->r_float(section,"engine_u");


	m_effects.clear		(); 
	{
		LPCSTR				S = pSettings->r_string(section,"effects");
		if (S && S[0]) 
		{
			string128		_eff;
			int				count		= _GetItemCount	(S);
			for (int it=0; it<count; ++it)	
				m_effects.push_back	(_GetItem(S,it,_eff));
		}
	}

	m_trailEffects.clear(); 
	{
		LPCSTR				S = pSettings->r_string(section,"trail");
		if (S && S[0]) 
		{
			string128		_eff;
			int				count		= _GetItemCount	(S);
			for (int it=0; it<count; ++it)	
				m_trailEffects.push_back(_GetItem(S,it,_eff));
		}
	}


	sscanf(pSettings->r_string(section,"trail_light_color"), "%f,%f,%f", 
					&m_TrailLightColor.r, &m_TrailLightColor.g, &m_TrailLightColor.b);
	m_fTrailLightRange	= pSettings->r_float(section,"trail_light_range");
		

	m_state = stInactive;
}

static const u32 EXPLODE_TIME	= 5000;
static const u32 FLASH_TIME		= 300;
static const u32 ENGINE_TIME	= 3000;

BOOL CWeaponRPG7Grenade::net_Spawn(LPVOID DC) 
{
	BOOL l_res = inherited::net_Spawn(DC);
	return l_res;
}

void CWeaponRPG7Grenade::Explode(const Fvector &pos, const Fvector &normal) 
{
	inherited::Explode(pos, normal);

	StopEngine();

	//уничтожить партиклы хвоста дыма
	while(m_trailEffectsPSs.size()) 
	{ 
		CParticlesObject::Destroy(*(m_trailEffectsPSs.begin())); 
		m_trailEffectsPSs.pop_front(); 
	}
	//выключить свет
	m_pTrailLight->set_active(false);
}



void CWeaponRPG7Grenade::net_Destroy() 
{
	while(m_trailEffectsPSs.size()) 
	{ 
		xr_delete(*(m_trailEffectsPSs.begin())); 
		m_trailEffectsPSs.pop_front(); 
	}

	CExplosive::net_Destroy();
	inherited::net_Destroy();
}

void CWeaponRPG7Grenade::OnH_B_Independent() 
{
	CExplosive::OnH_B_Independent();
}

void CWeaponRPG7Grenade::UpdateCL() 
{
	//вызывается UpdateCL GameObject, чтоб не выполнять логику 
	//CWeaponFakeGrenade и CExplosive

	switch (m_state)
	{
	case stInactive:
		CGameObject::UpdateCL();
		break;
	case stExplode: 
		CGameObject::UpdateCL();
		CExplosive::UpdateCL();
		break;
	case stEngine:
		if(getVisible() && m_pPhysicsShell) {
			CWeaponFakeGrenade::UpdateCL();
			if(m_dwEngineTime <= 0) 
			{
				m_state	= stFlying;
				StopEngine();
			}
			else
			{
				// двигатель все еще работает
				m_dwEngineTime -= Device.dwTimeDelta;
				Fvector l_pos, l_dir;; l_pos.set(0, 0, 3.f); l_dir.set(XFORM().k); l_dir.normalize();
				float l_force = m_engine_f * Device.dwTimeDelta / 1000.f;
				m_pPhysicsShell->applyImpulseTrace(l_pos, l_dir, l_force);
				l_dir.set(0, 1.f, 0);
				l_force = m_engine_u * Device.dwTimeDelta / 1000.f;
				m_pPhysicsShell->applyImpulse(l_dir, l_force);
				
				xr_list<CParticlesObject*>::iterator l_it;
				
				Fvector vel;
				PHGetLinearVell(vel);
				// обновить эффекты
				for(l_it = m_trailEffectsPSs.begin(); m_trailEffectsPSs.end() != l_it; ++l_it) 
							(*l_it)->UpdateParent(XFORM(),vel);

				m_pTrailLight->set_position(Position());
			}
		}
		else
			CGameObject::UpdateCL();
		break;
	case stFlying:
		CWeaponFakeGrenade::UpdateCL();
		break;
	}
}


void CWeaponRPG7Grenade::StopEngine()
{
	// остановить двигатель
	xr_list<CParticlesObject*>::iterator it;
	for(it = m_trailEffectsPSs.begin(); 
			  m_trailEffectsPSs.end() != it; ++it) 
					(*it)->Stop();
	m_pTrailLight->set_active(false);
}

void CWeaponRPG7Grenade::activate_physic_shell()
{
	Fmatrix trans;
	Level().Cameras.unaffected_Matrix(trans);

	CObject		*E = dynamic_cast<CObject*>(H_Parent());
	R_ASSERT	(E);
	CWeaponRPG7 *l_pW = dynamic_cast<CWeaponRPG7*>(E);
	Fmatrix l_p1, l_r; 
	l_r.rotateY(M_PI*2.f); 
	
	l_p1.mul(l_pW->GetHUDmode()?trans:XFORM(), l_r); 
	l_p1.c.set(*l_pW->m_pGrenadePoint);
	Fvector a_vel; a_vel.set(0, 0, 0);
	
	m_pPhysicsShell->Activate(l_p1, m_vel, a_vel);
	m_pPhysicsShell->Update	();
	XFORM().set(m_pPhysicsShell->mXFORM);
	Position().set(m_pPhysicsShell->mXFORM.c);
	
	m_pPhysicsShell->set_PhysicsRefObject(this);
	m_pPhysicsShell->set_ObjectContactCallback(ObjectContactCallback);
	m_pPhysicsShell->set_ContactCallback(NULL);

	m_state			= stEngine;
	m_dwEngineTime	= ENGINE_TIME;

	//запустить хвост из партиклов
	CParticlesObject* pStaticPG; s32 l_c = (s32)m_trailEffects.size();
	Fmatrix l_m; l_m.set(XFORM());
	Fvector zero_vel = {0.f,0.f,0.f};
	for(s32 i = 0; i < l_c; ++i) 
	{
		pStaticPG = xr_new<CParticlesObject>(*m_trailEffects[i],Sector(),false);
		pStaticPG->UpdateParent(l_m,zero_vel);
		pStaticPG->Play();
		m_trailEffectsPSs.push_back(pStaticPG);
	}

	//включить световую подсветку от двигателя
	m_pTrailLight->set_color(m_TrailLightColor.r, 
								m_TrailLightColor.g, 
								m_TrailLightColor.b);
	m_pTrailLight->set_range(m_fTrailLightRange);
	m_pTrailLight->set_position(Position()); 
	m_pTrailLight->set_active(true);
}