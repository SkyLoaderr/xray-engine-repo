#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateZombieAttackRunAbstract CStateZombieAttackRun<_Object>

TEMPLATE_SPECIALIZATION
CStateZombieAttackRunAbstract::CStateZombieAttackRun(_Object *obj) : inherited(obj)
{
}

TEMPLATE_SPECIALIZATION
CStateZombieAttackRunAbstract::~CStateZombieAttackRun()
{
}

TEMPLATE_SPECIALIZATION
void CStateZombieAttackRunAbstract::initialize()
{
	inherited::initialize();
	m_time_path_rebuild	= 0;

	if (object->HitMemory.is_hit() && (object->GetHealth() < 0.5f)) {
		action = ACT_RUN;
	} else {
		action = ACT_WALK_FWD;
	}

}

TEMPLATE_SPECIALIZATION
void CStateZombieAttackRunAbstract::execute()
{
	float dist = object->EnemyMan.get_enemy()->Position().distance_to(object->Position());

	// проверить необходимость перестройки пути
	bool b_need_rebuild = false;
	
	
	// check time interval
	TTime time_current = Level().timeServer();
	if (m_time_path_rebuild + 100 + 50.f * dist < time_current) {
		m_time_path_rebuild = time_current;
		b_need_rebuild		= true;
	}
	if (object->IsPathEnd(2,0.5f)) b_need_rebuild = true;

	
	//////////////////////////////////////////////////////////////////////////
	// обработать squad-данные
	//////////////////////////////////////////////////////////////////////////
	CMonsterSquad *squad	= monster_squad().get_squad(object);
	bool squad_active		= squad && squad->SquadActive();
	
	// Получить команду
	SSquadCommand command;
	squad->GetCommand(object, command);
	if (!squad_active || (command.type != SC_ATTACK)) squad_active = false;
	//////////////////////////////////////////////////////////////////////////

	
	if (b_need_rebuild)	{
		object->MoveToTarget(object->EnemyMan.get_enemy());
		if (squad_active) object->set_dest_direction(command.direction);
	}

	if (squad_active)
		object->set_use_dest_orient	(true);
	
	
	// установка параметров функциональных блоков
	object->MotionMan.m_tAction					= action;	
	if (action == ACT_RUN) 
		object->CMonsterMovement::set_try_min_time	(true);
	
	object->CSoundPlayer::play					(MonsterSpace::eMonsterSoundAttack, 0,0,object->get_sd()->m_dwAttackSndDelay);
	object->MotionMan.accel_activate			(eAT_Aggressive);
	object->MotionMan.accel_set_braking			(false);

#ifdef DEBUG
	if (psAI_Flags.test(aiMonsterDebug)) {
		object->HDebug->M_Add(0,"Attack :: Run", D3DCOLOR_XRGB(255,0,0));
	}
#endif

}

TEMPLATE_SPECIALIZATION
bool CStateZombieAttackRunAbstract::check_completion()
{
	float m_fDistMin, m_fDistMax, dist;
	dist = object->GetEnemyDistances(m_fDistMin, m_fDistMax);

	if (dist < m_fDistMin)	return true;

	return false;
}

TEMPLATE_SPECIALIZATION
bool CStateZombieAttackRunAbstract::check_start_conditions()
{
	float m_fDistMin, m_fDistMax, dist;
	dist = object->GetEnemyDistances(m_fDistMin, m_fDistMax);
	
	if (dist > m_fDistMax)	return true;

	return false;
}

