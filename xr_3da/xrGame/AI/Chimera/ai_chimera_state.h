#pragma once 

#include "..\\ai_monster_state.h"

class CAI_Chimera;


///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CChimeraAttack class
///////////////////////////////////////////////////////////////////////////////////////////////////////////

class CChimeraAttack : public IState {
	
	CAI_Chimera	*pMonster;

	enum {
		ACTION_RUN,
		ACTION_ATTACK_MELEE,
		ACTION_FIND_ENEMY
	} m_tAction;

	VisionElem		m_tEnemy;

	float			m_fDistMin;						//!< минимально допустимое расстояния для аттаки
	float			m_fDistMax;						//!< максимально допустимое расстояние для аттаки

	TTime			m_dwFaceEnemyLastTime;
	TTime			m_dwFaceEnemyLastTimeInterval;

	// регулировка приближением во время атаки
	u32				nStartStop;						//!< количество	старт-стопов

	TTime			m_dwSuperMeleeStarted;
public:
	CChimeraAttack(CAI_Chimera *p);

	void Reset();
	bool CheckCompletion();

private:
	void Init();
	void Run();
	void Replanning();
};
