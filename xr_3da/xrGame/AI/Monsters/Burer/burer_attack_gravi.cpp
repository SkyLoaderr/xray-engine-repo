#include "stdafx.h"
#include "../../ai_monster_state.h"
#include "burer_attack.h"
#include "burer.h"

#define GOOD_DISTANCE_FOR_GRAVI 8.f
#define GRAVI_DELAY				5000
#define GRAVI_HOLD				1500

bool CBurerAttack::CheckGravi()
{
	// если triple анимация активна - выход
	if (pMonster->MotionMan.TA_IsActive()) return false;
	bool b_normal_trans = ((m_tAction == ACTION_DEFAULT) || (m_tAction == ACTION_RUN) || (m_tAction == ACTION_MELEE));
	if (!b_normal_trans) return false;

	// обработать объекты
	float dist = pMonster->Position().distance_to(enemy->Position());
	if (dist < GOOD_DISTANCE_FOR_GRAVI) return false;

	bool see_enemy_now		= pMonster->EnemyMan.get_enemy_time_last_seen() == m_dwCurrentTime;
	bool good_time_to_start = time_next_gravi_available < m_dwCurrentTime;

	if (!see_enemy_now || !good_time_to_start) return false; 

	// всё ок, можно начать грави атаку
	return true;
}


void CBurerAttack::Execute_Gravi()
{

	if (m_tAction == ACTION_GRAVI_STARTED) {
		pMonster->MotionMan.TA_Activate(&pMonster->anim_triple_gravi);
		m_tAction = ACTION_GRAVI_CONTINUE;
		time_gravi_started			= m_dwCurrentTime;
		time_next_gravi_available	= u32(-1);
		pMonster->StartGraviPrepare();
	}

	if (m_tAction == ACTION_GRAVI_CONTINUE) {
		// проверить на грави удар
		if (time_gravi_started + GRAVI_HOLD < m_dwCurrentTime) {
			m_tAction = ACTION_GRAVI_FIRE;
		}
	}

	if (m_tAction == ACTION_GRAVI_FIRE) {
		pMonster->MotionMan.TA_PointBreak();
		Fvector from_pos;
		Fvector target_pos;
		from_pos	= pMonster->Position();	from_pos.y		+= 0.5f;
		target_pos	= enemy->Position();	target_pos.y	+= 0.5f;
		pMonster->m_gravi_object.activate(enemy, from_pos, target_pos);
		time_next_gravi_available = m_dwCurrentTime + GRAVI_DELAY;
		m_tAction = ACTION_WAIT_TRIPLE_END;
		pMonster->StopGraviPrepare();
		pMonster->CSoundPlayer::play(eMonsterSoundGraviAttack);
	}

}	

