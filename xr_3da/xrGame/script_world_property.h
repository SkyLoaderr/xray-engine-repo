////////////////////////////////////////////////////////////////////////////
//	Module 		: script_world_property.h
//	Created 	: 19.03.2004
//  Modified 	: 19.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Script world property
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "graph_engine.h"

class CScriptWorldProperty : public CGraphEngine::CWorldProperty {
protected:
	typedef CGraphEngine::CWorldProperty inherited;
public:
	IC						CScriptWorldProperty	();
	IC						CScriptWorldProperty	(_condition_type condition, _value_type value);
	IC	_condition_type		condition				() const;
	IC	_condition_type		value					() const;
};

#include "script_world_property_inline.h"