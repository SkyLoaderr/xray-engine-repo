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

	object->CMonsterMovement::initialize_movement();	

#ifdef DEBUG
	if (psAI_Flags.test(aiMonsterDebug)) {
		object->HDebug->M_Clear();
	}
#endif
}

TEMPLATE_SPECIALIZATION
void CStateMonsterEatingAbstract::execute()
{
	CEntityAlive *corpse = const_cast<CEntityAlive*>(object->CorpseMan.get_corpse());
	VERIFY(corpse);
	
	// Определить позицию ближайшей боны у трупа
	Fvector nearest_bone_pos;
	if ((corpse->m_pPhysicsShell == NULL) || (!corpse->m_pPhysicsShell->bActive)) {
		nearest_bone_pos	= corpse->Position(); 
	} else nearest_bone_pos = object->m_PhysicMovementControl->PHCaptureGetNearestElemPos(corpse);
	
	float dist = nearest_bone_pos.distance_to(object->Position());
	float dist_to_corpse = (object->get_sd()->m_fDistToCorpse); 
	
	float m_fDistMin = dist_to_corpse - 0.5f;
	float m_fDistMax = dist_to_corpse + 0.5f;
	
	if ((prev_state == eStateEat) && (dist < m_fDistMax)) cur_state = eStateEat;
	else cur_state = ((dist > m_fDistMin) ? eStateWalkCloser : eStateEat);


	// реализация состояния
	if (cur_state == eStateEat) {
		object->set_action				(ACT_EAT);
		object->set_state_sound			(MonsterSpace::eMonsterSoundEat);

		// съесть часть
		if (time_last_eat + u32(1000/object->get_sd()->m_fEatFreq) < object->m_current_update) {
			object->ChangeSatiety(object->get_sd()->m_fEatSlice);
			corpse->m_fFood -= object->get_sd()->m_fEatSliceWeight;
			time_last_eat = object->m_current_update;
		}

#ifdef DEBUG
		if (psAI_Flags.test(aiMonsterDebug)) {
			object->HDebug->M_Add(0,"Eat :: Eating", D3DCOLOR_XRGB(255,0,0));
			
			string64 s;
			sprintf(s,"Food[%f] Satiety[%f]", object->GetSatiety(), corpse->m_fFood);
			object->HDebug->M_Add(1, s, D3DCOLOR_XRGB(255,0,0));
		}
#endif

	} else {
		object->set_action									(ACT_WALK_FWD);
		object->set_state_sound								(MonsterSpace::eMonsterSoundIdle);
		object->CMonsterMovement::set_target_point			(nearest_bone_pos, corpse->level_vertex_id());
		object->CMonsterMovement::set_generic_parameters	();

#ifdef DEBUG
		if (psAI_Flags.test(aiMonsterDebug)) {
			object->HDebug->M_Add(0,"Eat :: Walk To Corpse", D3DCOLOR_XRGB(255,0,0));
		}
#endif
	}

	prev_state = cur_state;	
}

TEMPLATE_SPECIALIZATION
bool CStateMonsterEatingAbstract::check_completion()
{
	if (object->GetSatiety() > object->get_sd()->m_fMaxSatiety) return true;
	return false;
}

#undef TEMPLATE_SPECIALIZATION
#undef CStateMonsterEatingAbstract
