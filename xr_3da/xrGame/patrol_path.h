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
#include "intrusive_ptr.h"

class CPatrolPath : public CGraphAbstract<CPatrolPoint,float,u32> {
private:
	struct CAlwaysTrueEvaluator {
		IC	bool	operator()	(const Fvector &position) const
		{
			return	(true);
		}
	};

protected:
	typedef CGraphAbstract<CPatrolPoint,float,u32> inherited;

public:
#ifdef DEBUG
	shared_str				m_name;
							CPatrolPath		(shared_str name);
#endif
	virtual					~CPatrolPath	();
			CPatrolPath		&load_path		(IReader &stream);
	IC		const CVertex	*point			(shared_str name) const;
	template <typename T>
	IC		const CVertex	*point			(const Fvector &position, const T &evaluator) const;
	IC		const CVertex	*point			(const Fvector &position) const;
};

#include "patrol_path_inline.h"