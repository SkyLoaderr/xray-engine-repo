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
#include "..\\..\\inventory.h"

using namespace StalkerSpace;

class CSE_ALifeSimulator;
class CCharacterPhysicsSupport;
class CWeapon;
class CEntityAction;

//#define LOG_PARAMETERS

#ifdef LOG_PARAMETERS
extern FILE	*ST_VF;
#endif


class CAI_Stalker : public CCustomMonster, public CStalkerAnimations, public CInventoryOwner {
private:
	typedef CCustomMonster inherited;
	
	typedef struct tagSHurt {
		CEntity *tpEntity;
		u32	dwTime;
	} SHurt;

	// path structures
	EPathState					m_tPathState;	
	EPathType					m_tPathType;
	EPathType					m_tPrevPathType;
	EObjectAction				m_tWeaponState;
	EActionState				m_tActionState;
	EMentalState				m_tMentalState;
	u32							m_dwActionStartTime;
	u32							m_dwActionEndTime;
	bool						m_bIfSearchFailed;
	bool						_A,_B,_C,_D,_E,_F,_G,_H,_I,_J,_K,_L,_M;
	bool						A,B,C,D,E,F,G,H,I,J,K,L,M;
	u32							m_dwLookChangedTime;
	u32							m_dwBornTime;
	bool						m_bCanFire;

	// ALife members
	xr_vector<Fvector>			m_tpaPoints;
	xr_vector<Fvector>			m_tpaDeviations;
	xr_vector<Fvector>			m_tpaTravelPath;
	xr_vector<u32>				m_tpaPointNodes;
	xr_vector<Fvector>			m_tpaLine;
	xr_vector<u32>				m_tpaNodes;
	xr_vector<Fvector>			m_tpaTempPath;

	typedef svector<Fvector,MAX_SUSPICIOUS_NODE_COUNT>	SuspiciousPoints;
	typedef svector<Fvector,MAX_SUSPICIOUS_NODE_COUNT>	SuspiciousForces;

	// ref_sound
	ref_sound				m_tpSoundStep[STALKER_SND_STEP_COUNT];
	SOUND_VECTOR			m_tpSoundDie;
	SOUND_VECTOR			m_tpSoundHit;
	SOUND_VECTOR			m_tpSoundHumming;
	SOUND_VECTOR			m_tpSoundAlarm;
	SOUND_VECTOR			m_tpSoundSurrender;

	ref_sound				*m_tpCurrentSound;
	bool					m_bPlayHumming;
	
	float					m_fTimeToStep;
	bool					m_bStep;
	
	u32						m_dwMyMaterialID;
	u32						m_dwLastMaterialID;

	// dynamic objects, sounds, hurts, search positions, etc.
	svector<SDynamicObject,	MAX_DYNAMIC_OBJECTS>		m_tpaDynamicObjects;
	svector<SDynamicSound,	MAX_DYNAMIC_SOUNDS>			m_tpaDynamicSounds;
	svector<SHurt,			MAX_HURT_COUNT>				m_tpaHurts;
	u32						m_dwMaxDynamicObjectsCount;
	u32						m_dwMaxDynamicSoundsCount;
	float					m_fSensetivity;
	int						m_iSoundIndex;
	int						m_iSuspPoint;
	EActionState			m_tLastActionState;
	float					m_ls_yaw;
	
	bool					m_bHammerIsClutched;
	u32						m_dwRandomFactor;
	u32						m_dwInertion;
	u32						m_dwParticularState;
	bool					m_bSearchedForEnemy;
	u32						m_dwLastEnemySearch;
	u32						m_dwLastSoundUpdate;
	
	// firing
	bool					m_bFiring;
	u32						m_dwNoFireTime;
	u32						m_dwStartFireTime;
	float					m_fAttackSuccessProbability0;
	float					m_fAttackSuccessProbability1;
	float					m_fAttackSuccessProbability2;
	float					m_fAttackSuccessProbability3;
	u32						m_dwRandomState;
	
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
	float					m_fHitFactor;
	
	// VISIBILITY
	SEnemySelected			m_tEnemy;
	CEntity*				m_tSavedEnemy;
	Fvector					m_tSavedEnemyPosition;
	u32						m_dwLostEnemyTime;
	NodeCompressed*			m_tpSavedEnemyNode;
	u32						m_dwSavedEnemyNodeID;
	Fvector					m_tMySavedPosition;
	u32						m_dwMyNodeID;
	
	// items to take
	xr_vector<CInventoryItem*>	m_tpItemsToTake;
	u32						m_dwItemToTakeIndex;
	
	// pursuiting
	int						m_iCurrentSuspiciousNodeIndex;
	SuspiciousPoints		m_tpaSuspiciousPoints;
	SuspiciousForces		m_tpaSuspiciousForces;
	bool					m_bActionStarted;
	u32						m_dwSoundTime;

	CCharacterPhysicsSupport* m_pPhysics_support;
	
	// FSM
	u32						m_dwLastUpdate;
	u32						m_dwCurrentUpdate;
	u32						m_dwUpdateCount;
	bool					m_bStopThinking;
	bool					m_bStateChanged;
	CAI_NodeEvaluatorTemplate<aiSearchRange | aiEnemyDistance>			m_tSelectorFreeHunting;
	CAI_NodeEvaluatorTemplate<aiSearchRange | aiCoverFromEnemyWeight>	m_tSelectorReload;
	CAI_NodeEvaluatorTemplate<aiSearchRange | aiCoverFromEnemyWeight | aiEnemyDistance>	m_tSelectorCover;
	CAI_NodeEvaluatorTemplate<aiSearchRange | aiCoverFromEnemyWeight | aiEnemyDistance | aiEnemyViewDeviationWeight >	m_tSelectorRetreat;
	//CAI_NodeEvaluatorFull												m_tSelectorAttack;
	CAI_NodeEvaluatorTemplate<aiSearchRange | aiInsideNode>				m_tSelectorNode;
	u32						m_dwActionRefreshRate;
	float					m_fAttackSuccessProbability;
	
	// visibility constants
	u32						m_dwMovementIdleTime;
	float					m_fMaxInvisibleSpeed;
	float					m_fMaxViewableSpeed;
	float					m_fMovementSpeedWeight;
	float					m_fDistanceWeight;
	float					m_fSpeedWeight;
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
	float					m_fWalkFreeFactor;
	float					m_fRunFreeFactor;
	float					m_fPanicFactor;
	float					m_fDamagedWalkFactor;
	float					m_fDamagedRunFactor;
	float					m_fDamagedWalkFreeFactor;
	float					m_fDamagedRunFreeFactor;
	float					m_fDamagedPanicFactor;

	u32						m_dwLastRangeSearch;

	// ALife
	TERRAIN_VECTOR			m_tpaTerrain;
	ETaskState				m_tTaskState;
	// ALife path builder
	DWORD_VECTOR			m_tpGraphPath;
	u32						m_dwCurGraphPathNode;
	_GRAPH_ID				m_tGraphID;
	_GRAPH_ID				m_tNextGraphID;
	_GRAPH_ID				m_tDestGraphPointIndex;
	_TASK_ID				m_tTaskID;
	OBJECT_VECTOR			m_tpKnownCustomers;


			void			BackStraight					();
			void			BackCover						(bool bFire = true);
			void			ForwardCover					();
			void			ForwardStraight					();
			void			Camp							(bool bWeapon = true);
			void			Panic							();
			void			Hide							();
			void			Detour							();
			void			SearchEnemy						();
			void			ExploreDE						();
			void			ExploreDNE						();
			void			ExploreNDE						();
			void			ExploreNDNE						();
			void			TakeItems						();
			
			// ALife
			void			ALifeUpdate						();
			void			vfChooseTask					();
			void			vfHealthCare					();
			void			vfBuySupplies					();
			void			vfGoToCustomer					();
			void			vfBringToCustomer				();
			void			vfGoToSOS						();
			void			vfSendSOS						();
			void			vfAccomplishTask				();
			void			vfContinueWithALifeGoals		(IBaseAI_NodeEvaluator *tpNodeEvaluator = 0);
			void			vfSearchObject					();
			bool			bfHealthIsGood					();
			bool			bfItemCanTreat					(CInventoryItem *tpInventoryItem);
			void			vfUseItem						(CInventoryItem *tpInventoryItem);
			bool			bfCanTreat						();
			bool			bfEnoughMoneyToTreat			();
			bool			bfEnoughTimeToTreat				();
			bool			bfEnoughEquipmentToGo			();
			bool			bfDistanceToTraderIsDanger		();
			bool			bfEnoughMoneyToEquip			();
			void			vfChooseHumanTask				();
			bool			bfCheckIfTaskCompleted			();
			bool			bfCheckIfTaskCompleted			(OBJECT_IT		&I);
			void			vfSetCurrentTask				(_TASK_ID		&tTaskID);
			bool			bfAssignDestinationNode			();
			void			vfFinishTask					();

			// selectors
			void			vfInitSelector					(IBaseAI_NodeEvaluator &S, CSquad &Squad, CEntity* &Leader);
			void			vfSearchForBetterPosition		(IBaseAI_NodeEvaluator &S, CSquad &Squad, CEntity* &Leader);
			void			vfBuildPathToDestinationPoint	(IBaseAI_NodeEvaluator *tpNodeEvaluator);
			void			vfBuildTravelLine				(Fvector *tpDestinationPosition = 0);
			void			vfDodgeTravelLine				();
			void			vfChoosePointAndBuildPath		(IBaseAI_NodeEvaluator *tpNodeEvaluator, Fvector *tpDestinationPosition = 0, bool bSearchForNode = true);
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
			void			SetPointLookAngles				(const Fvector &tPosition, float &yaw, float &pitch);
			void			SetFirePointLookAngles			(const Fvector &tPosition, float &yaw, float &pitch);
			void			SetDirectionLook				();
			void			SetLessCoverLook				(NodeCompressed *tpNode, bool bDifferenceLook);
			void			SetLessCoverLook				(NodeCompressed *tpNode, float fMaxHeadTurnAngle, bool bDifferenceLook);
			void			vfValidateAngleDependency		(float x1, float &x2, float x3);
			// movement and look
			void			vfSetParameters					(IBaseAI_NodeEvaluator *tpNodeEvaluator, Fvector *tpDesiredPosition, bool bSearchNode, EObjectAction tWeaponState, EPathType tPathType, EBodyState tBodyState, EMovementType tMovementType, EMentalState tMentalState, ELookType tLookType);
			void			vfSetParameters					(IBaseAI_NodeEvaluator *tpNodeEvaluator, Fvector *tpDesiredPosition, bool bSearchNode, EObjectAction tWeaponState, EPathType tPathType, EBodyState tBodyState, EMovementType tMovementType, EMentalState tMentalState, ELookType tLookType, const Fvector &tPointToLook, u32 dwLookOverDelay = 2000);
			// fire
			bool			bfCheckForMember				(Fvector &tFireVector, Fvector &tMyPoint, Fvector &tMemberPoint);
			bool			bfCheckIfCanKillEnemy			();
			bool			bfCheckIfCanKillMember			();
			bool			bfCheckIfCanKillTarget			(CEntity *tpEntity, Fvector target_pos, float yaw2, float pitch2, float fSafetyAngle = FIRE_SAFETY_ANGLE);
			void			vfSetWeaponState				(EObjectAction tWeaponState);
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
			int				ifFindHurtIndex					(CEntity *tpEntity);
			void			vfAddHurt						(const SHurt &tHurt);
			void			vfUpdateHurt					(const SHurt &tHurt);
			bool			bfCheckIfSound					();
			void			vfUpdateParameters				(bool &A, bool &B, bool &C, bool &D, bool &E, bool &F, bool &G, bool &H, bool &I, bool &J, bool &K, bool &L, bool &M);
			void			vfUpdateVisibilityBySensitivity	();
			bool			bfIf_I_SeePosition				(Fvector tPosition);
			void			LoadSounds						(SOUND_VECTOR &tpSounds, LPCSTR	prefix, u32 dwMaxCount);
			void			vfValidatePosition				(Fvector &tPosition, u32 dwNodeID);
			bool			bfIfHuman						(CEntity *tpEntity = 0);
			void			vfSelectItemToTake				(CInventoryItem *&tpItemToTake);
			void			Init							();
			// physics
			void			CreateSkeleton					();

	IC		void			GetDirectionAnglesByPrevPositions(float &yaw, float &pitch)
	{
		yaw						= pitch = 0;
		Fvector					tDirection;
		int						i = ps_Size	();
		
		if (i < 2) 
			return;
		
		CObject::SavedPosition	tPreviousPosition = ps_Element(i - 2), tCurrentPosition = ps_Element(i - 1);
		VERIFY					(_valid(tPreviousPosition.vPosition));
		VERIFY					(_valid(tCurrentPosition.vPosition));
		tDirection.sub			(tCurrentPosition.vPosition,tPreviousPosition.vPosition);
		if (tDirection.magnitude() < EPS_L)	return;
		tDirection.getHP		(yaw,pitch);
		VERIFY					(_valid(yaw));
		VERIFY					(_valid(pitch));
	}

	IC		void			GetDirectionAngles				(float &yaw, float &pitch)
	{
		yaw						= pitch = 0;
//		Fvector					tDirection;
//		
//		if (!AI_Path.TravelPath.empty() && (AI_Path.TravelStart <= (AI_Path.TravelPath.size() - 1)) && (AI_Path.TravelStart >= 0))
//			if (Position().distance_to(AI_Path.TravelPath[AI_Path.TravelStart].P) > EPS_L) {
//				tDirection.sub(Position(),AI_Path.TravelPath[AI_Path.TravelStart].P);
//				if (tDirection.magnitude() < EPS_L)
//					GetDirectionAnglesByPrevPositions(yaw,pitch);
//			}
//			else
//				if ((AI_Path.TravelStart < (AI_Path.TravelPath.size() - 1)) && (Position().distance_to(AI_Path.TravelPath[AI_Path.TravelStart + 1].P) > EPS_L)) {
//					tDirection.sub(Position(),AI_Path.TravelPath[AI_Path.TravelStart + 1].P);
//					if (tDirection.magnitude() < EPS_L)
//						GetDirectionAnglesByPrevPositions(yaw,pitch);
//				}
//				else
//					GetDirectionAnglesByPrevPositions(yaw,pitch);
//		else
			GetDirectionAnglesByPrevPositions(yaw,pitch);
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
		R_ASSERT3				(int(m_dwSavedEnemyNodeID) > 0, "Invalid enemy node", m_tEnemy.Enemy->cName());
		m_dwLostEnemyTime		= Level().timeServer();
		m_tMySavedPosition		= Position();
		m_dwMyNodeID			= AI_NodeID;
		vfValidatePosition		(m_tSavedEnemyPosition,m_dwSavedEnemyNodeID);
	}

	IC	ERelationType	bfGetRelation(CEntityAlive *tpEntityAlive)
	{
		if (tpEntityAlive->g_Team() != g_Team())
			return(eRelationTypeEnemy);
		else
			return(eRelationTypeFriend);
	}

public:
	typedef CCustomMonster inherited;
							CAI_Stalker						();
	virtual					~CAI_Stalker					();
	virtual void			Death							();
	virtual void			Load							(LPCSTR	section );				
	virtual void			HitSignal						(float P,	Fvector& vLocalDir, CObject* who, s16 element);
	virtual float			HitScale						(int element);
	virtual void			g_WeaponBones					(int &L, int &R1, int &R2);
	virtual void			Think							();
	virtual void			Die								();
	virtual void			g_fireParams					(Fvector& P, Fvector& D);
	virtual BOOL			net_Spawn						(LPVOID DC);
	virtual void			net_Export						(NET_Packet& P);
	virtual void			net_Import						(NET_Packet& P);
	virtual void			net_Destroy						();
	virtual void			SelectAnimation					(const Fvector& _view, const Fvector& _move, float speed );
	virtual void			OnEvent							(NET_Packet& P, u16 type);
	virtual void			feel_touch_new					(CObject* O);
	virtual void			renderable_Render				();
	virtual void			Exec_Movement					(float dt);
	virtual void			Exec_Look						(float dt);
	virtual void			shedule_Update					(u32 dt);
	virtual void			UpdateCL						();
	virtual void			Hit								(float P,Fvector &dir,CObject*who,s16 element,Fvector p_in_object_space,float impulse, ALife::EHitType hit_type = eHitTypeWound);
	virtual float			EnemyHeuristics					(CEntity* E);
	virtual void			SelectEnemy						(SEnemySelected& S);
	virtual void			feel_sound_new					(CObject* who, int eType, const Fvector &Position, float power);
	virtual BOOL			feel_vision_isRelevant			(CObject* who);
	virtual float			Radius							() const;


	/////////////////////////
	// PDA functions
	/////////////////////////
	void ReceivePdaMessage(u16 who, EPdaMsg msg, EPdaMsgAnger anger);

	/////////////////////////
	// Script 
	/////////////////////////

protected:
	xr_deque<CEntityAction*>	m_tpActionQueue;
public:
	virtual void				UseObject				(const CObject			*tpObject);
	virtual ALife::EStalkerRank	GetRank					() const;
	virtual CWeapon				*GetCurrentWeapon		() const;
	virtual u32					GetWeaponAmmo			() const;
	virtual CInventoryItem		*GetCurrentEquipment	() const;
	virtual CInventoryItem		*GetMedikit				() const;
	virtual CInventoryItem		*GetFood				() const;
	virtual	void				AddAction				(const CEntityAction	*tpEntityAction);
	virtual void				ProcessScripts			();
			CEntityAction		*GetCurrentAction		();
			void				ResetScriptData			(bool			bResetPath = true);
			void				vfAssignMovement		(CEntityAction *tpEntityAction);
			void				vfAssignWatch			(CEntityAction *tpEntityAction);
			void				vfAssignAnimation		(CEntityAction *tpEntityAction);
			void				vfAssignSound			(CEntityAction *tpEntityAction);
			void				vfAssignObject			(CEntityAction *tpEntityAction);
};
