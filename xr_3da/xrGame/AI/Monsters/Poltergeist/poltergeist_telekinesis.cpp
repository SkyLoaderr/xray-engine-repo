#include "stdafx.h"
#include "poltergeist.h"
#include "../../../PhysicsShellHolder.h"
#include "../../../level.h"

#define TELE_RADIUS  10.f
#define TIME_TO_HOLD 2000

void CPoltergeist::ProcessTelekinesis(const CObject *target)
{
	if (CTelekinesis::is_active())	return;

	xr_vector<CObject*> tpObjects	;
	Level().ObjectSpace.GetNearest	(tpObjects,target->Position(), TELE_RADIUS, NULL); 
	

	if (tpObjects.empty()) return;

	u32 index = Random.randI(tpObjects.size());

	CPhysicsShellHolder  *obj = smart_cast<CPhysicsShellHolder *>(const_cast<CObject*>(tpObjects[index]));
	if (!obj || !obj->m_pPhysicsShell) return;

	CTelekinesis::activate(obj,1.5f, 2.f, 5000);

	time_tele_start = Device.dwTimeGlobal;
	tele_enemy		= target;
	tele_object		= obj;
}


void CPoltergeist::UpdateTelekinesis()
{
	if (!CTelekinesis::is_active()) return;
	if (!tele_enemy) return;
	if (time_tele_start + TIME_TO_HOLD > Device.dwTimeGlobal) return;
	
	Fvector enemy_pos;
	enemy_pos	= get_head_position(const_cast<CObject*>(tele_enemy));
	CTelekinesis::fire_t(tele_object,enemy_pos, 0.55f);

	tele_enemy = 0;
}


