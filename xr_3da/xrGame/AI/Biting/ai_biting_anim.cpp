////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_biting_anim.cpp
//	Created 	: 22.05.2003
//  Modified 	: 22.05.2003
//	Author		: Serge Zhem
//	Description : Animations, Bone transformations and Sounds for monsters of biting class 
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "ai_biting.h"
#include "ai_biting_space.h"

using namespace AI_Biting;

namespace AI_Biting {
	LPCSTR caStateNames			[] = {
		"stand_",
		"sit_",
		"lie_",
		0
	};

	LPCSTR caGlobalNames		[] = {
		"idle_",
		"idle_ls_",
		"walk_fwd_",
		"walk_bkwd_",
		"walk_ls_",
		"walk_rs_",
		"run_",
		"run_ls_",
		"run_rs_",
		"attack_",
		"attack2_",
		"attack_ls_",
		"eat_",
		"damage_",
		"scared_",
		"die_",		
		"lie_down_",
		"stand_up_",
		0
	};
};

static void __stdcall vfPlayCallBack(CBlend* B)
{
	CAI_Biting *tpBiting = (CAI_Biting*)B->CallbackParam;
	
	tpBiting->m_tpCurrentGlobalAnimation = 0;
	if (tpBiting->_CA.Playing)tpBiting->_CA.Finished = true;
}

void CAI_Biting::SelectAnimation(const Fvector &_view, const Fvector &_move, float speed )
{
	if (bShowDeath)	{
		m_tpCurrentGlobalAnimation = m_tAnims.A[ePostureStand].A[eActionDie].A[::Random.randI((int)m_tAnims.A[ePostureStand].A[eActionDie].A.size())];
		PKinematics(pVisual)->PlayCycle(m_tpCurrentGlobalAnimation,TRUE,vfPlayCallBack,this);
		bShowDeath  = false;
		return;
	}
	
	if (g_Alive())
		if (!m_tpCurrentGlobalAnimation && m_bActionFinished) {
			m_tpCurrentGlobalAnimation = m_tAnims.A[m_tPostureAnim].A[m_tActionAnim].A[::Random.randI((int)m_tAnims.A[m_tPostureAnim].A[m_tActionAnim].A.size())];
			PKinematics(pVisual)->PlayCycle(m_tpCurrentGlobalAnimation,TRUE,vfPlayCallBack,this);
		}

}

// выбор анимации в соответствии с текущим состоянием объекта
void CAI_Biting::vfSetAnimation(bool bForceChange)
{
	EPostureAnim PostureAnim_old = m_tPostureAnim;
	EActionAnim ActionAnim_old = m_tActionAnim;

	// тип положения тела
	switch (m_tBodyState) {
		case eBodyStateStand : 	// стоит
			m_tPostureAnim = ePostureStand;
			break;
		case eBodyStateSit : 	// сидит
			m_tPostureAnim = ePostureSit;
			break;	
		case eBodyStateLie : 	// ползёт
			m_tPostureAnim = ePostureLie;
			break;
		default : NODEFAULT;
	}

	if ( m_tMovementType == eMovementTypeStand &&  
		m_tStateType == eStateTypeNormal &&  
		m_tMovementDir == eMovementDirectionNone &&
		m_tActionType == eActionTypeStand)

		//if (fHealth < 30) 
		m_tActionAnim = eActionIdle;				// на месте / отдыхаем

	else if ( m_tMovementType == eMovementTypeStand &&  
		m_tStateType == eStateTypeNormal &&  
		m_tMovementDir == eMovementDirectionLeft &&
		m_tActionType == eActionTypeTurn) 

		m_tActionAnim = eActionIdleTurnLeft;		// поворот влево на месте

	else if ( m_tMovementType == eMovementTypeStand &&  
		m_tStateType == eStateTypeDanger &&  
		m_tMovementDir == eMovementDirectionLeft &&
		m_tActionType == eActionTypeTurn) 

		m_tActionAnim = eActionAttackTurnLeft;		// поворот влево на месте во время аттаки

	else if (m_tMovementType == eMovementTypeWalk && 
	//	m_tStateType == eStateTypeNormal &&  
		m_tMovementDir == eMovementDirectionForward && 
		m_tActionType == eActionTypeWalk) 

		m_tActionAnim = eActionWalkFwd;				// движение вперед

	else if (m_tMovementType == eMovementTypeWalk && 
		m_tMovementDir == eMovementDirectionLeft && 
		m_tActionType == eActionTypeTurn) 

		m_tActionAnim = eActionWalkTurnLeft;		// поворот в ходьбе влево

	else if (m_tMovementType == eMovementTypeWalk && 
		m_tMovementDir == eMovementDirectionRight && 
		m_tActionType == eActionTypeTurn) 

		m_tActionAnim = eActionWalkTurnRight;		// поворот в ходьбе вправо

	else if (m_tMovementType == eMovementTypeRun && 
		m_tActionType == eActionTypeRun) 

		m_tActionAnim = eActionRun;					// бег

	else if (m_tMovementType == eMovementTypeStand && 
		m_tActionType == eActionTypeAttack &&
		m_tStateType == eStateTypeDanger) 

		m_tActionAnim = eActionAttack;				// аттаковать

	else if (m_tMovementType == eMovementTypeStand && 
		m_tActionType == eActionTypeAttack &&
		m_tStateType == eStateTypeNormal) 

		m_tActionAnim = eActionAttack2;				// аттаковать крысу

	else if (m_tMovementType == eMovementTypeRun &&   
		m_tMovementDir == eMovementDirectionLeft &&
		m_tActionType == eActionTypeTurn ) 

		m_tActionAnim = eActionRunTurnLeft;			// поворот на бегу влево


	else if (m_tMovementType == eMovementTypeRun &&   
		m_tMovementDir == eMovementDirectionRight &&
		m_tActionType == eActionTypeTurn ) 

		m_tActionAnim = eActionRunTurnRight;			// поворот на бегу вправо

	else if (m_tMovementType == eMovementTypeWalk &&   
		m_tMovementDir == eMovementDirectionBack &&
		m_tStateType == eStateTypeDanger &&
		m_tActionType == eActionTypeWalk ) 

		m_tActionAnim = eActionWalkBkwd;			// пятиться назад

	else if (m_tMovementType == eMovementTypeStand &&   
		m_tMovementDir == eMovementDirectionNone &&
		m_tStateType == eStateTypePanic &&
		m_tActionType == eActionTypeStand ) 

		m_tActionAnim = eActionScared;			// стоять испуганно

	else if (m_tMovementType == eMovementTypeStand &&   
		m_tMovementDir == eMovementDirectionNone &&
		m_tStateType == eStateTypeNormal &&
		m_tActionType == eActionTypeEat ) 

		m_tActionAnim = eActionEat;			// обед
	else if (m_tMovementType == eMovementTypeStand &&   
		m_tMovementDir == eMovementDirectionNone &&
		m_tStateType == eStateTypeNormal &&
		m_tActionType == eActionTypeLieDown ) 

		m_tActionAnim = eActionLieDown;			// лечь
	else if (m_tMovementType == eMovementTypeStand &&   
		m_tMovementDir == eMovementDirectionNone &&
		m_tStateType == eStateTypeNormal &&
		m_tActionType == eActionTypeLie ) 

		m_tActionAnim = eActionIdle;			// лежать


	if (_CA.Started) {
		m_tPostureAnim = _CA.Posture;
		m_tActionAnim = _CA.Action;
		_CA.Playing = true;
		_CA.Started = false;
		_CA.Finished = false;	
		FORCE_ANIMATION_SELECT();
	}
	if (_CA.Finished) {
		_CA.Playing = false;
	}
	if (_CA.Playing) {
		return;
	}
	
	if ( ((PostureAnim_old != m_tPostureAnim) || (ActionAnim_old != m_tActionAnim)) && bForceChange) {
//		if (m_dwAnimLastSetTime + m_dwAnimFrameDelay <= m_dwCurrentUpdate) {
//			m_dwAnimLastSetTime = m_dwCurrentUpdate; 
	
		FORCE_ANIMATION_SELECT();
	}
}

