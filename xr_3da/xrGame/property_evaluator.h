////////////////////////////////////////////////////////////////////////////
//	Module 		: property_evaluator.h
//	Created 	: 12.03.2004
//  Modified 	: 12.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Property evaluator
////////////////////////////////////////////////////////////////////////////

#pragma once

template <typename _object_type>
class CPropertyEvaluator {
protected:
	_object_type	*m_object;

public:
					CPropertyEvaluator	();
	virtual 		~CPropertyEvaluator	();
			void	init				();
	virtual void	reinit				(_object_type *object);
	virtual void	Load				(LPCSTR section);
	virtual void	reload				(LPCSTR section);
	virtual	bool	evaluate			();
};

#include "property_evaluator_inline.h"