////////////////////////////////////////////////////////////////////////////
//	Module 		: motivation.h
//	Created 	: 22.03.2004
//  Modified 	: 22.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Motivation class
////////////////////////////////////////////////////////////////////////////

#pragma once

template <typename _object_type>
class CMotivation {
public:
	_object_type	*m_object;

public:
	IC				CMotivation		();
	virtual			~CMotivation	();
	IC		void	init			();
	virtual void	reinit			(_object_type *object);
	virtual void	Load			(LPCSTR section);
	virtual void	reload			(LPCSTR section);
	virtual void	destroy			();
	virtual float	evaluate		(u32 sub_motivation_id);
};

#include "motivation_inline.h"