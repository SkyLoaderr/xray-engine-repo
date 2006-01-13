////////////////////////////////////////////////////////////////////////////
//	Module 		: purchase_list_inline.h
//	Created 	: 12.01.2006
//  Modified 	: 12.01.2006
//	Author		: Dmitriy Iassenev
//	Description : purchase list class inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	const float &CPurchaseList::deficit						(const shared_str &section) const
{
	DEFICITS::const_iterator	I = m_deficits.find(section);
	VERIFY						(I == m_deficits.end());
	return						((*I).second);
}

IC	const CPurchaseList::DEFICITS &CPurchaseList::deficits	() const
{
	return						(m_deficits);
}
