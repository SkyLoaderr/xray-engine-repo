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
	u32						m_dwLastUpdateTime;  
	u32						m_dwCurrentUpdate;  // Level().timeServer()

	// saved enemy
	SEnemySelected			m_tEnemy;
	CEntity*				m_tSavedEnemy;
	Fvector					m_tSavedEnemyPosition;
	NodeCompressed*			m_tpSavedEnemyNode;
	u32						m_dwSavedEnemyNodeID;
	u32						m_dwLostEnemyTime;			// ����� ������ �����      
	Fvector					m_tMySavedPosition;			// ��������� �������, � ������� � ����� �����
	u32						m_dwMyNodeID;				// ���� ��������� �������, ...

	// search and path parameters
	u32						m_dwLastRangeSearch;		// ����� ���������� ������ ����
	bool					m_bIfSearchFailed;			// ���� ����� ������ �� �����
	xr_vector<Fvector>			m_tpaPoints;
	xr_vector<Fvector>			m_tpaTravelPath;
	xr_vector<u32>				m_tpaPointNodes;
	xr_vector<Fvector>			m_tpaLine;
	xr_vector<u32>				m_tpaNodes;
	xr_vector<Fvector>			m_tpaTempPath;
	u32						m_dwPathTypeRandomFactor;
	CAI_NodeEvaluatorTemplate<aiSearchRange | aiEnemyDistance>			m_tSelectorFreeHunting;
	CAI_NodeEvaluatorTemplate<aiSearchRange | aiCoverFromEnemyWeight | aiEnemyDistance | aiEnemyViewDeviationWeight >	m_tSelectorRetreat;

	
	// ���������� ���� � ��������� ���������� ��������
	// tpPoint - ���� �������� ��� ��������
	void vfSetParameters(IBaseAI_NodeEvaluator *tpNodeEvaluator, Fvector *tpDesiredPosition, 
						 bool bSearchNode, AI_Biting::EPathType tPathType, AI_Biting::EBodyState tBodyState, 
						 AI_Biting::EMovementType tMovementType, AI_Biting::EStateType tStateType,  Fvector *tpPoint = 0);
	
	
	// Animation Parameters
	AI_Biting::EActionAnim		m_tActionAnim;
	AI_Biting::EPostureAnim		m_tPostureAnim;

	void vfSetMotionActionParams(AI_Biting::EBodyState, AI_Biting::EMovementType, AI_Biting::EMovementDir, AI_Biting::EStateType, AI_Biting::EActionType);
	void vfSetAnimation();

	void SetDirectionLook();

};