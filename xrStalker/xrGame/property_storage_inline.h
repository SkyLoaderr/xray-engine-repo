////////////////////////////////////////////////////////////////////////////
//	Module 		: property_storage_inline.h
//	Created 	: 29.03.2004
//  Modified 	: 29.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Property storage class inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	void CPropertyStorage::set_property	(const _condition_type &condition_id, const _value_type &value)
{
	m_storage[condition_id]				= value;
}

IC	const CPropertyStorage::_value_type	&CPropertyStorage::property	(const _condition_type &condition_id) const
{
	CConditionStorage::const_iterator	I = m_storage.find(condition_id);
	VERIFY								(m_storage.end() != I);
	return								((*I).second);
}

IC	void CPropertyStorage::clear		()
{
	m_storage.clear						();
}
