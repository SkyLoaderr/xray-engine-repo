#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateMonsterAttackRunAbstract CStateMonsterAttackRun<_Object>

TEMPLATE_SPECIALIZATION
CStateMonsterAttackRunAbstract::CStateMonsterAttackRun(_Object *obj) : inherited(obj)
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
	float dist = object->EnemyMan.get_enemy()->Position().distance_to(object->Position());

	// ��������� ������������� ����������� ����
	bool b_need_rebuild = false;
	
	
	// check time interval
	TTime time_current = Level().timeServer();
	if (m_time_path_rebuild + 100 + 50.f * dist < time_current) {
		m_time_path_rebuild = time_current;
		b_need_rebuild		= true;
	}
	if (object->IsPathEnd(2,0.5f)) b_need_rebuild = true;

	if (b_need_rebuild)	object->MoveToTarget(object->EnemyMan.get_enemy());

	// ��������� ���������� �������������� ������
	object->MotionMan.m_tAction					= ACT_RUN;
	object->CMonsterMovement::set_try_min_time	(false);
	object->CSoundPlayer::play					(MonsterSpace::eMonsterSoundAttack, 0,0,object->get_sd()->m_dwAttackSndDelay);
	object->MotionMan.accel_activate			(eAT_Aggressive);
	object->MotionMan.accel_set_braking			(false);
}

TEMPLATE_SPECIALIZATION
bool CStateMonsterAttackRunAbstract::check_completion()
{
	float m_fDistMin, m_fDistMax, dist;
	dist = object->GetEnemyDistances(m_fDistMin, m_fDistMax);

	if (dist < m_fDistMin)	return true;

	return false;
}

TEMPLATE_SPECIALIZATION
bool CStateMonsterAttackRunAbstract::check_start_conditions()
{
	float m_fDistMin, m_fDistMax, dist;
	dist = object->GetEnemyDistances(m_fDistMin, m_fDistMax);
	
	if (dist > m_fDistMax)	return true;

	return false;
}

