////////////////////////////////////////////////////////////////////////////
//	Module 		: object_action_base.cpp
//	Created 	: 12.03.2004
//  Modified 	: 12.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Object base action
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "object_action_base.h"

CObjectActionBase::CObjectActionBase(const xr_vector<COperatorCondition> &conditions, const xr_vector<COperatorCondition> &effects, LPCSTR action_name) :
		inherited	(conditions,effects,action_name)
{
}