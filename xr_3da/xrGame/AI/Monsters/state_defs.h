#pragma once

enum EMonsterState {
	eGlobalState					= u32(1) << 15,

	// -------------------------------------------------------------
	
	eStateRest						= eGlobalState << 1,
	
	eStateRest_WalkGraphPoint		= eStateRest | 1,
	eStateRest_Idle					= eStateRest | 2,
	eStateRest_Fun					= eStateRest | 3,
	eStateRest_Sleep				= eStateRest | 4,

	// -------------------------------------------------------------

	eStateEat						= eGlobalState << 2,
	
	eStateEat_CorpseApproachRun		= eStateEat | 1,
	eStateEat_CorpseApproachWalk	= eStateEat | 2,
	eStateEat_CheckCorpse			= eStateEat | 3,
	eStateEat_Eat					= eStateEat | 4,
	eStateEat_WalkAway				= eStateEat | 5,
	eStateEat_Rest					= eStateEat | 6,

	// -------------------------------------------------------------

	eStateAttack					= eGlobalState << 3,
	
	eStateAttack_Run				= eStateAttack | 1,
	eStateAttack_Melee				= eStateAttack | 2,
	eStateAttack_RunAttack			= eStateAttack | 3,
	eStateAttack_RunAway			= eStateAttack | 4,
	eStateAttack_FindEnemy			= eStateAttack | 5,
	eStateAttack_Steal				= eStateAttack | 6,

	eStateAttack_AttackHidden		= eStateAttack | 7,
	eStateAttack_AttackRat			= eStateAttack | 8,

	// -------------------------------------------------------------

	eStatePanic						= eGlobalState << 4,
	
	eStatePanic_Run					= eStatePanic | 1,
	eStatePanic_FaceUnprotectedArea = eStatePanic | 2,
	
	// -------------------------------------------------------------

	eStateHitted					= eGlobalState << 5,
	
	eStateHitted_Hide				= eStateHitted | 1,
	eStateHitted_MoveOut			= eStateHitted | 2,

	// -------------------------------------------------------------

	eStateHearDangerousSound		= eGlobalState << 6,

	eStateHearDangerousSound_Hide			= eStateHearDangerousSound | 1,
	eStateHearDangerousSound_FaceOpenPlace	= eStateHearDangerousSound | 2,
	eStateHearDangerousSound_StandScared	= eStateHearDangerousSound | 3,
	
	// -------------------------------------------------------------

	eStateHearInterestingSound		= eGlobalState << 7,

	eStateHearInterestingSound_MoveToDest	= eStateHearInterestingSound | 1,
	eStateHearInterestingSound_LookAround	= eStateHearInterestingSound | 2,

	// -------------------------------------------------------------

	eStateControlled				= eGlobalState << 8,
		
	eStateControlled_Follow			= eStateControlled | 1,
	eStateControlled_Attack			= eStateControlled | 2,

	eStateControlled_Follow_Wait			= eStateControlled | 3,
	eStateControlled_Follow_WalkToObject	= eStateControlled | 4,


	// -------------------------------------------------------------
	
	eStateThreaten					= eGlobalState << 9,
	
	// -------------------------------------------------------------

	eStateFindEnemy					= eGlobalState << 10,

	eStateFindEnemy_Run				= eStateFindEnemy | 1,
	eStateFindEnemy_LookAround		= eStateFindEnemy | 2,
	eStateFindEnemy_Angry			= eStateFindEnemy | 3,
	eStateFindEnemy_WalkAround		= eStateFindEnemy | 4,

	eStateFindEnemy_LookAround_MoveToPoint	= eStateFindEnemy | 5,
	eStateFindEnemy_LookAround_LookAround	= eStateFindEnemy | 6,
	eStateFindEnemy_LookAround_TurnToPoint	= eStateFindEnemy | 7,

	// -------------------------------------------------------------

	eStateSquad						= eGlobalState << 11,
	
	eStateSquad_Rest				= eStateSquad | 1,
	eStateSquad_RestFollow			= eStateSquad | 2,
	
	eStateSquad_Rest_Idle				= eStateSquad | 3,
	eStateSquad_Rest_WalkAroundLeader	= eStateSquad | 4,

	eStateSquad_RestFollow_Idle			= eStateSquad | 5,
	eStateSquad_RestFollow_WalkToPoint	= eStateSquad | 6,


	// -------------------------------------------------------------

	eStateCustom					= eGlobalState << 15,

	eStateCustom_Vampire			= eStateCustom | eStateAttack | 1,

	eStateUnknown					= u32(-1),
};

#define is_state(state, type) ((state & type) == type)
