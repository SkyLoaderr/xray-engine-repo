#include "stdafx.h"
#include "../../ai_monster_state.h"
#include "burer_attack_misc.h"
#include "burer.h"
#include "../../ai_monster_utils.h"

CBurerAttackRunAround::CBurerAttackRunAround(CBurer *p)
{
	pMonster = p;
}

void CBurerAttackRunAround::Init()
{
	inherited::Init();

	enemy = pMonster->EnemyMan.get_enemy();	

	time_started = Level().timeServer();

	// select point
	
	Fvector dir;
	float dist = pMonster->Position().distance_to(enemy->Position());

	if (dist > 30.f) {								// бежать к врагу
		dir.sub(enemy->Position(),pMonster->Position());
		dir.normalize();
		selected_point.mad(pMonster->Position(),dir,10.f);
	} else if ((dist < 20.f) && (dist > 10.f)) {	// убегать от врага
		dir.sub(pMonster->Position(), enemy->Position());
		dir.normalize();
		selected_point.mad(pMonster->Position(),dir,10.f);
	} else											// выбрать случайную позицию
		selected_point = random_position(pMonster->Position(), 10.f);

}


void CBurerAttackRunAround::Run()
{
	pMonster->MotionMan.m_tAction = ACT_RUN;
	pMonster->MoveToTarget(selected_point);
	pMonster->CSoundPlayer::play(MonsterSpace::eMonsterSoundAttack, 0,0,pMonster->get_sd()->m_dwAttackSndDelay);
	
}

void CBurerAttackRunAround::Done()
{
}

void CBurerAttackRunAround::UpdateExternal()
{
	enemy = pMonster->EnemyMan.get_enemy();	
}

bool CBurerAttackRunAround::IsCompleted() 
{
	float dist = pMonster->Position().distance_to(selected_point);
	if ((time_started + 3500 < Level().timeServer()) || (dist < 2.f)) {
		pMonster->FaceTarget(enemy);
		return true;
	}

	else return false;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

CBurerAttackFaceTarget::CBurerAttackFaceTarget(CBurer *p)
{
	pMonster = p;
}

void CBurerAttackFaceTarget::Init()
{
	inherited::Init();

	enemy = pMonster->EnemyMan.get_enemy();	
	point = enemy->Position();
}


void CBurerAttackFaceTarget::Run()
{
	pMonster->MotionMan.m_tAction = ACT_STAND_IDLE;
	pMonster->FaceTarget(point);
	pMonster->CSoundPlayer::play(MonsterSpace::eMonsterSoundAttack, 0,0,pMonster->get_sd()->m_dwAttackSndDelay);
}

void CBurerAttackFaceTarget::Done()
{
}

void CBurerAttackFaceTarget::UpdateExternal()
{
	enemy = pMonster->EnemyMan.get_enemy();	
}

bool CBurerAttackFaceTarget::IsCompleted() 
{
	if (angle_difference(pMonster->m_body.current.yaw,pMonster->m_body.target.yaw) < PI_DIV_6) {
		return true;
	}
	return false;
}

