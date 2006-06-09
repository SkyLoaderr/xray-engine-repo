#pragma once
#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateMonsterEatingAbstract CStateMonsterEating<_Object>

#define TIME_TO_EAT 20000

TEMPLATE_SPECIALIZATION
CStateMonsterEatingAbstract::CStateMonsterEating(_Object *obj) : inherited(obj)
{
}

TEMPLATE_SPECIALIZATION
CStateMonsterEatingAbstract::~CStateMonsterEating()
{
}

TEMPLATE_SPECIALIZATION
void CStateMonsterEatingAbstract::initialize()
{
	inherited::initialize();
	time_last_eat = 0;
}

TEMPLATE_SPECIALIZATION
void CStateMonsterEatingAbstract::execute()
{
	if (object->CorpseMan.get_corpse() != corpse)  return;

	object->set_action				(ACT_EAT);
	object->set_state_sound			(MonsterSound::eMonsterSoundEat);

	// ������ �����
	if (time_last_eat + u32(1000/object->db().m_fEatFreq) < Device.dwTimeGlobal) {
		object->ChangeSatiety(object->db().m_fEatSlice);
		corpse->m_fFood -= object->db().m_fEatSliceWeight;
		time_last_eat = Device.dwTimeGlobal;
	}
}

TEMPLATE_SPECIALIZATION
bool CStateMonsterEatingAbstract::check_start_conditions()
{
	corpse	= const_cast<CEntityAlive*>(object->CorpseMan.get_corpse());
	VERIFY	(corpse);

	Fvector nearest_bone_pos;
	if ((corpse->m_pPhysicsShell == NULL) || (!corpse->m_pPhysicsShell->isActive())) {
		nearest_bone_pos	= corpse->Position(); 
	} else nearest_bone_pos = object->character_physics_support()->movement()->PHCaptureGetNearestElemPos(corpse);

	float dist				= nearest_bone_pos.distance_to(object->Position());
	float dist_to_corpse	= object->db().m_fDistToCorpse; 
	
	if (dist + 0.5f < dist_to_corpse) return true;
	return false;
}


TEMPLATE_SPECIALIZATION
bool CStateMonsterEatingAbstract::check_completion()
{
	if (time_state_started + TIME_TO_EAT < time())	return true;
	if (object->CorpseMan.get_corpse() != corpse)	return true;
	
	Fvector nearest_bone_pos;
	if ((corpse->m_pPhysicsShell == NULL) || (!corpse->m_pPhysicsShell->isActive())) {
		nearest_bone_pos	= corpse->Position(); 
	} else nearest_bone_pos = object->character_physics_support()->movement()->PHCaptureGetNearestElemPos(corpse);

	float dist				= nearest_bone_pos.distance_to(object->Position());
	float dist_to_corpse	= object->db().m_fDistToCorpse; 
	if (dist > dist_to_corpse+0.5f) return true;

	return false;
}

#undef TEMPLATE_SPECIALIZATION
#undef CStateMonsterEatingAbstract
