////////////////////////////////////////////////////////////////////////////
//	Module 		: motivation_action.h
//	Created 	: 26.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Motivation action class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "motivation.h"
#include "graph_engine_space.h"
#include "condition_state.h"

template <typename _object_type>
class CMotivationAction : public CMotivation<_object_type> {
protected:
	typedef CMotivation<_object_type>		inherited;
	typedef GraphEngineSpace::CWorldState	CWorldState;

protected:
	CWorldState					m_goal;

public:
	IC							CMotivationAction	(const CWorldState &goal);
	IC		const CWorldState	&goal				() const;
	virtual void				update				();
};

#include "motivation_action_inline.h"