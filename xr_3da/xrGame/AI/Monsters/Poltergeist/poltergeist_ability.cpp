#include "stdafx.h"
#include "poltergeist.h"
#include "../../../PhysicsShell.h"

#define IMPULSE					10.f
#define IMPULSE_RADIUS			5.f
#define TRACE_DISTANCE			10.f
#define TRACE_ATTEMPT_COUNT		3

void CPoltergeist::PhysicalImpulse(const Fvector &position)
{
	Level().ObjectSpace.GetNearest(position, IMPULSE_RADIUS); 
	xr_vector<CObject*> &tpObjects = Level().ObjectSpace.q_nearest;

	if (tpObjects.empty()) return;
	
	u32 index = Random.randI(tpObjects.size());
	
	CPhysicsShellHolder  *obj = smart_cast<CPhysicsShellHolder *>(tpObjects[index]);
	if (!obj || !obj->m_pPhysicsShell) return;

	Fvector dir;
	dir.sub(obj->Position(), position);
	dir.normalize();
	
	CPhysicsElement* E=obj->m_pPhysicsShell->get_ElementByStoreOrder(u16(Random.randI(obj->m_pPhysicsShell->get_ElementsNumber())));
	//E->applyImpulse(dir,IMPULSE * obj->m_pPhysicsShell->getMass());
	E->applyImpulse(dir,IMPULSE * E->getMass());
}

void CPoltergeist::StrangeSounds(const Fvector &position)
{
	if (m_strange_sound.feedback) return;
	
	for (u32 i = 0; i < TRACE_ATTEMPT_COUNT; i++) {
		Fvector dir;
		dir.random_dir();

		Collide::rq_result	l_rq;
		if (Level().ObjectSpace.RayPick(position, dir, TRACE_DISTANCE, Collide::rqtStatic, l_rq)) {
			if (l_rq.range < TRACE_DISTANCE) {

				// Получить пару материалов
				CDB::TRI*	pTri	= Level().ObjectSpace.GetStaticTris() + l_rq.element;
				SGameMtlPair* mtl_pair = GMLib.GetMaterialPair(CMaterialManager::self_material_idx(),pTri->material);
				if (!mtl_pair) continue;

				// Играть звук
				if (!mtl_pair->CollideSounds.empty()) {
					SELECT_RANDOM(m_strange_sound, mtl_pair, CollideSounds);

					Fvector pos;
					pos.mad(position, dir, ((l_rq.range - 0.1f > 0) ? l_rq.range - 0.1f  : l_rq.range));
					m_strange_sound.play_at_pos(this,pos);
					return;
				}			
			}
		}
	}
}

