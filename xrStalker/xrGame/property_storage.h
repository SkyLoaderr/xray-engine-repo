////////////////////////////////////////////////////////////////////////////
//	Module 		: property_storage.h
//	Created 	: 29.03.2004
//  Modified 	: 29.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Property storage class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "graph_engine.h"

class CPropertyStorage {
public:
	typedef CGraphEngine::_solver_condition_type	_condition_type;
	typedef CGraphEngine::_solver_value_type		_value_type;
	typedef CGraphEngine::CSolverConditionStorage	CConditionStorage;

public:
	CConditionStorage			m_storage;

public:
	IC		void				clear			();
	IC		void				set_property	(const _condition_type &condition_id, const _value_type &value);
	IC		const _value_type	&property		(const _condition_type &condition_id) const;
};

#include "property_storage_inline.h"