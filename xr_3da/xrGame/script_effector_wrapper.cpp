////////////////////////////////////////////////////////////////////////////
//	Module 		: script_effector_wrapper.cpp
//	Created 	: 06.02.2004
//  Modified 	: 06.02.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay Script effector wrapper class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_effector_wrapper.h"

CScriptEffectorWrapper::~CScriptEffectorWrapper	()
{
}

BOOL CScriptEffectorWrapper::Process		(SPPInfo &pp)
{
	BOOL		l_bResult = !!luabind::call_member<bool>(this,"process",pp);
	pp			= m_tInfo;
	return		(l_bResult);
}

bool CScriptEffectorWrapper::Process_static	(CScriptEffector *tpLuaEffector, SPPInfo &pp)
{
	return		(!!tpLuaEffector->CScriptEffector::Process(pp));
}
