///////////////////////////////////////////////////////////////
// BastArtifact.cpp
// BastArtifact - артефакт мочалка
///////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BastArtifact.h"
#include "PhysicsShell.h"


CBastArtifact::CBastArtifact(void) 
{
	m_fImpulseThreshold = 10.f;
	m_fEnergy = 1.f;
	m_fRadius = 10.f;
	m_fStrikeImpulse = 510.f;

	m_bStrike = false;

	m_pAttackingEntity = NULL;
}

CBastArtifact::~CBastArtifact(void) 
{
}
/*

void __stdcall CBastArtifact::ObjectContactCallback(bool& do_colide,dContact& c) 
{
	do_colide = false;
	dxGeomUserData *l_pUD1 = NULL;
	dxGeomUserData *l_pUD2 = NULL;
	if(dGeomGetClass(c.geom.g1)==dGeomTransformClass) 
	{
		const dGeomID geom=dGeomTransformGetGeom(c.geom.g1);
		l_pUD1 = dGeomGetUserData(geom);
	}
	else 
		l_pUD1 = dGeomGetUserData(c.geom.g1);

	if(dGeomGetClass(c.geom.g2)==dGeomTransformClass) 
	{
		const dGeomID geom=dGeomTransformGetGeom(c.geom.g2);
		l_pUD2 = dGeomGetUserData(geom);
	} 
	else 
		l_pUD2 = dGeomGetUserData(c.geom.g2);

	CBastArtifact *l_this = l_pUD1 ? dynamic_cast<CBastArtifact*>(l_pUD1->ph_ref_object) : NULL;
	if(!l_this) l_this = l_pUD2 ? dynamic_cast<CBastArtifact*>(l_pUD2->ph_ref_object) : NULL;
	
	if(!l_this) return;
	CGameObject *l_pOwner = l_pUD1 ? dynamic_cast<CGameObject*>(l_pUD1->ph_ref_object) : NULL;
	if(!l_pOwner || l_pOwner == (CGameObject*)l_this) l_pOwner = l_pUD2 ? dynamic_cast<CGameObject*>(l_pUD2->ph_ref_object) : NULL;
	if(!l_pOwner || l_pOwner != l_this->m_pOwner) 
	{
		if(l_this->m_pOwner) 
		{
			Fvector l_pos; 
			l_pos.set(l_this->Position());
			
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
*/

void CBastArtifact::Load(LPCSTR section) 
{
	// verify class
	LPCSTR Class = pSettings->r_string(section,"class");
	CLASS_ID load_cls = TEXT2CLSID(Class);
	R_ASSERT(load_cls==SUB_CLS_ID);

	inherited::Load(section);

//	m_fImpulseThreshold = pSettings->r_float(section,"impulse_threshold");
//	m_fEnergy = pSettings->r_float(section,"energy");
}



void CBastArtifact::UpdateCL() 
{
	inherited::UpdateCL();

	if(getVisible() && m_pPhysicsShell) 
	{
		m_pPhysicsShell->Update();
		XFORM().set(m_pPhysicsShell->mXFORM);
		Position().set(m_pPhysicsShell->mXFORM.c);

		if(!m_AliveList.empty() && m_bStrike)
		{
			CEntityAlive* pEntityToHit = m_AliveList.front();

			Fvector dir;
			//dir = pEntityToHit->Position();
			pEntityToHit->Center(dir);
			dir.sub(this->Position()); 
		
			m_pPhysicsShell->applyImpulse(dir, 
										  m_fStrikeImpulse * Device.fTimeDelta *
										  m_pPhysicsShell->getMass());
			m_bStrike = false;
		}
	} 
	else if(H_Parent()) XFORM().set(H_Parent()->XFORM());
}


void CBastArtifact::Hit(float P, Fvector &dir,	
						CObject* who, s16 element,
						Fvector position_in_object_space, 
						float impulse, 
						ALife::EHitType hit_type)
{
	if(impulse>m_fImpulseThreshold && !m_AliveList.empty())
	{
		m_bStrike = true;
		impulse = 0;
	}
	
	inherited::Hit(P, dir, who, element, position_in_object_space, impulse, hit_type);
}


void CBastArtifact::feel_touch_new(CObject* O) 
{
	CEntityAlive* pEntityAlive = dynamic_cast<CEntityAlive*>(O);

	if(pEntityAlive && pEntityAlive->g_Alive()) 
	{
		m_AliveList.push_back(pEntityAlive);
	}
}

void CBastArtifact::feel_touch_delete(CObject* O) 
{
	CEntityAlive* pEntityAlive = dynamic_cast<CEntityAlive*>(O);

	if(pEntityAlive)
	{
			m_AliveList.erase(std::find(m_AliveList.begin(), 
										m_AliveList.end(), 
										pEntityAlive));
	}
}

BOOL CBastArtifact::feel_touch_contact(CObject* O) 
{
	CEntityAlive* pEntityAlive = dynamic_cast<CEntityAlive*>(O);

	if(pEntityAlive && pEntityAlive->g_Alive()) 
		return TRUE;
	else
		return FALSE;
}


void CBastArtifact::shedule_Update(u32 dt) 
{
	inherited::shedule_Update(dt);

	Fvector	P; 
	P.set(Position());
	feel_touch_update(P,m_fRadius);
}