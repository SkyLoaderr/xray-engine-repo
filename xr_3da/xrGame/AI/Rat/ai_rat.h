////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_rat.h
//	Created 	: 23.04.2002
//  Modified 	: 26.11.2002
//	Author		: Dmitriy Iassenev
//	Description : AI Behaviour for monster "Rat"
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "../../CustomMonster.h"
#include "../../eatable_item.h"

class CAI_Rat : public CCustomMonster, public CEatableItem
{
	typedef	CCustomMonster inherited;
	protected:
		//////////////////////////
		// STRUCTURES
		//////////////////////////
		#define TIME_TO_GO		2000
		#define TIME_TO_RETURN	 500

		enum ESoundCcount {
			SND_HIT_COUNT=1,
			SND_DIE_COUNT=1,
			SND_ATTACK_COUNT=1,
			SND_VOICE_COUNT=2,
		};

		enum ERatStates 	{
			aiRatDie = 0,
			aiRatTurn,
			aiRatFreeHuntingActive,
			aiRatFreeHuntingPassive,
			aiRatAttackFire,
			aiRatAttackRun,
			aiRatUnderFire,
			aiRatRetreat,
			aiRatPursuit,
			aiRatFreeRecoil,
			aiRatReturnHome,
			aiRatEatCorpse,
		};



		typedef struct tagSNormalGlobalAnimations{
			CMotionDef *tpaDeath[2];
			CMotionDef *tpaAttack[3];
			CMotionDef *tpaIdle[2];
			SAnimState  tWalk;
			SAnimState  tRun;
			CMotionDef *tRunAttack;
			CMotionDef *tpTurnLeft;
			CMotionDef *tpTurnRight;
		}SNormalGlobalAnimations;

		// normal animations
		typedef struct tagSNormalAnimations{
			SNormalGlobalAnimations tGlobal;
		}SNormalAnimations;

		typedef struct tagSRatAnimations{
			SNormalAnimations	tNormal;
		}SRatAnimations;
		//////////////////////////
		// END OF STRUCTURES
		//////////////////////////

		//////////////////////////
		// CLASS MEMBERS
		//////////////////////////

		// Graph
		enum EZombieAction {
			eRatActionNone = u32(0),
			eRatActionAttackBegin,
			eRatActionAttackEnd,
		};

		EZombieAction			m_tAction;
		ALife::_GRAPH_ID		m_tCurGP;
		ALife::_GRAPH_ID		m_tNextGP;
		u32						m_dwTimeToChange;
		float					m_fGoingSpeed;

		// FSM
		xr_stack<ERatStates>	m_tStateStack;
		ERatStates				m_eCurrentState;
		ERatStates				m_ePreviousState;
		bool					m_bStopThinking;
		bool					m_bStateChanged;

		// ANIMATIONS
		SRatAnimations		m_tRatAnimations;
		CMotionDef*			m_tpCurrentGlobalAnimation;
		CBlend*				m_tpCurrentGlobalBlend;
		
		// SOUNDS
		ref_sound			m_tpaSoundHit[SND_HIT_COUNT];
		ref_sound			m_tpaSoundDie[SND_DIE_COUNT];
		ref_sound			m_tpaSoundAttack[SND_ATTACK_COUNT];
		ref_sound			m_tpaSoundVoice[SND_VOICE_COUNT];
		ref_sound*			m_tpSoundBeingPlayed;
		u32					m_dwLastSoundRefresh;
		float				m_fMinVoiceIinterval;
		float				m_fMaxVoiceIinterval;
		float				m_fVoiceRefreshRate;
		u32					m_dwLastVoiceTalk;
		
		// ATTACK
		bool				m_bActionStarted;
		bool				m_bFiring;
		u32					m_dwStartAttackTime;
		float				m_fAttackSpeed;
		// HIT
		u32					m_hit_time;
		Fvector				m_hit_direction;
		Fvector				m_tHitPosition;
		float				m_fHitPower;
		u32					m_dwHitInterval;
		//HIT PHYS
		float				m_saved_impulse;
		Fvector				m_saved_hit_position;
		Fvector				m_saved_hit_dir;
		//PHYS
		float				m_phMass;

		typedef struct tagSSimpleSound {
			ESoundTypes		eSoundType;
			u32				dwTime;
			float			fPower;
			Fvector			tSavedPosition;
			CEntityAlive	*tpEntity;
		} SSimpleSound;

		float				m_fMaxSpeed;
		float				m_fMinSpeed;
		// SOUND BEING FELT
		SSimpleSound		m_tLastSound;
		
		// BEHAVIOUR
		Fvector				m_tGoalDir;
		Fvector				m_tCurrentDir;
		Fvector				m_tHPB;
		float				m_fDHeading;

		// constants
		float				m_fGoalChangeDelta;
		float				m_fSafeSpeed;
		float				m_fASpeed;
		Fvector				m_tVarGoal;
		float				m_fIdleSoundDelta;
		Fvector				m_tSpawnPosition;
		Fvector				m_tSafeSpawnPosition;
		float				m_fAngleSpeed;
		float				m_fSafeGoalChangeDelta;
		Fvector				m_tGoalVariation;
		float				m_fNullASpeed;
		float				m_fMinASpeed;
		float				m_fMaxASpeed;
		float				m_fAttackASpeed;

		// variables
		float				m_fGoalChangeTime;
		Fvector				m_tOldPosition;
		bool				m_bNoWay;

		// Morale
		float				m_fMoraleSuccessAttackQuant;
		float				m_fMoraleDeathQuant;
		float				m_fMoraleFearQuant;
		float				m_fMoraleRestoreQuant;
		u32					m_dwMoraleRestoreTimeInterval;
		u32					m_dwMoraleLastUpdateTime;
		float				m_fMoraleMinValue;
		float				m_fMoraleMaxValue;
		float				m_fMoraleNormalValue;
		float				m_fMoraleDeathDistance;

		// active
		float				m_fChangeActiveStateProbability;
		u32					m_dwActiveCountPercent;
		u32					m_dwActiveScheduleMin;
		u32					m_dwActiveScheduleMax;
		u32					m_dwPassiveScheduleMin;
		u32					m_dwPassiveScheduleMax;
		u32					m_dwStandingCountPercent;
		bool				m_bStanding;
		bool				m_bActive;

		// attack parameters
		float				m_fAttackDistance;
		float				m_fAttackAngle;
		float				m_fMaxPursuitRadius;
		float				m_fMaxHomeRadius;

		// DDD
		u32					m_dwActionRefreshRate;
		float				m_fAttackSuccessProbability;

		// former constants
		u32					m_dwLostMemoryTime;
		u32					m_dwLostRecoilTime;
		float				m_fUnderFireDistance;
		u32					m_dwRetreatTime;
		float				m_fRetreatDistance;
		float				m_fAttackStraightDistance;
		float				m_fStableDistance;
		float				m_fWallMinTurnValue;
		float				m_fWallMaxTurnValue;
		float				m_fSoundThreshold;

		// eat troops
		BOOL				m_bEatMemberCorpses;
		BOOL				m_bCannibalism;
		u32					m_dwEatCorpseInterval;
		u32					m_previous_query_time;
public:
		float				m_fSpeed;
		bool				m_bMoving;
		bool				m_bCanAdjustSpeed;
		bool				m_bStraightForward;

	IC	void	vfChangeGoal			();
	IC	bool	bfCheckIfGoalChanged	(bool bForceChangeGoal = true);
	IC	void	vfChooseNewSpeed		();
	IC	void	vfUpdateTime			(float fTimeDelta);
	IC	void	vfAddActiveMember		(bool bForceActive = false);
	IC	void	vfRemoveActiveMember	();
	IC	void	vfAddStandingMember		();
	IC	void	vfRemoveStandingMember	();
	IC	bool	bfCheckIfSoundFrightful	();
	IC	bool	bfCheckIfOutsideAIMap	(Fvector &tTemp1);
		//////////////////////////
		// MISCELLANIOUS FUNCTIONS
		//////////////////////////
		void	vfAdjustSpeed();
		bool	bfComputeNewPosition(bool bCanAdjustSpeed = true, bool bStraightForward = false);
//		float	CorpHeuristics(CEntity* E);
//		void	SelectCorp(CEnemy& S);
		void	vfUpdateMoraleBroadcast(float fValue, float fRadius);
		void	vfUpdateMorale();
		void	vfLoadSounds();
		void	vfLoadAnimations();
		void	SetDirectionLook();
		void	vfAimAtEnemy();
		void	vfSetFire(bool bFire, CGroup &Group);
		void	vfSetMovementType(float fSpeed);
		void	vfUpdateDynamicObjects() {};
		void	CreateSkeleton();
		void	vfUpdateSpawnPosition();
		void	vfChooseNextGraphPoint();
		//////////////////////////
		// FSM STATES
		//////////////////////////
		void	Death();
		void	FreeHuntingActive();
		void	FreeHuntingPassive();
		void	AttackFire();
		void	AttackRun();
		void	Turn();
		void	UnderFire();
		void	Retreat();
		void	Pursuit();
		void	FreeRecoil();
		void	ReturnHome();
		void	EatCorpse();
		void	Init();
	public:
					   CAI_Rat();
		virtual		  ~CAI_Rat();
		virtual BOOL  renderable_ShadowReceive	()			{ return TRUE;	}
		virtual BOOL  net_Spawn(LPVOID DC);
		virtual void  net_Destroy();
		virtual void  net_Export(NET_Packet& P);
		virtual void  net_Import(NET_Packet& P);
		virtual void  HitSignal(float amount, Fvector& vLocalDir, CObject* who, s16 element);
		virtual void  Die();
		virtual void  Load( LPCSTR section );
		virtual void  Think();
//		virtual float EnemyHeuristics(CEntity* E);
//		virtual void  SelectEnemy(CEnemy& S);
		virtual void  SelectAnimation( const Fvector& _view, const Fvector& _move, float speed );
		virtual void  Exec_Action(float dt);
		virtual	void  feel_sound_new(CObject* who, int type, const Fvector &Position, float power);
		virtual void  feel_touch_new(CObject* O);
		virtual void  shedule_Update(u32 dt);
		virtual void  UpdateCL();
		virtual void  Hit(float P,Fvector &dir,CObject*who,s16 element,Fvector p_in_object_space,float impulse, ALife::EHitType hit_type = ALife::eHitTypeWound);
		virtual BOOL  feel_vision_isRelevant(CObject*);

		/////////////////////////////////////
		//rat as eatable item
		virtual void OnHUDDraw			(CCustomHUD* hud) {inherited::OnHUDDraw(hud);}
		virtual void OnH_B_Chield		();
		virtual void OnH_B_Independent	();
		virtual void OnEvent			(NET_Packet& P, u16 type) {inherited::OnEvent(P,type);}
		virtual void renderable_Render	() {inherited::renderable_Render();}
		virtual bool Useful				() const;
		virtual BOOL UsedAI_Locations	();
		///////////////////////////////////////////////////////////////////////
		virtual u16					PHGetSyncItemsNumber	()			{return inherited ::PHGetSyncItemsNumber();}
		virtual CPHSynchronize*		PHGetSyncItem			(u16 item)	{return inherited ::PHGetSyncItem(item);}
		virtual void				PHUnFreeze				()			{return inherited ::PHUnFreeze();}
		virtual void				PHFreeze				()			{return inherited ::PHFreeze();}
		///////////////////////////////////////////////////////////////////////
#ifdef DEBUG
		virtual void OnRender			();
#endif
		virtual bool				useful					(const CGameObject *object) const;
		virtual	float				evaluate				(const CGameObject *object) const;
		virtual	void				reinit					();
		virtual void				reload					(LPCSTR	section);
		virtual const SRotation		Orientation				() const
		{
			return					(inherited::Orientation());
		};
public:
		virtual void				make_Interpolation	();
		virtual void				PH_B_CrPr			(); // actions & operations before physic correction-prediction steps
		virtual void				PH_I_CrPr			(); // actions & operations after correction before prediction steps
		virtual void				PH_A_CrPr			(); // actions & operations after phisic correction-prediction steps
		virtual void				OnH_A_Chield		();
		virtual void				create_physic_shell	();
		virtual void				setup_physic_shell	();
		virtual void				activate_physic_shell();
		virtual	Feel::Sound*		dcast_FeelSound		()			{ return this;	}
};

#include "ai_rat_inline.h"