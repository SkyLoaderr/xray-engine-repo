////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_biting_misc.cpp
//	Created 	: 26.05.2003
//  Modified 	: 26.05.2003
//	Author		: Serge Zhem
//	Description : Miscellanious functions for all the biting monsters
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_biting.h"
#include "../../actor.h"
#include "../../hudmanager.h"

#include "../ai_monster_jump.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////
// ������� ����������� 
// ������, ����, ����������� ������, ���������� ����������
void CAI_Biting::vfUpdateParameters()
{
	UpdateMemory();

	//------------------------------------
	// ����
	A = B = false;
	
	SoundElem se;
	
	if (IsRememberSound()) {
		GetSound(se,A);
		B = !A;
	}

	J = A | B;

	//------------------------------------
	// ������
	// ����������, ����� �� ���� ����
	I = false;

	SEnemy ve;

	if (GetEnemy(ve)) {
		const CEntityAlive *pEA = dynamic_cast<const CEntityAlive*>(ve.obj);
		if (CVisualMemoryManager::see(pEA, this)) I = true;
	}

	//------------------------------------
	// ����������� ������
	C = D = E = F = G	= false;
	
	if (ve.obj) {
		switch (dwfChooseAction(0,m_fAttackSuccessProbability[0],m_fAttackSuccessProbability[1],m_fAttackSuccessProbability[2],m_fAttackSuccessProbability[3],g_Team(),g_Squad(),g_Group(),0,1,2,3,4,this,30.f)) {
			case 4 : 	C = true;	break;
			case 3 : 	D = true;	break;
			case 2 : 	E = true;	break;
			case 1 : 
			case 0 : 	F = true;	break;
		}
	}
	K					= C | D | E | F;
	
	// K ������ ���� true, ������ ���� ���������� ve.obj
	R_ASSERT(ve.obj || !K);

	//------------------------------------
	// ���� ������� ?
	H = true;

	// Fill flags, properties and vars for attack mode
	flagsEnemy	= 0;
	
#pragma todo("enemy hiding & enemy runaway flags")

	// Set current enemy
	m_tEnemy					= ve;
	
	if (m_tEnemy.obj && (m_tEnemyPrevFrame.obj == m_tEnemy.obj) && (m_tEnemy.time != m_current_update)) {
		flagsEnemy |= FLAG_ENEMY_LOST_SIGHT;		// ���� ������� �� ����
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

		if (((flagsEnemy & FLAG_ENEMY_STANDING) == FLAG_ENEMY_STANDING)  && !I) flagsEnemy |= FLAG_ENEMY_DOESNT_KNOW_ABOUT_ME;
	}
	
	if (m_tEnemy.obj && !I) flagsEnemy |= FLAG_ENEMY_DOESNT_SEE_ME;

	if (!m_tEnemy.obj && m_tEnemyPrevFrame.obj && m_tEnemyPrevFrame.obj->getDestroy()) flagsEnemy |= FLAG_ENEMY_GO_OFFLINE;

	// Save current enemy (only if valid)
	if (m_tEnemy.obj)
		m_tEnemyPrevFrame = m_tEnemy;

	// Setup is own additional flags
	m_bDamaged = ((GetHealth() < _sd->m_fDamagedThreshold) ? true : false);
	
}

