	////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_biting.h
//	Created 	: 21.05.2003
//  Modified 	: 21.05.2003
//	Author		: Serge Zhem
//	Description : AI Behaviour for all the monsters of class "Biting"
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "..\\..\\CustomMonster.h"
#include "..\\ai_monsters_misc.h"
#include "ai_biting_anim.h"
#include "ai_biting_space.h"
#include "ai_biting_state.h"
#include "..\\ai_monster_mem.h"


class CAI_Biting : public CCustomMonster, 
				   public CBitingAnimations,
				   public CMonsterMemory
{
	typedef struct tagSHurt {
		CEntity *tpEntity;
		u32	dwTime;
	} SHurt;

	enum ESoundCcount {
		SND_HIT_COUNT=1,
		SND_DIE_COUNT=1,
		SND_ATTACK_COUNT=1,
		SND_VOICE_COUNT=2,
	};

	typedef struct {
		u32		i_anim;				// �������� ������

		TTime	time_started;		//
		TTime	time_from;			// �������� ������� ����� ����� �������� hit
		TTime	time_to;

		Fvector	TraceFrom;
		float	dist;

		TTime	LastAttack;

	} SAttackAnimation;

public:

	typedef	CCustomMonster inherited;

							CAI_Biting		();
	virtual					~CAI_Biting		();
	virtual	BOOL			renderable_ShadowReceive	()			{ return FALSE;	}
	virtual void			Die				();
	virtual void			HitSignal		(float amount, Fvector& vLocalDir, CObject* who, s16 element);
	virtual void			SelectAnimation	(const Fvector& _view, const Fvector& _move, float speed );

	virtual void			Load			(LPCSTR section);

	// network routines
	virtual BOOL			net_Spawn		(LPVOID DC);
	virtual void			net_Destroy		();
	virtual void			net_Export		(NET_Packet& P);
	virtual void			net_Import		(NET_Packet& P);
	virtual void			Exec_Movement	(float dt);

	virtual void			Think			();

private:			
			void			Init							();
			void			vfInitSelector					(IBaseAI_NodeEvaluator &S, CSquad &Squad);
			void			vfSearchForBetterPosition		(IBaseAI_NodeEvaluator &tNodeEvaluator, CSquad &Squad, CEntity* &Leader);
			void			vfBuildPathToDestinationPoint	(IBaseAI_NodeEvaluator *tpNodeEvaluator);
			void			vfBuildTravelLine				(Fvector *tpDestinationPosition);
			void			vfChoosePointAndBuildPath		(IBaseAI_NodeEvaluator *tpNodeEvaluator, Fvector *tpDestinationPosition, bool bSearchForNode, bool bSelectorPath = false, u32 TimeToRebuild = 0);
			void			vfChooseNextGraphPoint			();

			u32				m_dwPathBuiltLastTime;


			void			vfSaveEnemy						();
			void			vfValidatePosition				(Fvector &tPosition, u32 dwNodeID);
			void			vfUpdateParameters				();
			float			EnemyHeuristics					(CEntity* E);
			// ���������� ���� � ��������� ���������� ��������
			// tpPoint - ���� �������� ��� ��������
			void			vfSetParameters					(AI_Biting::EPathType path_type,IBaseAI_NodeEvaluator *tpNodeEvaluator, Fvector *tpDesiredPosition, bool bSearchNode, Fvector *tpPoint = 0, bool moveback=false, bool bSelectorPath = false);
			void			vfSetMotionActionParams			(AI_Biting::EBodyState, AI_Biting::EMovementType, AI_Biting::EMovementDir, AI_Biting::EStateType, AI_Biting::EActionType);
			void			vfSetAnimation					(bool bForceChange = false);
			void			vfAssignPitch					();

			void			SetReversedDirectionLook		();
			int				ifFindHurtIndex					(CEntity *tpEntity);
			void			vfAddHurt						(const SHurt &tHurt);
			void			vfUpdateHurt					(const SHurt &tHurt);

			void			vfSetFireBones					(CInifile *ini, const char *section);
			
			// Eat corpse
			float			CorpHeuristics					(CEntity* E);


			void			DoDamage						(CEntity *pEntity);
///////////////////////////////////////////////////////////////////////////////
			void			CreateSkeleton					();

// members
public:

	virtual void			feel_sound_new					(CObject* who, int eType, const Fvector &Position, float power);
	virtual	void			feel_touch_new					(CObject* O);

	bool					m_bActionFinished;
	bool					bPlayDeath;
	bool					bStartPlayDeath;
	

	void OnAnimationEnd();
private:
	SOUND_VECTOR			m_tpSoundDie;
	SOUND_VECTOR			m_tpSoundHit;

	// ALife Simulation
	TERRAIN_VECTOR			m_tpaTerrain;
	u32						m_dwTimeToChange;	// ����� ����� ����� �����
	_GRAPH_ID				m_tCurGP;
	_GRAPH_ID				m_tNextGP;
	Fvector					m_tNextGraphPoint;
	// ������ � ������
	void					vfUpdateDetourPoint();

	float					m_fGoingSpeed;			

	// Fields
	bool					bPeaceful;
	
	// properties
	u32						m_dwHealth;
	u32						m_dwMoraleValue;

	
	/////////////////////////////////////////////////////
	AI_Biting::EMovementType		m_tMovementType;
	AI_Biting::EBodyState			m_tBodyState;
	AI_Biting::EStateType			m_tStateType;
	AI_Biting::EMovementDir			m_tMovementDir;
	AI_Biting::EActionType			m_tActionType;

	AI_Biting::EPathState			m_tPathState;
	AI_Biting::EPathType			m_tPathType;
	AI_Biting::EPathType			m_tPrevPathType;
	

	
	float					m_fWalkFactor;
	float					m_fWalkFreeFactor;

	// ����� ������� ������� Think()
 

	// search and path parameters
	u32							m_dwLastRangeSearch;		// ����� ���������� ������ ����
	bool						m_bIfSearchFailed;			// ���� ����� ������ �� �����
	xr_vector<Fvector>			m_tpaPoints;
	xr_vector<Fvector>			m_tpaTravelPath;
	xr_vector<u32>				m_tpaPointNodes;
	xr_vector<Fvector>			m_tpaLine;
	xr_vector<u32>				m_tpaNodes;
	xr_vector<Fvector>			m_tpaTempPath;
	u32							m_dwPathTypeRandomFactor;
	CAI_NodeEvaluatorTemplate<aiSearchRange | aiEnemyDistance>			m_tSelectorFreeHunting;
	CAI_NodeEvaluatorTemplate<aiSearchRange | aiCoverFromEnemyWeight | aiEnemyDistance | aiEnemyViewDeviationWeight >	m_tSelectorRetreat;
	CAI_NodeEvaluatorTemplate<aiSearchRange | aiCoverFromEnemyWeight | aiEnemyDistance>	m_tSelectorCover;
	u32							m_dwAnimFrameDelay;
	u32							m_dwAnimLastSetTime;

	//////////////////////////////////////////////////////////////////////////
	// FSM
	xr_vector<CObject*>		m_tpaVisibleObjects;	// ������ ������� ��������

	// Sound
//	SSimpleSound			m_tLastSound;
//	u32						m_dwLastSoundNodeID;
//	AI_Biting::ESoundType	SndType;
	float					m_fSoundThreshold;
	
	/////////////////////////////////////////////////////////////////////////////////////////
	AI_Biting::EActionState m_tActionState;

	// HIT
	u32						m_dwHitTime;
	Fvector					m_tHitDir;
	Fvector					m_tHitPosition;
	float					m_fHitPower;
	u32						m_dwHitInterval;

	float					m_fAttackSuccessProbability0;
	float					m_fAttackSuccessProbability1;
	float					m_fAttackSuccessProbability2;
	float					m_fAttackSuccessProbability3;
	
	bool					_A,_B,_C,_D,_E,_F,_G,_H,_I,_J,_K,_L,_M;
	bool					A,B,C,D,E,F,G,H,I,J,K,L,M;

	u32						m_dwInertion;							// ������� ���������
	u32						m_dwActionStartTime;
	bool					m_bStateChanged;
	float					m_ls_yaw;
	u32						m_dwRandomFactor;
	svector<SHurt,MAX_HURT_COUNT>	m_tpaHurts;
	u32						m_dwLostEnemyTime;

	// eat corpse
	BOOL					m_bCannibalism;
	BOOL					m_bEatMemberCorpses;
	u32						m_dwEatCorpseInterval;
	
	void					SetText();
	virtual void			UpdateCL();
	virtual void			shedule_Update(u32 dt);
	bool					IsLeftSide(const Fvector &Position);
	bool					IsLeftSide(float current_yaw,float target_yaw);
	bool					bTurning;

	
	// SOUNDS
	ref_sound					m_tpaSoundHit[SND_HIT_COUNT];
	ref_sound					m_tpaSoundDie[SND_DIE_COUNT];
	ref_sound					m_tpaSoundAttack[SND_ATTACK_COUNT];
	ref_sound					m_tpaSoundVoice[SND_VOICE_COUNT];
	ref_sound*					m_tpSoundBeingPlayed;
	u32						m_dwLastSoundRefresh;
	float					m_fMinVoiceIinterval;
	float					m_fMaxVoiceIinterval;
	float					m_fVoiceRefreshRate;
	u32						m_dwLastVoiceTalk;

	void					vfLoadSounds();
	bool					bShowDeath;


	u32						m_dwLastTimeEat;
	u32						m_dwEatInterval;


	u32						m_dwLieIndex;
	u32						m_dwActionIndex;

	u32						m_dwPointCheckLastTime;
	u32						m_dwPointCheckInterval;

	u32						m_AttackLastTime;			// ��������� ����� ������
	u32						m_AttackInterval;
	Fvector					m_AttackLastPosition;		// ��������� ������� ����� �� ����� ������
	
	// Fire bone indexes
	u32		m_iLeftFireBone;
	u32		m_iRightFireBone;


	u32		m_dwAttackMeleeTime;
	u32		m_dwAttackActorMeleeTime;
	bool	AttackMelee(CObject *obj,bool bAttackRat);


	// Extended FSM
	CBitingMotion		Motion;
	IState				*CurrentState;
	CBitingRest			*stateRest;
	CBitingAttack		*stateAttack;
	CBitingEat			*stateEat;
	CBitingHide			*stateHide;
	CBitingDetour		*stateDetour;
	CBitingPanic		*statePanic;
	CBitingExploreDNE	*stateExploreDNE;
	CBitingExploreDE	*stateExploreDE;
	CBitingExploreNDE	*stateExploreNDE;

	friend	class CBitingMotion;
	
	friend	class IState;
	friend	class CBitingRest;
	friend  class CBitingAttack;
	friend	class CBitingEat;
	friend	class CBitingHide;
	friend	class CBitingDetour;
	friend	class CBitingPanic;
	friend	class CBitingExploreDNE;
	friend	class CBitingExploreDE;
	friend	class CBitingExploreNDE;


	void SetState(IState *pS, bool bSkipInertiaCheck = false);
	
	void ControlAnimation();

	virtual BOOL			feel_vision_isRelevant	(CObject* O);

	// Animations
	//EMotionAnim		m_tAnim;
	EMotionAnim		m_tAnimPrevFrame;
	void			MotionToAnim(EMotionAnim motion, int &index1, int &index2, int &index3);

	SAttackAnimation	m_tAttack;
	void				FillAttackStructure(u32, TTime);
};