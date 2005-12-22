#pragma once
#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateMonsterEatingAbstract CStateMonsterEating<_Object>

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
	cur_state = prev_state = eStateWalkCloser;
	time_last_eat = 0;

	object->path().prepare_builder();	
}

TEMPLATE_SPECIALIZATION
void CStateMonsterEatingAbstract::execute()
{
	CEntityAlive *corpse = const_cast<CEntityAlive*>(object->CorpseMan.get_corpse());
	VERIFY(corpse);
	
	// Определить позицию ближайшей боны у трупа
	Fvector nearest_bone_pos;
	if ((corpse->m_pPhysicsShell == NULL) || (!corpse->m_pPhysicsShell->isActive())) {
		nearest_bone_pos	= corpse->Position(); 
	} else nearest_bone_pos = object->m_PhysicMovementControl->PHCaptureGetNearestElemPos(corpse);
	
	float dist = nearest_bone_pos.distance_to(object->Position());
	float dist_to_corpse = (object->db().m_fDistToCorpse); 
	
	float m_fDistMin = dist_to_corpse - 0.5f;
	float m_fDistMax = dist_to_corpse + 0.5f;
	
	if ((prev_state == eStateEat) && (dist < m_fDistMax)) cur_state = eStateEat;
	else cur_state = ((dist > m_fDistMin) ? eStateWalkCloser : eStateEat);


	// реализация состояния
	if (cur_state == eStateEat) {
		object->set_action				(ACT_EAT);
		object->set_state_sound			(MonsterSound::eMonsterSoundEat);

		// съесть часть
		if (time_last_eat + u32(1000/object->db().m_fEatFreq) < Device.dwTimeGlobal) {
			object->conditions().ChangeSatiety(object->db().m_fEatSlice);
			corpse->m_fFood -= object->db().m_fEatSliceWeight;
			time_last_eat = Device.dwTimeGlobal;
		}

	} else {
		object->set_action							(ACT_WALK_FWD);
		object->set_state_sound						(MonsterSound::eMonsterSoundIdle);
		object->path().set_target_point			(nearest_bone_pos, corpse->ai_location().level_vertex_id());
		object->path().set_generic_parameters	();
	}

	prev_state = cur_state;	
}

TEMPLATE_SPECIALIZATION
bool CStateMonsterEatingAbstract::check_completion()
{
	if (object->conditions().GetSatiety() > object->db().m_fMaxSatiety) return true;
	return false;
}

#undef TEMPLATE_SPECIALIZATION
#undef CStateMonsterEatingAbstract
