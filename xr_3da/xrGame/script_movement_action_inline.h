////////////////////////////////////////////////////////////////////////////
//	Module 		: script_movement_action_inline.h
//	Created 	: 30.09.2003
//  Modified 	: 29.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Script movement action class inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	CScriptMovementAction::CScriptMovementAction		()
{
	SetInputKeys		(eInputKeyNone);
	SetBodyState		(MonsterSpace::eBodyStateStand);
	SetMovementType		(MonsterSpace::eMovementTypeStand);
	SetPathType			(DetailPathManager::eDetailPathTypeSmooth);
	SetPatrolPath		(0,"");
	SetPatrolStart		(PatrolPathManager::ePatrolStartTypeNearest);
	SetPatrolStop		(PatrolPathManager::ePatrolRouteTypeContinue);
	SetPatrolRandom		(true);
	SetSpeed			(0);
	SetObjectToGo		(0);
	SetPosition			(Fvector().set(0,0,0));
	m_tGoalType			= eGoalTypeDummy;
	m_bCompleted		= true;
}

IC	CScriptMovementAction::CScriptMovementAction		(MonsterSpace::EBodyState tBodyState, MonsterSpace::EMovementType tMovementType, DetailPathManager::EDetailPathType tPathType, CScriptGameObject *tpObjectToGo, float fSpeed)
{
	SetBodyState		(tBodyState);
	SetMovementType		(tMovementType);
	SetPathType			(tPathType);
	SetObjectToGo		(tpObjectToGo);
	SetSpeed			(fSpeed);
}

IC	CScriptMovementAction::CScriptMovementAction		(MonsterSpace::EBodyState tBodyState, MonsterSpace::EMovementType tMovementType, DetailPathManager::EDetailPathType tPathType, const CPatrolPathParams &tPatrolPathParams, float fSpeed)
{
	SetBodyState		(tBodyState);
	SetMovementType		(tMovementType);
	SetPathType			(tPathType);
	SetPatrolPath		(tPatrolPathParams.m_path,tPatrolPathParams.m_path_name);
	SetPatrolStart		(tPatrolPathParams.m_tPatrolPathStart);
	SetPatrolStop		(tPatrolPathParams.m_tPatrolPathStop);
	SetPatrolRandom		(tPatrolPathParams.m_bRandom);
	SetSpeed			(fSpeed);
}

IC	CScriptMovementAction::CScriptMovementAction		(MonsterSpace::EBodyState tBodyState, MonsterSpace::EMovementType tMovementType, DetailPathManager::EDetailPathType tPathType, const Fvector &tPosition, float fSpeed)
{
	SetBodyState		(tBodyState);
	SetMovementType		(tMovementType);
	SetPathType			(tPathType);
	SetPosition			(tPosition);
	SetSpeed			(fSpeed);
}

IC	CScriptMovementAction::CScriptMovementAction		(const Fvector &tPosition, float fSpeed)
{
	SetBodyState		(MonsterSpace::eBodyStateStand);
	SetMovementType		(MonsterSpace::eMovementTypeStand);
	SetPathType			(DetailPathManager::eDetailPathTypeSmooth);
	SetPosition			(tPosition);
	SetSpeed			(fSpeed);
	m_tGoalType			= eGoalTypeNoPathPosition;
}

IC	CScriptMovementAction::CScriptMovementAction		(const EInputKeys tInputKeys, float fSpeed)
{
	SetInputKeys		(tInputKeys);
	SetSpeed			(fSpeed);
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------						
// Monsters
// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
IC	CScriptMovementAction::CScriptMovementAction		(MonsterSpace::EScriptMonsterMoveAction tAct, Fvector &tPosition, float	dist_to_end, MonsterSpace::EScriptMonsterSpeedParam speed_param)
{																																			
	m_tMoveAction		= tAct;
	SetPosition			(tPosition);																										
	m_tSpeedParam		= speed_param;
	m_fDistToEnd		= dist_to_end;
}						

IC	CScriptMovementAction::CScriptMovementAction		(MonsterSpace::EScriptMonsterMoveAction tAct, CPatrolPathParams &tPatrolPathParams, float dist_to_end, MonsterSpace::EScriptMonsterSpeedParam speed_param)
{																																			
	m_tMoveAction			= tAct;
	SetPatrolPath			(tPatrolPathParams.m_path,tPatrolPathParams.m_path_name);															
	SetPatrolStart			(tPatrolPathParams.m_tPatrolPathStart);																				
	SetPatrolStop			(tPatrolPathParams.m_tPatrolPathStop);																				
	SetPatrolRandom			(tPatrolPathParams.m_bRandom);																						
	m_tSpeedParam			= speed_param;
	m_previous_patrol_point = tPatrolPathParams.m_previous_index;
	m_fDistToEnd			= dist_to_end;
}						

IC	CScriptMovementAction::CScriptMovementAction		(MonsterSpace::EScriptMonsterMoveAction tAct, CScriptGameObject *tpObjectToGo, float dist_to_end, MonsterSpace::EScriptMonsterSpeedParam speed_param)
{
	m_tMoveAction		= tAct;
	SetObjectToGo		(tpObjectToGo);
	m_tSpeedParam		= speed_param;
	m_fDistToEnd		= dist_to_end;
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
IC	void CScriptMovementAction::SetBodyState			(const MonsterSpace::EBodyState tBodyState)
{
	m_tBodyState		= tBodyState;
	m_bCompleted		= false;
}

IC	void CScriptMovementAction::SetMovementType		(const MonsterSpace::EMovementType tMovementType)
{
	m_tMovementType		= tMovementType;
	m_bCompleted		= false;
}

IC	void CScriptMovementAction::SetPathType			(const DetailPathManager::EDetailPathType tPathType)
{
	m_tPathType			= tPathType;
	m_bCompleted		= false;
}

IC	void CScriptMovementAction::SetPatrolPath		(const CPatrolPath *path, shared_str path_name)
{
	m_path				= path;
	m_path_name			= path_name;
	m_tGoalType			= eGoalTypePatrolPath;
	m_bCompleted		= false;
}

IC	void CScriptMovementAction::SetPosition			(const Fvector &tPosition)
{
	m_tDestinationPosition = tPosition;
	m_tGoalType			= eGoalTypePathPosition;
	m_bCompleted		= false;
}

IC	void CScriptMovementAction::SetSpeed			(float fSpeed)
{
	m_fSpeed			= fSpeed;
	m_bCompleted		= false;
}

IC	void CScriptMovementAction::SetPatrolStart		(PatrolPathManager::EPatrolStartType tPatrolPathStart)
{
	m_tPatrolPathStart	= tPatrolPathStart;
	m_bCompleted		= false;
}

IC	void CScriptMovementAction::SetPatrolStop		(PatrolPathManager::EPatrolRouteType tPatrolPathStop)
{
	m_tPatrolPathStop	= tPatrolPathStop;
	m_bCompleted		= false;
}

IC	void CScriptMovementAction::SetPatrolRandom		(bool bRandom)
{
	m_bRandom			= bRandom;
	m_bCompleted		= false;
}

IC	void CScriptMovementAction::SetInputKeys		(const EInputKeys tInputKeys)
{
	m_tInputKeys		= tInputKeys;
	m_tGoalType			= eGoalTypeInput;
	m_bCompleted		= false;
}

IC	void CScriptMovementAction::initialize			()
{
}
