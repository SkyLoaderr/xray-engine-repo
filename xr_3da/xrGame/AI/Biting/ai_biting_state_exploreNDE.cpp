#include "stdafx.h"
#include "ai_biting.h"
#include "ai_biting_state.h"

//////////////////////////////////////////////////////////////////////////
// ќписание состо€ни€ <—лышу выгодный и неопасный звук>
// 1. ѕосмотреть на позицию звука
// 2. ѕрийти на точку звука
// 3. ќсмотретьс€
//////////////////////////////////////////////////////////////////////////

#define SF_MANY_DIFFERENT_SOUNDS	(1 << 1)

CBitingExploreNDE::CBitingExploreNDE(CAI_Biting *p)
{
	pMonster = p;
	SetPriority(PRIORITY_NORMAL);
}

void CBitingExploreNDE::Init()
{
	LOG_EX("nde init");
	inherited::Init();

	pMonster->GetSound(m_tSound);

	m_tAction = ACTION_LOOK_DESTINATION;
}

void CBitingExploreNDE::Run()
{
	// UpdateFlags();
	SoundElem new_sound;
	pMonster->GetSound(new_sound);
	if ((new_sound.time != m_tSound.time))  Init();

	
	switch (m_tAction) {
	case ACTION_LOOK_DESTINATION:			// повернутьс€ в сторону звука
		LOG_EX("nde: LOOK DEST");
		pMonster->enable_movement(false);
		
		pMonster->MotionMan.m_tAction = ACT_STAND_IDLE;
		pMonster->LookPosition(m_tSound.position);
		
		if (angle_difference(pMonster->m_body.target.yaw, pMonster->m_body.current.yaw) < PI_DIV_6) m_tAction = ACTION_GOTO_SOUND_SOURCE;
		break;
	case ACTION_GOTO_SOUND_SOURCE:			// идти к источнику
		LOG_EX("nde: GOTO_SOUND_SOURCE");
		
		pMonster->MotionMan.m_tAction = ACT_WALK_FWD;
		pMonster->Path_ApproachPoint(m_tSound.position);
		
		// если монстр дошел до конца пути, перейти к следующему заданию
		if (IS_NEED_REBUILD()) m_tAction = ACTION_LOOK_AROUND; 

		break;
	case ACTION_LOOK_AROUND:
		LOG_EX("nde: LOOK_AROUND");
		pMonster->MotionMan.m_tAction = ACT_LOOK_AROUND;
		pMonster->enable_movement(false);
		break;
	}

	pMonster->SetPathParams(pMonster->level_vertex_id(), pMonster->Position()); 
	pMonster->SetSound(SND_TYPE_IDLE, pMonster->_sd->m_dwIdleSndDelay);
}

//void CBitingExploreNDE::UpdateFlags()
//{
//	const xr_vector<SoundElem>	&sounds = pMonster->GetSoundData();
//	
//	SoundElem cur_sound;
//	pMonster->GetSound(cur_sound);
//
//	if (m_dwStateStartedTime != m_dwCurrentTime) {
//		if (cur_sound.type == m_tSound.time)
//		
//		if (cur_sound.time != m_tSound.time) {
//
//		}
//	}
//
//}

