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

typedef struct {
	AI_Biting::EMotionType motion;
	Fvector dest;
	float angle;

	u32 time;				// время нахождения в этом состоянии (in ms)
	u32 time_started;

} TMotionState;


class CAI_Biting : public CCustomMonster, public CBitingAnimations
{
public:
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

	virtual void			Think			();


//	virtual float EnemyHeuristics(CEntity* E);
//	virtual void  SelectEnemy(SEnemySelected& S);
//	virtual objQualifier* GetQualifier();
//	virtual	void  feel_sound_new(CObject* who, int type, const Fvector &Position, float power);
//	virtual void  Hit(float P,Fvector &dir,CObject*who,s16 element,Fvector p_in_object_space,float impulse);

private:			
			void			Init			();
			void			vfInitSelector	(IBaseAI_NodeEvaluator &S, CSquad &Squad);
			void			vfSearchForBetterPosition(IBaseAI_NodeEvaluator &tNodeEvaluator, CSquad &Squad, CEntity* &Leader);
			void			vfBuildPathToDestinationPoint(IBaseAI_NodeEvaluator *tpNodeEvaluator);
			void			vfBuildTravelLine(Fvector *tpDestinationPosition);
			void			vfChoosePointAndBuildPath(IBaseAI_NodeEvaluator *tpNodeEvaluator, Fvector *tpDestinationPosition, bool bSearchForNode);
			void			vfChooseNextGraphPoint();
			void			vfSaveEnemy		();
			void			vfValidatePosition(Fvector &tPosition, u32 dwNodeID);

// members
public:

private:
	SOUND_VECTOR			m_tpSoundDie;
	SOUND_VECTOR			m_tpSoundHit;

	// ALife Simulation
	TERRAIN_VECTOR			m_tpaTerrain;
	u32						m_dwTimeToChange;
	_GRAPH_ID				m_tCurGP;
	_GRAPH_ID				m_tNextGP;
	float					m_fGoingSpeed;			

	// Fields
	bool					bPeaceful;
	
	// properties
	u32						m_dwHealth;
	u32						m_dwMoraleValue;

	
	/////////////////////////////////////////////////////
	AI_Biting::EMovementType		m_tMovementType;
	AI_Biting::EStateType			m_tStateType;
	AI_Biting::EMovementDirection	m_tMovementDir;
	AI_Biting::EPathState			m_tPathState;
	AI_Biting::EPathType			m_tPathType;
	AI_Biting::EPathType			m_tPrevPathType;
	
	float					m_fWalkFactor;
	float					m_fWalkFreeFactor;

	// common parameters
	u32						m_dwLastUpdateTime;  
	u32						m_dwCurrentUpdate;  // Level().timeServer()

	// saved enemy
	SEnemySelected			m_tEnemy;
	CEntity*				m_tSavedEnemy;
	Fvector					m_tSavedEnemyPosition;
	NodeCompressed*			m_tpSavedEnemyNode;
	u32						m_dwSavedEnemyNodeID;
	u32						m_dwLostEnemyTime;			// время потери врага      
	Fvector					m_tMySavedPosition;			// последняя позиция, в которой я видел врага
	u32						m_dwMyNodeID;				// нода последней позиции, ...

	// search and path parameters
	u32						m_dwLastRangeSearch;		// время последнего поиска узла
	bool					m_bIfSearchFailed;			// если поиск ничего не нашёл
	vector<Fvector>			m_tpaPoints;
	vector<Fvector>			m_tpaTravelPath;
	vector<u32>				m_tpaPointNodes;
	vector<Fvector>			m_tpaLine;
	vector<u32>				m_tpaNodes;
	vector<Fvector>			m_tpaTempPath;
	u32						m_dwPathTypeRandomFactor;
	CAI_NodeEvaluatorTemplate<aiSearchRange | aiEnemyDistance>			m_tSelectorFreeHunting;

//	void vfSetParameters(IBaseAI_NodeEvaluator *tpNodeEvaluator, Fvector *tpDesiredPosition, bool bSearchNode, EWeaponState tWeaponState, EPathType tPathType, EBodyState tBodyState, EMovementType tMovementType, EStateType tStateType, ELookType tLookType, Fvector &tPointToLook, u32 dwLookOverDelay);

	// Animation Parameters
	AI_Biting::EActionAnim		m_tActionAnim;
	AI_Biting::EPostureAnim		m_tPostureAnim;
	void				TurnAround();  // TEMP
	bool				ExecMovement(TMotionState &motion);

	void SetDirectionLook();		// TEMP
};