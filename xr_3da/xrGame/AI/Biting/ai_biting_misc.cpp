////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_biting_misc.cpp
//	Created 	: 26.05.2003
//  Modified 	: 26.05.2003
//	Author		: Serge Zhem
//	Description : Miscellanious functions for all the biting monsters
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_biting.h"
#include "..\\..\\actor.h"
#include "..\\..\\hudmanager.h"

// A - я слышу опасный звук
// B - я слышу неопасный звук
// С - я вижу очень опасного врага
// D - я вижу опасного врага
// E - я вижу равного врага
// F - я вижу слабого врага
// H - враг выгодный
// I - враг видит меня
// J - A | B
// K - C | D | E | F 


////////////////////////////////////////////////////////////////////////////////////////////////////////
// Входные воздействия 
// Зрение, слух, вероятность победы, выгодность противника
void CAI_Biting::vfUpdateParameters()
{
	UpdateMemory();

	//------------------------------------
	// слух
	A = B = false;
	
	SoundElem se;
	
	if (IsRememberSound()) {
		GetSound(se,A);
		B = !A;
	}


	J = A | B;

	//------------------------------------
	// Зрение
	objVisible			&VisibleEnemies = Level().Teams[g_Team()].Squads[g_Squad()].KnownEnemys;

	Msg(" Visible Enemies = [%i]", VisibleEnemies.size());
	
	// определить, видит ли меня враг
	I = false;

	SEnemy ve;
	if (GetEnemy(ve)) {
		VisibleEnemies.insert(ve.obj);

		// определить, видит ли меня враг
		float			yaw1 = 0.f, pitch1 =0.f, yaw2, pitch2, fYawFov = 0.f, fPitchFov = 0.f, fRange = 0.f;
		

		CCustomMonster	*tpCustomMonster = dynamic_cast<CCustomMonster *>(ve.obj);
		if (tpCustomMonster) {
			yaw1		= -tpCustomMonster->r_current.yaw;
			pitch1		= -tpCustomMonster->r_current.pitch;
			fYawFov		= angle_normalize_signed(tpCustomMonster->ffGetFov()*PI/180.f);
			fRange		= tpCustomMonster->ffGetRange();
		}
		else {
			CActor		*tpActor = dynamic_cast<CActor *>(ve.obj);
			if (tpActor) {
				yaw1	= tpActor->Orientation().yaw;
				pitch1	= tpActor->Orientation().pitch;
				fYawFov	= angle_normalize_signed(tpActor->ffGetFov()*PI/180.f);
				fRange	= tpActor->ffGetRange();
			}
		}

		if (ve.position.distance_to(Position()) <= fRange) {
			
			fYawFov			= angle_normalize_signed((_abs(fYawFov) + _abs(atanf(1.f/ve.position.distance_to(Position()))))/2.f);
			fPitchFov		= angle_normalize_signed(fYawFov*1.f);
			ve.position.sub	(Position());
			ve.position.mul	(-1);
			ve.position.getHP	(yaw2,pitch2);
			yaw1			= angle_normalize_signed(yaw1);
			pitch1			= angle_normalize_signed(pitch1);
			yaw2			= angle_normalize_signed(yaw2);
			pitch2			= angle_normalize_signed(pitch2);
			
			I = getAI().bfTooSmallAngle(yaw1,yaw2,fYawFov) && (getAI().bfTooSmallAngle(pitch1,pitch2,fPitchFov));
		}
	}


	//------------------------------------
	// вероятность победы
	C = D = E = F = G	= false;

	if (bfIsAnyAlive(VisibleEnemies)) {
		switch (dwfChooseAction(0,m_fAttackSuccessProbability[0],m_fAttackSuccessProbability[1],m_fAttackSuccessProbability[2],m_fAttackSuccessProbability[3],g_Team(),g_Squad(),g_Group(),0,1,2,3,4,this,30.f)) {
			case 4 : 
				C = true;
				break;
			case 3 : 
				D = true;
				break;
			case 2 : 
				E = true;
				break;
			case 1 : 
			case 0 : 
				F = true;
				break;
		}
	}
	K					= C | D | E | F;

	// K должно быть true, только если корректный ve.obj
	R_ASSERT(ve.obj || !K);

	//------------------------------------
	// враг выгоден ?
	H = true;

	// Fill flags, properties and vars for attack mode
	flagsEnemy	= 0;
	
#pragma todo("enemy hiding & enemy runaway flags")

	// Set current enemy
	m_tEnemy					= ve;
	
	if (m_tEnemy.obj && (m_tEnemyPrevFrame.obj == m_tEnemy.obj) && (m_tEnemy.time != m_dwCurrentUpdate)) {
		flagsEnemy |= FLAG_ENEMY_LOST_SIGHT;		// враг потерян из вида
	}
	
	if (m_tEnemyPrevFrame.obj && !m_tEnemyPrevFrame.obj->g_Alive()) {
		flagsEnemy |= FLAG_ENEMY_DIE;
	}
	
	float dist_now, dist_prev;
	if (m_tEnemy.obj && (m_tEnemyPrevFrame.obj == m_tEnemy.obj)) {
		dist_now	= m_tEnemy.position.distance_to(Position());
		dist_prev	= m_tEnemyPrevFrame.position.distance_to(Position());
		
		if (_abs(dist_now - dist_prev) < 0.2f) flagsEnemy |= FLAG_ENEMY_STANDING;
		else {
			if (dist_now < dist_prev) flagsEnemy |= FLAG_ENEMY_GO_CLOSER;
			else flagsEnemy |= FLAG_ENEMY_GO_FARTHER;

			if (_abs(dist_now - dist_prev) < 1.2f) {
				if (dist_now < dist_prev)  flagsEnemy |= FLAG_ENEMY_GO_CLOSER_FAST;
				else flagsEnemy |= FLAG_ENEMY_GO_FARTHER_FAST;
			}
		}

		if (((flagsEnemy & FLAG_ENEMY_STANDING) == FLAG_ENEMY_STANDING)  && !I) flagsEnemy |= FLAG_ENEMY_DOESN_KNOW_ABOUT_ME;
	}
	
	if (!m_tEnemy.obj && m_tEnemyPrevFrame.obj && m_tEnemyPrevFrame.obj->getDestroy()) flagsEnemy |= FLAG_ENEMY_GO_OFFLINE;

	// Save current enemy (only if valid)
	if (m_tEnemy.obj)
		m_tEnemyPrevFrame = m_tEnemy;
}

////////////////////////////////////////////////////////////////////////////
//void TurnLeft(Angle);
//void TurnRight(Angle);
//
//CAnimManager::AddAnim	(eAnimStandIdle,		"stand_idle_",			-1, PI_DIV_3, PI_DIV_6);
//CAnimManager::AddAnim	(eAnimStandTurnLeft,	"stand_turn_left_",		-1, PI_DIV_3, PI_DIV_6);
//CAnimManager::AddAnim	(eAnimStandTurnRight,	"stand_turn_right_",	-1, PI_DIV_3, PI_DIV_6);
//CAnimManager::AddAnim	(eAnimLieIdle,			"stand_turn_right_",	-1, PI_DIV_3, PI_DIV_6);
//CAnimManager::AddAnim	(eAnimWalkFwd,			"stand_turn_right_",	-1, PI_DIV_3, PI_DIV_6);
//CAnimManager::AddAnim	(eAnimWalkTurnLeft,		"stand_turn_right_",	-1, PI_DIV_3, PI_DIV_6);
//CAnimManager::AddAnim	(eAnimWalkTurnRight,	"stand_turn_right_",	-1, PI_DIV_3, PI_DIV_6);
//CAnimManager::AddAnim	(eAnimRun,				"stand_turn_right_",	-1, PI_DIV_3, PI_DIV_6);
//CAnimManager::AddAnim	(eAnimCheckCorpse,		"stand_idle_",			 3, PI_DIV_3, PI_DIV_6);
//CAnimManager::AddAnim	(eAnimEat,				"lie_eat_",				-1, PI_DIV_3, PI_DIV_6);


//// the order is very important!!!  add motions according to EAction enum
//CMotionManager::AddMotion	(eAnimStandIdle, eAnimStandTurnLeft, eAnimStandTurnRight, PI_DIV_6);
//CMotionManager::AddMotion	(eAnimSitIdle);
//CMotionManager::AddMotion	(eAnimLieIdle);
//CMotionManager::AddMotion	(eAnimWalkFwd);
//


void CAI_Biting::ProcessAction()
{

	// преобразовать Action в Motion и получить новую анимацию
	SMotionItem MI = MotionMan.m_tMotions[m_tAction];
	AnimMan.cur_anim = MI.anim;
	

	// если новая анимация не совпадает с предыдущей, проверить переход
	if (AnimMan.prev_anim != AnimMan.cur_anim) {
		if (AnimMan.CheckTransition	(AnimMan.prev_anim, AnimMan.cur_anim)) return;
	}

	// Check special flags and replacing animations
	// pMan->CheckSpecFlags();

	if (MI.is_turn_params)
		// проверить необходимость поворота
		if (!getAI().bfTooSmallAngle(r_torso_target.yaw, r_torso_current.yaw, MI.turn.min_angle)) {
			// повернуться
			// необходим поворот влево или вправо
			if (angle_normalize_signed(r_torso_target.yaw - r_torso_current.yaw) > 0) {
				// вправо
				AnimMan.cur_anim = MI.turn.anim_right;
			} else {
				// влево
				AnimMan.cur_anim = MI.turn.anim_left;
			}
		}

	AnimMan.ApplyParams(this);					// ?
}





