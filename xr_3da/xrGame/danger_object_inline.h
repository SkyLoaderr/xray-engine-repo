////////////////////////////////////////////////////////////////////////////
//	Module 		: danger_object_inline.h
//	Created 	: 14.02.2005
//  Modified 	: 14.02.2005
//	Author		: Dmitriy Iassenev
//	Description : Danger object inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	CDangerObject::CDangerObject											(
	const CEntityAlive	*object,
	const Fvector &position,
	u32 time,
	const EDangerType &type,
	const EDangerPerceiveType &perceive_type
)
{
	m_object		= object;
	m_position		= position;
	m_time			= time;
	m_type			= type;
	m_perceive_type	= perceive_type;
}

IC	const CEntityAlive *CDangerObject::object								() const
{
	return			(m_object);
}

IC	const Fvector &CDangerObject::position									() const
{
	return			(m_position);
}

IC	u32	CDangerObject::time													() const
{
	return			(m_time);
}

IC	const CDangerObject::EDangerType &CDangerObject::type					() const
{
	return			(m_type);
}

IC	const CDangerObject::EDangerPerceiveType &CDangerObject::perceive_type	() const
{
	return			(m_perceive_type);
}

IC	bool CDangerObject::operator==											(const CDangerObject &object) const
{
	if (!m_object && object.object())
		return		(false);

	if (m_object && !object.object())
		return		(false);

	if (m_object && (m_object->ID() != object.object()->ID()))
		return		(false);

	return			((type() == object.type()) &&(perceive_type() == object.perceive_type()));
}
