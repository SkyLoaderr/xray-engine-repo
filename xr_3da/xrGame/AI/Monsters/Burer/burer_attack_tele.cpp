#include "stdafx.h"
#include "../../ai_monster_state.h"
#include "burer_attack.h"
#include "burer.h"

#define GOOD_DISTANCE_FOR_TELE	15.f


//////////////////////////////////////////////////////////////////////////
// ��������� �� ��������� ����������
//////////////////////////////////////////////////////////////////////////

bool CBurerAttack::CheckTele()
{
	// ���� triple �������� ������� - �����
	if (pMonster->MotionMan.TA_IsActive()) return false;
	bool b_normal_trans = ((m_tAction == ACTION_DEFAULT) || (m_tAction == ACTION_RUN) || (m_tAction == ACTION_MELEE));
	if (!b_normal_trans) return false;

	// ��������� ��� ������� (�.�. ������� ��� ������ � ���������)

	if (pMonster->CTelekinesis::get_objects_count() > 0) return true;

	// ���������� �������
	float dist = pMonster->Position().distance_to(enemy->Position());
	if (dist < GOOD_DISTANCE_FOR_TELE) return false;

	find_tele_objects();

	// ���� ��� ��������
	if (pMonster->CTelekinesis::get_objects_count() <= 0) return false;

	// �� �� ����� �������� ���������
	return true;
}

//////////////////////////////////////////////////////////////////////////
// ��������� ���������
//////////////////////////////////////////////////////////////////////////

void CBurerAttack::Execute_Tele()
{

	if (m_tAction == ACTION_TELE_STARTED) {
		pMonster->MotionMan.TA_Activate(&pMonster->anim_triple_tele);
		m_tAction = ACTION_TELE_CONTINUE;
	}

	CTelekineticObject  tele_object; // ������ ��� ������	
	bool object_found = false;

	if (m_tAction == ACTION_TELE_CONTINUE) {

		// ����� ������ ��� �����
		u32 i=0;
		while (i < pMonster->CTelekinesis::get_objects_count()) {
			tele_object = pMonster->CTelekinesis::get_object_by_index(i);
			if ((tele_object.get_state() == TS_Keep) && (tele_object.time_keep_started + 1500 < m_dwCurrentTime)) {

				object_found = true;
				break;

			} else i++;

		}

		if (object_found) m_tAction = ACTION_TELE_FIRE;		
	}

	if (m_tAction == ACTION_TELE_FIRE) {
		pMonster->MotionMan.TA_PointBreak();

		Fvector enemy_pos = enemy->Position();
		enemy_pos.y += 2 * enemy->Radius();

		float dist = tele_object.get_object()->Position().distance_to(enemy->Position());
		pMonster->CTelekinesis::fire(tele_object.get_object(), enemy_pos, dist/10);

		m_tAction = ACTION_WAIT_TRIPLE_END;
	}


	if (((m_tAction == ACTION_TELE_STARTED) || (m_tAction == ACTION_TELE_CONTINUE) ) && (pMonster->CTelekinesis::get_objects_count() <= 0)) {
		pMonster->MotionMan.TA_Deactivate();
		m_tAction = ACTION_DEFAULT;
	}

}
