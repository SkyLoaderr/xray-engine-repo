////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_movement_manager.h
//	Created 	: 27.12.2003
//  Modified 	: 27.12.2003
//	Author		: Dmitriy Iassenev
//	Description : Stalker velocity manager
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "movement_manager.h"
#include "ai_monster_space.h"

using namespace MonsterSpace;

class CStalkerMovementManager : virtual public CMovementManager {
	typedef CMovementManager inherited;
public:
	enum EMovementParameters {
		eMovementParameterStanding								= u32(1) <<  0,
		eMovementParameterWalk									= u32(1) <<  1,
		eMovementParameterRun									= u32(1) <<  2,

		eMovementParameterStand									= u32(1) <<  3,
		eMovementParameterCrouch								= u32(1) <<  4,

		eMovementParameterDamaged								= u32(1) <<  5,

		eMovementParameterDanger								= u32(1) <<  6,
		eMovementParameterFree									= u32(1) <<  7,
		eMovementParameterPanic									= u32(1) <<  8,

		eMovementParameterPositiveVelocity						= u32(1) << 31,
		eMovementParameterNegativeVelocity						= u32(1) << 30,

		eMovementParameterStandingFreeStand						= eMovementParameterStanding | eMovementParameterFree | eMovementParameterStand,
		eMovementParameterStandingPanicStand					= eMovementParameterStanding | eMovementParameterPanic | eMovementParameterStand,
		eMovementParameterStandingDangerStand					= eMovementParameterStanding | eMovementParameterDanger | eMovementParameterStand,
		eMovementParameterStandingFreeStandDamaged				= eMovementParameterStandingFreeStand | eMovementParameterDamaged,
		eMovementParameterStandingPanicStandDamaged				= eMovementParameterStandingPanicStand | eMovementParameterDamaged,
		eMovementParameterStandingDangerStandDamaged			= eMovementParameterStandingDangerStand | eMovementParameterDamaged,

		eMovementParameterStandingFreeCrouch					= eMovementParameterStanding | eMovementParameterFree | eMovementParameterCrouch,
		eMovementParameterStandingPanicCrouch					= eMovementParameterStanding | eMovementParameterPanic | eMovementParameterCrouch,
		eMovementParameterStandingDangerCrouch					= eMovementParameterStanding | eMovementParameterDanger | eMovementParameterCrouch,
		eMovementParameterStandingFreeCrouchDamaged				= eMovementParameterStandingFreeCrouch | eMovementParameterDamaged,
		eMovementParameterStandingPanicCrouchDamaged			= eMovementParameterStandingPanicCrouch | eMovementParameterDamaged,
		eMovementParameterStandingDangerCrouchDamaged			= eMovementParameterStandingDangerCrouch | eMovementParameterDamaged,

		eMovementParameterWalkFree								= eMovementParameterWalk | eMovementParameterFree | eMovementParameterStand,
		eMovementParameterWalkDangerStand						= eMovementParameterWalk | eMovementParameterDanger | eMovementParameterStand,
		eMovementParameterWalkDangerCrouch						= eMovementParameterWalk | eMovementParameterDanger | eMovementParameterCrouch,
		eMovementParameterRunFree								= eMovementParameterRun | eMovementParameterFree | eMovementParameterStand,
		eMovementParameterRunDangerStand						= eMovementParameterRun | eMovementParameterDanger | eMovementParameterStand,
		eMovementParameterRunDangerCrouch						= eMovementParameterRun | eMovementParameterDanger | eMovementParameterCrouch,
		eMovementParameterRunPanicStand							= eMovementParameterRun | eMovementParameterPanic | eMovementParameterStand,

		eMovementParameterWalkFreeDamaged						= eMovementParameterWalkFree | eMovementParameterDamaged,
		eMovementParameterWalkDangerStandDamaged				= eMovementParameterWalkDangerStand | eMovementParameterDamaged,
		eMovementParameterWalkDangerCrouchDamaged				= eMovementParameterWalkDangerCrouch | eMovementParameterDamaged,
		eMovementParameterRunFreeDamaged						= eMovementParameterRunFree | eMovementParameterDamaged,
		eMovementParameterRunDangerStandDamaged					= eMovementParameterRunDangerStand | eMovementParameterDamaged,
		eMovementParameterRunDangerCrouchDamaged				= eMovementParameterRunDangerCrouch | eMovementParameterDamaged,
		eMovementParameterRunPanicDamagedStand					= eMovementParameterRunPanicStand | eMovementParameterDamaged,

		eMovementParameterStandingFreeCrouchPositive			= eMovementParameterStandingFreeCrouch			| eMovementParameterPositiveVelocity,
		eMovementParameterStandingPanicCrouchPositive			= eMovementParameterStandingPanicCrouch			| eMovementParameterPositiveVelocity,
		eMovementParameterStandingDangerCrouchPositive			= eMovementParameterStandingDangerCrouch		| eMovementParameterPositiveVelocity,
		eMovementParameterStandingFreeCrouchDamagedPositive		= eMovementParameterStandingFreeCrouchDamaged	| eMovementParameterPositiveVelocity,
		eMovementParameterStandingPanicCrouchDamagedPositive	= eMovementParameterStandingPanicCrouchDamaged	| eMovementParameterPositiveVelocity,
		eMovementParameterStandingDangerCrouchDamagedPositive	= eMovementParameterStandingDangerCrouchDamaged	| eMovementParameterPositiveVelocity,

		eMovementParameterWalkFreePositive						= eMovementParameterWalkFree					| eMovementParameterPositiveVelocity,
		eMovementParameterWalkDangerStandPositive				= eMovementParameterWalkDangerStand				| eMovementParameterPositiveVelocity,
		eMovementParameterWalkDangerCrouchPositive				= eMovementParameterWalkDangerCrouch			| eMovementParameterPositiveVelocity,
		eMovementParameterRunFreePositive						= eMovementParameterRunFree						| eMovementParameterPositiveVelocity,
		eMovementParameterRunDangerStandPositive				= eMovementParameterRunDangerStand				| eMovementParameterPositiveVelocity,
		eMovementParameterRunDangerCrouchPositive				= eMovementParameterRunDangerCrouch				| eMovementParameterPositiveVelocity,
		eMovementParameterRunPanicStandPositive					= eMovementParameterRunPanicStand				| eMovementParameterPositiveVelocity,

		eMovementParameterWalkFreeDamagedPositive				= eMovementParameterWalkFreeDamaged				| eMovementParameterPositiveVelocity,
		eMovementParameterWalkDangerStandDamagedPositive		= eMovementParameterWalkDangerStandDamaged		| eMovementParameterPositiveVelocity,
		eMovementParameterWalkDangerCrouchDamagedPositive		= eMovementParameterWalkDangerCrouchDamaged		| eMovementParameterPositiveVelocity,
		eMovementParameterRunFreeDamagedPositive				= eMovementParameterRunFreeDamaged				| eMovementParameterPositiveVelocity,
		eMovementParameterRunDangerStandDamagedPositive			= eMovementParameterRunDangerStandDamaged		| eMovementParameterPositiveVelocity,
		eMovementParameterRunDangerCrouchDamagedPositive		= eMovementParameterRunDangerCrouchDamaged		| eMovementParameterPositiveVelocity,
		eMovementParameterRunPanicDamagedStandPositive			= eMovementParameterRunPanicDamagedStand		| eMovementParameterPositiveVelocity,

		eMovementParameterStandingFreeCrouchNegative			= eMovementParameterStandingFreeCrouch			| eMovementParameterNegativeVelocity,
		eMovementParameterStandingPanicCrouchNegative			= eMovementParameterStandingPanicCrouch			| eMovementParameterNegativeVelocity,
		eMovementParameterStandingDangerCrouchNegative			= eMovementParameterStandingDangerCrouch		| eMovementParameterNegativeVelocity,
		eMovementParameterStandingFreeCrouchDamagedNegative		= eMovementParameterStandingFreeCrouchDamaged	| eMovementParameterNegativeVelocity,
		eMovementParameterStandingPanicCrouchDamagedNegative	= eMovementParameterStandingPanicCrouchDamaged	| eMovementParameterNegativeVelocity,
		eMovementParameterStandingDangerCrouchDamagedNegative	= eMovementParameterStandingDangerCrouchDamaged	| eMovementParameterNegativeVelocity,

		eMovementParameterWalkFreeNegative						= eMovementParameterWalkFree					| eMovementParameterNegativeVelocity,
		eMovementParameterWalkDangerStandNegative				= eMovementParameterWalkDangerStand				| eMovementParameterNegativeVelocity,
		eMovementParameterWalkDangerCrouchNegative				= eMovementParameterWalkDangerCrouch			| eMovementParameterNegativeVelocity,
		eMovementParameterRunFreeNegative						= eMovementParameterRunFree						| eMovementParameterNegativeVelocity,
		eMovementParameterRunDangerStandNegative				= eMovementParameterRunDangerStand				| eMovementParameterNegativeVelocity,
		eMovementParameterRunDangerCrouchNegative				= eMovementParameterRunDangerCrouch				| eMovementParameterNegativeVelocity,
		eMovementParameterRunPanicStandNegative					= eMovementParameterRunPanicStand				| eMovementParameterNegativeVelocity,

		eMovementParameterWalkFreeDamagedNegative				= eMovementParameterWalkFreeDamaged				| eMovementParameterNegativeVelocity,
		eMovementParameterWalkDangerStandDamagedNegative		= eMovementParameterWalkDangerStandDamaged		| eMovementParameterNegativeVelocity,
		eMovementParameterWalkDangerCrouchDamagedNegative		= eMovementParameterWalkDangerCrouchDamaged		| eMovementParameterNegativeVelocity,
		eMovementParameterRunFreeDamagedNegative				= eMovementParameterRunFreeDamaged				| eMovementParameterNegativeVelocity,
		eMovementParameterRunDangerStandDamagedNegative			= eMovementParameterRunDangerStandDamaged		| eMovementParameterNegativeVelocity,
		eMovementParameterRunDangerCrouchDamagedNegative		= eMovementParameterRunDangerCrouchDamaged		| eMovementParameterNegativeVelocity,
		eMovementParameterRunPanicDamagedStandNegative			= eMovementParameterRunPanicDamagedStand		| eMovementParameterNegativeVelocity,
	};

protected:
	float								m_fCrouchFactor;
	float								m_fWalkFactor;
	float								m_fRunFactor;
	float								m_fWalkFreeFactor;
	float								m_fRunFreeFactor;
	float								m_fPanicFactor;
	float								m_fDamagedWalkFactor;
	float								m_fDamagedRunFactor;
	float								m_fDamagedWalkFreeFactor;
	float								m_fDamagedRunFreeFactor;
	float								m_fDamagedPanicFactor;

protected:
	Fvector								m_desired_position;
	Fvector								m_desired_direction;
	EBodyState							m_body_state;
	EMovementType						m_movement_type;
	EMentalState						m_mental_state;
	EPathType							m_path_type;
	EDetailPathType						m_detail_path_type;
	CAbstractVertexEvaluator			*m_node_evaluator;
	CAbstractVertexEvaluator			*m_path_evaluator;
	bool								m_use_desired_position;
	bool								m_use_desired_direction;

public:
	MonsterSpace::SBoneRotation			m_head;

public:
													CStalkerMovementManager	();
	virtual											~CStalkerMovementManager();
			void									Init					();
	virtual	void									Load					(LPCSTR section);
	virtual	void									reinit					();
	virtual	void									reload					(LPCSTR section);
	virtual	void									update					(u32 time_delta);
	IC		void									set_head_orientation	(const MonsterSpace::SBoneRotation &orientation);
	IC		const MonsterSpace::SBoneRotation		&head_orientation		() const;
	IC		const MonsterSpace::EMovementType		movement_type			() const;
	IC		const MonsterSpace::EMentalState		mental_state			() const;
	IC		const MonsterSpace::EBodyState			body_state				() const;
	IC		void									set_desired_position	(const Fvector *desired_position);
	IC		void									set_desired_direction	(const Fvector *desired_direction);
	IC		void									set_body_state			(EBodyState body_state);
	IC		void									set_movement_type		(EMovementType movement_type);
	IC		void									set_mental_state		(EMentalState mental_state);
	IC		void									set_path_type			(EPathType path_type);
	IC		void									set_detail_path_type	(EDetailPathType detail_path_type);
	IC		void									set_node_evaluator		(CAbstractVertexEvaluator *node_evaluator);
	IC		void									set_path_evaluator		(CAbstractVertexEvaluator *path_evaluator);
};

#include "stalker_movement_manager_inline.h"