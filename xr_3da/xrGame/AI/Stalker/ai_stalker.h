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
		eStalkerStateFiring,
		eStalkerStateUnderFire,
		eStalkerStatePursuit,
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

	// firing
	bool					m_bFiring;
	u32						m_dwStartFireAmmo;
	u32						m_dwNoFireTime;
	float					m_fAddWeaponAngle;
	
	// fire  constants
	u32						m_dwFireRandomMin;
	u32						m_dwFireRandomMax;
	u32						m_dwNoFireTimeMin;
	u32						m_dwNoFireTimeMax;
	float					m_fMinMissDistance;
	float					m_fMinMissFactor;
	float					m_fMaxMissDistance;
	float					m_fMaxMissFactor;

	// hit data
	u32						m_dwHitTime;
	Fvector					m_tHitDir;
	Fvector					m_tHitPosition;
	
	// VISIBILITY
	objSET					m_tpaVisibleObjects;
	SEnemySelected			m_Enemy;
		
	//HIT PHYS
	float					m_saved_impulse;
	Fvector					m_saved_hit_position;
	Fvector					m_saved_hit_dir;
	s16						m_saved_element;
	//PHYS
	float					m_phMass;
	//skeleton
	float					skel_density_factor;
	float					skel_airr_lin_factor;
	float					skel_airr_ang_factor;
	float					hinge_force_factor;
	float					hinge_force_factor1;
	float					hinge_force_factor2;
	float					hinge_vel;
	float					skel_fatal_impulse_factor;
	int						skel_ddelay;

	// Graph
	_GRAPH_ID				m_tCurGP;
	_GRAPH_ID				m_tNextGP;
	u32						m_dwTimeToChange;
	float					m_fGoingSpeed;
	TERRAIN_VECTOR			m_tpaTerrain;
	Fvector					m_tNextGraphPoint;
	// FSM
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
	float					r_head_speed;
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
			void			Firing							();
			void			UnderFire						();
			void			Pursuit							();

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
			void			vfValidateAngleDependency		(float x1, float &x2, float x3);
			// movement and look
			void			vfSetMovementType				(EBodyState tBodyState, EMovementType tMovementType, ELookType tLookType);
			void			vfSetMovementType				(EBodyState tBodyState, EMovementType tMovementType, ELookType tLookType, Fvector &tPointToLook);
			// fire
			bool			bfCheckForMember				(Fvector &tFireVector, Fvector &tMyPoint, Fvector &tMemberPoint);
			bool			bfCheckIfCanKillEnemy			();
			bool			bfCheckIfCanKillMember			();
			void			vfSetFire						(bool bFire, CGroup &Group);
			void			vfStopFire						();
			
			// miscellanious
			void			DropItem						();
			void			vfUpdateSearchPosition			();
			void			vfChooseNextGraphPoint			();
			// physics
			void			CreateSkeleton					();

	IC		void			GetDirectionAngles				(float &yaw, float &pitch)
	{
		yaw						= pitch = 0;
		Fvector					tDirection;
		
		int						i = ps_Size	();
		if (i < 2) 
			return;
		
		CObject::SavedPosition	tPreviousPosition = ps_Element(i - 2), tCurrentPosition = ps_Element(i - 1);
		tDirection.sub			(tCurrentPosition.vPosition,tPreviousPosition.vPosition);
		if (tDirection.magnitude() < EPS_L)
			return;
		
		tDirection.getHP		(yaw,pitch);
	};
	
	IC		bool		angle_lerp_bounds(float &a, float b, float c, float d, bool bCheck = false)
	{
		float fDifference;
		if ((fDifference = _abs(a - b)) > PI - EPS_L)
			fDifference = PI_MUL_2 - fDifference;

		if (c*d >= fDifference) {
			a = b;
			return(true);
		}
		
		angle_lerp(a,b,c,d);

		return(false);
	};

	IC		float		ffSubAngles(float a, float b)
	{
		//a = normalize_angle_signed(a);
		//b = normalize_angle_signed(b);
		float c = a - b;
		if (c > PI)
			return(c - PI_MUL_2);
		else
			if (c < -PI)
				return(c + PI_MUL_2);
			else
				return(c);
	};

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
	virtual void			Exec_Look						(float dt);
	virtual void			Update							(u32 dt);
	virtual void			UpdateCL						();
	virtual void			Hit								(float P,Fvector &dir,CObject*who,s16 element,Fvector p_in_object_space,float impulse);
	virtual float			EnemyHeuristics					(CEntity* E);
	virtual void			SelectEnemy						(SEnemySelected& S);
};
