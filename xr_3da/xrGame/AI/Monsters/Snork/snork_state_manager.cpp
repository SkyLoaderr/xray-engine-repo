#include "stdafx.h"
#include "snork.h"
#include "snork_state_manager.h"
#include "../../ai_monster_squad.h"
#include "../../ai_monster_squad_manager.h"
#include "../../ai_monster_utils.h"
#include "../states/monster_state_rest.h"
#include "../../ai_monster_debug.h"

CStateManagerSnork::CStateManagerSnork(CSnork *obj) : inherited(obj)
{
	add_state(eStateRest,				xr_new<CStateMonsterRest<CSnork> >	(obj));
}

CStateManagerSnork::~CStateManagerSnork()
{
}

void CStateManagerSnork::initialize()
{
	inherited::initialize();
}

void CStateManagerSnork::execute()
{
	u32 state_id = u32(-1);
//	const CEntityAlive* enemy	= object->EnemyMan.get_enemy();
//	const CEntityAlive* corpse	= object->CorpseMan.get_corpse();

	state_id	= eStateRest;

	// ���������� ������� ���������
	select_state(state_id); 

	// ��������� ������� ���������
	get_state_current()->execute();

	prev_substate = current_substate;

	// ������������� squad � ����� �����
	squad_notify();

#ifdef DEBUG
	if (monster_squad().get_squad(object)->GetLeader() == object) {
		object->HDebug->L_Clear();
		object->HDebug->L_AddPoint(monster_squad().get_squad(object)->GetLeader()->Position(), 0.35f, D3DCOLOR_XRGB(0,255,0));
	}
#endif
}

void CStateManagerSnork::squad_notify()
{
	CMonsterSquad	*squad = monster_squad().get_squad(object);
	SMemberGoal		goal;

	// ��� ����������� ����������� ��������� ����� ������������ current_substate
	// ��� ����������� ����������� ���� ��������� ����� ������������ get_state_type()

	//VERIFY(get_state_type() != ST_Unknown);

	if (current_substate == eStateAttack) {

		goal.type	= MG_AttackEnemy;
		goal.entity	= const_cast<CEntityAlive*>(object->EnemyMan.get_enemy());

	} else if (current_substate == eStateRest) {

		if (get_state_type() == ST_Rest) {

			goal.type	= MG_Rest;

		} else if (get_state_type() == ST_WalkGraphPoint) {

			goal.type	= MG_WalkGraph;

		}
	} else if ((current_substate == eStateSquadRest) || (current_substate == eStateSquadRestFollow)) {

		goal.type	= MG_Rest;

	}

	squad->UpdateGoal(object, goal);
}
