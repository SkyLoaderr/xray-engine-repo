#include "stdafx.h"
#include "base_monster.h"
#include "base_monster_state.h"

//////////////////////////////////////////////////////////////////////////
// �������� ��������� <����� �������� � ��������� ����>
// 1. ���������� �� ������� �����
// 2. ������ �� ����� �����
// 3. �����������
//////////////////////////////////////////////////////////////////////////

#define SF_MANY_DIFFERENT_SOUNDS	(1 << 1)

CBaseMonsterExploreNDE::CBaseMonsterExploreNDE(CBaseMonster *p)
{
	pMonster = p;
	SetPriority(PRIORITY_NORMAL);
}

void CBaseMonsterExploreNDE::Init()
{
	LOG_EX("nde init");
	inherited::Init();

	m_tSound	= pMonster->SoundMemory.GetSound();
	target_pos	= m_tSound.position;

	m_tAction = ACTION_LOOK_DESTINATION;

	pMonster->CMonsterMovement::initialize_movement	();
}

void CBaseMonsterExploreNDE::Run()
{
	// �������� ������� �����
	SoundElem se = pMonster->SoundMemory.GetSound();
	if ((se.who != m_tSound.who) || (se.position.distance_to(m_tSound.position) > 5.f)) Init();
	else m_tSound = se;


	switch (m_tAction) {
	case ACTION_LOOK_DESTINATION:			// ����������� � ������� �����
		LOG_EX("nde: LOOK DEST");
		
		pMonster->MotionMan.m_tAction = ACT_STAND_IDLE;
		pMonster->disable_path();
		pMonster->LookPosition(m_tSound.position);
		
		if (angle_difference(pMonster->CMovementManager::m_body.target.yaw, pMonster->CMovementManager::m_body.current.yaw) < PI_DIV_6) m_tAction = ACTION_GOTO_SOUND_SOURCE;
		break;
	
	case ACTION_GOTO_SOUND_SOURCE:			// ���� � ���������
		LOG_EX("nde: GOTO_SOUND_SOURCE");
		pMonster->MotionMan.m_tAction = ACT_WALK_FWD;

		pMonster->CMonsterMovement::set_target_point		(m_tSound.position);
		pMonster->CMonsterMovement::set_generic_parameters	();

		// ���� ������ ����� �� ������� �����, ������� � ���������� �������
		if (m_tSound.position.distance_to(pMonster->Position()) < 1.0f) m_tAction = ACTION_LOOK_AROUND; 
		break;
	case ACTION_LOOK_AROUND:
		LOG_EX("nde: LOOK_AROUND");
		pMonster->MotionMan.m_tAction = ACT_LOOK_AROUND;
		break;
	}

	pMonster->State_PlaySound(MonsterSpace::eMonsterSoundIdle, pMonster->get_sd()->m_dwIdleSndDelay);

}
