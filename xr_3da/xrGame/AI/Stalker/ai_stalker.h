////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_stalker.h
//	Created 	: 25.02.2003
//  Modified 	: 25.02.2003
//	Author		: Dmitriy Iassenev
//	Description : AI Behaviour for monster "Stalker"
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "..\\..\\CustomMonster.h"
#include "..\\ai_selector_template.h"
#include "ai_stalker_animations.h"
#include "ai_stalker_space.h"

class CAI_Stalker : public CCustomMonster, public CStalkerAnimations, public CInventoryOwner {
private:
	typedef CCustomMonster inherited;
	
	enum EStalkerStates {
		eStalkerStateDie = 0,
		eStalkerStateAccomplishingTask,
		eStalkerStateAttack,
		eStalkerStateDefend,
		eStalkerStateRetreatKnown,
		eStalkerStateRetreatUnknown,
		eStalkerStatePursuitKnown,
		eStalkerStatePursuitUnknown,
		eStalkerStateSearchCorp,
		
		eStalkerStateRecharge,
		eStalkerStateHolsterItem,
		eStalkerStateTakeItem,
		eStalkerStateDropItem,
	};

	typedef struct tagSStalkerStates {
		EStalkerStates		eState;
		u32					dwTime;
	} SStalkerStates;

	typedef struct tagSHurt {
		CEntity *tpEntity;
		u32	dwTime;
	} SHurt;

	typedef struct tagSSearch {
		u32	dwTime;
		u32	dwNodeID;
	} SSearch;

	// path structures
	EPathState				m_tPathState;	
	EPathType				m_tPathType;
	EPathType				m_tPrevPathType;
	EWeaponState			m_tWeaponState;
	EActionState			m_tActionState;
	u32						m_dwActionStartTime;
	u32						m_dwActionEndTime;
	bool					m_bIfSearchFailed;

	vector<Fvector>			m_tpaPoints;
	vector<Fvector>			m_tpaDeviations;
	vector<Fvector>			m_tpaTravelPath;
	vector<u32>				m_tpaPointNodes;
	vector<Fvector>			m_tpaLine;
	vector<u32>				m_tpaNodes;
	vector<Fvector>			m_tpaTempPath;

	typedef svector<Fvector,MAX_SUSPICIOUS_NODE_COUNT>	SuspiciousPoints;
	typedef svector<Fvector,MAX_SUSPICIOUS_NODE_COUNT>	SuspiciousForces;

	svector<SDynamicObject,	MAX_DYNAMIC_OBJECTS>		m_tpaDynamicObjects;
	svector<SDynamicSound,	MAX_DYNAMIC_SOUNDS>			m_tpaDynamicSounds;
	svector<SHurt,			MAX_HURT_COUNT>				m_tpaHurts;
	svector<SSearch,		MAX_SEARCH_COUNT>			m_tpaSearchPositions;
	objSET					m_tpaVisibleObjects;
	u32						m_dwMaxDynamicObjectsCount;
	u32						m_dwMaxDynamicSoundsCount;
	float					m_fSensetivity;
	int						m_iSoundIndex;
	u32						m_dwDeathTime;
	bool					m_bHammerIsClutched;
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
	SEnemySelected			m_tEnemy;
	CEntity*				m_tSavedEnemy;
	Fvector					m_tSavedEnemyPosition;
	u32						m_dwLostEnemyTime;
	NodeCompressed*			m_tpSavedEnemyNode;
	u32						m_dwSavedEnemyNodeID;
	CInventoryItem*			m_tpItemToTake;
	// pursuiting
	int						m_iCurrentSuspiciousNodeIndex;
	SuspiciousPoints		m_tpaSuspiciousPoints;
	SuspiciousForces		m_tpaSuspiciousForces;
	vector<u32>				m_tpaNodeStack;
	bool					m_bActionStarted;
	u32						m_dwSoundTime;

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
	CAI_NodeEvaluatorTemplate<aiSearchRange | aiEnemyDistance>			m_tSelectorFreeHunting;
	CAI_NodeEvaluatorTemplate<aiSearchRange | aiCoverFromEnemyWeight>	m_tSelectorReload;
	CAI_NodeEvaluatorTemplate<aiSearchRange | aiCoverFromEnemyWeight | aiEnemyDistance>	m_tSelectorCover;
	CAI_NodeEvaluatorTemplate<aiSearchRange | aiCoverFromEnemyWeight | aiEnemyDistance | aiEnemyViewDeviationWeight >	m_tSelectorRetreat;
	CAI_NodeEvaluatorFull												m_tSelectorAttack;
	u32						m_dwActionRefreshRate;
	float					m_fAttackSuccessProbability;
	Fvector					m_tMySavedPosition;
	u32						m_dwMyNodeID;
	
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

	u32						m_dwLastRangeSearch;

			// state machine

	IC		void			vfAddStateToList(EStalkerStates eState)
	{
		if ((m_tStateList.size()) && (m_tStateList[m_tStateList.size() - 1].eState == eState)) {
			m_tStateList[m_tStateList.size() - 1].dwTime = m_dwCurrentUpdate;
			return;
		}
		if (m_tStateList.size() >= MAX_STATE_LIST_SIZE)
			m_tStateList.erase(u32(0));
		SStalkerStates tStalkerStates;
		tStalkerStates.dwTime = m_dwCurrentUpdate;
		tStalkerStates.eState = eState;
		m_tStateList.push_back(tStalkerStates);
	}
			// states
//			void			AccomplishTask					();
//			void			Attack							();
//			void			Defend							();
//			void			RetreatKnown					();
//			void			RetreatUnknown					();
//			void			PursuitKnown					();
//			void			PursuitUnknown					();
//			void			SearchCorp						();
//
//			void			Recharge						();
//			void			HolsterItem						();
//			void			TakeItem						();
//			void			DropItem						();

			void			BackDodge						();
			void			BackCover						();
			void			ForwardCover					();

			// selectors
			void			vfInitSelector					(IBaseAI_NodeEvaluator &S, CSquad &Squad, CEntity* &Leader);
			void			vfSearchForBetterPosition		(IBaseAI_NodeEvaluator &S, CSquad &Squad, CEntity* &Leader);
			void			vfBuildPathToDestinationPoint	(IBaseAI_NodeEvaluator *S);
			void			vfBuildTravelLine				(Fvector *tpDestinationPosition = 0);
			void			vfDodgeTravelLine				();
			void			vfChoosePointAndBuildPath		(IBaseAI_NodeEvaluator &tSelector, Fvector *tpDestinationPosition = 0);
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
			void			SetLessCoverLook				(NodeCompressed *tpNode, bool bDifferenceLook);
			void			SetLessCoverLook				(NodeCompressed *tpNode, float fMaxHeadTurnAngle, bool bDifferenceLook);
			void			vfValidateAngleDependency		(float x1, float &x2, float x3);
			// movement and look
			void			vfSetParameters					(IBaseAI_NodeEvaluator &tNodeEvaluator, Fvector *tpDesiredPosition, EWeaponState tWeaponState, EPathType tPathType, EBodyState tBodyState, EMovementType tMovementType, ELookType tLookType);
			void			vfSetParameters					(IBaseAI_NodeEvaluator &tNodeEvaluator, Fvector *tpDesiredPosition, EWeaponState tWeaponState, EPathType tPathType, EBodyState tBodyState, EMovementType tMovementType, ELookType tLookType, Fvector &tPointToLook);
			// fire
			bool			bfCheckForMember				(Fvector &tFireVector, Fvector &tMyPoint, Fvector &tMemberPoint);
			bool			bfCheckIfCanKillEnemy			();
			bool			bfCheckIfCanKillMember			();
			bool			bfCheckIfCanKillTarget			(Fvector fire_pos, Fvector target_pos, float yaw2, float pitch2);
			void			vfSetWeaponState				(EWeaponState tWeaponState);
			void			vfCheckForItems					();
			
			// miscellanious
			void			DropItemSendMessage				(CObject *O);
			void			HolsterItemSendMessage			();
			void			vfUpdateSearchPosition			();
			void			vfChooseNextGraphPoint			();
			void			vfMarkVisibleNodes				(CEntity *tpEntity);
			void			vfFindAllSuspiciousNodes		(u32 StartNode, Fvector tPointPosition, const Fvector& BasePos, float Range, CGroup &Group);
			void			vfClasterizeSuspiciousNodes		(CGroup &Group);
			void			vfChooseSuspiciousNode			(IBaseAI_NodeEvaluator &tSelector);
			int				ifGetSuspiciousAvailableNode	(int iLastIndex, CGroup &Group);
			bool			bfCheckForNodeVisibility		(u32 dwNodeID, bool bIfRyPick = false);
			void			SelectSound						(int &iIndex);
			void			vfUpdateDynamicObjects			();
			int				ifFindDynamicObject				(CEntity *tpEntity);
			bool			bfAddEnemyToDynamicObjects		(CAI_Stalker *tpStalker);
			bool			bfCheckIfSound					();

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

	IC		void		vfSaveEnemy()
	{
		m_tSavedEnemy			= m_tEnemy.Enemy;
		m_tSavedEnemyPosition	= m_tEnemy.Enemy->Position();
		m_tpSavedEnemyNode		= m_tEnemy.Enemy->AI_Node;
		m_dwSavedEnemyNodeID	= m_tEnemy.Enemy->AI_NodeID;
		m_dwLostEnemyTime		= Level().timeServer();
		m_tMySavedPosition		= vPosition;
		m_dwMyNodeID			= AI_NodeID;
	}

	IC		void		vfAddToSearchList()
	{
		if (m_tpaSearchPositions.size() >= MAX_SEARCH_COUNT)
			m_tpaSearchPositions.erase(u32(0));

		SSearch							tSearch;
		tSearch.dwTime					= m_dwCurrentUpdate;
		tSearch.dwNodeID				= AI_Path.DestNode;
		m_tpaSearchPositions.push_back	(tSearch);
	}

public:
	typedef CCustomMonster inherited;
							CAI_Stalker						();
	virtual					~CAI_Stalker					();
	virtual objQualifier*	GetQualifier					();
	virtual BOOL			net_Spawn						(LPVOID DC);
	virtual void			Death							();
	virtual void			Load							(LPCSTR	section );				
	virtual void			HitSignal						(float P,	Fvector& vLocalDir, CObject* who, s16 element);
	virtual void			g_WeaponBones					(int &L, int &R1, int &R2);
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
	virtual void			feel_sound_new					(CObject* who, int eType, Fvector& Position, float power);
};
