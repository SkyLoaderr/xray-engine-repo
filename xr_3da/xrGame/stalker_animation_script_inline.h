////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_animation_script_inline.h
//	Created 	: 25.02.2003
//  Modified 	: 19.11.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker animation script inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	CStalkerAnimationScript::CStalkerAnimationScript(bool hand_usage, CMotionDef *animation) :
	m_hand_usage	(hand_usage),
	m_animation		(animation)
{
}

IC	bool CStalkerAnimationScript::hand_usage		() const
{
	return			(m_hand_usage);
}

IC	CMotionDef *CStalkerAnimationScript::animation	() const
{
	VERIFY			(m_animation);
	return			(m_animation);
}
