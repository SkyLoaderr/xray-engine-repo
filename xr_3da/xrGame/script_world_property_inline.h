////////////////////////////////////////////////////////////////////////////
//	Module 		: script_world_property_inline.h
//	Created 	: 19.03.2004
//  Modified 	: 19.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Script world property inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	CScriptWorldProperty::CScriptWorldProperty	() :
	inherited			(_condition_type(-1),_value_type(-1))
{
}

IC	CScriptWorldProperty::CScriptWorldProperty	(_condition_type condition, _value_type value) :
	inherited			(condition,value)
{
}

IC	CScriptWorldProperty::_condition_type CScriptWorldProperty::condition	() const
{
	return				(m_condition);
}

IC	CScriptWorldProperty::_condition_type CScriptWorldProperty::value	() const
{
	return				(m_value);
}
