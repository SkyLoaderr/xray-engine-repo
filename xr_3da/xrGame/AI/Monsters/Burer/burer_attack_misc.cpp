#include "stdafx.h"
#include "../ai_monster_state.h"
#include "burer_attack_misc.h"
#include "burer.h"
#include "../ai_monster_utils.h"

#define DIST_QUANT 10.f

CBurerAttackRunAround::CBurerAttackRunAround(CBurer *p)
{
	pMonster = p;
}

void CBurerAttackRunAround::Init()
{
	inherited::Init();

	enemy = pMonster->EnemyMan.get_enemy();	

	time_started = Level().timeServer();

	dest_direction.set(0.f,0.f,0.f);

	// select point
	
	Fvector dir_to_enemy;
	dir_to_enemy.sub(enemy->Position(),pMonster->Position());
	dir_to_enemy.normalize();
	
	Fvector dir_from_enemy;
	dir_from_enemy.sub(pMonster->Position(),enemy->Position());
	dir_from_enemy.normalize();

	float dist = pMonster->Position().distance_to(enemy->Position());
	

	if (dist > 30.f) {								// бежать к врагу
		selected_point.mad(pMonster->Position(),dir_to_enemy,DIST_QUANT);
	} else if ((dist < 20.f) && (dist > 6.f)) {	// убегать от врага
		selected_point.mad(pMonster->Position(),dir_from_enemy,DIST_QUANT);
		dest_direction.set(dir_to_enemy);
	} else {											// выбрать случайную позицию
		selected_point = random_position(pMonster->Position(), DIST_QUANT);
		dest_direction.set(dir_to_enemy);
	}
	pMonster->CMonsterMovement::disable_path();
	
	pMonster->CMonsterMovement::initialize_movement();
}


void CBurerAttackRunAround::Run()
{
	if (!fis_zero(dest_direction.square_magnitude())) {
		pMonster->CMonsterMovement::set_use_dest_orient		(true);
		pMonster->CMonsterMovement::set_dest_direction	(dest_direction);
	}

	pMonster->MotionMan.m_tAction = ACT_RUN;
	pMonster->CMonsterMovement::set_target_point		(selected_point);
	pMonster->CMonsterMovement::set_generic_parameters	();

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
	if ((time_started + 3500 < Level().timeServer()) || 
		(pMonster->IsMovingOnPath() && pMonster->IsPathEnd(2.f))) {

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
	if (angle_difference(pMonster->m_body.current.yaw,pMonster->m_body.target.yaw) < deg(5)) {
		return true;
	}
	return false;
}

