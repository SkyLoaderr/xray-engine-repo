	////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_chimera.h
//	Created 	: 21.05.2003
//  Modified 	: 21.05.2003
//	Author		: Serge Zhem
//	Description : AI Behaviour for all the monsters of class "Chimera"
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "..\\..\\CustomMonster.h"
#include "..\\ai_monsters_misc.h"
#include "ai_chimera_anim.h"
#include "ai_chimera_space.h"
#include "ai_chimera_state.h"
#include "..\\ai_monster_mem.h"


#define SILENCE

#undef	WRITE_TO_LOG
#ifdef SILENCE
#define WRITE_TO_LOG(s) ;

#else
#define WRITE_TO_LOG(s) {\
	Msg("Monster %s : \n* State : %s",cName(),s);\
}
#endif


typedef VisionElem SEnemy;

class CAI_Chimera : public CCustomMonster, 
				   public CChimeraAnimations,
				   public CMonsterMemory
{
	typedef struct tagSHurt {
		CEntity *tpEntity;
		u32	dwTime;
	} SHurt;
	
	typedef struct {
		u32		i_anim;				// анимация аттаки

		TTime	time_started;		//
		TTime	time_from;			// диапазон времени когда можно наносить hit
		TTime	time_to;

		Fvector	TraceFrom;
		float	dist;

		TTime	LastAttack;
		bool	b_fire_anyway;		// трассировка не нужна
		bool	b_attack_rat;

	} SAttackAnimation;

public:

	typedef	CCustomMonster inherited;

							CAI_Chimera		();
	virtual					~CAI_Chimera		();
	virtual	BOOL			renderable_ShadowReceive	()			{ return TRUE;	}
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
			void			vfValidatePosition				(Fvector &tPosition, u32 dwNodeID);
			void			vfUpdateParameters				();
			float			EnemyHeuristics					(CEntity* E);
			
			u32				m_dwPathBuiltLastTime;

			AI_Chimera::EPathState			m_tPathState;
			AI_Chimera::EPathType			m_tPathType;
			AI_Chimera::EPathType			m_tPrevPathType;

			// определения target.yaw
			void			SetDirectionLook				();
			void			SetReversedDirectionLook		();

			int				ifFindHurtIndex					(CEntity *tpEntity);
			void			vfAddHurt						(const SHurt &tHurt);
			void			vfUpdateHurt					(const SHurt &tHurt);

			void			Death							();

			// Eat corpse
			float			CorpHeuristics					(CEntity* E);
			void			DoDamage						(CEntity *pEntity);

			float			m_fGoingSpeed;

// members
public:

	virtual void			feel_sound_new					(CObject* who, int eType, const Fvector &Position, float power);
	virtual	void			feel_touch_new					(CObject* O);

	void OnAnimationEnd();
private:
	SOUND_VECTOR			m_tpSoundDie;
	SOUND_VECTOR			m_tpSoundHit;

	// ALife Simulation
	TERRAIN_VECTOR			m_tpaTerrain;
	u32						m_dwTimeToChange;	// время смены точки графа
	_GRAPH_ID				m_tCurGP;
	_GRAPH_ID				m_tNextGP;
	Fvector					m_tNextGraphPoint;
	// работа с графом
	void					vfUpdateDetourPoint();

	bool					bPeaceful;
	
	// properties
	u32						m_dwHealth;
	u32						m_dwMoraleValue;

	
	
	// время вызовов функции Think()
	u32						m_dwCurrentUpdate;  

	// search and path parameters
	u32							m_dwLastRangeSearch;		// время последнего поиска узла
	bool						m_bIfSearchFailed;			// если поиск ничего не нашёл
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

	// Sound
	float					m_fSoundThreshold;
	
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
	
	bool					A,B,C,D,E,F,G,H,I,J,K,L,M;

	u32						m_dwRandomFactor;
	svector<SHurt,MAX_HURT_COUNT>	m_tpaHurts;

	// eat corpse
	BOOL					m_bCannibalism;
	BOOL					m_bEatMemberCorpses;
	u32						m_dwEatCorpseInterval;
	void					CreateSkeleton();
	virtual void			UpdateCL();
	virtual void			shedule_Update(u32 dt);
	bool					bShowDeath;

	u32						m_dwLieIndex;
	u32						m_dwActionIndex;

	u32						m_dwPointCheckLastTime;
	u32						m_dwPointCheckInterval;

	//////////////////////////////////////////////////////////////////////////
	// Extended FSM
	CChimeraMotion		Motion;
	IState				*CurrentState;
	CChimeraRest		*stateRest;
	CChimeraAttack		*stateAttack;
	CChimeraEat			*stateEat;
	CChimeraHide		*stateHide;
	CChimeraDetour		*stateDetour;
	CChimeraPanic		*statePanic;
	CChimeraExploreDNE	*stateExploreDNE;
	CChimeraExploreDE	*stateExploreDE;
	CChimeraExploreNDE	*stateExploreNDE;

	friend	class CChimeraMotion;
	
	friend	class IState;
	friend	class CChimeraRest;
	friend  class CChimeraAttack;
	friend	class CChimeraEat;
	friend	class CChimeraHide;
	friend	class CChimeraDetour;
	friend	class CChimeraPanic;
	friend	class CChimeraExploreDNE;
	friend	class CChimeraExploreDE;
	friend	class CChimeraExploreNDE;

	void SetState(IState *pS, bool bSkipInertiaCheck = false);
	void ControlAnimation();

	virtual BOOL			feel_vision_isRelevant	(CObject* O);

	// Animations
	EMotionAnim		m_tAnimPrevFrame;
	void			MotionToAnim(EMotionAnim motion, int &index1, int &index2, int &index3);

	SAttackAnimation	m_tAttack;
	void				FillAttackStructure(u32 i, TTime t);

	SEnemy				m_tEnemy;
	SEnemy				m_tEnemyPrevFrame;

	// combat flags 
	bool	flagEnemyDie;
	bool	flagEnemyLostSight;
	bool	flagEnemyGoCloser;
	bool	flagEnemyGoFarther;
	bool	flagEnemyGoCloserFast;
	bool	flagEnemyGoFartherFast;
	bool	flagEnemyStanding;
	bool	flagEnemyDoesntKnowAboutMe;
	bool	flagEnemyHiding;
	bool	flagEnemyRunAway;


};