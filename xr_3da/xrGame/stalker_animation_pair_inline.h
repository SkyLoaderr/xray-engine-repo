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
#ifdef DEBUG
	m_skeleton_animated		= 0;
	m_object_name			= "unassigned";
	m_animation_type_name	= "unassigned";
#endif
}

IC	void CStalkerAnimationPair::reset				()
{
	m_animation				= 0;
	m_blend					= 0;
	m_actual				= true;
}

IC	bool CStalkerAnimationPair::actual				() const
{
	return					(m_actual);
}

IC	void CStalkerAnimationPair::animation			(CMotionDef	*animation)
{
	m_actual				= m_actual && (m_animation == animation);
	m_animation				= animation;
}

IC	CMotionDef *CStalkerAnimationPair::animation	() const
{
	return					(m_animation);
}

IC	CBlend *CStalkerAnimationPair::blend			() const
{
	return					(m_blend);
}

#ifdef DEBUG
IC	void CStalkerAnimationPair::set_dbg_info		(CSkeletonAnimated *skeleton_animated, LPCSTR object_name, LPCSTR animation_type_name)
{
	m_skeleton_animated		= skeleton_animated;
	m_object_name			= object_name;
	m_animation_type_name	= animation_type_name;
}
#endif
