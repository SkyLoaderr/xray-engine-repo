////////////////////////////////////////////////////////////////////////////
//	Module 		: patrol_path.h
//	Created 	: 15.06.2004
//  Modified 	: 15.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Patrol path
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "graph_abstract.h"
#include "patrol_point.h"

class CPatrolPath : public CGraphAbstract<CPatrolPoint,float,u32,u32> {
protected:
	typedef CGraphAbstract<CPatrolPoint,float,u32,u32> inherited;

public:
	virtual					~CPatrolPath	();
			CPatrolPath		&load			(IReader &stream);
	IC		const CVertex	*point			(ref_str name) const;
	IC		const CVertex	*point			(const Fvector &position) const;
};

#include "patrol_path_inline.h"