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

class CAI_Biting : public CCustomMonster, public CBitingAnimations
{
public:
	typedef struct tagSHurt {
		CEntity *tpEntity;
		u32	dwTime;
	} SHurt;

	typedef	CCustomMonster inherited;

							CAI_Biting		();
	virtual					~CAI_Biting		();
	virtual	BOOL			ShadowReceive	()			{ return FALSE;	}
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


//	virtual float EnemyHeuristics(CEntity* E);
	
//	virtual objQualifier* GetQualifier();
//	virtual	void  feel_sound_new(CObject* who, int type, const Fvector &Position, float power);
//	virtual void  Hit(float P,Fvector &dir,CObject*who,s16 element,Fvector p_in_object_space,float impulse);

private:			
			void			Init							();
			void			vfInitSelector					(IBaseAI_NodeEvaluator &S, CSquad &Squad);
			void			vfSearchForBetterPosition		(IBaseAI_NodeEvaluator &tNodeEvaluator, CSquad &Squad, CEntity* &Leader);
			void			vfBuildPathToDestinationPoint	(IBaseAI_NodeEvaluator *tpNodeEvaluator);
			void			vfBuildTravelLine				(Fvector *tpDestinationPosition);
			void			vfChoosePointAndBuildPath		(IBaseAI_NodeEvaluator *tpNodeEvaluator, Fvector *tpDestinationPosition, bool bSearchForNode);
			void			vfChooseNextGraphPoint			();
			void			vfSaveEnemy						();
			void			vfValidatePosition				(Fvector &tPosition, u32 dwNodeID);
			void			vfUpdateParameters				();
			void			SelectEnemy						(SEnemySelected& S);
			float			EnemyHeuristics					(CEntity* E);
			void			vfSetMotionActionParams			(AI_Biting::EBodyState, AI_Biting::EMovementType, AI_Biting::EMovementDir, AI_Biting::EStateType, AI_Biting::EActionType);
			void			vfSetAnimation					(bool bForceChange = false);

			void			SetDirectionLook				();
			void			SetReversedDirectionLook		();
			int				ifFindHurtIndex					(CEntity *tpEntity);
			void			vfAddHurt						(const SHurt &tHurt);
			void			vfUpdateHurt					(const SHurt &tHurt);

			void			Death							();
			void			BackCover						(bool bFire = true);
			void			ForwardStraight					();
			void			Panic							();
			void			Hide							();
			void			Detour							();
			void			ExploreDE						();
			void			ExploreDNE						();
			void			ExploreNDE						();
			void			ExploreNDNE						();
			void			AccomplishTask					(IBaseAI_NodeEvaluator *tpNodeEvaluator = 0);

			// Eat corpse
			void			SelectCorp						(SEnemySelected& S);
			float			CorpHeuristics					(CEntity* E);

			

// members
public:

	virtual void			feel_sound_new					(CObject* who, int eType, const Fvector &Position, float power);
	virtual objQualifier*	GetQualifier					();
private:
	SOUND_VECTOR			m_tpSoundDie;
	SOUND_VECTOR			m_tpSoundHit;

	// ALife Simulation
	TERRAIN_VECTOR			m_tpaTerrain;
	u32						m_dwTimeToChange;	// врем€ смены точки графа
	_GRAPH_ID				m_tCurGP;
	_GRAPH_ID				m_tNextGP;
	Fvector					m_tNextGraphPoint;
	// работа с графом
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

	// врем€ вызовов функции Think()
	u32						m_dwLastUpdateTime;  
	u32						m_dwCurrentUpdate;  // Level().timeServer()

	// saved enemy
	SEnemySelected			m_tEnemy;
	CEntity*				m_tSavedEnemy;
	Fvector					m_tSavedEnemyPosition;
	NodeCompressed*			m_tpSavedEnemyNode;
	u32						m_dwSavedEnemyNodeID;
	u32						m_dwSeenEnemyLastTime;		// врем€, когда последний раз видел врага
	Fvector					m_tMySavedPosition;			// мо€ последн€€ позици€, в которой € видел врага
	u32						m_dwMyNodeID;				// мо€ нода последней позиции, ...

	// search and path parameters
	u32						m_dwLastRangeSearch;		// врем€ последнего поиска узла
	bool					m_bIfSearchFailed;			// если поиск ничего не нашЄл
	xr_vector<Fvector>			m_tpaPoints;
	xr_vector<Fvector>			m_tpaTravelPath;
	xr_vector<u32>				m_tpaPointNodes;
	xr_vector<Fvector>			m_tpaLine;
	xr_vector<u32>				m_tpaNodes;
	xr_vector<Fvector>			m_tpaTempPath;
	u32							m_dwPathTypeRandomFactor;
	CAI_NodeEvaluatorTemplate<aiSearchRange | aiEnemyDistance>			m_tSelectorFreeHunting;
	CAI_NodeEvaluatorTemplate<aiSearchRange | aiCoverFromEnemyWeight | aiEnemyDistance | aiEnemyViewDeviationWeight >	m_tSelectorRetreat;

	
	// построение пути и установка параметров скорости
	// tpPoint - куда смотреть при движении
	void vfSetParameters(IBaseAI_NodeEvaluator *tpNodeEvaluator, Fvector *tpDesiredPosition, bool bSearchNode, Fvector *tpPoint = 0, bool moveback=false);
	
	
	// Animation Parameters
	AI_Biting::EActionAnim		m_tActionAnim;
	AI_Biting::EPostureAnim		m_tPostureAnim;

	//////////////////////////////////////////////////////////////////////////
	// FSM
	objSET					m_tpaVisibleObjects;	// массив видимых объектов

	// Sound
	SSimpleSound			m_tLastSound;
	u32						m_dwLastSoundNodeID;
	AI_Biting::ESoundType	SndType;
	float					m_fSoundThreshold;
	
	bool					bAnimCanBeNew;

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

	u32						m_dwInertion;
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
	// saved corpse
	SEnemySelected			m_tCorpse;
};