#include "stdafx.h"
#include "poltergeist.h"
#include "../../../PhysicsShell.h"

#define IMPULSE 2.f

void CPoltergeist::PhysicalImpulse(const Fvector &position)
{
	Level().ObjectSpace.GetNearest(position, 10.f); 
	xr_vector<CObject*> &tpObjects = Level().ObjectSpace.q_nearest;

	if (tpObjects.empty()) return;
	
	u32 index = Random.randI(tpObjects.size());
	
	CPhysicsShellHolder  *obj = dynamic_cast<CPhysicsShellHolder *>(tpObjects[index]);
	if (!obj || !obj->m_pPhysicsShell) return;

	Fvector dir;
	dir.sub(obj->Position(), position);
	dir.normalize();
	obj->m_pPhysicsShell->applyImpulse(dir,IMPULSE * obj->m_pPhysicsShell->getMass());
}

void CPoltergeist::StrangeSounds()
{
	
}

