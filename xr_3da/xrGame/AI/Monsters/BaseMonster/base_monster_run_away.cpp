#include "stdafx.h"
#include "base_monster.h"
#include "base_monster_state.h"

CBaseMonsterRunAway::CBaseMonsterRunAway(CBaseMonster *p)
{
	pMonster = p;
}

void CBaseMonsterRunAway::Init()
{
	inherited::Init();

	Fvector dir = pMonster->HitMemory.get_last_hit_dir();
	danger_pos.mad(pMonster->Position(), dir, 5.f);

	m_tAction = ACTION_RUN;

	pMonster->CMonsterMovement::initialize_movement();	
}

void CBaseMonsterRunAway::Run()
{
	switch (m_tAction) {
	case ACTION_RUN:	// ���� � ���������
		pMonster->set_action								(ACT_RUN);
		pMonster->CMonsterMovement::set_retreat_from_point	(danger_pos);
		pMonster->CMonsterMovement::set_generic_parameters	();
		break;
	case ACTION_LOOK_AROUND:
		pMonster->set_action		(ACT_LOOK_AROUND);
		pMonster->DirMan.face_target(danger_pos);
		break;
	}
	pMonster->set_state_sound(MonsterSpace::eMonsterSoundPanic);
}
