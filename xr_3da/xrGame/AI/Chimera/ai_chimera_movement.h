#pragma once

#include "../../movement_manager.h"
#include "../ai_monster_share.h"


struct _chimera_movement_shared : public CSharedResource {
	float	m_fsTurnNormalAngular;
	float	m_fsWalkFwdNormal;
	float	m_fsWalkFwdDamaged;
	float	m_fsWalkBkwdNormal;
	float	m_fsWalkAngular;
	float	m_fsRunFwdNormal;
	float	m_fsRunFwdDamaged;
	float	m_fsRunAngular;
	float	m_fsDrag;
	float	m_fsSteal;

};



class CChimeraMovementManager : virtual	public CMovementManager,
								public CSharedClass<_chimera_movement_shared> {
		
		typedef CMovementManager inherited;
		typedef CSharedClass<_chimera_movement_shared> inherited_shared;
public:
	enum EMovementParameters {
		eVelocityParameterStand			= u32(1) <<  0,
		eVelocityParameterWalk			= u32(1) <<  1,
		eVelocityParameterRun			= u32(1) <<  2,
		
		eVelocityParameterDamaged		= u32(1) <<  3,
	
		eVelocityParameterStandDamaged	= eVelocityParameterStand	| eVelocityParameterDamaged,
		eVelocityParameterWalkDamaged	= eVelocityParameterWalk	| eVelocityParameterDamaged,
		eVelocityParameterRunDamaged	= eVelocityParameterRun		| eVelocityParameterDamaged,

		eVelocityParamsStand			= eVelocityParameterStand, 
		eVelocityParamsWalk				= eVelocityParameterStand			| eVelocityParameterWalk,
		eVelocityParamsRun				= eVelocityParameterStand			| eVelocityParameterWalk			| eVelocityParameterRun,
		eVelocityParamsWalkDamaged		= eVelocityParameterStandDamaged	| eVelocityParameterWalkDamaged,
		eVelocityParamsRunDamaged		= eVelocityParamsWalkDamaged		| eVelocityParameterRunDamaged,
	};

public:
					CChimeraMovementManager	();
	virtual			~CChimeraMovementManager();
	
			void	Init					();
	virtual	void	Load					(LPCSTR section);
	virtual	void	reinit					();
	virtual	void	reload					(LPCSTR section);

	virtual void	load_shared				(LPCSTR section);

			void	update					();
};

