#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateMonsterAttackRunAbstract CStateMonsterAttackRun<_Object>

TEMPLATE_SPECIALIZATION
CStateMonsterAttackRunAbstract::CStateMonsterAttackRun(LPCSTR state_name) : inherited(state_name)
{
}

TEMPLATE_SPECIALIZATION
CStateMonsterAttackRunAbstract::~CStateMonsterAttackRun()
{
}

TEMPLATE_SPECIALIZATION
void CStateMonsterAttackRunAbstract::initialize()
{
	inherited::initialize();
	
	m_time_path_rebuild	= 0;
}

TEMPLATE_SPECIALIZATION
void CStateMonsterAttackRunAbstract::execute()
{
	const CEntity *enemy = m_object->m_tEnemy.obj;
	float dist = enemy->Position().distance_to(m_object->Position());

	// проверить необходимость перестройки пути
	bool b_need_rebuild = false;
	
	
	// check time interval
	TTime time_current = Level().timeServer();
	if (m_time_path_rebuild + 100 + 50.f * dist < time_current) {
		m_time_path_rebuild = time_current;
		b_need_rebuild		= true;
	}
	if (PATH_NEED_REBUILD()) b_need_rebuild = true;

	if (b_need_rebuild)	m_object->MoveToTarget(m_object->m_tEnemy.obj);

	// установка параметров функциональных блоков
	m_object->MotionMan.m_tAction					= ACT_RUN;
	m_object->CMonsterMovement::set_try_min_time	(false);
	m_object->CSoundPlayer::play					(MonsterSpace::eMonsterSoundAttack, 0,0,m_object->_sd->m_dwAttackSndDelay);
	m_object->MotionMan.accel_activate				(eAT_Aggressive);
	m_object->MotionMan.accel_set_braking			(false);
}

TEMPLATE_SPECIALIZATION
void CStateMonsterAttackRunAbstract::finalize()
{
	inherited::finalize();
}

