// WeaponFakeGrenade.cpp: объект для эмулирования подствольной гранаты
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "WeaponFakeGrenade.h"

#include "WeaponMagazinedWGrenade.h"

#include "Entity.h"
#include "PSObject.h"
#include "ParticlesObject.h"
#include "PhysicsShell.h"

#include "extendedgeom.h"



#define CHOOSE_MAX(x,inst_x,y,inst_y,z,inst_z)\
	if(x>y)\
		if(x>z){inst_x;}\
		else{inst_z;}\
	else\
		if(y>z){inst_y;}\
		else{inst_z;}

CWeaponFakeGrenade::CWeaponFakeGrenade() 
{
	m_state = stInactive;
	m_pos.set(0, 0, 0); m_vel.set(0, 0, 0);
	m_pOwner = NULL;

	m_explodeTime = m_engineTime = m_flashTime = 0;
	m_fJumpHeight = 0;
}

CWeaponFakeGrenade::~CWeaponFakeGrenade	()
{
	xr_delete(m_pPhysicsShell);
}

void __stdcall CWeaponFakeGrenade::ObjectContactCallback(bool& do_colide,dContact& c) 
{
	do_colide = false;
	dxGeomUserData *l_pUD1 = NULL;
	dxGeomUserData *l_pUD2 = NULL;
	l_pUD1 = retrieveGeomUserData(c.geom.g1);
	l_pUD2 = retrieveGeomUserData(c.geom.g2);

	CWeaponFakeGrenade *l_this = l_pUD1 ? dynamic_cast<CWeaponFakeGrenade*>(l_pUD1->ph_ref_object) : NULL;
	if(!l_this) l_this = l_pUD2 ? dynamic_cast<CWeaponFakeGrenade*>(l_pUD2->ph_ref_object) : NULL;
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
			l_pos.mad(*(Fvector*)&c.geom.normal, l_this->m_fJumpHeight);
			l_this->Explode(l_pos, *(Fvector*)&c.geom.normal);
		}
	} else {}
}

void CWeaponFakeGrenade::Load(LPCSTR section) 
{
	inherited::Load(section);

	CExplosive::Load(section);

	m_mass = pSettings->r_float(section,"ph_mass");
	m_fJumpHeight = pSettings->r_float(section, "jump_height");

	m_state = stInactive;
}

static const u32 EXPLODE_TIME	= 5000;
static const u32 FLASH_TIME		= 300;
static const u32 ENGINE_TIME	= 3000;

void CWeaponFakeGrenade::Explode(const Fvector &pos, const Fvector &/**normal/**/) 
{
	m_state			= stExplode;
	m_explodeTime	= EXPLODE_TIME;
	m_flashTime		= FLASH_TIME;

	Position().set(pos);
	
	CExplosive::Explode();
	
	m_pOwner = NULL;
}

BOOL CWeaponFakeGrenade::net_Spawn(LPVOID DC) 
{
	BOOL result = inherited::net_Spawn(DC);
	
	m_state = stInactive;
	m_pos.set(0, 0, 0); m_vel.set(0, 0, 0);
	m_pOwner = NULL;
	m_explodeTime = m_engineTime = m_flashTime = 0;

//	CSkeletonAnimated* V = PSkeletonAnimated(Visual());
//	if(V) V->PlayCycle("idle");

	CSkeletonRigid* V = PSkeletonRigid(Visual());
	R_ASSERT(V);

	
	//setVisible					(true);
	//setEnabled					(true);

	if (0==m_pPhysicsShell)
	{
		// Physics (Box)
		Fobb								obb;
		Visual()->vis.box.get_CD			(obb.m_translate,obb.m_halfsize);
		obb.m_translate.set(0, 0, 0); obb.m_halfsize.set(.07f, .07f, .35f);
		obb.m_rotate.identity				();

		// Physics (Elements)
		CPhysicsElement* E					= P_create_Element	();
		R_ASSERT							(E);
		
		Fvector ax;
		float	radius;
		CHOOSE_MAX(	obb.m_halfsize.x,ax.set(obb.m_rotate.i) ; ax.mul(obb.m_halfsize.x); radius=_min(obb.m_halfsize.y,obb.m_halfsize.z) ;obb.m_halfsize.y/=2.f;obb.m_halfsize.z/=2.f,
					obb.m_halfsize.y,ax.set(obb.m_rotate.j) ; ax.mul(obb.m_halfsize.y); radius=_min(obb.m_halfsize.x,obb.m_halfsize.z) ;obb.m_halfsize.x/=2.f;obb.m_halfsize.z/=2.f,
					obb.m_halfsize.z,ax.set(obb.m_rotate.k) ; ax.mul(obb.m_halfsize.z); radius=_min(obb.m_halfsize.y,obb.m_halfsize.x) ;obb.m_halfsize.y/=2.f;obb.m_halfsize.x/=2.f
					)
		//radius*=1.4142f;
		Fsphere sphere1,sphere2;
		sphere1.P.add						(obb.m_translate,ax);
		sphere1.R							=radius*1.4142f;

		sphere2.P.sub						(obb.m_translate,ax);
		sphere2.R							=radius/2.f;

		E->add_Box							(obb);
		E->add_Sphere						(sphere1);
		E->add_Sphere						(sphere2);
	
		// Physics (Shell)
		m_pPhysicsShell						= P_create_Shell	();
		R_ASSERT							(m_pPhysicsShell);
		m_pPhysicsShell->add_Element		(E);
		//m_pPhysicsShell->setDensity			(8000.f);
		m_pPhysicsShell->setMass			(m_mass);
		//m_pPhysicsShell->Activate			(XFORM(),0,XFORM());
		//m_pPhysicsShell->Activate			();
		m_pPhysicsShell->mDesired.identity	();
		m_pPhysicsShell->fDesiredStrength	= 0.f;
		m_pPhysicsShell->SetAirResistance(.000f, 0.f);
		//m_pPhysicsShell->Deactivate();
	}
	return result;
}


void CWeaponFakeGrenade::net_Destroy() 
{
	if(m_pPhysicsShell) m_pPhysicsShell->Deactivate();
	xr_delete(m_pPhysicsShell);
	
	 inherited::net_Destroy();
}


void CWeaponFakeGrenade::Destroy() 
{
	//Generate Expode event
	if (Local()) 
	{
		NET_Packet		P;
		u_EventGen		(P,GE_GRENADE_EXPLODE,ID());	
		u_EventSend		(P);
	};
}

void CWeaponFakeGrenade::OnH_B_Independent() 
{
	inherited::OnH_B_Independent();
	
	setVisible		(true);
	setEnabled		(true);
	
	CObject*	 E = dynamic_cast<CObject*>(H_Parent());
	R_ASSERT	(E);
	XFORM().set(E->XFORM());
	
	if(m_pPhysicsShell) 
	{
		Fmatrix trans;
		Level().Cameras.unaffected_Matrix(trans);
		CWeaponMagazinedWGrenade *l_pW = dynamic_cast<CWeaponMagazinedWGrenade*>(E);
		Fmatrix l_p1, l_r; 
		l_r.rotateY(M_PI*2.f); 
		l_p1.mul(l_pW->GetHUDmode()?trans:XFORM(), l_r); 
		l_p1.c.set(*l_pW->m_pGrenadePoint);
		
		Fvector a_vel; 
		a_vel.set(0, 0, 0);
		m_pPhysicsShell->Activate(l_p1, m_vel, a_vel);
		XFORM().set(m_pPhysicsShell->mXFORM);
		Position().set(m_pPhysicsShell->mXFORM.c);
		m_pPhysicsShell->set_PhysicsRefObject(this);
		m_pPhysicsShell->set_ObjectContactCallback(ObjectContactCallback);
		m_pPhysicsShell->set_ContactCallback(NULL);

		m_state			= stEngine;
		m_engineTime	= ENGINE_TIME;
	}
}

void CWeaponFakeGrenade::UpdateCL() 
{
	inherited::UpdateCL();
	
	switch (m_state)
	{
	case stInactive:
		break;
	case stDestroying: 
		return;
	case stExplode: 
		if(m_explodeTime <= 0) 
		{
			//while(m_trailEffectsPSs.size()) { xr_delete(*(m_trailEffectsPSs.begin())); m_trailEffectsPSs.pop_front(); }
			m_state			= stDestroying;
			NET_Packet		P;
			u_EventGen		(P, GE_DESTROY, ID());
			u_EventSend		(P);
			return;
		}
		else
			m_explodeTime	-= Device.dwTimeDelta;
		
		if(m_flashTime <= 0)
		{
			m_pLight->set_active(false);
		}
		else
		{
			if(m_pLight->get_active())
			{
				//float scale	= float(m_flashTime)/float(FLASH_TIME);

				//Fcolor m_curColor;
				//m_curColor.mul_rgb	(m_lightColor,scale);
				//m_pLight->set_color	(m_curColor);
			}
			m_flashTime		-= Device.dwTimeDelta;
		}
		break;
	case stEngine:
		if(getVisible() && m_pPhysicsShell) 
		{
			m_pPhysicsShell->Update	();
			XFORM().set	(m_pPhysicsShell->mXFORM);
			Position().set	(m_pPhysicsShell->mXFORM.c);
			if(m_engineTime <= 0) 
			{
				m_state		= stFlying;
				// остановить двигатель
				xr_list<CParticlesObject*>::iterator l_it;
				//for(l_it = m_trailEffectsPSs.begin(); m_trailEffectsPSs.end() != l_it; ++l_it) (*l_it)->Stop();
			}
			else
			{
				// двигатель все еще работает
				m_engineTime -= Device.dwTimeDelta;
				Fvector l_pos, l_dir;
				l_pos.set(0, 0, 3.f); 
				l_dir.set(XFORM().k); 
				l_dir.normalize();

				float l_force = m_engine_f * Device.dwTimeDelta / 1000.f;
				//m_pPhysicsShell->applyImpulseTrace(l_pos, l_dir, l_force);
				l_dir.set(0, 1.f, 0);
				l_force = m_engine_u * Device.dwTimeDelta / 1000.f;
				//m_pPhysicsShell->applyImpulse(l_dir, l_force);
				xr_list<CParticlesObject*>::iterator l_it;
				Fvector vel;
				PHGetLinearVell(vel);
				//m_pPhysicsShell->get_LinearVel(vel);
				// обновить эффекты
				//for(l_it = m_trailEffectsPSs.begin(); m_trailEffectsPSs.end() != l_it; ++l_it) (*l_it)->UpdateParent(XFORM(),vel);
			}
		}
		break;
	case stFlying:
		if(getVisible() && m_pPhysicsShell) 
		{
			m_pPhysicsShell->Update	();
			XFORM().set	(m_pPhysicsShell->mXFORM);
			Position().set	(m_pPhysicsShell->mXFORM.c);
		}
		break;
	}
}