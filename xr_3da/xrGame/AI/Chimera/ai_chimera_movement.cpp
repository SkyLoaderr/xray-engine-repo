#include "stdafx.h"
#include "ai_chimera_movement.h"


CChimeraMovementManager::CChimeraMovementManager	()
{
	Init							();
}

CChimeraMovementManager::~CChimeraMovementManager	()
{
}

void CChimeraMovementManager::Init					()
{
}

void CChimeraMovementManager::Load	(LPCSTR section)
{
	inherited::Load					(section);
	
	inherited_shared::load_shared	(SUB_CLS_ID, section);

	m_movement_params.insert		(std::make_pair(eVelocityParameterStand,		STravelParams(0.f,						_sd->m_fsTurnNormalAngular	)));
	m_movement_params.insert		(std::make_pair(eVelocityParameterWalk,			STravelParams(_sd->m_fsWalkFwdNormal,	_sd->m_fsWalkAngular		)));
	m_movement_params.insert		(std::make_pair(eVelocityParameterRun,			STravelParams(_sd->m_fsRunFwdNormal,	_sd->m_fsRunAngular			)));
	m_movement_params.insert		(std::make_pair(eVelocityParameterWalkDamaged,	STravelParams(_sd->m_fsWalkFwdDamaged,	_sd->m_fsWalkAngular		)));
	m_movement_params.insert		(std::make_pair(eVelocityParameterRunDamaged,	STravelParams(_sd->m_fsRunFwdDamaged,	_sd->m_fsRunAngular			)));


}

void CChimeraMovementManager::reinit()
{
	inherited::reinit				();
}

void CChimeraMovementManager::reload(LPCSTR section)
{
	inherited::reload				(section);
}

void CChimeraMovementManager::load_shared(LPCSTR section)
{
	_sd->m_fsTurnNormalAngular	= pSettings->r_float(section,"TurnNormalAngular");
	_sd->m_fsWalkFwdNormal		= pSettings->r_float(section,"WalkFwdNormal");
	_sd->m_fsWalkFwdDamaged		= pSettings->r_float(section,"WalkFwdDamaged");
	_sd->m_fsWalkBkwdNormal		= pSettings->r_float(section,"WalkBkwdNormal");
	_sd->m_fsWalkAngular		= pSettings->r_float(section,"WalkAngular");
	_sd->m_fsRunFwdNormal		= pSettings->r_float(section,"RunFwdNormal");
	_sd->m_fsRunFwdDamaged		= pSettings->r_float(section,"RunFwdDamaged");
	_sd->m_fsRunAngular			= pSettings->r_float(section,"RunAngular");
	_sd->m_fsDrag				= pSettings->r_float(section,"Drag");
	_sd->m_fsSteal				= pSettings->r_float(section,"Steal");
}


void CChimeraMovementManager::update()
{
	CLevelLocationSelector::set_evaluator	(0);
	CDetailPathManager::set_path_type		(eDetailPathTypeSmooth);
	set_path_type							(CMovementManager::ePathTypeGamePath);
	set_selection_type						(CMovementManager::eSelectionTypeRandomBranching);
	
	set_velocity_mask						(eVelocityParamsRun);	
	set_desirable_mask						(eVelocityParamsWalk);

	CDetailPathManager::set_try_min_time	(false); 
	enable_movement							(true);

	update_path								();
}

