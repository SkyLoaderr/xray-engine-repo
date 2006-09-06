////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_animation_pair_inline.h
//	Created 	: 25.02.2003
//  Modified 	: 19.11.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker animation pair inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	CStalkerAnimationPair::CStalkerAnimationPair	()
{
	reset					();
	m_step_dependence		= false;
	m_global_animation		= false;
#ifdef DEBUG
	m_object_name			= "unassigned";
	m_animation_type_name	= "unassigned";
#endif
}

IC	void CStalkerAnimationPair::reset				()
{
	m_animation.invalidate	();
	m_blend					= 0;
	m_actual				= true;
}

IC	bool CStalkerAnimationPair::actual				() const
{
	return					(m_actual);
}

IC	bool CStalkerAnimationPair::animation			(const MotionID &animation)
{
	bool					result = (m_animation == animation);
	m_actual				= m_actual && result;
	m_animation				= animation;
	return					(result);
}

IC	const MotionID &CStalkerAnimationPair::animation() const
{
	return					(m_animation);
}

IC	CBlend *CStalkerAnimationPair::blend			() const
{
	return					(m_blend);
}

IC	void CStalkerAnimationPair::step_dependence		(bool value)
{
	m_step_dependence		= value;
}

IC	bool CStalkerAnimationPair::step_dependence		() const
{
	return					(m_step_dependence);
}

#ifdef DEBUG
IC	void CStalkerAnimationPair::set_dbg_info		(LPCSTR object_name, LPCSTR animation_type_name)
{
	m_object_name			= object_name;
	m_animation_type_name	= animation_type_name;
}
#endif

IC	void CStalkerAnimationPair::global_animation	(bool global_animation)
{
	m_global_animation		= global_animation;
}

IC	bool CStalkerAnimationPair::global_animation	() const
{
	return					(m_global_animation);
}

IC	void CStalkerAnimationPair::make_inactual		()
{
	m_actual				= false;
}
