////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_stalker.h
//	Created 	: 25.02.2003
//  Modified 	: 25.02.2003
//	Author		: Dmitriy Iassenev
//	Description : AI Behaviour for monster "Stalker"
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "..\\..\\CustomMonster.h"
#include "..\\ai_monsters.h"
#include "ai_stalker_animations.h"
#include "ai_stalker_selectors.h"

#define MAX_STATE_LIST_SIZE				256
#define MAX_TIME_RANGE_SEARCH			5000.f
#define	MAX_HEAD_TURN_ANGLE				(2.f*PI_DIV_6)

class CAI_Stalker : public CCustomMonster, public CStalkerAnimations {
private:
	typedef CCustomMonster inherited;
	
	enum EStalkerStates {
		eStalkerStateDie = 0,
		eStalkerStateTurnOver,
		eStalkerStateWaitForAnimation,
		eStalkerStateWaitForTime,
		eStalkerStateRecharge,
		eStalkerStateDrop,
		eStalkerStateLookingOver,
		eStalkerStateSearching,
	};

	enum EBodyState {
		eBodyStateCrouch = 0,
		eBodyStateStand,
	};

	enum EMovementType {
		eMovementTypeWalk = 0,
		eMovementTypeRun,
		eMovementTypeStand,
	};

	enum EMovementDirection {
		eMovementDirectionForward = 0,
		eMovementDirectionBack,
		eMovementDirectionLeft,
		eMovementDirectionRight,
	};

	enum ELookType {
		eLookTypeDirection = 0,
		eLookTypeView,
		eLookTypeSearch,
		eLookTypeDanger,
		eLookTypePoint,
	};

	typedef struct tagSStalkerStates {
		EStalkerStates		eState;
		u32					dwTime;
	} SStalkerStates;

	stack<EStalkerStates>	m_tStateStack;
	svector<SStalkerStates,MAX_STATE_LIST_SIZE>	m_tStateList;
	u32						m_dwLastUpdate;
	u32						m_dwCurrentUpdate;
	u32						m_dwUpdateCount;
	bool					m_bStopThinking;
	EStalkerStates			m_eCurrentState;
	EStalkerStates			m_ePreviousState;
	bool					m_bStateChanged;
	CStalkerSelectorFreeHunting	m_tSelectorFreeHunting;
	// visibility constants
	u32						m_dwMovementIdleTime;
	float					m_fMaxInvisibleSpeed;
	float					m_fMaxViewableSpeed;
	float					m_fMovementSpeedWeight;
	float					m_fDistanceWeight;
	float					m_fSpeedWeight;
	float					m_fCrouchVisibilityMultiplier;
	float					m_fLieVisibilityMultiplier;
	float					m_fVisibilityThreshold;
	float					m_fLateralMultiplier;
	float					m_fShadowWeight;
	ELookType				m_tLookType;
	// movement
	EBodyState				m_tBodyState;
	EMovementType			m_tMovementType;
	EMovementDirection		m_tMovementDirection;
	EMovementDirection		m_tDesirableDirection;
	u32						m_dwDirectionStartTime;
	u32						m_dwAnimationSwitchInterval;
	float					m_fCrouchFactor;
	float					m_fWalkFactor;
	float					m_fRunFactor;

			// state machine
			void			vfAddStateToList				(EStalkerStates eState);
			// states
			void			TurnOver						();
			void			WaitForAnimation				();
			void			WaitForTime						();
			void			Recharge						();
			void			Drop							();
			void			LookingOver						();
			void			Searching						();
			// selectors
			void			vfBuildPathToDestinationPoint	(CAISelectorBase *S);
			void			vfSearchForBetterPosition		(CAISelectorBase &S, CSquad &Squad, CEntity* &Leader);
			void			vfInitSelector					(CAISelectorBase &S, CSquad &Squad, CEntity* &Leader);
			void			vfChoosePointAndBuildPath		(CAISelectorBase &tSelector);
			// animations
			void			vfAssignGlobalAnimation			(CMotionDef *&tpGlobalAnimation);
			void			vfAssignTorsoAnimation			(CMotionDef *&tpGlobalAnimation);
			void			vfAssignLegsAnimation			(CMotionDef *&tpLegsAnimation);
			// bones
			void			vfAssignBones					(CInifile *ini, const char *section);
	static	void __stdcall	HeadCallback					(CBoneInstance *B);
	static	void __stdcall	ShoulderCallback				(CBoneInstance *B);
	static	void __stdcall	SpinCallback					(CBoneInstance *B);
			// look
			bool			bfCheckForVisibility			(CEntity* tpEntity);
			void			SetDirectionLook				();
			void			SetLook							(Fvector tPosition);
			void			SetLessCoverLook				();
			void			SetLessCoverLook				(NodeCompressed *tpNode);
			void			SetLessCoverLook				(NodeCompressed *tpNode, float fMaxHeadTurnAngle);
			// movement and look
			void			vfSetMovementType				(EBodyState tBodyState, EMovementType tMovementType, ELookType tLookType);
			// miscellanious
			void			DropItem						();
public:
	typedef CCustomMonster inherited;
							CAI_Stalker						();
	virtual					~CAI_Stalker					();
	virtual objQualifier*	GetQualifier					();
	virtual BOOL			net_Spawn						(LPVOID DC);
	virtual void			Death							();
	virtual void			OnDeviceCreate					();
	virtual void			Load							(LPCSTR	section );				
	virtual void			HitSignal						(float P,	Fvector& vLocalDir, CObject* who, s16 element);
	virtual void			g_WeaponBones					(int& L,	int& R	);
	virtual void			Think							();
	virtual void			Die								();
	virtual void			g_fireParams					(Fvector& P, Fvector& D);
	virtual void			net_Export						(NET_Packet& P);
	virtual void			net_Import						(NET_Packet& P);
	virtual void			SelectAnimation					(const Fvector& _view, const Fvector& _move, float speed );
	virtual void			OnEvent							(NET_Packet& P, u16 type);
	virtual void			feel_touch_new					(CObject* O);
	virtual void			OnVisible						();
	virtual void			Exec_Movement					(float dt);
};
