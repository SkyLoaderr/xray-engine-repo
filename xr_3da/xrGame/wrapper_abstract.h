////////////////////////////////////////////////////////////////////////////
//	Module 		: wrapper_abstract.h
//	Created 	: 28.01.2004
//  Modified 	: 10.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Abastract wrapper
////////////////////////////////////////////////////////////////////////////

#pragma once

template <
	typename _object_type,
	template <typename _base_object_type> class ancestor
>
class CWrapperAbstract : public ancestor<_base_object_type> {
protected:
	typedef ancestor<_base_object_type> inherited;

protected:
	_object_type	*m_object;

public:
	virtual			~CWrapperAbstract	();
	virtual void	reinit				(_object_type *object);
};

#include "wrapper_abstract_inline.h"