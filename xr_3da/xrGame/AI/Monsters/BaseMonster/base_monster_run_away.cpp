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
}

void CBaseMonsterRunAway::Run()
{
	switch (m_tAction) {
	case ACTION_RUN:	// ���� � ���������
		pMonster->MotionMan.m_tAction = ACT_RUN;
		pMonster->MoveAwayFromTarget (danger_pos);
		break;
	case ACTION_LOOK_AROUND:
		pMonster->MotionMan.m_tAction = ACT_LOOK_AROUND;
		pMonster->FaceTarget(danger_pos);
		break;
	}
	pMonster->State_PlaySound(MonsterSpace::eMonsterSoundPanic, pMonster->get_sd()->m_dwAttackSndDelay);

}
