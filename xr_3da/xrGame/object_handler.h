////////////////////////////////////////////////////////////////////////////
//	Module 		: object_handler.h
//	Created 	: 02.10.2001
//  Modified 	: 16.01.2004
//	Author		: Dmitriy Iassenev
//	Description : Object handler
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ai_monster_space.h"
#include "path_manager_level_selector.h"
#include "state_manager_state.h"

class CObjectHandler : public CStateManagerState<CObjectHandler> {
	typedef CStateManagerState<CObjectHandler> inherited;
protected:
	bool						m_bHammerIsClutched;

private:
	// firing
	bool						m_bFiring;
	u32							m_dwNoFireTime;
	u32							m_dwStartFireTime;
	bool						m_bCanFire;

	// fire  constants
	u32							m_dwFireRandomMin;
	u32							m_dwFireRandomMax;
	u32							m_dwNoFireTimeMin;
	u32							m_dwNoFireTimeMax;
	float						m_fMinMissDistance;
	float						m_fMinMissFactor;
	float						m_fMaxMissDistance;
	float						m_fMaxMissFactor;
	MonsterSpace::EObjectAction	m_tWeaponState;


public:
					CObjectHandler			();
	virtual			~CObjectHandler			();
			void	Init					();
	virtual	void	Load					(LPCSTR section);
	virtual	void	reinit					();
			void	update					(const MonsterSpace::EObjectAction &weapon_state);
	virtual	bool	check_if_can_kill_enemy	(const SRotation &orientation) const;
	virtual	bool	check_if_can_kill_member() const;
	virtual	bool	check_if_can_kill_target(const CEntity *tpEntity, const Fvector &target_pos, float yaw2, float pitch2, float fSafetyAngle = FIRE_SAFETY_ANGLE) const;
	IC		bool	firing					() const;
};

#include "object_handler_inline.h"