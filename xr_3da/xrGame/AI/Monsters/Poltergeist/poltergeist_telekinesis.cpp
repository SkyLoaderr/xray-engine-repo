#include "stdafx.h"
#include "poltergeist.h"
#include "../../../PhysicsShellHolder.h"

#define TELE_RADIUS  10.f
#define TIME_TO_HOLD 2000

void CPoltergeist::ProcessTelekinesis(const CObject *target)
{
	if (CTelekinesis::is_active()) return;

	Level().ObjectSpace.GetNearest(target->Position(), TELE_RADIUS); 
	xr_vector<CObject*> &tpObjects = Level().ObjectSpace.q_nearest;

	if (tpObjects.empty()) return;

	u32 index = Random.randI(tpObjects.size());

	CPhysicsShellHolder  *obj = smart_cast<CPhysicsShellHolder *>(const_cast<CObject*>(tpObjects[index]));
	if (!obj || !obj->m_pPhysicsShell) return;

	CTelekinesis::activate(obj,1.5f, 2.f, 5000);

	time_tele_start = Level().timeServer();
	tele_enemy		= target;
	tele_object		= obj;
}


void CPoltergeist::UpdateTelekinesis()
{
	if (!CTelekinesis::is_active()) return;
	if (!tele_enemy) return;
	if (time_tele_start + TIME_TO_HOLD > Level().timeServer()) return;
	
	Fvector enemy_pos = tele_enemy->Position();
	enemy_pos.y += 5 * tele_enemy->Radius();

	float dist = tele_object->Position().distance_to(tele_enemy->Position());
	CTelekinesis::fire(tele_object, enemy_pos, dist/12);

	tele_enemy = 0;
}


