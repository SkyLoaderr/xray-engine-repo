#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateBurerAttackTeleAbstract CStateBurerAttackTele<_Object>

TEMPLATE_SPECIALIZATION
CStateBurerAttackTeleAbstract::CStateBurerAttackTele(LPCSTR state_name) : inherited(state_name)
{
}

TEMPLATE_SPECIALIZATION
CStateBurerAttackTeleAbstract::~CStateBurerAttackTele()
{
}

TEMPLATE_SPECIALIZATION
void CStateBurerAttackTeleAbstract::initialize()
{
	inherited::initialize();
}

TEMPLATE_SPECIALIZATION
void CStateBurerAttackTeleAbstract::execute()
{
	const CEntityAlive *enemy		= m_object->EnemyMan.get_enemy();
	m_object->MotionMan.m_tAction	= ACT_STAND_IDLE;
	float dist = enemy->Position().distance_to(m_object->Position());

	TTime cur_time = Level().timeServer();

	u32 i=0;
	while (i < m_object->CTelekinesis::get_objects_count()) {
		CTelekineticObject tele_object = m_object->CTelekinesis::get_object_by_index(i);
		if ((tele_object.get_state() == TS_Keep) && (tele_object.time_keep_started + 2000 < cur_time)) {
			Fvector enemy_pos = enemy->Position();
			enemy_pos.y += enemy->Radius() * dist;
			m_object->CTelekinesis::fire(tele_object.get_object(), enemy_pos);
		} else i++;
	}
}

TEMPLATE_SPECIALIZATION
void CStateBurerAttackTeleAbstract::finalize()
{
	inherited::finalize();
}

