////////////////////////////////////////////////////////////////////////////
//	Module 		: property_evaluator.h
//	Created 	: 12.03.2004
//  Modified 	: 12.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Property evaluator
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "property_storage.h"

template <typename _object_type>
class CPropertyEvaluator {
protected:
	typedef CGraphEngine::_solver_condition_type	_condition_type;
	typedef CGraphEngine::_solver_value_type		_value_type;

public:
	_object_type		*m_object;
	CPropertyStorage	*m_storage;

public:
								CPropertyEvaluator	(_object_type *object = 0);
	virtual 					~CPropertyEvaluator	();
			void				init				(_object_type *object);
	virtual void				reinit				(_object_type *object, CPropertyStorage *storage);
	virtual void				Load				(LPCSTR section);
	virtual void				reload				(LPCSTR section);
	virtual	_value_type			evaluate			();
	IC		const _value_type	&property			(const _condition_type &condition_id) const;
};

#include "property_evaluator_inline.h"