////////////////////////////////////////////////////////////////////////////
//	Module 		: script_animation_action_inline.h
//	Created 	: 30.09.2003
//  Modified 	: 29.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Script animation action class inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	CScriptAnimationAction::CScriptAnimationAction	()
{
	m_tMentalState		= MonsterSpace::eMentalStateDanger;
	m_tGoalType			= eGoalTypeMental;
	m_bHandUsage		= true;
	m_tAnimAction		= MonsterSpace::eAA_NoAction;
	m_bCompleted		= true;
}

IC	CScriptAnimationAction::CScriptAnimationAction	(LPCSTR caAnimationToPlay, bool use_single_hand)
{
	SetAnimation		(caAnimationToPlay);
	m_bHandUsage		= !use_single_hand;
}

IC	CScriptAnimationAction::CScriptAnimationAction	(MonsterSpace::EMentalState tMentalState)
{
	SetMentalState		(tMentalState);
}

// -------------------------------------------------------------------------------------------------
// Monster
// -------------------------------------------------------------------------------------------------
IC	CScriptAnimationAction::CScriptAnimationAction	(MonsterSpace::EScriptMonsterAnimAction tAnimAction, int index)
{
	m_tAnimAction		= tAnimAction;
	m_bCompleted		= false;
	anim_index			= index;
}

IC	void CScriptAnimationAction::SetAnimation		(LPCSTR caAnimationToPlay)
{
	m_caAnimationToPlay	= caAnimationToPlay;
	m_tMentalState		= MonsterSpace::eMentalStateDanger;
	m_tGoalType			= eGoalTypeAnimation;
	m_bCompleted		= false;
}

IC	void CScriptAnimationAction::SetMentalState		(MonsterSpace::EMentalState tMentalState)
{
	m_tMentalState		= tMentalState;
	m_tGoalType			= eGoalTypeMental;
	m_bCompleted		= true;
}


IC	void CScriptAnimationAction::initialize			()
{
}
