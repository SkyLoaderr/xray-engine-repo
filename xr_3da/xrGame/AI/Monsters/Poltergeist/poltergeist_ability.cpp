#include "stdafx.h"
#include "poltergeist.h"
#include "../../../PhysicsShell.h"

#define IMPULSE			2.f
#define TRACE_DISTANCE  10.f

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

void CPoltergeist::StrangeSounds(const Fvector &position)
{
	//Fvector dir;
	//dir.random_dir(position);

	//Collide::rq_result	l_rq;
	//if (Level().ObjectSpace.RayPick(position, dir, TRACE_DISTANCE, Collide::rqtStatic, l_rq)) {
	//	if (l_rq.range < TRACE_DISTANCE) {
	//		
	//		// Получить материал
	//		CDB::TRI*	pTri	= Level().ObjectSpace.GetStaticTris() + l_rq.element;
	//		u16 material_idx	= pTri->material;
	//		SGameMtl* mtl		= GMLib.GetMaterialByIdx(material_idx);
	//								
	//			
	//		
	//		SGameMtlPair* mtl_pair		= pMonster->CMaterialManager::get_current_pair();
	//	}
	//}
}

