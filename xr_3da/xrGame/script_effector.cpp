////////////////////////////////////////////////////////////////////////////
//	Module 		: script_effector.cpp
//	Created 	: 06.02.2004
//  Modified 	: 06.02.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay Script effector class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_effector.h"
#include "level.h"

CScriptEffector::~CScriptEffector	()
{
}

BOOL CScriptEffector::Process		(SPPInfo	&pp)
{
	return							(inherited::Process(pp));
}

void CScriptEffector::Add			()
{
	Level().Cameras.AddEffector		(this);
}

void CScriptEffector::Remove		()
{
	Level().Cameras.RemoveEffector	(m_tEffectorType);
}
