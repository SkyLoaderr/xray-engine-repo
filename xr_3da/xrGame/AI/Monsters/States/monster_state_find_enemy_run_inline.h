#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateMonsterFindEnemyRunAbstract CStateMonsterFindEnemyRun<_Object>

TEMPLATE_SPECIALIZATION
CStateMonsterFindEnemyRunAbstract::CStateMonsterFindEnemyRun(_Object *obj) : inherited(obj)
{
}

TEMPLATE_SPECIALIZATION
CStateMonsterFindEnemyRunAbstract::~CStateMonsterFindEnemyRun()
{
}

TEMPLATE_SPECIALIZATION
void CStateMonsterFindEnemyRunAbstract::initialize()
{
	inherited::initialize();
	
	target_point	= object->EnemyMan.get_enemy_position();
	target_vertex	= object->EnemyMan.get_enemy_vertex();

	Fvector dir;
	dir.sub(target_point, object->Position());
	dir.normalize();
	
	Fvector test_position;
	test_position.mad(target_point, dir, 10.f);
	
	// �������� ����������� ��������� ������
	if (ai().level_graph().valid_vertex_position(test_position)) {
		u32 vertex_id = ai().level_graph().vertex_id(test_position);
		if (ai().level_graph().valid_vertex_id(vertex_id)) {
			target_point	= test_position;
			target_vertex	= vertex_id;
		}
	}
}

TEMPLATE_SPECIALIZATION
void CStateMonsterFindEnemyRunAbstract::execute()
{
	object->MotionMan.m_tAction			= ACT_RUN;
	object->MotionMan.accel_activate	(eAT_Aggressive);
	object->MoveToTarget				(target_point, target_vertex);
}

TEMPLATE_SPECIALIZATION
bool CStateMonsterFindEnemyRunAbstract::check_completion()
{	
	float dist = object->Position().distance_to(target_point);

	if (dist > 1.5f) return false;
	return true;
}

