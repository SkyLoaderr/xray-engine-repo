#pragma once
#include "../ai_monster_movement.h"

class CPoltergeist;

class CPoltergeisMovementManager : public CMonsterMovement {
	typedef CMonsterMovement inherited;

	CPoltergeist	*m_monster;

public:
					CPoltergeisMovementManager	(CPoltergeist *monster) : inherited((CBaseMonster*)monster), m_monster(monster) {}
	virtual			~CPoltergeisMovementManager	(){}

	virtual	void	move_along_path				(CPHMovementControl *movement_control, Fvector &dest_position, float time_delta);

			Fvector	CalculateRealPosition		();
};

